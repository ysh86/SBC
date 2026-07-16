package main

import (
	"fmt"
)

func Mandelbrot() {
	for y := -12.0; y <= 12.0; y += 1.0 {
		for x := -39.0; x <= 39.0; x += 1.0 {
			ca := x * 0.0458
			cb := y * 0.08333
			a := ca
			b := cb
			chr := " "
			for i := 0; i < 16; i++ {
				t := a*a - b*b + ca
				b = 2.0*a*b + cb
				a = t
				if a*a+b*b > 4 {
					if i > 9 {
						i = i + 7
					}
					chr = string(rune(48 + i))
					break
				}
			}
			fmt.Print(chr)
		}
		fmt.Println("")
	}
}

const (
	F = 50
)

func MandelbrotInt() {
	for y := -12; y <= 12; y++ {
		for x := -39; x <= 39; x++ {
			ca := x * 229 / 100
			cb := y * 416 / 100
			a := ca
			b := cb
			chr := " "
			for i := 0; i <= 15; i++ {
				q := b / F
				s := b - q*F
				t := (a*a-b*b)/F + ca
				b = 2*(a*q+a*s/F) + cb
				a = t
				p := a / F
				q = b / F
				if p*p+q*q > 4 {
					if i > 9 {
						i = i + 7
					}
					chr = string(rune(48 + i))
					//fmt.Printf("(%d,%d,%d,%d, %d),", a, p, b, q, p*p+q*q)
					break
				}
				/*
					if i == 15 {
						chr = ""
					}
					fmt.Printf("(%d,%d,%d,%d, %d),%s", a, p, b, q, p*p+q*q, chr)
				*/
			}
			fmt.Print(chr)
			//fmt.Println("")
		}
		fmt.Println("")
	}
}

const (
	FP88_X_STEP = 12 /* round(0.0458 * 256) */
	FP88_Y_STEP = 21 /* round(0.08333 * 256) */
)

func mul88(a int16, b int16) int16 {
	product := int32(a) * int32(b)
	if product < 0 {
		product += 255
	}

	ov := uint32(product) >> 21
	if ov != 0 && ov != 0x7ff {
		panic(fmt.Errorf("overflow: %08x", uint32(product)))
	}

	return int16(product >> 8)
}

func MandelbrotFixed88() {
	var x, y, i int16
	var ca, cb, a, b, t int16
	var square_diff, aa, bb int16

	for y = -12; y <= 12; y += 1 {
		for x = -39; x <= 39; x += 1 {
			ca = int16(int32(x) * FP88_X_STEP)
			cb = int16(int32(y) * FP88_Y_STEP)
			a = ca
			b = cb
			square_diff = mul88(a-b, a+b)
			chr := " "
			for i = 0; i < 16; i++ {
				t = square_diff + ca
				b = 2*mul88(a, b) + cb
				a = t
				aa = mul88(a, a)
				bb = mul88(b, b)
				square_diff = aa - bb
				if aa+bb > (4 << 8) {
					if i > 9 {
						i = i + 7
					}
					chr = string(rune(48 + i))
					break
				}
			}
			fmt.Print(chr)
		}
		fmt.Println("")
	}
}

func main() {
	MandelbrotFixed88()
}
