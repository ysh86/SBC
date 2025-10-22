/*
    PIC18F47Q84 ROM image uploader and UART emulation firmware

    Target: EMUZ80 with 68008+RAM
    Compiler: Microchip MPLAB XC8 C Compiler V2.40 (Build date: Jul  3 2022)

    based on:

    PIC18F47Q43 ROM RAM and UART emulation firmware
    This single source file contains all code

    Target: EMUZ80 - The computer with only Z80 and PIC18F47Q43
    Compiler: MPLAB XC8 v2.36
    Written by Tetsuya Suzuki
*/

// CONFIG1
#pragma config FEXTOSC = OFF	// External Oscillator Selection (Oscillator not enabled)
#pragma config RSTOSC = HFINTOSC_64MHZ// Reset Oscillator Selection (HFINTOSC with HFFRQ = 64 MHz and CDIV = 1:1)

// CONFIG2
#pragma config CLKOUTEN = OFF	// Clock out Enable bit (CLKOUT function is disabled)
#pragma config PR1WAY = ON		// PRLOCKED One-Way Set Enable bit (PRLOCKED bit can be cleared and set only once)
#pragma config CSWEN = ON		// Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = ON		// Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor enabled)
#ifndef _18F47Q43
#pragma config JTAGEN = OFF
#pragma config FCMENP = OFF
#pragma config FCMENS = OFF
#endif

// CONFIG3
#pragma config MCLRE = EXTMCLR	// MCLR Enable bit (If LVP = 0, MCLR pin is MCLR; If LVP = 1, RE3 pin function is MCLR )
#pragma config PWRTS = PWRT_OFF // Power-up timer selection bits (PWRT is disabled)
#pragma config MVECEN = ON		// Multi-vector enable bit (Multi-vector enabled, Vector table used for interrupts)
#pragma config IVT1WAY = ON		// IVTLOCK bit One-way set enable bit (IVTLOCKED bit can be cleared and set only once)
#pragma config LPBOREN = OFF	// Low Power BOR Enable bit (Low-Power BOR disabled)
#pragma config BOREN = SBORDIS	// Brown-out Reset Enable bits (Brown-out Reset enabled , SBOREN bit is ignored)

// CONFIG4
#pragma config BORV = VBOR_1P9	// Brown-out Reset Voltage Selection bits (Brown-out Reset Voltage (VBOR) set to 1.9V)
#pragma config ZCD = OFF		// ZCD Disable bit (ZCD module is disabled. ZCD can be enabled by setting the ZCDSEN bit of ZCDCON)
#pragma config PPS1WAY = OFF	// PPSLOCK bit One-Way Set Enable bit (PPSLOCKED bit can be set and cleared repeatedly (subject to the unlock sequence))
#pragma config STVREN = ON		// Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = ON			// Low Voltage Programming Enable bit (Low voltage programming enabled. MCLR/VPP pin function is MCLR. MCLRE configuration bit is ignored)
#pragma config XINST = OFF		// Extended Instruction Set Enable bit (Extended Instruction Set and Indexed Addressing Mode disabled)

// CONFIG5
#pragma config WDTCPS = WDTCPS_31// WDT Period selection bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF		// WDT operating mode (WDT Disabled; SWDTEN is ignored)

// CONFIG6
#pragma config WDTCWS = WDTCWS_7// WDT Window Select bits (window always open (100%); software control; keyed access not required)
#pragma config WDTCCS = SC		// WDT input clock selector (Software Control)

// CONFIG7
#pragma config BBSIZE = BBSIZE_512// Boot Block Size selection bits (Boot Block size is 512 words)
#pragma config BBEN = OFF		// Boot Block enable bit (Boot block disabled)
#pragma config SAFEN = OFF		// Storage Area Flash enable bit (SAF disabled)
#ifdef _18F47Q43
#pragma config DEBUG = OFF		// Background Debugger (Background Debugger disabled)
#endif

