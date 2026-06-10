# Pico + MPU 6802

## 目的
外付けの MPU 6802 に対して、Raspberry Pi Pico が ROM, RAM, UART を提供する。そのための Pico 用ファームウェアを c 言語で書く。

## 仕様
### 接続
* Pico GPIO 0-15 = Address bus
* Pico GPIO 16-23 = Data bus
* Pico GPIO 24 = E (clock in 2MHz が戻ってくる)
* Pico GPIO 25 = R/W in
* Pico GPIO 26 = VMA in
* Pico GPIO 27 = N.C.
* Pico GPIO 28 = /RESET out
* Pico GPIO 29 = clock out 8MHz 固定

### ROM
Pico 内の 16KB 配列。初期値は別ヘッダファイルに hex dump の形式で書く。
MPU から見たアドレスは $C000-

### RAM
Pico 内の 32KB 配列。初期値は 0 埋め。
MPU から見たアドレスは $0000-

### UART
Pico の USB CDC を MPU に対して UART として提供する。
memory mapped I/O の 6850 互換で、$8000 がコントロール、$8001 がデータレジスタとする。

### 動き
* Pico 側の初期化が終わったら /RESET H
* E & VMA の立ち上がりでバスの状態を取得する(パフォーマンスのため 1core を占拠してポーリングでいい)
* R/W とアドレス値で ROM, RAM, UART を判定する
    * ROM R: データバスを out にして、指定アドレスの値をデータバスに出力
    * RAM R: データバスを out にして、指定アドレスの値をデータバスに出力
    * RAM W: データバスから値を内部変数に取り込む。配列に書き込む。
    * UART status: USB CDC の状態を読み取って、6850 互換に変換して、データバスに出力
    * UART R: CDC で受信済みだったらその値をデータバスに出力、未受信だったら $ff を返す(block しない)
    * UART W: データバスから値を内部変数に取り込む。CDC が空いていたら送信する。埋まっていたら何もせず捨てる(block しない)
    * 以上を、半クロック以内で終わらせる (<250ns)
* PIO を使って E の立ち下がりで、データバスを in にする
