#include <stdbool.h>
#include <stdint.h>

#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/regs/sio.h"
#include "hardware/structs/sio.h"
#include "hardware/sync.h"
#include "hardware/vreg.h"
#include "pico/multicore.h"
#include "pico/stdio_usb.h"
#include "pico/stdlib.h"

#include "bus.pio.h"

#include "fbkb.h"
#include "ppu.h"

#define USE_FB_KB 1

enum {
    PIN_ADDR_BASE = 0,
    PIN_ADDR_COUNT = 16,
    PIN_DATA_BASE = 16,
    PIN_DATA_COUNT = 8,
    PIN_NMI_N = 24,
    PIN_RW = 25,
    PIN_IRQ_N = 26,
    PIN_RDY = 27,
    PIN_RESET_N = 28,
    PIN_CLK_OUT = 29,
};

enum {
    SYS_CLOCK_KHZ = 232000,
    CLOCK_OUT_HALF_CYCLES = 34,
    CLOCK_OUT_CLKDIV_INT = 1,
    CLOCK_OUT_CLKDIV_FRAC8 = 232,
    CLOCK_OUT_CLKDIV_256 = CLOCK_OUT_CLKDIV_INT * 256 + CLOCK_OUT_CLKDIV_FRAC8,
    MPU_CLOCK_KHZ = (SYS_CLOCK_KHZ * 256) / (2 * CLOCK_OUT_HALF_CYCLES * CLOCK_OUT_CLKDIV_256),
    MEMORY_SIZE = 64 * 1024,
    RAM_SIZE = 8 * 1024,
    ROM_SIZE = 32 * 1024,
    MPU_RAM2_BASE = 0x6000,
    MPU_ROM_BASE = 0x8000,
    MPU_IO_BASE = 0x4000,
    FBKB_PORT_4016_ADDR = MPU_IO_BASE + 0x16,
    FBKB_PORT_4017_ADDR = MPU_IO_BASE + 0x17,
    ACIA_STATUS_ADDR = MPU_IO_BASE + 0x18,
    ACIA_DATA_ADDR = MPU_IO_BASE + 0x19,
};

enum {
    ADDRESS_MASK = 0x0000ffffu,
    DATA_MASK = 0x00ff0000u,
    NMI_N_MASK = 1u << PIN_NMI_N,
    RESET_N_MASK = 1u << PIN_RESET_N,
    RW_MASK = 1u << PIN_RW,
    CLOCK_PHASE_MASK = 3u << 30,
    CLOCK_PHASE_POS = 0u << 30,
    CLOCK_PHASE_NEG = 1u << 30,
};

enum {
    ACIA_SR_RDRF = SIO_FIFO_ST_VLD_BITS,
    ACIA_SR_TDRE = SIO_FIFO_ST_RDY_BITS,
};

enum {
    VSYNC_PERIOD_CYCLES = 29780,
    VBLANK_BEGIN_CYCLE = VSYNC_PERIOD_CYCLES - (340 * 21 / 3),
    IS_COMMAND_MASK = 0xf0000000u,
};

static uint8_t memory[MEMORY_SIZE] __attribute__((section(".data.memory_image"), aligned(4), used)) = {
    [0 ... MEMORY_SIZE - 1] = 0xff,
    [0x0000] =
#include "../../ROM/RAM4KB.h"
    ,
    [0x1000 ... RAM_SIZE - 1] = 0x00,
    [0x4000 ... 0x4fff] = 0x00,
    [MPU_RAM2_BASE ... MPU_RAM2_BASE + RAM_SIZE - 1] = 0x00,
#if USE_FB_KB
    [MPU_ROM_BASE] =
#include "rom_image.h"
#else
    [0xc000] =
#include "../../firmware_65816_pico/src/rom_image.h"
#endif
};

static PIO bus_pio = pio0;
static uint data_value_sm = 0;
static uint data_dir_lo_sm = 1;
static uint data_dir_hi_sm = 2;
static uint clock_sm = 3;

static uint32_t core_command_data;
static uint32_t core_command_full;

static inline __attribute__((always_inline)) void drain_core_fifo(void) {
    while (sio_hw->fifo_st & SIO_FIFO_ST_VLD_BITS) {
        (void)sio_hw->fifo_rd;
    }
}

