/* sizeof */
/*
    c += sizeof(long);  // 2
    c += sizeof(int);   // 2
    c += sizeof(short); // 1
    c += sizeof(char);  // 1
*/

#define int8_t  char
#define int16_t int

/* in crt0.s */
int putchar(/* int c */);
int16_t mul8p8(/* int16_t _4_S, int16_t _2_S_D */);
int16_t mulus8(/* int16_t Au8_Bs8 */);

#define FP88_X_STEP    12  /* round(0.0458 * 256) */
#define FP88_Y_STEP    21  /* round(0.08333 * 256) */

int main()
{
    int16_t x, y, i;
    int16_t ca, cb, a, b, t;
    int16_t square_diff, aa, bb;

    for (y = -12; y <= 12; ++y) {
        cb = mulus8((FP88_Y_STEP << 8) | ((unsigned)y&0xff));
        for (x = -39; x <= 39; ) {
            ca = mulus8((FP88_X_STEP << 8) | ((unsigned)x&0xff));
            a = ca;
            b = cb;
            square_diff = mul8p8((a - b), (a + b));
            for (i = 0; i <= 15; ++i) {
                t = square_diff + ca;
                b = 2 * mul8p8(a,b) + cb;
                a = t;
                aa = mul8p8(a,a);
                bb = mul8p8(b,b);
                square_diff = aa - bb;
                if (aa + bb > (4 << 8)) {
                    if (i > 9)
                        i = i + 7;
                    putchar(48+i);
                    goto x_next;
                }
            }
            putchar(' ');
x_next:
            ++x;
        }
        putchar('\n');
    }

    return y;
/*
    int x, y, d;

    x = -0x10;
    y = 0xab0;
    d = x * y;

    return d;
*/
}
