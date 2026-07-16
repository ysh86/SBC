	page	0
	cpu	6800

W	equ	$0C		; B:23-16, W:15-8, W+1:7-0
F	equ	$2F		; counter

	org	$EA00

MUL88:
	psha
	pshb
	stx	W
	ldaa	W+1
	psha
	ldab	W
	pshb
	psha			; sign
	tsx
	ldaa	#$10
	staa	F
	clrb
	stab	W
	stab	W+1
; sign
	clra
	tst	$01,x
	bpl	_pl1
; neg1
	com	$01,x
	ldaa	$02,x
	coma
	adda	#1
	staa	$02,x
	ldaa	$01,x
	adca	#0
	staa	$01,x
	ldaa	#$ff
_pl1:
	staa	0,x
	tst	$03,x
	bpl	MLT2
	eora	#$ff
	staa	0,x
; neg3
	com	$03,x
	ldaa	$04,x
	coma
	adda	#1
	staa	$04,x
	ldaa	$03,x
	adca	#0
	staa	$03,x
MLT2:
	asl	W+1
	rol	W
	rolb
	rol	$02,x
	rol	$01,x
	bcc	MLTN
	ldaa	W+1
	adda	$04,x
	staa	W+1
	ldaa	W
	adca	$03,x
	staa	W
	adcb	#0
MLTN:
	dec	F
	bne	MLT2
	ldaa	W
	tst	0,x
	beq	_mret
; neg
	coma
	comb
	adda	#1
	adcb	#0
_mret:
	ins
	ins
	ins
	ins
	ins
	rts