static inline __attribute__((always_inline)) uint8_t acia_read_status(void) {
    uint8_t status = (uint8_t)(sio_hw->fifo_st & ACIA_SR_RDRF);

    if (__atomic_load_n(&core_command_full, __ATOMIC_ACQUIRE) == 0u) {
        status |= ACIA_SR_TDRE;
    }
    return status;
}

static inline __attribute__((always_inline)) uint8_t acia_read_data(void) {
    // MC6850 software is expected to read data only when RDRF is set. If the
    // emulated data register is read while empty, the returned value is
    // intentionally undefined; skipping the VLD check keeps the hot path short.
    return (uint8_t)sio_hw->fifo_rd;
}

static inline __attribute__((always_inline)) void core_command_send(uint32_t command) {
    while (__atomic_load_n(&core_command_full, __ATOMIC_ACQUIRE) != 0u) {
        tight_loop_contents();
    }

    core_command_data = command;
    __atomic_store_n(&core_command_full, 1u, __ATOMIC_RELEASE);
}

static inline __attribute__((always_inline)) void acia_write_data(uint32_t value) {
    core_command_send(value);
}

static inline __attribute__((always_inline)) void drive_data_bus_for_read(uint8_t value) {
    bus_pio->txf[data_value_sm] = value;
}

static void usb_puts_raw(const char *text) {
    while (*text != '\0') {
        putchar_raw(*text++);
    }
}

static void usb_put_uint_raw(uint32_t value) {
    char digits[10];
    uint count = 0;

    do {
        digits[count++] = (char)('0' + (value % 10u));
        value /= 10u;
    } while (value != 0u);

    while (count != 0u) {
        putchar_raw(digits[--count]);
    }
}

static void usb_put_hex_raw(uint32_t value, uint digits) {
    static const char hex[] = "0123456789abcdef";

    while (digits != 0u) {
        uint shift = (--digits) * 4u;
        putchar_raw(hex[(value >> shift) & 0xfu]);
    }
}

static void draw(int bg_index) {
    static int view = 0;

    if (view == 0) {
        uint16_t bg_address = (uint16_t)(PPU_VRAM_BG + bg_index * PPU_NAMETABLE_SIZE);
#if 1
        for (size_t i = 0; i < 0x400; i += 32) {
            for (size_t j = 0; j < 32; ++j) {
                char c = (ppu.vram[(bg_address + i + j) & PPU_VRAM_MASK]);
                if (c == 0xb2) {
                    putchar_raw('[');
                } else if (c == 0xb3) {
                    putchar_raw(']');
                } else {
                    if (c >= 0x20 && c < 0x7f) {
                        putchar_raw(c);
                    } else {
                        if (c < 0x20) {
                            putchar_raw('.');
                        } else {
                            putchar_raw('*');
                        }
                    }
                }
            }
            putchar_raw('\n');
        }
        putchar_raw('\n');
#else
        for (size_t i = 0; i < PPU_REGISTER_COUNT; ++i) {
            usb_put_hex_raw(ppu.registers[i], 2);
            putchar_raw(' ');
        }
        putchar_raw('\n');

        usb_put_hex_raw(ppu.scroll[PPU_SCROLL_X], 2);
        putchar_raw(',');
        usb_put_hex_raw(ppu.scroll[PPU_SCROLL_Y], 2);
        putchar_raw(' ');
        putchar_raw('$');
        usb_put_hex_raw(ppu_vram_addr(), 4);
        putchar_raw(' ');
        putchar_raw(ppu.write_latch ? '1' : '0');
        putchar_raw('\n');

        for (size_t i = 0; i < 0x200; i += 16) {
            usb_put_hex_raw((uint32_t)(bg_address + i), 4);
            usb_puts_raw(": ");
            for (size_t j = 0; j < 16; ++j) {
                uint8_t c = ppu.vram[(bg_address + i + j) & PPU_VRAM_MASK];
                usb_put_hex_raw(c, 2);
                putchar_raw(' ');
            }
            putchar_raw('\n');
        }
        putchar_raw('\n');
#endif
    }
    view = (view + 1) % 240;
}

static void usb_write_banner(void) {
    usb_puts_raw("\n\nPico 65816 Ver.FB KB/BG ");
    usb_put_uint_raw(MPU_CLOCK_KHZ);
    usb_puts_raw(" kHz\n\n");
}

