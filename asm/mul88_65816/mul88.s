	page	0
	cpu	65816

;;
;; const
;;
FP88_X_STEP	equ	12	; /* round(0.0458 * 256) */
FP88_Y_STEP	equ	21	; /* round(0.08333 * 256) */
FP88_FOUR	equ	(4<<8)+1

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
W	equ	$86		; W:7-0, W+1:15-8, W+2:23-16, W+3:31-24
WT	equ	$8a		; temp ML/MH
WM	equ	$8c		; const ML/MH
WN	equ	$8e		; const NL/NH

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

	sep	#$10
	rep	#$20
	ASSUME  M:0, X:1

	ldy	#Y_MIN
FOR_Y:
	stz	WN
	sty	WN
	lda	#FP88_Y_STEP	; unsigned -> a reg
	jsr	MULUS8		; CB = FP88_Y_STEP(unsigned) * Y(signed)
	lda	W+1
	sta	CB

	ldx	#X_MIN
FOR_X:
	lda	CB		; CB -> B
	sta	B+0

	stz	WN
	stx	WN
	lda	#FP88_X_STEP	; unsigned -> a reg
	jsr	MULUS8		; CA = FP88_X_STEP * X, CA -> A
	lda	W+1
	sta	CA
	sta	A+0

	jsr	CALC_SQDIFF

	lda	#$2010		; L:#$10, H:#' '
	sta	I		; I=L, CHR=H
FOR_I:
	jsr	CALC_FOR_I
	clc			; T = AA + BB
	lda	AA
	adc	BB
	sta	T
	cmp	#FP88_FOUR	; if T > (4 << 8)
	sep	#$20		; 8bit
	ASSUME  M:1, X:1
	bmi	NEXT_I
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
	rep	#$20		; 16bit
	bne	FOR_I

NEXT_X:
	sep	#$20		; 8bit
	lda	CHR
	jsr	CONOUT
	inx
	cpx	#X_MAX
	rep	#$20		; 16bit
	bne	FOR_X

NEXT_Y:
	sep	#$20		; 8bit
	lda	#CR
	jsr	CONOUT
	lda	#LF
	jsr	CONOUT
	iny
	cpy	#Y_MAX
	beq	DONE
	rep	#$20		; 16bit
	bra	FOR_Y

DONE:
	brk			; back to Monitor


	ASSUME  M:0, X:1


CALC_SQDIFF:
	sec
	lda	A+0
	sbc	B+0
	sta	WM

	clc
	lda	A+0
	adc	B+0
	sta	WN

	jsr	MUL88		; SQD = (a-b) * (a+b)
	lda	W+1
	sta	SQD

	rts


CALC_FOR_I:
	clc			; T = SQD + CA
	lda	SQD
	adc	CA
	sta	T

	lda	A+0		; B = ((A * B) << 1) + CB
	sta	WM
	lda	B+0
	sta	WN
	jsr	MUL88
	asl	W+1
	clc
	lda	W+1
	adc	CB
	sta	B+0

	lda	T		; A = T, AA = A * A
	sta	A+0
	sta	WM
	sta	WN
	jsr	MUL88
	lda	W+1
	sta	AA

	lda	B+0		; BB = B * B
	sta	WM
	sta	WN
	jsr	MUL88
	lda	W+1
	sta	BB

	sec			; SQD = AA - BB
	lda	AA
	sbc	BB
	sta	SQD

	rts


;;
;; utils
;;
	org	$6400

;
; unsigned 8bit x signed 8bit -> signed 16bit
;
; a reg x WN -> W+1,W+2
;
MULUS8:
	phx

	sta	WM
	xba
	sta	WT
	ldx	#8
	lda	#0
	sta	W+1
MLT82:
	asl	W+1		; prod << 1
	rol	WT		; c,M = M << 1
	bcc	MLT8N
	clc
	lda	W+1		; prod += N
	adc	WN
	sta	W+1
MLT8N:
	dex
	bne	MLT82
; sign
	ldx	WN
	bpl	_m8ret
	lda	WM
	xba
	sta	WT
	sec
	lda	W+1
	sbc	WT
	sta	W+1
_m8ret:
	plx
	rts

;
; signed 8.8 x signed 8.8 -> signed 8.8 (round down)
;
; WM x WN -> W+1,W+2
;
MUL88:
	phx

	lda	WM
	sta	WT
	ldx	#$10
	lda	#0
	sta	W
	sta	W+2
MLT2:
	asl	W		; prod << 1
	rol	W+2
	rol	WT		; c,M = M << 1
	bcc	MLTN
	clc
	lda	W		; prod += N
	adc	WN
	sta	W
	lda	W+2
	adc	#0
	sta	W+2
MLTN:
	dex
	bne	MLT2
; M sign
	lda	WM
	bpl	_plM
	stz	WT
	ldx	WN
	stx	WT+1
	sec
	lda	W+1
	sbc	WT
	sta	W+1
_plM:
	lda	WN
	bpl	_rdown
	stz	WT
	ldx	WM
	stx	WT+1
	sec
	lda	W+1
	sbc	WT
	sta	W+1
_rdown
	lda	W+1
	bpl	_mret
	clc
	lda	W
	adc	#255
	sta	W
	lda	W+2
	adc	#0
	sta	W+2
_mret:
	plx
	rts
