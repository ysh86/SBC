#include <stdint.h>

#define UART_BASE      0x0007FFF0
#define UART_STATUS    (*(volatile uint8_t *)(UART_BASE + 0))
#define UART_DATA      (*(volatile uint8_t *)(UART_BASE + 1))
#define UART_TX_READY  0x02

static void uart_putc(char c)
{
    while ((UART_STATUS & UART_TX_READY) == 0)
        ;

    UART_DATA = (uint8_t)c;
}

static void uart_puts(const char *s)
{
    while (*s)
        uart_putc(*s++);
}

static const char message[] = "Hello, user mode world!\r\n";

int main(void)
{
    uart_puts(message);
    return sizeof(message);
}