static void __not_in_flash_func(usb_service_loop)(void) {
    drain_core_fifo();

    sleep_ms(1000);
    if (stdio_usb_connected()) {
        usb_write_banner();
    }

    sio_hw->gpio_set = RESET_N_MASK;

    for (;;) {
        bool connected = stdio_usb_connected();

#if !USE_FB_KB
        int ch;
        while (connected && (sio_hw->fifo_st & SIO_FIFO_ST_RDY_BITS) &&
               (ch = getchar_timeout_us(0)) != PICO_ERROR_TIMEOUT) {
            sio_hw->fifo_wr = (uint8_t)ch;
        }
#endif

        if (__atomic_load_n(&core_command_full, __ATOMIC_ACQUIRE) != 0u) {
            uint32_t cmd = core_command_data;

            if ((cmd & IS_COMMAND_MASK) == 0) {
                // USB CDC: write to host
                if (connected) {
                    putchar_raw((char)cmd);
                }
#if USE_FB_KB
            } else if ((cmd & IS_COMMAND_MASK) == FBKB_WRITE_COMMAND) {
                // FBKB: USB CDC as keyboard
                fbkb_write_register((uint8_t)(cmd >> FBKB_COMMAND_PORT_SHIFT), (uint8_t)cmd);
            } else if ((cmd & PPU_WRITE_COMMAND) != 0u) {
                // PPU: write regs
                uint8_t index = (uint8_t)((cmd >> PPU_COMMAND_INDEX_SHIFT) & PPU_COMMAND_INDEX_MASK);
                if (ppu_write_register(index, (uint8_t)cmd)) {
                    sio_hw->gpio_clr = NMI_N_MASK;
                }
#endif
            } else if ((cmd & PPU_READ_COMMAND) != 0u) {
                // PPU: read regs
                uint8_t index = (uint8_t)((cmd >> PPU_COMMAND_INDEX_SHIFT) & PPU_COMMAND_INDEX_MASK);
                ppu_read_register(index);
            } else if (cmd == PPU_VBLANK_BEGIN_COMMAND) {
                // PPU: begin VBlank and assert NMI when enabled.
                if (ppu_vblank_begin()) {
                    sio_hw->gpio_clr = NMI_N_MASK;
                }
            } else if (cmd == PPU_VBLANK_END_COMMAND) {
                // PPU: end VBlank and release NMI.
                ppu_vblank_end();
                sio_hw->gpio_set = NMI_N_MASK;
                if ((ppu.registers[PPU_REG_PPUMASK] & PPU_SHOW_BG_BIT) != 0u) {
                    draw(ppu.registers[PPU_REG_PPUCTRL] & PPU_BG_INDEX_MASK);
                }
            }

            // Keep the one-word slot full until command processing completes.
            __atomic_store_n(&core_command_full, 0u, __ATOMIC_RELEASE);
        }

        tight_loop_contents();
    }
}

static void init_gpio(void) {
    gpio_init(PIN_RESET_N);
    gpio_put(PIN_RESET_N, false);
    gpio_set_dir(PIN_RESET_N, GPIO_OUT);

    gpio_init(PIN_NMI_N);
    gpio_put(PIN_NMI_N, true);
    gpio_set_dir(PIN_NMI_N, GPIO_OUT);

    gpio_init(PIN_IRQ_N);
    gpio_put(PIN_IRQ_N, true);
    gpio_set_dir(PIN_IRQ_N, GPIO_OUT);

    gpio_init(PIN_RDY);
    gpio_put(PIN_RDY, true);
    gpio_set_dir(PIN_RDY, GPIO_OUT);

    for (uint pin = PIN_ADDR_BASE; pin < PIN_ADDR_BASE + PIN_ADDR_COUNT; ++pin) {
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_IN);
        gpio_disable_pulls(pin);
    }

    gpio_init(PIN_RW);
    gpio_set_dir(PIN_RW, GPIO_IN);
    gpio_disable_pulls(PIN_RW);

}

