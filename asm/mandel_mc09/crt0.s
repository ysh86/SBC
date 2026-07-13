*
* micro-C driver
*

MEMEND	EQU	$DFFF
CRLF	EQU	$F6DB		Universal monitor

	ORG	$100

_00000
	LDX	#MEMEND
	LEAS	1,X		STACK S

	JSR	CRLF

	LEAS	-256,S		ALLOCATE WORK AREA
	LEAU	128,S		POINT TO CONTENT OF ARGUMENT VECTOR

	LEAY	_99999,PCR	clear globals
	LDX	#_GLOBALS
_0C002	BEQ	_0C003
	CLR	,Y+
	LEAX	-1,X
	BRA	_0C002

_0C003	LEAY	_99999,PCR
	LBSR	_INITIALIZE	call initializer
	LBSR	main
	SWI			return to mon
exit	BRA	exit


*
* run time support
*

*
* putchar
*
UARTST	EQU	$E000
UARTDT	EQU	$E001
UREADY	EQU	$02

putchar
_pc3
	LDA	UARTST
	ANDA	#UREADY
	BEQ	_pc3
	STB	UARTDT
	RTS

*
* Signed fixed point 8.8x8.8 -> 8.8
*
* Input:
*   4,S multiplicand
*   2,S multiplier or D (depends on compilers)
*
* Output:
*   D = product >> 8 (round down)
*
* Notes:
*   HD63C09 only
*
mul8p8
*	LDD	4,S
*	FCB	$11,$AF,$62	* MULD 2,S -> A,B,E,F
	FCB	$11,$AF,$64	* MULD 4,S -> A,B,E,F
	BPL	_mRetBE
	FCB	$10,$8B,$00,$FF	* ADDW #255 W = W + 255
*	FCB	$10,$89,$00,$00	* ADCD #0   D = D + 0 + C
	ADCB	#0
_mRetBE	TFR	B,A
	FCB	$1F,$E9		* TFR E,B
	RTS

*
* unsigned 8bit x signed 8bit -> signed 16bit
*
* input:  unsigned A, signed B
* output: D (signed 16bit product)
*
mulus8
	TSTB
	BMI	_BNEG		_NEGD for HD63C09
        MUL
        RTS
_BNEG
*	NEGB
*	MUL
*	COMA
*	COMB
*	ADDD	#1
*	RTS
	MUL
	SUBA	2,S
	RTS
_NEGD
        NEGB
        MUL
	FCB	$10,$40		NEGD
        RTS

*
_00001
mul16	PSHS	D,X,Y		multiply: both signed/unsigned 16x16 -> 16
	
	LDA	,S
	LDB	3,S
	MUL
	STB	4,S
	
	LDD	1,S
	MUL
	STB	5,S
	
	LDA	1,S
	LDB	3,S
	MUL
	ADDA	4,S
	ADDA	5,S
	
	LEAS	6,S
	RTS
*
_00002	CLR	,-S		signed divide
	
	CMPX	#0
	BPL	_02000
	
	COM	,S
	
	EXG	D,X
	LBSR	_00020
	EXG	D,X

_02000	TSTA
	BPL	_02001
	
	COM	,S
	
	LBSR	_00020
	
_02001	LBSR	_00010
	TFR	X,D
	TST	,S+
	BPL	_02002
	
	LBSR	_00020
	
_02002	RTS
*
_00003	LBSR	_00010		unsigned divide
	TFR	X,D
	RTS
*
_00004	CLR	,-S		signed modulous
	
	CMPX	#0
	BPL	_04000
	
	EXG	D,X
	BSR	_00020
	EXG	D,X

_04000	TSTA
	BPL	_04001
	
	COM	,S
	BSR	_00020
	
_04001	BSR	_00010
	
	TST	,S+
	BPL	_04002
	
	BSR	_00020
	
_04002	RTS
*
_00005	BSR	_00010		unsigned modulous

	RTS
*
_00006	CMPX	#0		signed left shift
	BMI	_06001
 
_06000	BEQ	_06009
	LSLB
	ROLA
	LEAX	-1,X
	BRA	_06000
	
_06001	BEQ	_06009
	ASRA
	RORB
	LEAX	1,X
	BRA	_06001
	
_06009	RTS
*
_00007	CMPX	#0		unsined left shift
	BMI	_07001
	
_07000	BEQ	_07009
	LSLB
	ROLA
	LEAX	-1,X
	BRA	_07000
	
_07001	BEQ	_07009
	LSRA
	RORB
	LEAX	1,X
	BRA	_07001
	
_07009	RTS
*
_00008	CMPX	#0		sined right shift
	BMI	_08001
	
_08000	BEQ	_08009
	ASRA
	RORB
	LEAX	-1,X
	BRA	_08000
	
_08001	BEQ	_08009
	LSLB
	ROLA
	LEAX	1,X
	BRA	_08001
	
_08009	RTS
*
_00009	CMPX	#0		unsined right shift
	BMI	_09001
	
_09000	BEQ	_09009
	LSRA
	RORB
	LEAX	-1,X
	BRA	_09000
	
_09001	BEQ	_09009
	LSLB
	ROLA
	LEAX	1,X
	BRA	_09001
	
_09009	RTS
*
_00020	NEGA			negate D reg
	NEGB
	SBCA	#0
	RTS
*
_00010	PSHS	D,X		divide subroutine
	
	CLRA
	CLRB
	
	LDX	#17
	
_00011	SUBD	2,S
	BCC	_00012
	
	ADDD	2,S
	
_00012	ROL	1,S
	ROL	,S
	ROLB
	ROLA
	
	LEAX	-1,X
	BNE	_00011
	
	RORA
	RORB
	
	COM	1,S
	COM	,S
	PULS	X
	
	LEAS	2,S
	RTS
*
* micro-C user program
*
*	OPT	LIST
	LIB	c.out		include compilers output
*	OPT	NOL
*
*
*
_99999	EQU	*		global vars allocated here
*
	END	_00000

