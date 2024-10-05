	page    0
	cpu     z80

	org 07000h

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
vram_temp:
	ds	256

; GVRAM0/1/2 を 00 で埋める = 真っ黒にする
sub_78fah:
	xor a
	ld hl,vram_temp
	ld (hl),a
	ld de,vram_temp+1
	ld bc,0ffh
	ldir			; clear vram_temp
	ld bc,0ff40h		; dst
clear256:
	ld hl,vram_temp		; src
	otir
	outi
	inc c
	jp nz,clear256
	ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
l79f6h:				; src head アドレス
	dw	pict_start
l79f8h:				; offset x [char] ;;;; 30
	db	00h
l79f9h:				; offset y [line] ;;;; 48
	db	00h
l79fah: 			; カウンタ？ -> フラグだな 0 or ffh を外から入れる？
	db	00h
l79fbh:				; ヘッダの最初のバイト
	db	00h
planeBRGM:
	db	00h		; 5Ch + 0:B, 1:R, 2:G, 3:main RAM

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; draw
sub_79fch:
	nop			; debug?
l79fdh:
	ld a,077h		; 自己書換用コード 077h ld (hl),a or nop
	ld (l7bd7h),a
	ld (l7bdfh),a
	ld (l7be7h),a
	ld ix,(l79f6h)		; ix=src head
	; -------- 1st plane --------
	ld a,(ix+003h)		;;;; a=12h
	ld b,05ch		; 青
	and 003h		;;;; a&3 -> 2
	add a,b			;;;; 5e=緑 G
	ld (l7bd6h),a		; 自己書換 GVRAM 青 バンク選択
	ld (sw7b2ah+1),a
	ld (sw7d22h+1),a
	ld (sw7d80h+1),a
	ld (sw7dc2h+1),a
	ld (sw7de9h+1),a
	ld (sw7e73h+1),a
	ld (sw7ed9h+1),a
	ld (sw7f14h+1),a
	; -------- 2nd plane --------
	ld a,(ix+003h)		;;;; a=12h
	rrca
	rrca
	and 003h		;;;; (a>>2)&3 -> 0
	add a,b			;;;; 5c=青 B
	ld (l7bdeh),a		; 自己書換 GVRAM 赤 バンク選択
	ld (sw7b35h+1),a
	ld (sw7e28h+1),a
	ld (sw7e8dh+1),a
	ld (sw7ec3h+1),a
	ld (sw7f03h+1),a
	; -------- 3rd plane --------
	ld a,(ix+003h)		;;;; a=12h
	rrca
	rrca
	rrca
	rrca
	and 003h		;;;; (a>>4)&3 -> 1
	add a,b			;;;; 5d=赤 R
	ld (l7be6h),a		; 自己書換 GVRAM 緑 バンク選択
	ld (sw7b40h+1),a
	;
	push ix
	pop hl			; HL=src head
	ld de,00008h		; ヘッダの長さ
	ld bc,sub_l7bb4h	; サブルーチン選択
	ld a,(l79fah)		; カウンタ ;;;; ここが ff の場合はいつ？
	inc a
	jr z,l7a6bh
	ld bc,l7b15h		; サブルーチン選択 カウンタ+1 0: bc=7bb4h, 1-255: bc=7b15h
l7a6bh:
	add hl,de		; ヘッダ分飛ばして HL=src
	ld (l7c2bh),hl		; 自己書換 src
	ld (sw7b1dh+1),hl	; 自己書換 src
	ld (sw7b12h+1),bc	; 自己書換 飛び先: カウンタ 0:7bb4, 1-255:7b15
	push ix
	pop de			; de=src head
	ld l,(ix+004h)		; offset0 8 固定では？
	ld h,(ix+005h)		; offset0 ;;;; HL=8
	add hl,de		; HL=src head+8 == src0
	ld (l7cf8h),hl		; 自己書換 src0
	ld l,(ix+006h)		; offset1
	ld h,(ix+007h)		; offset1 ;;;; 418
	add hl,de		; HL=src head+418 == src1
	ld (l7df9h),hl		; 自己書換 src1
	ld hl,00000h		; 自己書換 現在位置?
	ld (l7c2eh),hl		; 青？
	ld (l7cfbh),hl		; 赤？
	ld (l7dfch),hl		; 緑？
	; -------- 青 --------
	ld a,(ix+000h)		; 1色につき 2bit のフラグ ??ggrrbb
	rrca			;;;; (a=0000_1100b)>>1 = 0000_0110b c=0
	jr c,l7aaah		; flag 01 or 11
	bit 0,a			;;;; z=0
	jr z,l7aa9h		; flag 00 ;;;; jump
	push af			; flag 10
	xor a
	ld (l7bd7h),a		; 自己書換 nop
	pop af
l7aa9h:				; flag 00 or 10
	dec hl			;;;; hl=ffffh
