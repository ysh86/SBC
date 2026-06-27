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

#define OVERCLOCK 0

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
#if OVERCLOCK
    SYS_CLOCK_KHZ = 295200,
    CLOCK_OUT_HALF_CYCLES = 13,
    CLOCK_OUT_CLKDIV_INT = 3,
    CLOCK_OUT_CLKDIV_FRAC8 = 44,
#else
    SYS_CLOCK_KHZ = 157500,
    CLOCK_OUT_HALF_CYCLES = 44,
    CLOCK_OUT_CLKDIV_INT = 1,
    CLOCK_OUT_CLKDIV_FRAC8 = 0,
#endif
    CLOCK_OUT_CLKDIV_256 = CLOCK_OUT_CLKDIV_INT * 256 + CLOCK_OUT_CLKDIV_FRAC8,
    MPU_CLOCK_KHZ = (SYS_CLOCK_KHZ * 256) / (2 * CLOCK_OUT_HALF_CYCLES * CLOCK_OUT_CLKDIV_256),
    MEMORY_SIZE = 64 * 1024,
    RAM_SIZE = 8 * 1024,
    ROM_SIZE = 32 * 1024,
    MPU_RAM2_BASE = 0x6000,
    MPU_ROM_BASE = 0x8000,
    MPU_IO_BASE = 0x4000,
    ACIA_STATUS_ADDR = MPU_IO_BASE + 0x18,
    ACIA_DATA_ADDR = MPU_IO_BASE + 0x19,
};

enum {
    ADDRESS_MASK = 0x0000ffffu,
    DATA_MASK = 0x00ff0000u,
    RESET_N_MASK = 1u << PIN_RESET_N,
    RW_MASK = 1u << PIN_RW,
    CLK_OUT_MASK = 1u << PIN_CLK_OUT,
};

enum {
    ACIA_SR_RDRF = SIO_FIFO_ST_VLD_BITS,
    ACIA_SR_TDRE = SIO_FIFO_ST_RDY_BITS,
    ACIA_STATUS_MASK = ACIA_SR_RDRF | ACIA_SR_TDRE,
};

static uint8_t memory[MEMORY_SIZE] __attribute__((section(".data.memory_image"), aligned(4), used)) = {
    [0 ... MEMORY_SIZE - 1] = 0xff,
    [0 ... RAM_SIZE - 1] = 0x00,
    [MPU_RAM2_BASE ... MPU_RAM2_BASE + RAM_SIZE - 1] = 0x00,
    // MPU_ROM_BASE through $bfff remains $ff; the included ROM image starts at $c000.
    [0xc000] =
#include "rom_image.h"
};

static PIO bus_pio = pio0;
static uint data_value_sm = 0;
static uint data_dir_lo_sm = 1;
static uint data_dir_hi_sm = 2;
static uint clock_sm = 3;

static inline __attribute__((always_inline)) void drain_core_fifo(void) {
    while (sio_hw->fifo_st & SIO_FIFO_ST_VLD_BITS) {
        (void)sio_hw->fifo_rd;
    }
}

static inline __attribute__((always_inline)) uint8_t acia_read_status(void) {
    return (uint8_t)(sio_hw->fifo_st & ACIA_STATUS_MASK);
}

static inline __attribute__((always_inline)) uint8_t acia_read_data(void) {
    // MC6850 software is expected to read data only when RDRF is set. If the
    // emulated data register is read while empty, the returned value is
    // intentionally undefined; skipping the VLD check keeps the hot path short.
    return (uint8_t)sio_hw->fifo_rd;
}

static inline __attribute__((always_inline)) void acia_write_data(uint8_t value) {
    if (sio_hw->fifo_st & SIO_FIFO_ST_RDY_BITS) {
        sio_hw->fifo_wr = value;
    }
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

static void usb_write_banner(void) {
    usb_puts_raw("\n\nPico 65816 ");
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

        int ch;
        while (connected && (sio_hw->fifo_st & SIO_FIFO_ST_RDY_BITS) &&
               (ch = getchar_timeout_us(0)) != PICO_ERROR_TIMEOUT) {
            sio_hw->fifo_wr = (uint8_t)ch;
        }

        if (!connected) {
            drain_core_fifo();
        } else if (sio_hw->fifo_st & SIO_FIFO_ST_VLD_BITS) {
            putchar_raw((char)sio_hw->fifo_rd);
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
#if OVERCLOCK
    uint offset = pio_add_program(pio, &clock_mpu_overclock_program);
    pio_sm_config config = clock_mpu_overclock_program_get_default_config(offset);
#else
    uint offset = pio_add_program(pio, &clock_mpu_program);
    pio_sm_config config = clock_mpu_program_get_default_config(offset);
#endif

    sm_config_set_set_pins(&config, PIN_CLK_OUT, 1);
    sm_config_set_clkdiv_int_frac(&config, CLOCK_OUT_CLKDIV_INT, CLOCK_OUT_CLKDIV_FRAC8);

    pio_gpio_init(pio, PIN_CLK_OUT);
    pio_sm_set_consecutive_pindirs(pio, sm, PIN_CLK_OUT, 1, true);
    pio_sm_init(pio, sm, offset, &config);
    pio_sm_set_enabled(pio, sm, true);
}

static void __attribute__((noinline, noreturn)) __not_in_flash_func(bus_service_loop)(void) {
    // Wait until RESET_N is released.
    while ((sio_hw->gpio_in & RESET_N_MASK) == 0u) {
        tight_loop_contents();
    }

    for (;;) {
        uint32_t pins;

        do {
            pins = sio_hw->gpio_in;
        } while ((pins & CLK_OUT_MASK) == 0u);

        uint16_t address = (uint16_t)(pins & ADDRESS_MASK);

        if (pins & RW_MASK) {
            uint8_t value;

            if (address == ACIA_STATUS_ADDR) {
                value = acia_read_status();
            } else if (address == ACIA_DATA_ADDR) {
                value = acia_read_data();
            } else {
                value = memory[address];
            }

            drive_data_bus_for_read(value);

            do {
                pins = sio_hw->gpio_in;
            } while (pins & CLK_OUT_MASK);
        } else {
            uint32_t data_pins;

            do {
                data_pins = pins;
                pins = sio_hw->gpio_in;
            } while (pins & CLK_OUT_MASK);

            uint8_t value = (uint8_t)(data_pins >> PIN_DATA_BASE);

            if (address < RAM_SIZE) {
                memory[address] = value;
            } else if (address == ACIA_DATA_ADDR) {
                acia_write_data(value);
            } else if (address >= MPU_RAM2_BASE && address < MPU_RAM2_BASE + RAM_SIZE) {
                memory[address] = value;
            }
        }
    }
}

static void __attribute__((noreturn)) __not_in_flash_func(bus_core_entry)(void) {
    uint32_t irq_state = save_and_disable_interrupts();
    (void)irq_state;

    bus_service_loop();
}

int main(void) {
#if OVERCLOCK
    vreg_set_voltage(VREG_VOLTAGE_1_30);
#else
    vreg_set_voltage(VREG_VOLTAGE_DEFAULT);
#endif
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
