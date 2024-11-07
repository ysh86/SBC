;;; -*- asm -*-
;;;
;;; Universal Monitor 6809 config file
;;;

;;;
;;; Memory
;;;

ROM_B:	equ	$F000		; ROM begin
ENTRY:	EQU	$FF80		; Entry point

WORK_B:	equ	$EF00		; WORK begin
STACK:	equ	$F000		; Stack end

BUFLEN:	EQU	16		; Buffer length ( 16 or above )

;;;
;;;  Options
;;;

USE_IDENT = 1			; CPU Identification
USE_REGCMD = 1			; Register related commands
USE_NATIVE = 1			; HD6309 NATIVE mode

USE_WARMUP = 0			; DRAM warming up

;;;
;;; Special initialize routine
;;;

USE_SPINIT = 0
	IF USE_SPINIT

SPINIT	MACRO
	ENDM

	ENDIF

;;;
;;; Motorola MC6850
;;;

USE_DEV_6850 = 1
	IF USE_DEV_6850
	;; ACIA
ACIAC:	equ	$E000		; Control / Status Register
ACIAD:	equ	$E001		; Data Register
ACCR_V:	EQU	$15		; Control: x16, 8-bit, N, 1
	ENDIF