l7aaah:				; flag が何でもここは通る
	ld (l7c31h),hl		; 自己書換 値? ;;;; 0000h or ffffh
	ld hl,l7c94h
	rrca			;;;; (a=0000_0110b)>>1 = 0000_0011b c=0
	jr nc,l7ab6h		; flag 0? の場合 ;;;; jump
	ld hl,l7c7fh		; flag 1? の場合
l7ab6h:
	ld (sw7c7ch+1),hl	; 自己書換 飛び先 ;;;; hl=7c94h
	; -------- 赤 --------
	ld hl,00000h
	rrca			; 1色につき 2bit のフラグ ??ggrrbb
	jr c,l7acah		;;;; 11 jump
	bit 0,a
	jr z,l7ac9h
	push af
	xor a
	ld (l7bdfh),a		; 自己書換 nop
	pop af
l7ac9h:
	dec hl
l7acah:
	ld (l7cfeh),hl		; 自己書換 値? ;;;; 0
	ld hl,l7d68h
	rrca
	jr nc,l7ad6h		;;; 11 through
	ld hl,l7d53h		; フラグ見て切り替え
l7ad6h:
	ld (sw7d50h+1),hl	; 自己書換 飛び先 ;;;; hl=7d53h
	; -------- 緑 --------
	ld hl,00000h
	rrca			; 1色につき 2bit のフラグ ??ggrrbb
	jr c,l7aeah
	bit 0,a
	jr z,l7ae9h		;;;; 00 jump
	push af
	xor a
	ld (l7be7h),a		; 自己書換 77 or nop
	pop af
l7ae9h:
	dec hl			;;;; hl=ffffh
l7aeah:
	ld (l7dffh),hl		; 自己書換 値? ;;;; ffffh
	ld hl,l7e5bh
	rrca
	jr nc,l7af6h		;;;; 00 jump
	ld hl,l7e46h
l7af6h:
	ld (sw7e43h+1),hl	; 自己書換 飛び先 ;;;; hl=7e5bh
	; -------- 3 枚終わって、 --------
	xor a			; 自己書換 0
	ld (l79fah),a		; カウンタ 0 リセット
	ld (sw7c4eh+1),a	; 色ごとのフラグ
	ld (sw7d1bh+1),a	; 色ごとのフラグ
	ld (sw7e1ch+1),a	; 色ごとのフラグ
	call sub_7f2eh		; calc VRAM base addr(x:79f8h,y:79f9h) {hl = 0x4000 + (80 * y + x)}
	ld de,00050h		; stride 80[bytes]
	ld c,(ix+001h)		; ヘッダ: width  ;;;; 0f=15 char = 120 pix
	ld b,(ix+002h)		; ヘッダ: height ;;;; 44=68 line
sw7b12h:
	jp sub_l7bb4h		; 自己書換 飛び先 カウンタ 0:7bb4, 1-255:7b15 ;;;; 7b15


l7b15h:				; 1周回ってない時？
	ld (sw7b69h+1),hl	; 自己書換 VRAM base addr
	ld (sw7b6ch+1),bc	; 自己書換 H,W
	exx			; src に切り替え
sw7b1dh:
	ld hl,00000h		; 自己書換 中身へのポインタ HL src
	ld bc,00000h		; src 側クリア
	exx			; dst にもどる
	ld a,(ix+000h)
	ld (l79fbh),a		; ヘッダの最初のバイト
	; -------- 1st --------
sw7b2ah:
	ld c,05ch		; 自己書換 GVRAM 1st plane バンク選択 ;;;; 5e 緑プレーン
	ld hl,sw7c4eh+1
	ld de,sub_l7c43h
	call sub_7b4fh		;;;; 黒塗りつぶし
	; -------- 2nd --------
sw7b35h:
	ld c,05dh		; 自己書換 GVRAM 2nd plane バンク選択 ;;;; 5c 青プレーン
	ld hl,sw7d1bh+1
	ld de,sub_l7d10h
	call sub_7b4fh		;;;; 塗(c:5d, hl:7d1c, de:7d10)
	; -------- 3rd --------
sw7b40h:
	ld c,05eh		; 自己書換 GVRAM 3rd plane バンク選択 ;;;; 5d 赤プレーン
	ld hl,sw7e1ch+1
	ld de,sub_l7e11h
	call sub_7b4fh		;;;; 黒塗りつぶし
	;
	ld a,05fh
	ld (planeBRGM),a	; メインメモリに戻す
l7b4dh:
	nop
	ret			; --------------------- 描画終わり


	; 塗り (c:vram 選択, hl:フラグ保存場所, de:色ごとの描画ルーチン)
sub_7b4fh:			;;;; 塗(c:5c, hl:7d1c, de:7d10)
	ld a,c
	ld (planeBRGM),a	; VRAM 切り替え
	ld a,(l79fbh)		; ヘッダの最初のバイト
	rrca			;;;; a=0000_0011b -> c=1
	jp nc,sub_l7f4bh	; LSB 0 だったら 黒で塗りつぶして ret までする ;;;; 飛ぶ a=0000_0110b
	rrca
	ld (l79fbh),a		; LSB 2bit 削ったものを保存
	ld (sw7b73h+1),hl	; 自己書換 列フラグのアドレス
	ld (sw7b90h+1),de	; 自己書換 色ごとの描画ルーチンへのアドレス