// CONFIG8
#pragma config WRTB = OFF		// Boot Block Write Protection bit (Boot Block not Write protected)
#pragma config WRTC = OFF		// Configuration Register Write Protection bit (Configuration registers not Write protected)
#pragma config WRTD = OFF		// Data EEPROM Write Protection bit (Data EEPROM not Write protected)
#pragma config WRTSAF = OFF		// SAF Write protection bit (SAF not Write Protected)
#pragma config WRTAPP = OFF		// Application Block write protection bit (Application Block not write protected)

// CONFIG10
#pragma config CP = OFF			// PFM and Data EEPROM Code Protection bit (PFM and Data EEPROM code protection disabled)

#ifndef _18F47Q43
// CONFIG9
#pragma config BOOTPINSEL = RC5	// CRC on boot output pin selection (CRC on boot output pin is RC5)
#pragma config BPEN = OFF		// CRC on boot output pin enable bit (CRC on boot output pin disabled)
#pragma config ODCON = OFF		// CRC on boot output pin open drain bit (Pin drives both high-going and low-going signals)

// CONFIG11
#pragma config BOOTSCEN = OFF	// CRC on boot scan enable for boot area (CRC on boot will not include the boot area of program memory in its calculation)
#pragma config BOOTCOE = HALT	// CRC on boot Continue on Error for boot areas bit (CRC on boot will stop device if error is detected in boot areas)
#pragma config APPSCEN = OFF	// CRC on boot application code scan enable (CRC on boot will not include the application area of program memory in its calculation)
#pragma config SAFSCEN = OFF	// CRC on boot SAF area scan enable (CRC on boot will not include the SAF area of program memory in its calculation)
#pragma config DATASCEN = OFF	// CRC on boot Data EEPROM scan enable (CRC on boot will not include data EEPROM in its calculation)
#pragma config CFGSCEN = OFF	// CRC on boot Config fuses scan enable (CRC on boot will not include the configuration fuses in its calculation)
#pragma config COE = HALT		// CRC on boot Continue on Error for non-boot areas bit (CRC on boot will stop device if error is detected in non-boot areas)
#pragma config BOOTPOR = OFF	// Boot on CRC Enable bit (CRC on boot will not run)

// CONFIG12
#pragma config BCRCPOLT = hFF	// Boot Sector Polynomial for CRC on boot bits 31-24 (Bits 31:24 of BCRCPOL are 0xFF)

// CONFIG13
#pragma config BCRCPOLU = hFF	// Boot Sector Polynomial for CRC on boot bits 23-16 (Bits 23:16 of BCRCPOL are 0xFF)

// CONFIG14
#pragma config BCRCPOLH = hFF	// Boot Sector Polynomial for CRC on boot bits 15-8 (Bits 15:8 of BCRCPOL are 0xFF)

// CONFIG15
#pragma config BCRCPOLL = hFF	// Boot Sector Polynomial for CRC on boot bits 7-0 (Bits 7:0 of BCRCPOL are 0xFF)

// CONFIG16
#pragma config BCRCSEEDT = hFF	// Boot Sector Seed for CRC on boot bits 31-24 (Bits 31:24 of BCRCSEED are 0xFF)

// CONFIG17
#pragma config BCRCSEEDU = hFF	// Boot Sector Seed for CRC on boot bits 23-16 (Bits 23:16 of BCRCSEED are 0xFF)

// CONFIG18
#pragma config BCRCSEEDH = hFF	// Boot Sector Seed for CRC on boot bits 15-8 (Bits 15:8 of BCRCSEED are 0xFF)

// CONFIG19
#pragma config BCRCSEEDL = hFF	// Boot Sector Seed for CRC on boot bits 7-0 (Bits 7:0 of BCRCSEED are 0xFF)

// CONFIG20
#pragma config BCRCEREST = hFF	// Boot Sector Expected Result for CRC on boot bits 31-24 (Bits 31:24 of BCRCERES are 0xFF)