static void init_data_bus_pio(PIO pio, uint value_sm, uint dir_lo_sm, uint dir_hi_sm) {
    uint value_offset = pio_add_program(pio, &data_bus_read_program);
    uint dir_offset = pio_add_program(pio, &data_bus_dir4_program);

    pio_sm_config value_config = data_bus_read_program_get_default_config(value_offset);
    sm_config_set_out_pins(&value_config, PIN_DATA_BASE, PIN_DATA_COUNT);
    sm_config_set_out_shift(&value_config, true, true, 8);

    for (uint pin = PIN_DATA_BASE; pin < PIN_DATA_BASE + PIN_DATA_COUNT; ++pin) {
        pio_gpio_init(pio, pin);
        gpio_disable_pulls(pin);
        gpio_set_input_enabled(pin, true);
    }

    pio_sm_set_consecutive_pindirs(pio, value_sm, PIN_DATA_BASE, PIN_DATA_COUNT, false);
    pio_sm_clear_fifos(pio, value_sm);
    pio_sm_init(pio, value_sm, value_offset, &value_config);

    pio_sm_config dir_lo_config = data_bus_dir4_program_get_default_config(dir_offset);
    sm_config_set_set_pins(&dir_lo_config, PIN_DATA_BASE, 4);
    sm_config_set_in_pins(&dir_lo_config, PIN_RW);
    sm_config_set_in_shift(&dir_lo_config, false, false, 32);
    pio_sm_set_consecutive_pindirs(pio, dir_lo_sm, PIN_DATA_BASE, 4, false);
    pio_sm_clear_fifos(pio, dir_lo_sm);
    pio_sm_init(pio, dir_lo_sm, dir_offset, &dir_lo_config);

    pio_sm_config dir_hi_config = data_bus_dir4_program_get_default_config(dir_offset);
    sm_config_set_set_pins(&dir_hi_config, PIN_DATA_BASE + 4, 4);
    sm_config_set_in_pins(&dir_hi_config, PIN_RW);
    sm_config_set_in_shift(&dir_hi_config, false, false, 32);
    pio_sm_set_consecutive_pindirs(pio, dir_hi_sm, PIN_DATA_BASE + 4, 4, false);
    pio_sm_clear_fifos(pio, dir_hi_sm);
    pio_sm_init(pio, dir_hi_sm, dir_offset, &dir_hi_config);

    pio_sm_set_enabled(pio, value_sm, true);
    pio_sm_set_enabled(pio, dir_lo_sm, true);
    pio_sm_set_enabled(pio, dir_hi_sm, true);
}

static void init_clock_pio(PIO pio, uint sm) {
    uint offset = pio_add_program(pio, &clock_mpu_program);
    pio_sm_config config = clock_mpu_program_get_default_config(offset);

    sm_config_set_set_pins(&config, PIN_CLK_OUT, 1);
    sm_config_set_in_pins(&config, PIN_ADDR_BASE);
    sm_config_set_in_shift(&config, false, false, 32);
    sm_config_set_clkdiv_int_frac(&config, CLOCK_OUT_CLKDIV_INT, CLOCK_OUT_CLKDIV_FRAC8);

    pio_gpio_init(pio, PIN_CLK_OUT);
    pio_sm_set_consecutive_pindirs(pio, sm, PIN_CLK_OUT, 1, true);
    pio_sm_clear_fifos(pio, sm);
    pio_sm_init(pio, sm, offset, &config);
    pio_sm_set_enabled(pio, sm, true);
}

static inline __attribute__((always_inline)) uint32_t clock_receive_pins(void) {
    const uint32_t rx_empty_mask = 1u << (PIO_FSTAT_RXEMPTY_LSB + clock_sm);

    while (bus_pio->fstat & rx_empty_mask) {
        tight_loop_contents();
    }
    return bus_pio->rxf[clock_sm];
}

static inline __attribute__((always_inline)) void clock_acknowledge(void) {
    bus_pio->txf[clock_sm] = 1u;
}