sw7b69h:
	ld hl,00000h		; 自己書換 HL=VRAM base addr
sw7b6ch:
	ld bc,00000h		; 自己書換 BC=HW
	ld de,00050h		; DE=80
	call sub_plane		; h=VRAM high addr
	ld a,05fh
	ld (planeBRGM),a	; メインメモリに戻す
l7b72h:
	push bc			; HW 保存
sw7b73h:
	ld a,(sw7c4eh+1)	; 自己書換 ;;;; (hl=7d1c) 列フラグ
	and 00fh
	jr z,l7b8fh		; 0 だったら ;;;; 飛ぶ
	exx
	dec a
	jr z,l7b8ah		; 1 だったら ;;;; (hl=7d1c)
	dec a
	jr z,l7b86h		; 2 だったら ;;;; (hl=7d1c)
	ld a,d			; 3以上f以下 だったら ;;;; (hl=7d1c)
	ld d,e
	ld e,a
	jr l7b8eh		; de をスワップして飛ぶ
l7b86h:				; 2 だったら e,d >>2
	rrc e
	rrc d
l7b8ah:				; 1 だったら e,d >>1
	rrc e
	rrc d
l7b8eh:				; 3以上f以下 だったら
	exx
	; 描画ループ 列で描画
l7b8fh:				; 0 だったら飛んでくる
	exx			; HL src に切り替え
sw7b90h:
	call sub_l7c43h		; 自己書換 色ごとの描画ルーチン ;;;; call 7d10h de=ffff (b:0, c:0)
	exx			;;;; a=(dst+0), b=(src+1) MSB 0 - 1 となっている
	push bc
	ld b,l			; swap dst addr = Low,High
	ld c,h
	out (c),a		; 1char 8pix 描画
	pop bc
	add hl,de		; stride 足して/引いて次の/前の line
	djnz l7b8fh		; ループ b ;;;; b=roi の残りライン, b'=(src+1) MSB 0 = 46h = 70 lines
	; 縦1列描画した
	or a
	sbc hl,de		; -stride して最後の描画した行の先頭にする
	inc hl			; 次の列に
	pop bc			; HW 復活
	dec c			; W--
	;
	; ループ出口 全列終わったら
	;
	ret z
	;
	ld a,e
	cp 050h
	jr z,l7baeh		; e=stride == 50h=80 だったら飛ぶ
	ld de,00050h		; 80 に戻す
	jp l7b72h
l7baeh:
	ld de,0ffb0h		; 符号反転 -80
	jp l7b72h
	; ループ終わり


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; TODO: 未実装
sub_l7bb4h:
	rst 038h

l7bd6h:
	db	00h
l7bd7h:
	db	00h
l7bdeh:
	db	00h
l7bdfh:
	db	00h
l7be6h:
	db	00h
l7be7h:
	db	00h

l7c2bh:
	dw	0000h
l7c2eh:
	dw	0000h
l7c31h:
	dw	0000h

l7cf8h:
	dw	0000h
l7cfbh:
	dw	0000h
l7cfeh:
	dw	0000h

l7df9h:
	dw	0000h
l7dfch:
	dw	0000h
l7dffh:
	dw	0000h


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; algo 1st plane 画素値 a を算出 (cb:長さ) コメントないところは 青? と同じ
sub_l7c43h:
	xor a
	cp b
	jp nz,l7c4dh		; 残りがある時
	cp c
	jp z,l7c66h		; 残りがない時
	dec c

	; 残りがあった時の処理
l7c4dh:
	dec b
sw7c4eh:
	ld a,000h		; 自己書換 0 列フラグ ;;;; 2列目以降変わる
l7c50h:				; bit5 20h の判定がない
	or a
	jp z,l7c64h		; a==0
	dec a
	jp z,l7c62h		; a==1
	dec a
	jp z,l7c60h		; a==2
	ld a,d			; a >= 3
	ld d,e
	ld e,a
	ret			; --------------- 出口
l7c60h:				; a==2
	rrc e
l7c62h:				; a==1,2
	rrc e
l7c64h:				; a==0,1,2
	ld a,e
	ret			; --------------- 出口

	; 残りがない時の処理
l7c66h:
	ld a,(hl)
	inc hl
	ld e,a
	or a
	jp z,l7c9eh
	cp 0ffh
	jp z,l7ca8h
	cp 055h
	jp z,l7cb9h
	cp 0aah
	jp z,l7cb9h
sw7c7ch:
	jp l7c94h		; 自己書換 飛び先: 1x -> 7c7f, 0x -> 7c94

l7c7fh:
	cp 033h
	jp z,l7cd4h
	cp 066h
	jp z,l7cd4h
	cp 099h
	jp z,l7cd4h
	cp 0cch
	jp z,l7cd4h
	ret			; --------------- 出口

