;;;
;;; FC Dumper
;;;   Copyright (C) 2026 Yoshiharu
;;;

; asl -L asm.asm

	CPU	6502

	INCLUDE	"../../unimon/65816/config.inc"

	INCLUDE "../../unimon/common.inc"


;;;
;;; RAM	area
;;;

	;;
	;; Work Area
	;;

	ORG	WORK_B

INBUF:	RMB	BUFLEN		; Line input buffer
DSADDR:	RMB	2		; Dump start address
DEADDR:	RMB	2		; Dump end address
DSTATE:	RMB	1		; Dump state
GADDR:	RMB	2		; Go address
SADDR:	RMB	2		; Set address
HEXMOD:	RMB	1		; HEX file mode
RECTYP:	RMB	1		; Record type
PSPEC:	RMB	1		; Processor spec.

	IF USE_REGCMD
REGA:	RMB	1		; Accumulator A
REGX:	RMB	1		; Index register X
REGY:	RMB	1		; Index register Y
REGSP:	RMB	1		; Stack pointer SP
REGPC:	RMB	2		; Program counter PC
REGPSR:	RMB	1		; Processor status register PSR

REGSIZ:	RMB	1		; Register size
	ENDIF

DMPPT:	RMB	2
CKSUM:	RMB	1		; Checksum
HITMP:	RMB	1		; Temporary (used in HEXIN)

PT0:	RMB	2		; Generic Pointer 0
PT1:	RMB	2		; Generic Pointer 1
CNT:	RMB	1		; Generic Counter

PG:	RMB	1		; page#
PGMAX:	EQU	8		; max page#


;;;
;;; sub routines
;;;

DUMP:		EQU	$06CA
PATCH0:		EQU	$0754
PATCH1:		EQU	$0762
WSTART:		EQU	$067F
PROMPT:		EQU	$0C2B
STROUT:		EQU	$0AE2
HEXOUT2:	EQU	$0AF0
CRLF:		EQU	$0B33

EXTROM:		EQU	$4800	; 1: default, 0: ext. ROM
M7TBL:		EQU	$FE11	; Solstice
;M7TBL:		EQU	$FFC7	; Battletoads

;;;
;;; main
;;;

	ORG	$0E00

	LDA	#0		; page#
	STA	PG

FCDB:
	LDA	#0		; ext. ROM
	STA	EXTROM
	LDA	#low(MYRET)
	STA	PATCH0+1
	STA	PATCH1+1
	LDA	#high(MYRET)
	STA	PATCH0+2
	STA	PATCH1+2

FCDMP:
	LDA	#low(PROMPT)	; print "] PG"
	STA	PT0
	LDA	#high(PROMPT)
	STA	PT0+1
	JSR	STROUT
	LDA	PG
	JSR	HEXOUT2
	JSR	CRLF

	LDY	#0		; INBUF "Dstart,end"
	LDA	#'D'
I0:
	STA	INBUF,Y
	INY
	LDA	WND,Y
	BNE	I0
	STA	INBUF,Y
	LDX	#0

	LDY	PG		; page# -> Y
	LDA	M7TBL,Y		; page# -> A
	PHA			; page# -> DBR
	;FCB	$18		; CLC
	;FCB 	$FB		; XCE
	FCB	$AB		; PLB (W65C02S: NOP)
	;FCB	$38		; SEC
	;FCB 	$FB		; XCE
	STA	M7TBL,Y		; page# -> reg

	JMP	DUMP
MYRET:
	LDA	PG
	CLC
	ADC	#1
	CMP	#PGMAX
	BEQ	FCDE

	STA	PG
	JMP	FCDMP

FCDE:
	LDA	#low(WSTART)
	STA	PATCH0+1
	STA	PATCH1+1
	LDA	#high(WSTART)
	STA	PATCH0+2
	STA	PATCH1+2
	LDA	#1		; default
	STA	EXTROM
	JMP	WSTART


WND:
	FCB $00,"8000,FFFF",$00

	END
