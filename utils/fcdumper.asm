;;;
;;; FC Dumper
;;;   Copyright (C) 2026 Yoshiharu
;;;

; asl -L asm.asm

	CPU	6502

TARGET:	EQU	"6502"

	INCLUDE	"../unimon/6502/config.inc"

	INCLUDE "../unimon/common.inc"


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

PG8KB:	RMB	1		; page 8KB: $00-PGMAX
PGMAX:	EQU	64		; 8KB x 64 -> Max 512KB


;;;
;;; sub routines
;;;

DUMP:		EQU	$F6CA
PATCH0:		EQU	$F754
PATCH1:		EQU	$F762
WSTART:		EQU	$F67F
PROMPT:		EQU	$FC2B
STROUT:		EQU	$FAE2
HEXOUT2:	EQU	$FAF0
CRLF:		EQU	$FB33

;;;
;;; main
;;;

	ORG	$FF00

	LDA	#0		; page#
	STA	PG8KB

FCDB:
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
	LDA	PG8KB
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

	LDA	#6		; MMC3 R6
	STA	$8000
	LDA	PG8KB		; page# -> MMC3 R6
	STA	$8001

	JMP	DUMP
MYRET:
	LDA	PG8KB
	CLC
	ADC	#1
	CMP	#PGMAX
	BEQ	FCDE

	STA	PG8KB
	JMP	FCDMP

FCDE:
	LDA	#low(WSTART)
	STA	PATCH0+1
	STA	PATCH1+1
	LDA	#high(WSTART)
	STA	PATCH0+2
	STA	PATCH1+2
	JMP	WSTART


WND:
	FCB $00,"8000,9FFF",$00

	END