l7c94h:
	rrca
	rrca
	rrca
	rrca
	cp e
	ld a,e
	jp z,l7cd4h
	ret			; --------------- 出口

l7c9eh:				; src+0 a==0 だったら、 ; ここが 青? より簡略
	ld a,(hl)
	inc hl
	or a
	jp z,l7cadh		; src+1 a==0 なら飛ぶ
	ld b,a			; b = (src+1)
	jp l7cb1h
				; 以下は同じ、
l7ca8h:				; src+0 a==ff だったら、
	ld b,(hl)
	inc hl
	jp l7cb1h
l7cadh:				; src+1==0 だったら、
	ld e,(hl)
	inc hl
	ld b,(hl)
	inc hl
l7cb1h:				; (src+0==0 && src+1!=0 の場合)
	xor a			; flag a = 00h
	ld (sw7c4eh+1),a
	dec a			; a=ffh MSB 見てないのでこうなる 簡略
	jp l7ce5h		; 続きへ飛ぶ

l7cb9h:				; src+0 a==55 or aa の場合
	xor a
	cp (hl)
	jp nz,l7ccfh
	ld a,003h		; flag a = 03h
	ld (sw7c4eh+1),a
l7cc3h:
	inc hl			; src+2
	ld e,(hl)		; e=(src+2)
	inc hl			; src+3
	ld d,(hl)		; d=(src+3)
	inc hl			; src+4
	ld b,(hl)		; b=(src+4)
	inc hl			; src+5
	ld a,0ffh		; a=ffh
	jp l7ce5h		; 続きへ飛ぶ

l7ccfh:				; (src+1)!=00 だったら、 ; 簡略化されている、
	ld a,001h		; flag a= 01h
	jp l7cd6h

l7cd4h:				; src+0 a==33/66/99/cc/ニブルが同じ の場合
	ld a,002h		; flag a = 02h
l7cd6h:
	ld b,(hl)
	inc hl
	bit 7,b
	jp z,l7ce0h
	res 7,b
	xor a			; flag a = 00h
l7ce0h:
	ld (sw7c4eh+1),a


	; 画素値算出 続き
l7ce3h:
	ld a,07fh
l7ce5h:
	; パターン
	; HL=src+2, (src+0)==00,    (src+1)!=00->b MSB==0,    : e=(src+0)00,           b=(src+1) MSB0,  a=B 7fh/R ffh, 00h flag
	; HL=src+2, (src+0)==00,    (src+1)!=00->b MSB==1->0  : e=(dst+0)&(d=ffh),     b=(src+1) MSB0,  a=7fh, 20h flag d処理は元にはない
	; HL=src+3, (src+0)==00,    (src+1)==00,              : e=(src+2),             b=(src+3),       a=ffh,         00h flag
	;
	; HL=src+2, (src+0)==ff,                              : e=(src+0)ff,           b=(src+1),       a=ffh,         00h flag
	;
	; HL=src+2, (src+0)==55/aa, (src+1)!=00->b MSB==0,    : e=(src+0)55/aa,        b=(src+1) MSB0,  a=7fh,         01h flag
	; HL=src+2, (src+0)==55/aa, (src+1)!=00->b MSB==10->00: e=(src+0)55/aa,        b=(src+1) MSB00, a=B 3fh/R 7fh, 00h flag
	; HL=src+2, (src+0)==55/aa, (src+1)!=00->b MSB==11->00: e=(dst+0)&(d=(src+0)), b=(src+1) MSB00, a=3fh, 21h flag
	;
	; HL=src+5, (src+0)==55/aa, (src+1)==00,              : e/d=(src+2)(src+3)     b=(src+4),       a=ffh,         03h flag
	;
	; HL=src+2, (src+0)==33/.., (src+1)!=00->b MSB==0,    : e=(src+0)33/..,        b=(src+1) MSB0,  a=7fh,         02h flag
	; HL=src+2, (src+0)==33/.., (src+1)!=00->b MSB==10->00: e=(src+0)33/..,        b=(src+1) MSB00, a=B 3fh/R 7fh, 00h flag
	; HL=src+2, (src+0)==33/.., (src+1)!=00->b MSB==11->00: e=(dst+0)&(d=(src+0)), b=(src+1) MSB00, a=3fh, 22h flag
	;
	cp b			;;;; b=長さが入っている
	jp nz,l7cf3h		;;;; 7f/ff 以外は(a!=b だったら)飛ぶ
	ld c,(hl)		; len=7f/ff だったら
	inc hl
	ld b,(hl)
	inc hl			; cb=(src 次)(src 次の次)
	xor a
	cp b
	jp nz,l7cf3h		; b != 0 なら飛ぶ
	dec c			; 繰り下がり
