	page	0
	cpu	6502

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
CONOUT		equ	$6f90

;;
;; Zero page
;;
W	equ	$87		; W:7-0, W+1:15-8, W+2:23-16
				; temp ML/MH
				; const ML/MH
				; const NL/NH

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
	org	$6000

	ldy	#Y_MIN
FOR_Y:
	tya
	sta	W+7
	lda	#FP88_Y_STEP	; unsigned -> a reg
	jsr	MULUS8		; CB = FP88_Y_STEP(unsigned) * Y(signed)
	lda	W+1
	sta	CB
	lda	W+2
	sta	CB+1

	ldx	#X_MIN
FOR_X:
	lda	CB		; CB -> B
	sta	B
	lda	CB+1
	sta	B+1

	txa
	sta	W+7
	lda	#FP88_X_STEP	; unsigned -> a reg
	jsr	MULUS8		; CA = FP88_X_STEP * X, CA -> A
	lda	W+1
	sta	CA
	sta	A+0
	lda	W+2
	sta	CA+1
	sta	A+1

	jsr	CALC_SQDIFF

	lda	#$10
	sta	I
	lda	#' '
	sta	CHR
FOR_I:
	jsr	CALC_FOR_I
	clc			; T = AA + BB
	lda	AA
	adc	BB
	sta	T
	lda	AA+1
	adc	BB+1
	sta	T+1
	cmp	#FP88_FOUR	; if T > (4 << 8)
	bmi	NEXT_I
	bne	CHAR
	lda	T
	beq	NEXT_I
CHAR:
	lda	#$10
	sec
	sbc	I
	cmp	#10
	bmi	NUM		; i = 0,1,2,3,4,5,6,7,8,9
	clc			; i = a,b,c,d,e,f
	adc	#7
NUM:
	adc	#'0'
	sta	CHR
	bne	NEXT_X
NEXT_I:
	dec	I
	bne	FOR_I

NEXT_X:
	lda	CHR
	jsr	CONOUT
	inx
	txa
	cmp	#X_MAX
	bne	FOR_X

NEXT_Y:
	lda	#CR
	jsr	CONOUT
	lda	#LF
	jsr	CONOUT
	iny
	tya
	cmp	#Y_MAX
	beq	DONE
	jmp	FOR_Y

DONE:
	brk			; back to Monitor


CALC_SQDIFF:
	sec
	lda	A+0
	sbc	B
	sta	W+5
	lda	A+1
	sbc	B+1
	sta	W+6

	clc
	lda	A+0
	adc	B
	sta	W+7
	lda	A+1
	adc	B+1
	sta	W+8

	jsr	MUL88		; SQD = (a-b) * (a+b)
	lda	W+1
	sta	SQD
	lda	W+2
	sta	SQD+1

	rts


CALC_FOR_I:
	lda	A+0		; B = ((A * B) << 1) + CB
	sta	W+5
	lda	A+1
	sta	W+6
	lda	B
	sta	W+7
	lda	B+1
	sta	W+8
	jsr	MUL88
	asl	W+1
	rol	W+2
	clc
	lda	W+1
	adc	CB
	sta	B
	lda	W+2
	adc	CB+1
	sta	B+1

	clc			; A = SQD + CA, AA = A * A
	lda	SQD
	adc	CA
	sta	A+0
	sta	W+5
	sta	W+7
	lda	SQD+1
	adc	CA+1
	sta	A+1
	sta	W+6
	sta	W+8
	jsr	MUL88
	lda	W+1
	sta	AA
	lda	W+2
	sta	AA+1

	lda	B		; BB = B * B
	sta	W+5
	sta	W+7
	lda	B+1
	sta	W+6
	sta	W+8
	jsr	MUL88
	lda	W+1
	sta	BB
	lda	W+2
	sta	BB+1

	sec			; SQD = AA - BB
	lda	AA
	sbc	BB
	sta	SQD
	lda	AA+1
	sbc	BB+1
	sta	SQD+1

	rts


;;
;; utils
;;
	org	$6400

;
; unsigned 8bit x signed 8bit -> signed 16bit
;
; a reg x W+7 -> W+1,W+2
;
MULUS8:
	sta	W+5
	sta	W+3
	txa
	pha
	ldx	#8
	lda	#0
	sta	W+1
	sta	W+2
MLT82:
	asl	W+1		; prod << 1
	rol	W+2
	rol	W+3		; c,M = M << 1
	bcc	MLT8N
	clc
	lda	W+1		; prod += N
	adc	W+7
	sta	W+1
	lda	W+2
	adc	#0
	sta	W+2
MLT8N:
	dex
	bne	MLT82
; sign
	lda	W+7
	bpl	_m8ret
	sec
	lda	W+2
	sbc	W+5
	sta	W+2
_m8ret:
	pla
	tax
	rts

;
; signed 8.8 x signed 8.8 -> signed 8.8 (round down)
;
; M(W+5,6) x N(W+7,8) -> W+1,W+2
;
MUL88:
	txa
	pha

	lda	W+5
	sta	W+3
	lda	W+6
	sta	W+4
	ldx	#$10
	lda	#0
	sta	W
	sta	W+1
	sta	W+2
MLT2:
	asl	W		; prod << 1
	rol	W+1
	rol	W+2
	rol	W+3		; c,M = M << 1
	rol	W+4
	bcc	MLTN
	clc
	lda	W		; prod += N
	adc	W+7
	sta	W
	lda	W+1
	adc	W+8
	sta	W+1
	lda	W+2
	adc	#0
	sta	W+2
MLTN:
	dex
	bne	MLT2
; M sign
	lda	W+6
	bpl	_plM
	lda	W+2
	sec
	sbc	W+7
	sta	W+2
_plM:
	lda	W+8
	bpl	_rdown
	lda	W+2
	sec
	sbc	W+5
	sta	W+2
_rdown
	lda	W+2
	bpl	_mret
	clc
	lda	W
	adc	#255
	sta	W
	lda	W+1
	adc	#0
	sta	W+1
	lda	W+2
	adc	#0
	sta	W+2
_mret:
	pla
	tax
	rts
