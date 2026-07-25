	page	0
	cpu	6800

;;
;; const
;;
FP88_X_STEP	equ	12	; /* round(0.0458 * 256) */
FP88_Y_STEP	equ	21	; /* round(0.08333 * 256) */
FP88_FOUR	equ	4

X_MIN		equ	-39
Y_MIN		equ	-12
X_MAX		equ	40
Y_MAX		equ	13

CR		equ	$0D
LF		equ	$0A
CONOUT		equ	$f839

;;
;; Zero page
;;
W	equ	$0A		; (B:23-16,) W:15-8, W+1:7-0
WT	equ	$0C		; temp MHML
WN	equ	$0E		; const NHNL
F	equ	$2F		; counter

CA	equ	$90
CB	equ	$92
A	equ	$94
B	equ	$96
T	equ	$98
SQD	equ	$9A
AA	equ	$9C
BB	equ	$9E
I	equ	$A0
CHR	equ	$A1

;;
;; main
;;
	org	$E100

	ldab	#Y_MIN
FOR_Y:
	stab	WN
	ldaa	#FP88_Y_STEP
	staa	WT+1
	pshb
	jsr	MULUS8a		; CB = FP88_Y_STEP(unsigned) * Y(signed)
	stab	CB
	staa	CB+1
	pulb

	ldx	#X_MIN
FOR_X:
	ldaa	CB		; CB -> B
	staa	B+0
	ldaa	CB+1
	staa	B+1

	stx	WN
	ldaa	WN+1
	staa	WN
	ldaa	#FP88_X_STEP
	staa	WT+1
	pshb
	jsr	MULUS8a		; CA = FP88_X_STEP * X, CA -> A
	stab	CA
	stab	A+0
	staa	CA+1
	staa	A+1
	pulb

	jsr	CALC_SQDIFF

	ldaa	#$10
	staa	I
	ldaa	#' '
	staa	CHR
FOR_I:
	jsr	CALC_FOR_I
	ldaa	AA+1		; T = AA + BB
	adda	BB+1
	staa	T+1
	ldaa	AA
	adca	BB
	staa	T
	cmpa	#FP88_FOUR	; if T > (4 << 8)
	blt	NEXT_I
	bne	CHAR
	ldaa	T+1
	beq	NEXT_I
CHAR:
	ldaa	#$10
	suba	I
	cmpa	#10
	blt	NUM		; i = 0,1,2,3,4,5,6,7,8,9
	adda	#7		; i = a,b,c,d,e,f
NUM:
	adda	#'0'
	staa	CHR
	bra	NEXT_X
NEXT_I:
	dec	I
	bne	FOR_I

NEXT_X:
	ldaa	CHR
	jsr	CONOUT
	inx
	cpx	#X_MAX
	bne	FOR_X

NEXT_Y:
	ldaa	#CR
	jsr	CONOUT
	ldaa	#LF
	jsr	CONOUT
	incb
	cmpb	#Y_MAX
	beq	DONE
	jmp	FOR_Y

DONE:
	swi			; back to Monitor


CALC_SQDIFF:
	ldaa	A+1
	suba	B+1
	staa	WT+1
	ldaa	A+0
	sbca	B+0
	staa	WT

	ldaa	A+1
	adda	B+1
	staa	WN+1
	ldaa	A+0
	adca	B+0
	staa	WN

	stx	T
	pshb
	jsr	MUL88a		; SQD = (a-b) * (a+b)
	stab	SQD
	staa	SQD+1
	pulb
	ldx	T

	rts


CALC_FOR_I:
	stx	T
	pshb

	ldaa	A+1		; B = ((A * B) << 1) + CB
	staa	WT+1
	ldaa	A+0
	staa	WT
	ldaa	B+1
	staa	WN+1
	ldaa	B+0
	staa	WN
	jsr	MUL88a
	asla
	rolb
	adda	CB+1
	adcb	CB
	staa	B+1
	stab	B+0

	ldaa	SQD+1		; A = SQD + CA, AA = A * A
	adda	CA+1
	staa	A+1
	staa	WT+1
	staa	WN+1
	ldaa	SQD
	adca	CA
	staa	A+0
	staa	WT
	staa	WN
	jsr	MUL88a
	staa	AA+1
	stab	AA

	ldaa	B+1		; BB = B * B
	staa	WT+1
	staa	WN+1
	ldaa	B+0
	staa	WT
	staa	WN
	jsr	MUL88a
	staa	BB+1
	stab	BB

	ldaa	AA+1		; SQD = AA - BB
	suba	BB+1
	staa	SQD+1
	ldaa	AA
	sbca	BB
	staa	SQD

	pulb
	ldx	T
	rts


;;
;; utils
;;

	org	$EA00

;
; unsigned 8bit x signed 8bit -> signed 16bit
;
; M(XL) x N(A) -> BA
;
MULUS8:
	staa	WN
	stx	WT
MULUS8a:
	ldaa	WT+1
	sta	WT		; MLML -> WT
	ldaa	#8
	staa	F
	clrb
	stab	W
MLT82:
	asl	W		; prod << 1
	rolb
	rol	WT		; c,M = M << 1
	bcc	MLT8N
	ldaa	W		; B,prod += N
	adda	WN
	staa	W
	adcb	#0
MLT8N:
	dec	F
	bne	MLT82
	ldaa	W
; sign
	tst	WN
	bpl	_m8ret
	subb	WT+1
_m8ret:
	rts

;
; signed 8.8 x signed 8.8 -> signed 8.8 (round down)
;
; M(X) x N(BA) -> BA
;
MUL88:
	staa	WN+1
	stab	WN
	stx	WT
MUL88a:
	ldaa	WT+1
	psha			; ML -> 1,x
	ldab	WT
	pshb			; MH -> 0,x
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
	rol	WT+1		; c,M = M << 1
	rol	WT
	bcc	MLTN
	ldaa	W+1		; B,prod += N
	adda	WN+1
	staa	W+1
	ldaa	W
	adca	WN
	staa	W
	adcb	#0
MLTN:
	dec	F
	bne	MLT2
	ldaa	W
; M sign
	tst	0,x
	bpl	_plM
	subb	WN+1
_plM:
	tst	WN
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