static void __attribute__((noinline, noreturn)) __not_in_flash_func(bus_service_loop)(void) {
    uint32_t pins;
    uint32_t vsync_counter = 0;

    for (;;) {
        // Wait for a rising-edge (POS) snapshot with RESET_N released.
        pins = clock_receive_pins();
        if ((pins & (CLOCK_PHASE_MASK | RESET_N_MASK)) ==
            (CLOCK_PHASE_POS | RESET_N_MASK)) {
            break;
        }
        clock_acknowledge();
    }

    for (;;) {
        uint16_t address = (uint16_t)(pins & ADDRESS_MASK);

        // CPU: clock H period

        if (pins & RW_MASK) {
            uint8_t value;
            uint8_t ppu_index = 0;

            if ((address & PPU_REGISTER_RANGE_MASK) == PPU_REGISTER_BASE) {
                ppu_index = address & (PPU_REGISTER_COUNT - 1);
                value = ppu.registers[ppu_index];
            } else if (address == FBKB_PORT_4016_ADDR || address == FBKB_PORT_4017_ADDR) {
                uint8_t fbkb_index = address & 1;
                value = fbkb.read_data[fbkb_index];
            } else if (address == ACIA_STATUS_ADDR) {
                value = acia_read_status();
            } else if (address == ACIA_DATA_ADDR) {
                value = acia_read_data();
            } else {
                value = memory[address];
            }

            drive_data_bus_for_read(value);

            clock_acknowledge();
            // Wait for the falling-edge (NEG) snapshot of this read cycle.
            (void)clock_receive_pins();

            // PPU: clock L period

            if (ppu_index != 0u) {
                core_command_send(ppu_make_read_command(ppu_index));
            }
        } else {
            enum {
                WRITE_TARGET_NONE,
                WRITE_TARGET_MEMORY,
                WRITE_TARGET_ACIA,
                WRITE_TARGET_PPU,
                WRITE_TARGET_FBKB,
            };

            uint8_t write_target = WRITE_TARGET_NONE;
            uint8_t ppu_index = 0;
            uint8_t fbkb_index = 0;

            if ((address & PPU_REGISTER_RANGE_MASK) == PPU_REGISTER_BASE) {
                write_target = WRITE_TARGET_PPU;
                ppu_index = address & (PPU_REGISTER_COUNT - 1);
            } else if (address == FBKB_PORT_4016_ADDR || address == FBKB_PORT_4017_ADDR) {
                write_target = WRITE_TARGET_FBKB;
                fbkb_index = address & 1;
            } else if (address < RAM_SIZE || (address >= MPU_RAM2_BASE && address < MPU_RAM2_BASE + RAM_SIZE)) {
                write_target = WRITE_TARGET_MEMORY;
            } else if (address == ACIA_DATA_ADDR) {
                write_target = WRITE_TARGET_ACIA;
            }

            clock_acknowledge();
            // Wait for the falling-edge (NEG) snapshot containing write data.
            uint32_t data_pins = clock_receive_pins();

            // PPU: clock L period

            uint8_t value = (uint8_t)(data_pins >> PIN_DATA_BASE);

            switch (write_target) {
            case WRITE_TARGET_MEMORY:
                memory[address] = value;
                break;
            case WRITE_TARGET_ACIA:
                acia_write_data(value);
                break;
            case WRITE_TARGET_PPU:
                core_command_send(ppu_make_write_command(ppu_index, value));
                break;
            case WRITE_TARGET_FBKB:
                core_command_send(fbkb_make_write_command(fbkb_index, value));
                break;
            default:
                break;
            }
        }

        // PPU: V-sync

        ++vsync_counter;
        if (vsync_counter == VBLANK_BEGIN_CYCLE) {
            core_command_send(PPU_VBLANK_BEGIN_COMMAND);
        }
        if (vsync_counter == VSYNC_PERIOD_CYCLES) {
            core_command_send(PPU_VBLANK_END_COMMAND);
            vsync_counter = 0;
        }

        clock_acknowledge();
        // Wait for the rising-edge (POS) snapshot of the next MPU cycle.
        pins = clock_receive_pins();
    }
}

static void __attribute__((noreturn)) __not_in_flash_func(bus_core_entry)(void) {
    uint32_t irq_state = save_and_disable_interrupts();
    (void)irq_state;

    bus_service_loop();
}

int main(void) {
    vreg_set_voltage(VREG_VOLTAGE_1_15);
    sleep_ms(10);
    set_sys_clock_khz(SYS_CLOCK_KHZ, true);

    init_gpio();
    init_data_bus_pio(bus_pio, data_value_sm, data_dir_lo_sm, data_dir_hi_sm);
    init_clock_pio(bus_pio, clock_sm);
    (void)stdio_usb_init();

    sleep_ms(10);

    multicore_launch_core1(bus_core_entry);
    usb_service_loop();
}
