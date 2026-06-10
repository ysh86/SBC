#include <stdbool.h>
#include <stdint.h>

#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/regs/sio.h"
#include "hardware/structs/sio.h"
#include "hardware/sync.h"
#include "pico/multicore.h"
#include "pico/stdio_usb.h"
#include "pico/stdlib.h"

#include "bus.pio.h"

enum {
    PIN_ADDR_BASE = 0,
    PIN_ADDR_COUNT = 16,
    PIN_DATA_BASE = 16,
    PIN_DATA_COUNT = 8,
    PIN_E = 24,
    PIN_RW = 25,
    PIN_VMA = 26,
    PIN_RESET_N = 28,
    PIN_CLK_OUT = 29,
};

enum {
    SYS_CLOCK_KHZ = 128000,
    PIO_CLOCK_DIV_LOG2 = 4,
    MPU_CLOCK_DIV_LOG2 = 2,
    MPU_CLOCK_KHZ = SYS_CLOCK_KHZ >> (PIO_CLOCK_DIV_LOG2 + MPU_CLOCK_DIV_LOG2),
    MEMORY_SIZE = 64 * 1024,
    RAM_SIZE = 32 * 1024,
    ROM_SIZE = 16 * 1024,
    MPU_ROM_BASE = 0xc000,
    ACIA_STATUS_ADDR = 0x8000,
    ACIA_DATA_ADDR = 0x8001,
    UNMAPPED_BASE = 0x8002,
    UNMAPPED_END = 0xbfff,
};

enum {
    ADDRESS_MASK = 0x0000ffffu,
    DATA_MASK = 0x00ff0000u,
    E_MASK = 1u << PIN_E,
    RW_MASK = 1u << PIN_RW,
    VMA_MASK = 1u << PIN_VMA,
    VALID_CYCLE_MASK = E_MASK | VMA_MASK,
};

enum {
    ACIA_SR_RDRF = SIO_FIFO_ST_VLD_BITS,
    ACIA_SR_TDRE = SIO_FIFO_ST_RDY_BITS,
    ACIA_STATUS_MASK = ACIA_SR_RDRF | ACIA_SR_TDRE,
};

static uint8_t memory[MEMORY_SIZE] __attribute__((section(".data.memory_image"), aligned(4), used)) = {
    [UNMAPPED_BASE ... UNMAPPED_END] = 0xff,
    [MPU_ROM_BASE] =
#include "rom_image.h"
};

static volatile bool usb_core_ready;

static PIO bus_pio = pio0;
static uint data_sm = 0;

static inline __attribute__((always_inline)) void drain_core_fifo(void) {
    while (sio_hw->fifo_st & SIO_FIFO_ST_VLD_BITS) {
        (void)sio_hw->fifo_rd;
    }
}

static inline __attribute__((always_inline)) uint8_t acia_read_status(void) {
    return (uint8_t)(sio_hw->fifo_st & ACIA_STATUS_MASK);
}

static inline __attribute__((always_inline)) uint8_t acia_read_data(void) {
    if (sio_hw->fifo_st & SIO_FIFO_ST_VLD_BITS) {
        return (uint8_t)sio_hw->fifo_rd;
    }
    return 0xffu;
}

static inline __attribute__((always_inline)) void acia_write_data(uint8_t value) {
    if (sio_hw->fifo_st & SIO_FIFO_ST_RDY_BITS) {
        sio_hw->fifo_wr = value;
    }
}

static inline __attribute__((always_inline)) void drive_data_bus_for_read(uint8_t value) {
    bus_pio->txf[data_sm] = value;
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
    usb_puts_raw("\nPico 6802 ");
    usb_put_uint_raw(MPU_CLOCK_KHZ);
    usb_puts_raw(" kHz\n\n");
}

static void __not_in_flash_func(usb_core_entry)(void) {
    (void)stdio_usb_init();
    drain_core_fifo();
    usb_core_ready = true;

    if (stdio_usb_connected()) {
        usb_write_banner();
    }

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

    for (uint pin = PIN_ADDR_BASE; pin < PIN_ADDR_BASE + PIN_ADDR_COUNT; ++pin) {
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_IN);
        gpio_disable_pulls(pin);
    }

    gpio_init(PIN_E);
    gpio_set_dir(PIN_E, GPIO_IN);
    gpio_disable_pulls(PIN_E);

    gpio_init(PIN_RW);
    gpio_set_dir(PIN_RW, GPIO_IN);
    gpio_disable_pulls(PIN_RW);

    gpio_init(PIN_VMA);
    gpio_set_dir(PIN_VMA, GPIO_IN);
    gpio_disable_pulls(PIN_VMA);
}

static void init_data_bus_pio(PIO pio, uint sm) {
    uint offset = pio_add_program(pio, &data_bus_read_program);
    pio_sm_config config = data_bus_read_program_get_default_config(offset);

    sm_config_set_out_pins(&config, PIN_DATA_BASE, PIN_DATA_COUNT);
    sm_config_set_out_shift(&config, true, false, 32);

    for (uint pin = PIN_DATA_BASE; pin < PIN_DATA_BASE + PIN_DATA_COUNT; ++pin) {
        pio_gpio_init(pio, pin);
        gpio_disable_pulls(pin);
        gpio_set_input_enabled(pin, true);
    }

    pio_sm_set_consecutive_pindirs(pio, sm, PIN_DATA_BASE, PIN_DATA_COUNT, false);
    pio_sm_clear_fifos(pio, sm);
    pio_sm_init(pio, sm, offset, &config);
    pio->txf[sm] = 0xffu;
    pio_sm_set_enabled(pio, sm, true);
}

static void init_clock_pio(PIO pio, uint sm) {
    uint offset = pio_add_program(pio, &clock_8mhz_program);
    pio_sm_config config = clock_8mhz_program_get_default_config(offset);

    sm_config_set_set_pins(&config, PIN_CLK_OUT, 1);
    sm_config_set_clkdiv(&config, 1.0f);

    pio_gpio_init(pio, PIN_CLK_OUT);
    pio_sm_set_consecutive_pindirs(pio, sm, PIN_CLK_OUT, 1, true);
    pio_sm_init(pio, sm, offset, &config);
    pio_sm_set_enabled(pio, sm, true);
}

static void __attribute__((noinline, noreturn)) __not_in_flash_func(bus_service_loop)(void) {
    for (;;) {
        uint32_t pins;

        do {
            pins = sio_hw->gpio_in;
        } while ((pins & VALID_CYCLE_MASK) != VALID_CYCLE_MASK);

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
            } while (pins & E_MASK);
        } else {
            uint32_t data_pins;

            do {
                data_pins = pins;
                pins = sio_hw->gpio_in;
            } while (pins & E_MASK);

            uint8_t value = (uint8_t)(data_pins >> PIN_DATA_BASE);

            if (address < RAM_SIZE) {
                memory[address] = value;
            } else if (address == ACIA_DATA_ADDR) {
                acia_write_data(value);
            }
        }
    }
}

int main(void) {
    set_sys_clock_khz(SYS_CLOCK_KHZ, true);

    init_gpio();
    init_data_bus_pio(bus_pio, data_sm);
    init_clock_pio(bus_pio, 1);

    multicore_launch_core1(usb_core_entry);
    while (!usb_core_ready) {
        tight_loop_contents();
    }

    sleep_ms(10);

    uint32_t irq_state = save_and_disable_interrupts();
    (void)irq_state;

    gpio_put(PIN_RESET_N, true);
    bus_service_loop();
}