// CONFIG21
#pragma config BCRCERESU = hFF	// Boot Sector Expected Result for CRC on boot bits 23-16 (Bits 23:16 of BCRCERES are 0xFF)

// CONFIG22
#pragma config BCRCERESH = hFF	// Boot Sector Expected Result for CRC on boot bits 15-8 (Bits 15:8 of BCRCERES are 0xFF)

// CONFIG23
#pragma config BCRCERESL = hFF	// Boot Sector Expected Result for CRC on boot bits 7-0 (Bits 7:0 of BCRCERES are 0xFF)

// CONFIG24
#pragma config CRCPOLT = hFF	// Non-Boot Sector Polynomial for CRC on boot bits 31-24 (Bits 31:24 of CRCPOL are 0xFF)

// CONFIG25
#pragma config CRCPOLU = hFF	// Non-Boot Sector Polynomial for CRC on boot bits 23-16 (Bits 23:16 of CRCPOL are 0xFF)

// CONFIG26
#pragma config CRCPOLH = hFF	// Non-Boot Sector Polynomial for CRC on boot bits 15-8 (Bits 15:8 of CRCPOL are 0xFF)

// CONFIG27
#pragma config CRCPOLL = hFF	// Non-Boot Sector Polynomial for CRC on boot bits 7-0 (Bits 7:0 of CRCPOL are 0xFF)

// CONFIG28
#pragma config CRCSEEDT = hFF	// Non-Boot Sector Seed for CRC on boot bits 31-24 (Bits 31:24 of CRCSEED are 0xFF)

// CONFIG29
#pragma config CRCSEEDU = hFF	// Non-Boot Sector Seed for CRC on boot bits 23-16 (Bits 23:16 of CRCSEED are 0xFF)

// CONFIG30
#pragma config CRCSEEDH = hFF	// Non-Boot Sector Seed for CRC on boot bits 15-8 (Bits 15:8 of CRCSEED are 0xFF)

// CONFIG31
#pragma config CRCSEEDL = hFF	// Non-Boot Sector Seed for CRC on boot bits 7-0 (Bits 7:0 of CRCSEED are 0xFF)

// CONFIG32
#pragma config CRCEREST = hFF	// Non-Boot Sector Expected Result for CRC on boot bits 31-24 (Bits 31:24 of CRCERES are 0xFF)

// CONFIG33
#pragma config CRCERESU = hFF	// Non-Boot Sector Expected Result for CRC on boot bits 23-16 (Bits 23:16 of CRCERES are 0xFF)

// CONFIG34
#pragma config CRCERESH = hFF	// Non-Boot Sector Expected Result for CRC on boot bits 15-8 (Bits 15:8 of CRCERES are 0xFF)

// CONFIG35
#pragma config CRCERESL = hFF	// Non-Boot Sector Expected Result for CRC on boot bits 7-0 (Bits 7:0 of CRCERES are 0xFF)
#endif

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>

// CPU clock: Max 10MHz
#define CPU_CLK_MHz 10UL
#define CPU_CLK_STR "MEZ68K8RAM 10.0MHz"
// INC = (cpu[MHz] * 2^20) * 2 / 64[MHz] = (cpu * 2^20) / 32 = cpu * 1,048,576/32 = cpu * 32768
#define CPU_CLK_INC (CPU_CLK_MHz * 32768UL)

// PIC port
#define PORT_CAT(x, y)  PORT_CAT_(x, y)
#define PORT_CAT_(x, y) x ## y

#define TRIS(port)      PORT_CAT(TRIS, port)
#define LAT(port)       PORT_CAT(LAT, port)
#define R(port)         PORT_CAT(R, port)
#define WPU(port)       PORT_CAT(WPU, port)
#define PORT(port)      PORT_CAT(PORT, port)

#define M68K_RESET      E0
#define M68K_BR         E1 // BUS request
#define M68K_BG         C2 // BUS GRANT
#define M68K_CLK        A5

