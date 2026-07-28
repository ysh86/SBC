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

#define FP88_X_STEP    12  /* round(0.0458 * 256) */
#define FP88_Y_STEP    21  /* round(0.08333 * 256) */

static inline int16_t mul88(int16_t a, int16_t b)
{
    int32_t product;

    product = (int32_t)a * (int32_t)b;
    if (product < 0)
        product += 255;

    return (int16_t)(product >> 8);
}

int main(void)
{
    int16_t x, y, i;
    int16_t ca, cb, a, b, square_diff;
    int32_t aa, bb;

    for (y = -12; y <= 12; ++y) {
        cb = (int16_t)(y * FP88_Y_STEP);
        for (x = -39; x <= 39; ) {
            ca = (int16_t)(x * FP88_X_STEP);
            a = ca;
            b = cb;
            square_diff = mul88((int16_t)(a - b), (int16_t)(a + b));
            for (i = 0; i < 16; ++i) {
                b = (mul88(a,b)<<1) + cb;
                a = square_diff + ca;
                aa = a * a;
                bb = b * b;
                square_diff = (int16_t)((aa - bb) >> 8);
                if ((uint32_t)aa + (uint32_t)bb > (4u << 16)) {
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
