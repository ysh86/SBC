	page	0
	cpu	6800

W	equ	$0A		; (B:23-16,) W:15-8, W+1:7-0, XHXL, const BA
F	equ	$2F		; counter

	org	$EA00

MUL88:
	staa	W+5
	stab	W+4
	stx	W+2
	ldaa	W+3
	psha			; XL -> 1,x
	ldab	W+2
	pshb			; XH -> 0,x
	tsx
	ldaa	#$10
	staa	F
	clrb
	stab	W
	stab	W+1
MLT2:
	asl	W+1		; prod << 1
	rol	W
	rolb
	rol	W+3		; c,X = X << 1
	rol	W+2
	bcc	MLTN
	ldaa	W+1		; B,prod += BA
	adda	W+5
	staa	W+1
	ldaa	W
	adca	W+4
	staa	W
	adcb	#0
MLTN:
	dec	F
	bne	MLT2
	ldaa	W
; B sign
	tst	0,x
	bpl	_plX
	subb	W+5
_plX:
	tst	W+4
	bpl	_sign
	subb	1,x
_sign
	tstb
	bpl	_mret
	ldaa	W+1
	adda	#255
	ldaa	W
	adca	#0
	adcb	#0
_mret:
	ins
	ins
	rts