l7cf3h:
	dec b			; b=(src+1) MSB 0 - 1 ;;;; 何かの長さ ここで a を返すので一つ終わったということ
	ld a,e			;;;; a=e
	ret			; --------------- 出口
	; 画素値算出 終わり


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; algo 2nd plane 画素値 a を算出 (cb:長さ)
sub_l7d10h:
	xor a
	cp b			;;;; b=2
	jp nz,l7d1ah		; cb==???? b に残りがある時に飛ぶ
	cp c
	jp z,l7d3ah		; cb==0000h 残りがない時
	dec c			; cb==??00 繰り下がり

	; 残りがあった時の処理
l7d1ah:
	dec b			; b 残りを一つ減らす
sw7d1bh:
	ld a,000h		; 自己書換 0 列フラグ ;;;; 2列目以降変わる
	bit 5,a			;;;; 20h なのでビンゴ
	jp z,l7c50h		; flag==0 の時は飛んでそのまま ret しちゃう
	; 残りがあって、flag 20h の時
sw7d22h:
	ld a,05ch		; 自己書換 GVRAM 1st plane バンク選択 ;;;; 5e
	ld (planeBRGM),a
	ld a,(sw7d1bh+1)	; a を列フラグに戻す
l7d24h:
	and 00fh
	jp z,l7d36h		; a==0 ;;;; 飛ぶ
	dec a
	jp z,l7d2fh		; a==1
	rrc d			; a>=2 の時はシフトが増える
l7d2fh:
	rrc d
	exx			; dst に切り替え
	push hl
	call sub_plane		; h=VRAM high addr
	push bc
	ld b,l			; swap dst addr = Low,High
	ld c,h
	in a,(c)		; a=(dst+?) 1char 8pix 前 plane を参照する
	pop bc
	pop hl
	exx			; src に切り替え
	and d			; d を dst に混ぜ込む
	ret			; --------------- 出口
l7d36h:				; a==0 の時
	exx			;;;; dst に切り替え
	push hl
	call sub_plane		; h=VRAM high addr
	push bc
	ld b,l			; swap dst addr = Low,High
	ld c,h
	in a,(c)		;;;; 最後の行の2列目の画素値
	pop bc
	pop hl
	exx			;;;; src に切り替え
	ret			; --------------- 出口

	; 残りがない時の処理
l7d3ah:
	ld a,(hl)		; src+0 から読む
	inc hl			; src+1
	ld e,a			; e = src+0 a
	or a
	jp z,l7d72h		; src+0 a==00 だったら飛ぶ
	cp 0ffh
	jp z,l7d90h		; src+0 a==ff だったら飛ぶ
	cp 055h
	jp z,l7da1h		; src+0 a==55 だったら飛ぶ
	cp 0aah
	jp z,l7da1h		; src+0 a==aa だったら飛ぶ
sw7d50h:
	jp l7d68h		; 自己書換 飛び先 ;;;; 7d53

l7d53h:
	cp 033h
	jp z,l7dd5h
	cp 066h
	jp z,l7dd5h
	cp 099h
	jp z,l7dd5h
	cp 0cch
	jp z,l7dd5h		; src+0 33/66/99/cc だったら飛ぶ
	ret			; --------------- 出口 (それ以外は何もせず ret)

l7d68h:
	rrca
	rrca
	rrca
	rrca
	cp e
	ld a,e
	jp z,l7dd5h		; src+0 ニブルが同じだったら飛ぶ
	ret			; --------------- 出口


l7d72h:				; src+0 a==0 だったら、
	ld a,(hl)		; HL src+1 を読む
	inc hl			; src+2
	or a
	jp z,l7d95h		; src+1 a==0 なら飛ぶ
	ld b,a			; b = (src+1)
	and 080h		; a MSB 見る
	jp z,l7d8ah		; a MSB==0 なら飛ぶ
	res 7,b			; b MSB 下げる ;;;; MSB 1 の時は dst のまま、という意味
	exx			; HL dst に戻す, b (src+1) MSB 0 を保存
	push hl
sw7d80h:
	ld a,05ch		; 自己書換 GVRAM 1st plane バンク選択 ;;;; 5e
	ld (planeBRGM),a
	call sub_plane		; h=VRAM high addr
	ld a,05fh		; メインメモリに戻す
	ld (planeBRGM),a
	push bc
	ld b,l			; swap dst addr = Low,High
	ld c,h
	in a,(c)		; dst の先頭を読む
	pop bc
	pop hl
	exx			; HL src に戻す, b (src+1) MSB 0 ;;;; src+2 されている
	ld e,a			; e=(dst+0)
	ld a,020h		; 何かのフラグ。二回目はこの値になって、以下 dst のまま、という意味？
	; パターン
	; HL=src+2, (src+0)==00, (src+1)!=00->b MSB==0,    e=(src+0)00, a=00h flag
	; HL=src+2, (src+0)==00, (src+1)!=00->b MSB==1->0, e=(dst+0), a=20h flag
l7d8ah:
	ld (sw7d1bh+1),a	; フラグ更新
	jp l7ce3h		; 飛ぶ 飛び先で a=7fh

