/*
    PIC18F47Q84 ROM image uploader, I/O RAM and UART emulation firmware

    Target: EMUZ80 with 6809+RAM
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

#define _XTAL_FREQ 64000000UL // for __delay_us

// CPU clock: Max 16MHz
#define CPU_CLK_MHz 12UL
#define CPU_CLK_STR "MEZ6809RAM 3.0MHz"
// INC = (cpu[MHz] * 2^20) * 2 / 64[MHz] = (cpu * 2^20) / 32 = cpu * 1,048,576/32 = cpu * 32768
#define CPU_CLK_INC (CPU_CLK_MHz * 32768UL)

// ROM equivalent: Max 16KB
#define ROM_TOP  0xc000
#define ROM_SIZE 0x4000
const unsigned char rom[ROM_SIZE] __at(0xc000) = {
#include "../ROM/ROM16KB.h"
};

// I/O RAM
#define RAM_SIZE 0x1000
unsigned char ioram[RAM_SIZE];

// UART
#define UART_DREG 0x8019  // Data REG
#define UART_CREG 0x8018  // Control REG
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
            ioram[ab.w & 0x0fff] = PORTC;
        }

        // Release MRDY (D-FF reset)
        G3POL = 1;
        G3POL = 0;
        return;
    }

    // I/O read cycle
    TRISC = 0x00;               // Set data bus as output
    if (ab.w == UART_CREG)      // UART CTL
        LATC = PIR9;            // Out PIR9
    else if (ab.w == UART_DREG) // UART RX
        LATC = U3RXB;           // Out U3RXB
    else                        // Read from I/O RAM
        LATC = ioram[ab.w & 0x0fff];

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

    // /RESET (RE2) output pin
    ANSELE2 = 0;	// Disable analog function
    LATE2 = 0;		// Reset
    TRISE2 = 0;		// Set as output

    // /DMA,BREQ (RE0) output pin
    ANSELE0 = 0;	// Disable analog function
    LATE0 = 1;		// BUS not request
    TRISE0 = 0;		// Set as output

    // Address bus A15-A8 pin
    ANSELD = 0x00;	// Disable analog function
    WPUD = 0xff;    // Weak pull up
    TRISD = 0xff;	// Set as input

    // Address bus A7-A0 pin
    ANSELB = 0x00;	// Disable analog function
    WPUB = 0xff;	// Weak pull up
    TRISB = 0xff;	// Set as input

    // Data bus D7-D0 pin
    ANSELC = 0x00;	// Disable analog function
    LATC = 0x00;
    TRISC = 0x00;	// Set as output

    // CPU clock(RE1) by NCO FDC mode
    RE1PPS = 0x41;	// RE1 asign NCO3
    ANSELE1 = 0;	// Disable analog function
    TRISE1 = 0;		// NCO output pin
    NCO3INC = (2UL /*MHz*/ * 32768UL); // slow down for memcpy
    NCO3CLK = 0x00;	// Clock source Fosc = 64MHz
    NCO3PFM = 0; 	// Fixed Duty Cycle mode
    NCO3OUT = 1; 	// NCO output enable
    NCO3EN = 1;		// NCO enable

    // MRDY (RA0) output pin (Low = Halt)
    ANSELA0 = 0;	// Disable analog function
    RA0PPS = 0x00;	// LATA0 -> RA0
    LATA0 = 1;		// MRDY ready
    TRISA0 = 0;		// Set as output

    // E (RA1) input pin
    ANSELA1 = 0;	// Disable analog function
    WPUA1 = 1;		// Weak pull up
    TRISA1 = 1;		// Set as input

    // R/W (RA4) input pin
    ANSELA4 = 0;	// Disable analog function
    WPUA4 = 1;		// Weak pull up
    TRISA4 = 1;		// Set as input

    // Bank (RA3) output pin
    ANSELA3 = 0;	// Disable analog function
    RA3PPS = 0x00;	// LATA3 -> RA3
    LATA3 = 0;		// Bank 0
    TRISA3 = 0;		// Set as output

    // /WE (RA2) output pin
    ANSELA2 = 0;	// Disable analog function
    RA2PPS = 0x00;	// LATA2 -> RA2
    LATA2 = 1;		// Disable WE
    TRISA2 = 0;		// Set as output

    // /OE (RA5) output pin
    ANSELA5 = 0;	// Disable analog function
    RA5PPS = 0x00;	// LATA5 -> RA5
    LATA5 = 1;		// Disable OE
    TRISA5 = 0;		// Set as output

    // UART3 initialize
    U3BRG = UART_BPS;
    U3RXEN = 1;		// Receiver enable
    U3TXEN = 1;		// Transmitter enable

    // UART3 Receiver
    ANSELA7 = 0;	// Disable analog function
    TRISA7 = 1;		// RX set as input
    U3RXPPS = 0x07;	//RA7->UART3:RX3;

    // UART3 Transmitter
    ANSELA6 = 0;	// Disable analog function
    LATA6 = 1;		// Default level
    TRISA6 = 0;		// TX set as output
    RA6PPS = 0x26;	//RA6->UART3:TX3;

    U3ON = 1;		// Serial port enable

    // Upload PIC Flash ROM -> CPU RAM
    for (int bank = 2; bank > 0; bank--) {
        unsigned int i,j;
        i = 0;
        do {
            // E
            while(RA1);
            while(!RA1);
            LATE0 = 0; // /DMA,BREQ: Request BUS
            while(RA1);
            while(!RA1);
            while(RA1);

            // A15-A8,A7-A0: Set as output
            TRISD = 0x00;
            TRISB = 0x00;

            LATA3 = (__bit)(bank-1);
            for (j = 0; j < 32; j++) {
                ab.w = ROM_TOP + i;
                LATD = ab.h;
                LATB = ab.l;
                LATA2 = 0;     // /WE = 0
                LATC = rom[i]; // from PIC Flash ROM
                i++;
                LATA2 = 1;     // /WE = 1
            }

            // A15-A8,A7-A0: Set as input
            TRISD = 0xff;
            TRISB = 0xff;

            // E
            while(RA1);
            while(!RA1);
            LATE0 = 1; // /DMA,BREQ: Release BUS
            __delay_us(30);
        } while (i < ROM_SIZE);
    }

    // Address bus A15-A8 pin
    ANSELD = 0x00;	// Disable analog function
    WPUD = 0xff;    // Weak pull up
    TRISD = 0xff;	// Set as input

    // Address bus A7-A0 pin
    ANSELB = 0x00;	// Disable analog function
    WPUB = 0xff;	// Weak pull up
    TRISB = 0xff;	// Set as input

    // Data bus D7-D0: Set as input
    ANSELC = 0x00; // Disable analog function
    WPUC = 0xff;   // Weak pull up
    TRISC = 0xff;  // Set as input

    // CPU clock
    NCO3EN = 0;            // NCO disable
    NCO3INC = CPU_CLK_INC; // full speed
    NCO3EN = 1;            // NCO enable

    //==========    CPU info    ===========
    const char *cpu_info = "\r\n" CPU_CLK_STR "\r\n";
    while (*cpu_info != '\0') {
        putch(*cpu_info);
        ++cpu_info;
    }

    //========== Clear all CLC outs ==========
    CLCDATA = 0x07; // 0b0000_0111

    //========== CLC input pin assign ===========
    // CLCx Input 1,2,5,6: only PortA or C
    // CLCx Input 3,4,7,8: only PortB or D
    // PortA: 0b00_000_xxx
    // PortB: 0b00_001_xxx
    // PortC: 0b00_010_xxx
    // PortD: 0b00_011_xxx
    // PortE: cannot be connected to CLC inputs/outputs
    CLCIN0PPS = 0x01; // CLCIN0PPS <- RA1 <- E
