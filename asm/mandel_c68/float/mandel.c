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

int main()
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