l7d90h:				; src+0 a==ff だったら、
	ld b,(hl)		; src+1 を読む
	inc hl			; src+2
	jp l7d99h

l7d95h:				; src+1 a==00 だったら、
	ld e,(hl)
	inc hl
	ld b,(hl)
	inc hl
	; パターン
	; HL=src+3, (src+0)==00, (src+1)==00, e=(src+2), b=(src+3)
	; HL=src+2, (src+0)==ff, (src+1)->b,  e=(src+0)ff

l7d99h:
	xor a			; フラグ 00h
	ld (sw7d1bh+1),a	; フラグ更新
	dec a			; a=ffh
	jp l7ce5h		; 飛ぶ


l7da1h:				; src+0 a==55 or aa だったら、
	xor a
	cp (hl)
	jp nz,l7daeh
	ld a,003h		; (src+1)==00 だったら、
	ld (sw7d1bh+1),a	; flag::03h
	jp l7cc3h		; シンプルアルゴと同じルーチンへ飛ぶ

l7daeh:				; (src+1)!=00 だったら、
	ld b,(hl)		; b=(src+1)
	inc hl			; src+2
	ld a,001h		; flag a = 01h
	bit 7,b
	jp z,l7d8ah		; b MSB 0 だったら飛ぶ フラグ更新して jmp l7ce3h flag:01h
	res 7,b			; b MSB 1 だったら MSB 0
	xor a			; flag a = 00h
	bit 6,b
	jp z,l7dcdh		; b MSB 10xx -> 00xx
	res 6,b
	ld d,e			; d=(src+0)
	exx
	push hl
sw7dc2h:
	ld a,05ch		; 自己書換 GVRAM 1st plane バンク選択
	ld (planeBRGM),a
	call sub_plane		; h=VRAM high addr
	ld a,05fh		; メインメモリに戻す
	ld (planeBRGM),a
	push bc
	ld b,l			; swap dst addr = Low,High
	ld c,h
	in a,(c)		; a=(dst+0)
	pop bc
	pop hl
	exx
	and d
	ld e,a			; e=(dst+0)&(d=(src+0)) 混ぜる
	ld a,021h		; flag a = 21h
l7dcdh:
	ld (sw7d1bh+1),a
	ld a,03fh
	jp l7ce5h		; パターンに追加したので詳細は飛び先で


l7dd5h:				; src+0 a 33/66/99/cc/ニブルが同じだったら、(コメント以外は 55 と同じ)
	ld b,(hl)
	inc hl
	ld a,002h		; a=02h flag
	bit 7,b
	jp z,l7d8ah
	res 7,b
	xor a
	bit 6,b
	jp z,l7dcdh
	res 6,b
	ld d,e
	exx
	push hl
sw7de9h:
	ld a,05ch		; 自己書換 GVRAM 1st plane バンク選択
	ld (planeBRGM),a
	call sub_plane		; h=VRAM high addr
	ld a,05fh		; メインメモリに戻す
	ld (planeBRGM),a
	push bc
	ld b,l			; swap dst addr = Low,High
	ld c,h
	in a,(c)
	pop bc
	pop hl
	exx
	and d
	ld e,a
	ld a,022h		; flag a=22h
	jp l7dcdh		; パターンに追加したので詳細は飛び先で


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; algo 3rd plane 画素値 a を算出 (cb:長さ) コメントないところは 青? と同じ
sub_l7e11h:
	xor a
	cp b
	jp nz,l7e1bh
	cp c
	jp z,l7e2dh
	dec c

	; 残りがあった時の処理
l7e1bh:
	dec b
sw7e1ch:
	ld a,000h		; 自己書換 0 列フラグ
	bit 5,a
	jp nz,bit5_nz
	bit 4,a
	jp z,l7c50h		; bit54 == 00 の時 赤? と同じ処理
sw7e28h:
	ld a,05dh		; 自己書換 GVRAM 2nd plane バンク選択
	ld (planeBRGM),a
	ld a,(sw7e1ch+1)	; a を列フラグに戻す
	jp l7d24h		; bit54 == 01 の時 青? と同じ処理
bit5_nz:
	ld a,(sw7d22h+1)	; 自己書換 GVRAM 1st plane バンク選択
	ld (planeBRGM),a
	ld a,(sw7e1ch+1)	; a を列フラグに戻す
	jp l7d24h		; bit54 == 10/11 の時 青? と同じ処理 ただし dst は前のままとなる

	; 残りがない時の処理
l7e2dh:
	ld a,(hl)
	inc hl
	ld e,a
	or a
	jp z,l7e65h
	cp 0ffh
	jp z,l7e83h
	cp 055h
	jp z,l7e9ah
	cp 0aah
	jp z,l7e9ah
sw7e43h:
	jp l7e5bh		; 自己書換 飛び先

l7e46h:
	cp 033h
	jp z,l7ee7h
	cp 066h
	jp z,l7ee7h
	cp 099h
	jp z,l7ee7h
	cp 0cch
	jp z,l7ee7h
	ret
