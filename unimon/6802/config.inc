;;; -*- asm -*-
;;;
;;; Universal Monitor 6800 config file
;;;

;;;
;;; Memory
;;;

ROM_B:	equ	$F000		; ROM begin
ENTRY:	EQU	$FF80		; Entry point

WORK_B:	equ	$0FC0		; WORK area begin
STACK:	equ	$0FFF		; Stack end

BUFLEN:	EQU	16		; Buffer length ( 16 or above )

;;;
;;; Options
;;;

USE_IDENT = 1			; CPU Identification
USE_REGCMD = 1			; Register related commands

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

;;;
;;; Motorola MC6801/3 (Embedded UART)
;;;

USE_DEV_6801 = 0
	IF USE_DEV_6801
	;; 6801/6803 Internal
RMCR:	EQU	$0010		; Rate and Mode Control Register
RMCR_V:	EQU	$05		; CC[1:0]=01, SS[1:0]=01
TRCSR:	EQU	$0011		; Transmit/Receive Control and Status Register
RDR:	EQU	$0012		; Receive Data Register
TDR:	EQU	$0013		; Transmit Data Register
	ENDIF