#define M68K_AS         C1
#define M68K_RW         A4 // R/#W
#define M68K_DS         C0 // SRAM #CE, #DS
#define M68K_SRAM_WE    C4
#define M68K_DTACK      C5

#define M68K_ADBUS      B
#define M68K_ADR_H      D
#define M68K_A16        A0
#define M68K_A17        A1
#define M68K_A18        A2
#define M68K_A19        A3

#define M68K_LE         E2
#define M68K_LTOE       C3

// SPI
#define MISO            C6
#define MOSI            B0
#define SPI_CK          B1
#define SPI_SS          C7

// ROM equivalent: Max 32KB
#define ROM_SIZE 0x2000UL
const unsigned char rom[ROM_SIZE] __at(0xe000) = { // -1: compiler bug?
#include "../ROM/ROM68K8.h"
};

// SRAM: MAX 512KB
#define DO_RAM_TEST 1
#define RAM_SIZE    0x80000UL

// UART
#define UART_SREG 0xe000 // Status REG
#define UART_DREG 0xe001 // Data REG
//#define UART_BPS  0x1a0 //  9600bps @ 64MHz
//#define UART_BPS  0xd0  // 19200bps @ 64MHz
#define UART_BPS  0x68  // 38400bps @ 64MHz

// UART3 Transmit
void putch(char c) {
    while(!U3TXIF); // Wait for Tx interrupt flag set
    U3TXB = c;      // Write data
}
/*
// UART3 Receive
char getch(void) {
    while(!U3RXIF); // Wait for Rx interrupt flag set
    return U3RXB;   // Read data
}
*/

// Address Bus
union {
    uint16_t w;
    struct {
        uint8_t l; // low
        uint8_t h; // high
    };
} ab;

// Never called, logically
void __interrupt(irq(default),base(8)) Default_ISR(){}

// Called at MRDY falling edge(Immediately after CLK rising)
void __interrupt(irq(CLC5),base(8)) CLC5_ISR(){
    CLC5IF = 0;   // Clear interrupt flag

    ab.h = PORTD; // Read address high
    ab.l = PORTB; // Read address low

    // I/O write cycle
    if (!RA4) {
        if (ab.w == UART_DREG) {
            // Write into U3TXB
            U3TXB = PORTC;
        } else {
            // Write into I/O RAM
            //ioram[ab.w & 0x0fff] = PORTC;
        }

        // Release MRDY (D-FF reset)
        G3POL = 1;
        G3POL = 0;
        return;
    }

    // I/O read cycle
    TRISC = 0x00;               // Set data bus as output
    if (ab.w == UART_SREG)      // UART Status
        LATC = PIR9;            // Out PIR9: Bit 1 - U3TXIF(TX is empty), Bit 0 - U3RXIF(RX is full)
    else if (ab.w == UART_DREG) // UART RX
        LATC = U3RXB;           // Out U3RXB
    else                        // Read from I/O RAM
        //LATC = ioram[ab.w & 0x0fff];

    // Detect CLK rising edge
    while(!RE1);
    NOP(); // wait 62.5ns

    // Release MRDY (D-FF reset)
    G3POL = 1;
    G3POL = 0;

#if 1
    NOP();      // wait 62.5ns
#else
    while(RA1); // Detect E falling edge <2.75MHz (11MHz)
#endif
    TRISC = 0xff; // Set data bus as input
}