l7e5bh:
	rrca
	rrca
	rrca
	rrca
	cp e
	ld a,e
	jp z,l7ee7h
	ret


l7e65h:				; src+0 a==0 だったら、
	ld a,(hl)
	inc hl
	or a
	jp z,l7f22h
	ld b,a
	and 080h
	jp z,l7e7dh
	res 7,b
	exx
	push hl
sw7e73h:
	ld a,05ch		; 自己書換 GVRAM 1st plane バンク選択
	ld (planeBRGM),a
	call sub_plane
	ld a,05fh
	ld (planeBRGM),a
	push bc
	ld b,l
	ld c,h
	in a,(c)
	pop bc
	pop hl
	exx
	ld e,a
	ld a,020h
l7e7dh:
	ld (sw7e1ch+1),a
	jp l7ce3h

l7e83h:				; src+0 a==ff だったら、
	ld b,(hl)		; b=(src+1)
	inc hl
	xor a			; flag a=00h
	bit 7,b
	jp z,l7e7dh		; b=(src+1) MSB 0 だったら、flag 00h , 7f して描画
	res 7,b			; b=(src+1) MSB 1->0
	exx
	push hl
sw7e8dh:
	ld a,05dh		; 自己書換 GVRAM 2nd plane バンク選択
	ld (planeBRGM),a
	call sub_plane
	ld a,05fh
	ld (planeBRGM),a
	push bc
	ld b,l
	ld c,h
	in a,(c)		; a=(dst+0)
	pop bc
	pop hl
	exx
	ld e,a			; e=(dst+0)
	ld a,010h		; flag a=10h
	jp l7e7dh		; flag 10h, 7f して描画

l7e9ah:				; src+0 a==55 or aa だったら、
	xor a
	cp (hl)
	jp nz,l7ea7h
	ld a,003h		; (src+1)==00 だったら、
	ld (sw7e1ch+1),a	; flag::03h
	jp l7cc3h		; シンプルアルゴと同じルーチンへ飛ぶ
	; パターン
	; (src+1)==00h: flag 03h, ...

l7ea7h:				; (src+1)!=00 だったら、
	ld b,(hl)		; b=(src+1)
	inc hl
	bit 7,b
	jp nz,l7ebbh		; b=(src+1) MSB 10 or 11 だったら、飛ぶ
	ld a,001h
	bit 6,b
	jp z,l7eceh		; b=(src+1) MSB 00 だったら、flag a = 01h
	xor a			; b=(src+1) MSB 01 だったら、flag a = 00h
	res 6,b			; MSB 01->00
	jp l7eceh
l7ebbh:				; b=(src+1) MSB 10 or 11 だったら、
	res 7,b			; MSB 1x->0x
	bit 6,b
	jp nz,l7ed6h		; b=(src+1) MSB 11 だったら、
	ld d,e			; b=(src+1) MSB 10 だったら、d=(src+0)
	exx
	push hl
sw7ec3h:
	ld a,05dh		; 自己書換 GVRAM 2nd plane バンク選択
	ld (planeBRGM),a
	call sub_plane
	ld a,05fh
	ld (planeBRGM),a
	push bc
	ld b,l
	ld c,h
	in a,(c)		; a=(dst+0)
	pop bc
	pop hl
	exx
	and d
	ld e,a			; e=(dst+0)&(src+0)
	ld a,011h		; flag a=11h
l7eceh:
	ld (sw7e1ch+1),a	; save flag
	ld a,03fh		; a=3fh
	jp l7ce5h		; 描画
	; パターン
	; (src+1) MSB 00: flag 01h, 3fh
	; (src+1) MSB 01: flag 00h, 3fh
	; (src+1) MSB 10: flag 11h, 3fh
	; (src+1) MSB 11: flag 21h, 3fh
l7ed6h:				; b=(src+1) MSB 11 だったら、
	res 6,b
	ld d,e			; d=(src+0)
	exx
	push hl
sw7ed9h:
	ld a,05ch		; 自己書換 GVRAM 1st plane バンク選択
	ld (planeBRGM),a
	call sub_plane
	ld a,05fh
	ld (planeBRGM),a
	push bc
	ld b,l
	ld c,h
	in a,(c)		; a=(dst+0)
	pop bc
	pop hl
	exx
	and d
	ld e,a			; e=(dst+0)&(src+0)
	ld a,021h		; flag a=21h
	jp l7eceh		; とんで、さらに描画に飛ぶ


l7ee7h:				; src+0 a 33/66/99/cc/ニブルが同じだったら、
	ld b,(hl)
	inc hl
	bit 7,b
	jp nz,l7efbh		; 80 or c0
	ld a,002h		; 40 or 00
	bit 6,b
	jp z,l7eceh		; 00 の時
	xor a			; 40 の時
	res 6,b
	jp l7eceh