//  CLCIN1PPS = 0x1b; // CLCIN1PPS <- RD3 <- A11

    CLCIN2PPS = 0x1f; // CLCIN2PPS <- RD7 <- A15
    CLCIN3PPS = 0x1e; // CLCIN3PPS <- RD6 <- A14

    CLCIN4PPS = 0x04; // CLCIN4PPS <- RA4 <- R/W
    CLCIN5PPS = 0x10; // CLCIN5PPS <- RC0 <- D0

    CLCIN6PPS = 0x1d; // CLCIN6PPS <- RD5 <- A13
    CLCIN7PPS = 0x1c; // CLCIN7PPS <- RD4 <- A12

    //========== CLC1 RAM,ROM /OE ==========
    CLCSELECT = 0;   // Select CLC1

    CLCnSEL0 = 53;   // CLC3: /IORQ
    CLCnSEL1 = 4;    // CLCIN4PPS <- R/W
    CLCnSEL2 = 0;    // CLCIN0PPS <- E
    CLCnSEL3 = 55;   // CLC5: MRDY

    CLCnGLS0 = 0x02; // /IORQ
    CLCnGLS1 = 0x08; // R
    CLCnGLS2 = 0x20; // E
    CLCnGLS3 = 0x80; // MRDY

    CLCnPOL = 0x80;  // inverted the output of the logic cell.
    CLCnCON = 0x82;  // 4 input AND

    //========== CLC2 RAM /WE ==========
    CLCSELECT = 1;   // Select CLC2

    CLCnSEL0 = 57;   // CLC7: RAM
    CLCnSEL1 = 4;    // CLCIN4PPS <- R/W
    CLCnSEL2 = 0;    // CLCIN0PPS <- E
    CLCnSEL3 = 55;   // CLC5: MRDY

    CLCnGLS0 = 0x02; // RAM
    CLCnGLS1 = 0x04; // !/W
    CLCnGLS2 = 0x20; // E
    CLCnGLS3 = 0x80; // MRDY

    CLCnPOL = 0x80;  // inverted the output of the logic cell.
    CLCnCON = 0x82;  // 4 input AND

    // ----------------------------------------------------------------------
    // address decoder:
    //
    // 0000-dfff: RAM
    // e000-efff: I/O RAM & I/O regs
    // f000-ffff: Read: ROM, Write: Bank switch(0 or 1)
    // ----------------------------------------------------------------------

    //========== CLC3 I/O /IORQ ==========
    CLCSELECT = 2;   // Select CLC3

    CLCnSEL0 = 2;    // CLCIN2PPS <- A15
    CLCnSEL1 = 3;    // CLCIN3PPS <- A14
    CLCnSEL2 = 6;    // CLCIN6PPS <- A13
    CLCnSEL3 = 7;    // CLCIN7PPS <- A12

    // $8xxx
    CLCnGLS0 = 0x02; // A15=1
    CLCnGLS1 = 0x04; // A14=0
    CLCnGLS2 = 0x10; // A13=0
    CLCnGLS3 = 0x40; // A12=0
    // $exxx
    /*
    CLCnGLS0 = 0x02; // A15=1
    CLCnGLS1 = 0x08; // A14=1
    CLCnGLS2 = 0x20; // A13=1
    CLCnGLS3 = 0x40; // A12=0
    */

    CLCnPOL = 0x80;  // inverted the output of the logic cell.
    CLCnCON = 0x82;  // 4 input AND

    //========== CLC4 /BANK switch ==========
    CLCSELECT = 3;   // Select CLC4

    CLCnSEL0 = 2;    // CLCIN2PPS <- A15
    CLCnSEL1 = 3;    // CLCIN3PPS <- A14
    CLCnSEL2 = 6;    // CLCIN6PPS <- A13
    CLCnSEL3 = 7;    // CLCIN7PPS <- A12

    // $fxxx
    CLCnGLS0 = 0x02; // A15=1
    CLCnGLS1 = 0x08; // A14=1
    CLCnGLS2 = 0x20; // A13=1
    CLCnGLS3 = 0x80; // A12=1

    CLCnPOL = 0x80;  // inverted the output of the logic cell.
    CLCnCON = 0x82;  // 4 input AND

    //========== CLC7 RAM ==========
    CLCSELECT = 6;   // Select CLC7

    CLCnSEL0 = 2;    // CLCIN2PPS <- A15
    CLCnSEL1 = 3;    // CLCIN3PPS <- A14
    CLCnSEL2 = 6;    // CLCIN6PPS <- A13
    CLCnSEL3 = 127;  // N/C

    // $0000-$dfff = !($exxx,$fxxx)
    CLCnGLS0 = 0x02; // A15=1
    CLCnGLS1 = 0x08; // A14=1
    CLCnGLS2 = 0x20; // A13=1
    CLCnGLS3 = 0x00; // not gated

    CLCnPOL = 0x88;  // inverted the output of the logic cell. The gate4 outputs '1'.
    CLCnCON = 0x82;  // 4 input AND

    // ----------------------------------------------------------------------
    // D-FF
    // ----------------------------------------------------------------------

    //========== CLC6 RAM A16(Bank) ==========
    CLCSELECT = 5;   // Select CLC6

    CLCnSEL0 = 54;   // CLC4: /BANK
    CLCnSEL1 = 4;    // CLCIN4PPS <- R/W
    CLCnSEL2 = 0;    // CLCIN0PPS <- E
    CLCnSEL3 = 5;    // CLCIN5PPS <- D0

    CLCnGLS0 = 0x1a; // D-FF CLK <- inv(!/BANK & !/W & E) (falling edge) = (/BANK | /W | !E )
    CLCnGLS1 = 0x80; // D-FF D   <- D0
    CLCnGLS2 = 0x00; // D-FF R   <- not gated ('0')
    CLCnGLS3 = 0x00; // D-FF S   <- not gated ('0')

    CLCnPOL = 0x00;  // not inverted all
    CLCnCON = 0x84;  // Select D-FF

    //========== CLC5 I/O MRDY ==========
    CLCSELECT = 4;   // Select CLC5

    CLCnSEL0 = 0;    // CLCIN0PPS <- E
    CLCnSEL1 = 53;   // CLC3: /IORQ
    CLCnSEL2 = 127;  // N/C
    CLCnSEL3 = 127;  // N/C

    CLCnGLS0 = 0x01; // D-FF CLK <- inv!E = E (pos edge)
    CLCnGLS1 = 0x04; // D-FF D   <- !/IORQ
    CLCnGLS2 = 0x00; // D-FF R   <- not gated ('0') <- G3POL
    CLCnGLS3 = 0x00; // D-FF S   <- not gated ('0')

    CLCnPOL = 0x81;  // inverted the output of the logic cell. inverted D-FF CLK.
    CLCnCON = 0x8c;  // Select D-FF, falling edge inturrupt

    //========== CLC output pin assign ===========
    // CLCxOUT: 0x01-0x08
    // CLCx Output 1,2,5,6: only PortA or C
    // CLCx Output 3,4,7,8: only PortB or D
    RA5PPS = 0x01;    // CLC1OUT -> RA5 -> /OE
    RA2PPS = 0x02;    // CLC2OUT -> RA2 -> /WE
    RA0PPS = 0x05;    // CLC5OUT -> RA0 -> MRDY
    RA3PPS = 0x06;    // CLC6OUT -> RA3 -> A16(Bank)

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

    // Enable CLC VI
    CLC5IF = 0; // Clear the CLC5 interrupt flag
    CLC5IE = 1; // Enabling CLC5 interrupt

    // Start CPU
    GIE = 1;   // Enable global interrupt
    LATE2 = 1; // Release reset

    // All things come to those who wait
    while(1);
}