// main routine
void main(void) {
    // System initialize
    OSCFRQ = 0x08; // 64MHz internal OSC

    // Disable analog function
    ANSELA = 0x00;
    ANSELB = 0x00;
    ANSELC = 0x00;
    ANSELD = 0x00;
    ANSELE0 = 0;
    ANSELE1 = 0;
    ANSELE2 = 0;

    // RESET & HALT
    LAT(M68K_RESET) = 0;     // Reset & Halt
    TRIS(M68K_RESET) = 0;    // Set as output

    // #BG input
    WPU(M68K_BG) = 1;        // Weak pull up
    LAT(M68K_BG) = 1;
    TRIS(M68K_BG) = 1;       // Set as input

    // #BR
    WPU(M68K_BR) = 0;        // Disable pull up
    LAT(M68K_BR) = 0;        // m68k: HiZ
    TRIS(M68K_BR) = 0;       // Set as output

    // CLK
    WPU(M68K_CLK) = 0;       // Disable pull up
    LAT(M68K_CLK) = 1;
    TRIS(M68K_CLK) = 0;      // Set as output

    // CPU clock(RA5) by NCO FDC mode
    RA5PPS = 0x3f;           // RA5 asign NCO1
    NCO1INC = CPU_CLK_INC;
    NCO1CLK = 0x00;          // Clock source Fosc = 64MHz
    NCO1PFM = 0;             // Fixed Duty Cycle mode
    NCO1OUT = 1;             // NCO output enable
    NCO1EN = 1;              // NCO enable

    while (R(M68K_BG)) {}    // wait until bus release


    // SRAM #CE, #DS
    WPU(M68K_DS) = 1;        // Weak pull up
    LAT(M68K_DS) = 1;
    TRIS(M68K_DS) = 0;       // Set as output

    // R/W
    WPU(M68K_RW) = 1;        // Weak pull up
    LAT(M68K_RW) = 1;        // READ
    TRIS(M68K_RW) = 0;       // Set as output

    // #SRAM_WE
    WPU(M68K_SRAM_WE) = 0;   // Disable pull up
    LAT(M68K_SRAM_WE) = 1;
    TRIS(M68K_SRAM_WE) = 0;  // Set as output

    // #AS
    WPU(M68K_AS) = 1;        // Weak pull up
    LAT(M68K_AS) = 1;
    TRIS(M68K_AS) = 0;       // Set as output

    // #DTACK
    WPU(M68K_DTACK) = 0;     // Disable pull up
    LAT(M68K_DTACK) = 1;
    TRIS(M68K_DTACK) = 0;    // Set as output


    // #LTOE
    WPU(M68K_LTOE) = 0;      // Disable pull up
    LAT(M68K_LTOE) = 1;      // 74LS373 as HiZ
    TRIS(M68K_LTOE) = 0;     // Set as output

    // LE
    WPU(M68K_LE) = 0;        // Disable pull up
    LAT(M68K_LE) = 0;        // Disable LE
    TRIS(M68K_LE) = 0;       // Set as output

    // Init address to 0
    // low
    WPU(M68K_ADBUS) = 0xff;  // Weak pull up
    LAT(M68K_ADBUS) = 0x00;
    TRIS(M68K_ADBUS) = 0x00; // Set as output
    // high
    WPU(M68K_ADR_H) = 0xff;  // Weak pull up
    LAT(M68K_ADR_H) = 0x00;
    TRIS(M68K_ADR_H) = 0x00; // Set as output
    // A16
    WPU(M68K_A16) = 1;       // Weak pull up
    LAT(M68K_A16) = 0;
    TRIS(M68K_A16) = 0;      // Set as output
    // A17
    WPU(M68K_A17) = 1;       // Weak pull up
    LAT(M68K_A17) = 0;
    TRIS(M68K_A17) = 0;      // Set as output
    // A18
    WPU(M68K_A18) = 1;       // Weak pull up
    LAT(M68K_A18) = 0;
    TRIS(M68K_A18) = 0;      // Set as output
    // A19
    WPU(M68K_A19) = 0;       // Disable pull up
    LAT(M68K_A19) = 0;
    TRIS(M68K_A19) = 0;      // Set as output

    // TODO: SPI for SD
    // RC6
    WPU(MISO) = 1;           // Weak pull up
    LAT(MISO) = 0;
    TRIS(MISO) = 0;          // Set as onput
    // RC7
    WPU(SPI_SS) = 1;         // Weak pull up
    LAT(SPI_SS) = 0;
    TRIS(SPI_SS) = 0;        // Set as onput

    // UART3 initialize
    U3BRG = UART_BPS;
    U3RXEN = 1;        // Receiver enable
    U3TXEN = 1;        // Transmitter enable

    // UART3 (RA7) Receiver
    TRISA7 = 1;        // RX set as input
    U3RXPPS = 0x07;    // RA7 -> RXD

    // UART3 (RA6) Transmitter
    LATA6 = 1;         // Default level
    TRISA6 = 0;        // TX set as output
    RA6PPS = 0x26;     // TXD -> RA6

    U3ON = 1;          // Serial port enable

    //========== Clear all CLC outs ==========
    CLCDATA = 0x03;  // 0b0000_0011

    //========== CLC input pin assign ===========
    // CLCx Input 1,2,5,6: only PortA or C
    // CLCx Input 3,4,7,8: only PortB or D
    // PortA: 0b00_000_xxx
    // PortB: 0b00_001_xxx
    // PortC: 0b00_010_xxx
    // PortD: 0b00_011_xxx
    // PortE: cannot be connected to CLC inputs/outputs
    CLCIN0PPS = 0x03; // CLCIN0PPS <- RA3 <- A19
    CLCIN1PPS = 0x04; // CLCIN1PPS <- RA4 <- R/#W
    CLCIN4PPS = 0x10; // CLCIN4PPS <- RC0 <- #DS
    CLCIN5PPS = 0x11; // CLCIN5PPS <- RC1 <- #AS

    // ----------------------------------------------------------------------
    // address decoder:
    //
    // 0_0000-7_ffff: SRAM
    // 8_0000-f_ffff: I/O regs
    // ----------------------------------------------------------------------

    //========== CLC1 #SRAM_WE ==========
    CLCSELECT = 0;   // Select CLC1

    CLCnSEL0 = 0;    // CLCIN0PPS <- A19
    CLCnSEL1 = 1;    // CLCIN1PPS <- R/#W
    CLCnSEL2 = 127;  // N/C
    CLCnSEL3 = 127;  // N/C

    CLCnGLS0 = 0x01; // !A19
    CLCnGLS1 = 0x04; // !R/#W = W
    CLCnGLS2 = 0x00; // not gated
    CLCnGLS3 = 0x00; // not gated

    CLCnPOL = 0x8c;  // inverted the output of the logic cell. The gate3/4 outputs '1'.
    CLCnCON = 0x82;  // 4 input AND

#if 0
    //========== CLC2 #DTACK ==========
    CLCSELECT = 1;   // Select CLC2

    //CLCnSEL0 = 5;    // CLCIN5PPS <- #AS
    CLCnSEL0 = 4;    // CLCIN5PPS <- #DS
    CLCnSEL1 = 127;  // N/C
    CLCnSEL2 = 127;  // N/C
    CLCnSEL3 = 127;  // N/C

    CLCnGLS0 = 0x02; // #AS
    CLCnGLS1 = 0x00; // not gated
    CLCnGLS2 = 0x00; // not gated
    CLCnGLS3 = 0x00; // not gated

    CLCnPOL = 0x0e;  // The gate2/3/4 outputs '1'.
    CLCnCON = 0x82;  // 4-input AND
#endif

    // ----------------------------------------------------------------------
    // D-FF
    // ----------------------------------------------------------------------

    //========== CLC2 #DTACK ==========
    CLCSELECT = 1;   // Select CLC2

    CLCnSEL0 = 42;   // NCO1
    CLCnSEL1 = 5;    // CLCIN5PPS <- #AS
    CLCnSEL2 = 127;  // N/C
    CLCnSEL3 = 127;  // N/C

    CLCnGLS0 = 0x02; // D-FF CLK <- NCO1 (pos edge)
    CLCnGLS1 = 0x04; // D-FF D   <- !#AS
    CLCnGLS2 = 0x00; // D-FF R   <- not gated ('0') <- G3POL
    CLCnGLS3 = 0x00; // D-FF S   <- not gated ('0')

    CLCnPOL = 0x80;  // inverted the output of the logic cell.
    CLCnCON = 0x84;  // Select D-FF

    G3POL = 1;       // Reset FF
    G3POL = 0;

    //========== CLC3 IOREQ ==========
    CLCSELECT = 2;   // Select CLC3

    CLCnSEL0 = 5;    // CLCIN5PPS <- #AS
    CLCnSEL1 = 0;    // CLCIN0PPS <- A19
    CLCnSEL2 = 53;   // CLC3(self): IOREQ
    CLCnSEL3 = 127;  // N/C

    CLCnGLS0 = 0x01; // D-FF CLK <- !#AS (neg edge)
    CLCnGLS1 = 0x08; // D-FF D2  <- A19
    CLCnGLS2 = 0x00; // D-FF R   <- not gated ('0') <- G3POL
    CLCnGLS3 = 0x20; // D-FF D1  <- IOREQ(self)

    CLCnPOL = 0x00;  // not inverted all
    CLCnCON = 0x85;  // Select D-FF with (D1 | D2) input

    G3POL = 1;       // Reset FF
    G3POL = 0;

    //========== CLC output pin assign ===========
    // CLCxOUT: 0x01-0x08
    // CLCx Output 1,2,5,6: only PortA or C
    // CLCx Output 3,4,7,8: only PortB or D
    RC4PPS = 0x01;   // CLC1OUT -> RC4 -> #SRAM_WE
    RC5PPS = 0x02;   // CLC2OUT -> RC5 -> #DTACK
                     // CLC3OUT -> polling @ PIC

    //==========    CPU info    ===========
    const char *cpu_info = "\r\n" CPU_CLK_STR "\r\n";
    while (*cpu_info != '\0') {
        putch(*cpu_info);
        ++cpu_info;
    }

    //==========    Mem test    ===========
    unsigned long addr = 0;
#if DO_RAM_TEST
    LAT(M68K_LTOE) = 0;
    while (addr < RAM_SIZE) {
        // RAM address
        LAT(M68K_ADBUS) =  addr        & 0xff;
        LAT(M68K_ADR_H) = (addr >>  8) & 0xff;
        LAT(M68K_A16) =   (addr >> 16) & 1;
        LAT(M68K_A17) =   (addr >> 17) & 1;
        LAT(M68K_A18) =   (addr >> 18) & 1;
        LAT(M68K_LE) = 1;
        LAT(M68K_LE) = 0;

        // wait
        NOP();
        addr += 1;

        // write
        LAT(M68K_RW) = 0; // /WE
        LAT(M68K_DS) = 0; // SRAM #CE
        LAT(M68K_ADBUS) = addr & 0xff;
        NOP();
        LAT(M68K_DS) = 1; // SRAM #CE
        LAT(M68K_RW) = 1; // /WE

        // read
        unsigned char r = 0;
        TRIS(M68K_ADBUS) = 0xff; // input
        LAT(M68K_DS) = 0; // SRAM #CE
        NOP();
        r = PORT(M68K_ADBUS);
        LAT(M68K_DS) = 1; // SRAM #CE
        TRIS(M68K_ADBUS) = 0x00; // output

        // verify
        if (r != (addr & 0xff)) {
            const char *msg = "RAM bad!\r\n";
            while (*msg != '\0') {
                putch(*msg);
                ++msg;
            }
            goto END;
        }
        if ((addr & 0x7fff) == 0x7fff) {
            // progress (every 32KB)
            putch('.');
            putch(' ');
        }
    }
    {
        const char *msg = "\r\nRAM OK\r\n";
        while (*msg != '\0') {
            putch(*msg);
            ++msg;
        }
    }
END:
    LAT(M68K_LTOE) = 1;
#endif

    // Upload PIC Flash ROM -> CPU main RAM

    // $00000: vectors
    LAT(M68K_A16) = 0;
    LAT(M68K_A17) = 0;
    LAT(M68K_A18) = 0;
    addr = 0;
    LAT(M68K_LTOE) = 0;
    LAT(M68K_RW) = 0; // /WE
    while (addr < ROM_SIZE) {
        // RAM address
        LAT(M68K_ADBUS) =  addr        & 0xff;
        LAT(M68K_ADR_H) = (addr >>  8) & 0xff;
        LAT(M68K_LE) = 1;
        LAT(M68K_LE) = 0;

        // wait
        NOP();
        NOP();

        // write
        LAT(M68K_DS) = 0; // SRAM #CE
        LAT(M68K_ADBUS) = rom[addr];
        NOP();
        addr += 1;
        LAT(M68K_DS) = 1; // SRAM #CE
    }
    LAT(M68K_RW) = 1; // /WE
    LAT(M68K_LTOE) = 1;

    // $7xxxx: ROM
    unsigned long romaddr = RAM_SIZE - ROM_SIZE;
    LAT(M68K_A16) = (romaddr >> 16) & 1;
    LAT(M68K_A17) = (romaddr >> 17) & 1;
    LAT(M68K_A18) = (romaddr >> 18) & 1;
    addr = romaddr & 0xffff;
    LAT(M68K_LTOE) = 0;
    LAT(M68K_RW) = 0; // /WE
    while (addr < ROM_SIZE) {
        // RAM address
        LAT(M68K_ADBUS) =  addr        & 0xff;
        LAT(M68K_ADR_H) = (addr >>  8) & 0xff;
        LAT(M68K_LE) = 1;
        LAT(M68K_LE) = 0;

        // wait
        NOP();
        NOP();

        // write
        LAT(M68K_DS) = 0; // SRAM #CE
        LAT(M68K_ADBUS) = rom[addr];
        NOP();
        addr += 1;
        LAT(M68K_DS) = 1; // SRAM #CE
    }
    LAT(M68K_RW) = 1; // /WE
    LAT(M68K_LTOE) = 1;

    {
        const char *msg = "ROM ready\r\n";
        while (*msg != '\0') {
            putch(*msg);
            ++msg;
        }
    }


    // Set as input
    TRIS(M68K_ADBUS) = 0xff; // D bus(data only): m68k <-> PIC
    TRIS(M68K_ADR_H) = 0xff; // A bus(A15-8 only): m68k -> PIC
    TRIS(M68K_A16) = 1;
    TRIS(M68K_A17) = 1;
    TRIS(M68K_A18) = 1;
    TRIS(M68K_A19) = 1;
    TRIS(M68K_RW) = 1;
    TRIS(M68K_AS) = 1;
    TRIS(M68K_DS) = 1;


    // Unlock IVT
    IVTLOCK = 0x55;
    IVTLOCK = 0xAA;
    IVTLOCKbits.IVTLOCKED = 0x00;

    // Default IVT base address
    IVTBASE = 0x000008;

    // Lock IVT
    IVTLOCK = 0x55;
    IVTLOCK = 0xAA;
    IVTLOCKbits.IVTLOCKED = 0x01;

    // Reset FF
    CLCSELECT = 1; // Select CLC2
    G3POL = 1;
    G3POL = 0;
    CLCSELECT = 2; // Select CLC3
    G3POL = 1;
    G3POL = 0;

#if 0
    // Enable CLC VI
    CLC3IF = 0; // Clear the CLC3 interrupt flag
    CLC3IE = 1; // Enabling CLC3 interrupt

    GIE = 1;    // Enable global interrupt
#endif

    // Start CPU
    LAT(M68K_BR) = 1;      // PIC releases bus, m68k holds bus
    while (!R(M68K_BG)) {} // wait

    {
        const char *msg = "MPU ready\r\n\r\n";
        while (*msg != '\0') {
            putch(*msg);
            ++msg;
        }
    }

#if 0
    LAT(M68K_RESET) = 1;  // Release reset
    TRIS(M68K_RESET) = 1; // Set as input
#endif

    // All things come to those who wait
    while(1);
}