l7efbh:
	res 7,b
	bit 6,b
	jp nz,l7f11h
	ld d,e			; 80 の時
	exx
	push hl
sw7f03h:
	ld a,05dh		; 自己書換 GVRAM 2nd plane バンク選択
	ld (planeBRGM),a
	call sub_plane
	ld a,05fh
	ld (planeBRGM),a
	push bc
	ld b,l
	ld c,h
	in a,(c)
	pop bc
	pop hl
	exx
	and d
	ld e,a
	ld a,012h
	jp l7eceh
l7f11h:
	res 6,b			; c0 の時
	ld d,e
	exx
	push hl
sw7f14h:
	ld a,05ch		; 自己書換 GVRAM 1st plane バンク選択
	ld (planeBRGM),a
	call sub_plane
	ld a,05fh
	ld (planeBRGM),a
	push bc
	ld b,l
	ld c,h
	in a,(c)
	pop bc
	pop hl
	exx
	and d
	ld e,a
	ld a,022h
	jp l7eceh

l7f22h:
	ld e,(hl)
	inc hl
	ld b,(hl)
	inc hl
	xor a
	ld (sw7e1ch+1),a
	dec a
	jp l7ce5h




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; calc VRAM base addr(offset x:79f8h,y:79f9h)
sub_7f2eh:
	ld h,000h
	ld a,(l79f9h)		; y
	ld l,a
	add hl,hl		; << 1
	add hl,hl		; << 2
	add hl,hl		; << 3
	add hl,hl		; << 4
	add hl,hl		; << 5
	add hl,hl		; << 6, hl = a * 64
	ex de,hl		; de = a * 64
	ld h,000h
	ld l,a
	add hl,hl		; << 1
	add hl,hl		; << 2
	add hl,hl		; << 3
	add hl,hl		; << 4, hl = a * 16
	add hl,de		; hl = a * 16 + a * 64 = a * 80
	ld d,040h		; d = VRAM base addr 4000h
	ld a,(l79f8h)		; x
	ld e,a
	add hl,de		; hl = 0x4000 + (80 * y + x)
	ret

sub_plane:
	ld a,h
	and 03fh
	ld h,a
	ld a,(planeBRGM)	; plane
	sub 05ch		; 00:B, 01:R, 02:G, 03:main RAM
	rlca
	rlca
	rlca
	rlca
	rlca
	rlca			; 00:B, 40:R, 80:G, c0:main RAM
	add a,040h		; 40:B, 80:R, c0:G, 00:main RAM
	add a,h
	ld h,a			; h = VRAM high addr
	ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; plane の指定領域を黒で塗りつぶす a=ヘッダの最初のバイト>>1
sub_l7f4bh:			;;;; hl:7c4f, de:7c43 -> 使わない
	rrca			;;;; a=0000_0110b -> c=0
	ld (l79fbh),a		; LSB 2bit 削ったものを保存
	ret c			;;;; 戻らない
	call sub_7f2eh		; calc VRAM base addr
	call sub_plane		; hl=VRAM addr with offset x,y
	ld de,00050h		; stride 80 * 8 = 640 pix
	xor a			; 画素値 0
	ld c,(ix+002h)		; ヘッダ height[line] ;;;; 44h = 68
l7f5ah:
	ld b,(ix+001h)		; ヘッダ width[char] ;;;; 0fh = 15
	push hl
l7f5eh:
	push bc
	ld b,l			; swap dst addr = Low,High
	ld c,h
	out (c),a		; 0 黒で塗りつぶす
	pop bc
	inc hl
	djnz l7f5eh		; 1 line 分 ;;;; 15 char = 120 pix
	pop hl
	add hl,de		; 次の行
	dec c
	jr nz,l7f5ah		;;;; 68 line 繰り返し
	ret			; 塗りつぶし終わり




	org 03000h

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; pict/2_47500_00_00.pict 13312[bytes] = 0x3400
pict_start:
	binclude "pict/2_47500_00_00.pict"


	org 07800h

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
main:
	call sub_78fah		; 真っ黒にする
	xor a
	ld (l79f8h),a		; offset x
	ld (l79f9h),a		; offset y
	call sub_79fch		; offset0,0 で描画

	rst 038h




	org 07900h

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
utils:
	; I/O RAM -> RAM
plane_0:
	ld b,040h		; size = 40h * 256 = 16KB
	ld c,040h		; src = 40h 1st plane
	ld h,080h		; dst = 80h
	jp io2main

plane_1:
	ld b,040h		; size = 40h * 256 = 16KB
	ld c,080h		; src = 80h 2nd plane
	ld h,080h		; dst = 80h
	jp io2main

plane_2:
	ld b,040h		; size = 40h * 256 = 16KB
	ld c,0c0h		; src = c0h 3rd plane
	ld h,080h		; dst = 80h
	jp io2main

io2main:
	push bc
	ld b,0ffh
	ld l,0ffh
	indr
	ini
	pop bc
	inc c
	inc h
	djnz io2main

	rst 038h
