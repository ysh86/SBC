#include <stdint.h>

#define UART_BASE      0x0007FFF0
#define UART_STATUS    (*(volatile uint8_t *)(UART_BASE + 0))
#define UART_DATA      (*(volatile uint8_t *)(UART_BASE + 1))
#define UART_TX_READY  0x02

static inline void uart_putc(char c)
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


static inline void myputchar(int c)
{
    uart_putc(c);
}

int main(void)
{
    int x, y, i;
    float ca, cb, a, b, t;

    for (y = -12; y <= 12; ++y) {
        for (x = -39; x <= 39; ) {
            ca = x * 0.0458f;
            cb = y * 0.08333f;
            a = ca;
            b = cb;
            for (i = 0; i <= 15; ++i) {
                t = a*a - b*b + ca;
                b = 2 * a * b + cb;
                a = t;
                if ((a * a + b * b) > 4) {
                    if (i > 9)
                        i = i + 7;
                    myputchar(48+i);
                    goto x_next;
                }
            }
            myputchar(' ');
x_next:
            ++x;
        }
        myputchar('\n');
    }

    return y;
}
