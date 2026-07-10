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

#define F         50
#define X_STEP    229)/100  /* /50 = 0.0458 */
#define Y_STEP    416)/100  /* /50 = 0.0832 */

int main()
{
    int8_t x, y, i;
    int16_t ca, cb, a, b, t;
    int16_t p, q, s;

    for (y = -12; y <= 12; ++y) {
        cb = (y * Y_STEP;
        for (x = -39; x <= 39; ) {
            ca = (x * X_STEP;
            a = ca;
            b = cb;
            for (i = 0; i <= 15; ++i) {
                q = b / F;
                s = b - q * F;
                t = ((a - b) * (a + b)) / F + ca;
                b = 2 * (a*q + a*s/F) + cb;
                a = t;
                p = a / F;
                q = b / F;
                if (p*p + q*q > 4) {
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
}
