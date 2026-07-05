#define SBC 1
#define PICO 1

#if SBC
#if PICO
#define CONOUT 0x7F45EUL
#else
#define CONOUT 0x7F46AUL
#endif
#define STACKSIZE 256

char stack[STACKSIZE];
char *stackpt = &stack[STACKSIZE];

static int dummy(int c)
{
    return c;
}

static void (*conout)() = (void (*)())CONOUT;

static void myputchar(int c)
{
    int d0 = dummy(c);
    conout();
}
#else
#include <stdio.h>

static void myputchar(int c)
{
    putchar(c);
}
#endif

#define int16_t short
#define int32_t int
#define uint32_t unsigned int

#define FP88_X_STEP    12  /* round(0.0458 * 256) */
#define FP88_Y_STEP    21  /* round(0.08333 * 256) */
#define FP88_TRUNCATE_TO_ZERO 1

static int16_t mul88(int16_t a, int16_t b)
{
    int32_t product;

    product = (int32_t)a * (int32_t)b;
#if FP88_TRUNCATE_TO_ZERO
    if (product < 0)
        product += 255;
#endif

    return (int16_t)(product >> 8);
}

int main()
{
    int16_t x, y, i;
    int16_t ca, cb, a, b, t, square_diff;
    int32_t aa, bb;

    for (y = -12; y <= 12; ++y) {
        for (x = -39; x <= 39; ) {
            ca = (int16_t)(x * FP88_X_STEP);
            cb = (int16_t)(y * FP88_Y_STEP);
            a = ca;
            b = cb;
            square_diff = mul88((int16_t)(a - b), (int16_t)(a + b));
            for (i = 0; i <= 15; ++i) {
                t = square_diff + ca;
                b = 2 * mul88(a,b) + cb;
                a = t;
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
