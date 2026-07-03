;;;
;;; 	EMILY Board Console Driver -> MEZ UART Console Driver
;;;

INIT:
	;; init table
	LEA	SMBASE,A0
	MOVE	#(req_e-SMBASE)-1,D0
shad_0:
	CLR.B	(A0)+
	DBF	D0,shad_0

	RTS

CONIN:
	move.b	#REQ_CONIN, CREQ_COM	; set command

exec:
	move.b	PIC_REQ, d0		; kick PIC
wait:
	move.b	CREQ_COM, d0		; wait
	or.b	d0, d0
	bne	wait

	move.b	CBI_CHR, d0		; result -> D0
	rts

CONST:
	move.b	#REQ_CONST, CREQ_COM	; set command
	bsr	exec			; result -> D0
	and.b	#$01,d0			; check status
	rts

CONOUT:
	move.b	#REQ_CONOUT, CREQ_COM	; set command
	move.b	d0, CBI_CHR		; D0 -> output
	bra	exec


;;;
;;; PIC command request table
;;;

	ORG	SMBASE
				; offset:
UREQ_COM:	DS.B	1	; 0: monitor CONIN/CONOUT request command
UNI_CHR:	DS.B	1	; 1: one charcter (CONIN/CONOUT) or length of string
STR_addr:	DS.L	1	; 2: string address
;--------------------------------------------------------------------------------
CREQ_COM:	DS.B	1	; 6: PIC function CONIN/CONOUT request command
CBI_CHR:	DS.B	1	; 7: charcter or return status
disk_drive:	DS.B	1	; 8
disk_track:	DS.B	1	; 9
disk_sector:	DS.W	1	; A
data_dma:	DS.L	1	; C
req_e:

;;;
;;; xREQ_COM: request command to PIC
;;;

REQ_CONIN:	equ	1	; return char in UNI_CHR
REQ_CONOUT	equ	2	; UNI_CHR = output char
REQ_CONST:	equ	3	; return status in UNI_CHR ( 0: no key, 1 : key exist )
REQ_STROUT	equ	4	; string address
;			0	; request is done by PIC
PIC_REQ:	equ	$80000	; kick PIC
