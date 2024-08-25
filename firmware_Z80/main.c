/*
    PIC18F47Q43 ROM image uploader, I/O RAM and UART emulation firmware

    Target: EMUZ80 with Z80+RAM
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

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>

// CPU clock: Max 8MHz
#if 0
#define CPU_CLK_MHz 6UL
#define CPU_CLK_STR "MEZ80RAM 6.0MHz"
#else
#define CPU_CLK_MHz 8UL
#define CPU_CLK_STR "MEZ80RAM 8.0MHz"
#endif
// INC = (cpu[MHz] * 2^20) * 2 / 64[MHz] = (cpu * 2^20) / 32 = cpu * 1,048,576/32 = cpu * 32768
#define CPU_CLK_INC (CPU_CLK_MHz * 32768UL)

// ROM equivalent: Max 32KB
#define ROM_SIZE 0x8000
const unsigned char rom[ROM_SIZE-1] __at(0x8000) = { // -1: compiler bug?
//#include "../ROM/ROM32KB.h"
#include "../ROM/ROM32KB_monitor_BASIC_TinyBASIC_GAME80IC_VTL.h"
};

// UART
#define UART_DREG 0x00  // Data REG
#define UART_CREG 0x01  // Control REG
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

// Called at /WAIT falling edge(Immediately after Z80 /IORQ falling)
void __interrupt(irq(CLC3),base(8)) CLC_ISR(){
    ab.l = PORTB; // Read IO address low

    // Z80 IO write cycle
    if (RA5) {
        if(ab.l == UART_DREG) // UART TX
            U3TXB = PORTC;    // Write into U3TXB
        // Release wait (D-FF reset)
        G3POL = 1;
        G3POL = 0;

        // Clear interrupt flag
        CLC3IF = 0;

        return;
    }

    // Z80 IO read cycle
    TRISC = 0x00;               // Set data bus as output
    if (ab.l == UART_CREG)      // UART CTL
        LATC = PIR9;            // Out PIR9
    else if (ab.l == UART_DREG) // UART RX
        LATC = U3RXB;           // Out U3RXB
    else                        // Empty
        LATC = 0xff;            // Invalid data

    // Release wait (D-FF reset)
    G3POL = 1;
    G3POL = 0;

    // Post processing
//  while(!RA0);  // Wait for the rising edge /IORQ (CPU <  5.6MHz)
    while(!RD7);  // Wait for the rising edge /WAIT (CPU >= 5.6MHz)
    TRISC = 0xff; // Set data bus as input

    // Clear interrupt flag
    CLC3IF = 0;
}

// main routine
void main(void) {
    unsigned int i;

    // System initialize
    OSCFRQ = 0x08; // 64MHz internal OSC

    // RESET (RE1) output pin
    ANSELE1 = 0;	// Disable analog function
    LATE1 = 0;		// Reset
    TRISE1 = 0;		// Set as output

    // /BUSREQ (RE0) output pin
    ANSELE0 = 0;	// Disable analog function
    LATE0 = 0;		// BUS request
    TRISE0 = 0;		// Set as output

    // Address bus (A15:/WAIT, A14:/RFSH) A13-A8 pin
    ANSELD = 0x00;	// Disable analog function
    LATD = 0x00;    // WAIT
    TRISD = 0x40;	// Set as output (/RFSH: input)

    // A14 (RE2) output pin
    ANSELE2 = 0;	// Disable analog function
    LATE2 = 0;
    TRISE2 = 0;		// Set as output

    // Address bus A7-A0 pin
    ANSELB = 0x00;	// Disable analog function
    LATB = 0x00;
    TRISB = 0x00;	// Set as output

    // Data bus D7-D0 pin
    ANSELC = 0x00;	// Disable analog function
    LATC = 0x00;
    TRISC = 0x00;	// Set as output

    // CPU clock(RA3) by NCO FDC mode
    RA3PPS = 0x3f;	// RA3 asign NCO1
    ANSELA3 = 0;	// Disable analog function
    TRISA3 = 0;		// NCO output pin
    NCO1INC = CPU_CLK_INC;
    NCO1CLK = 0x00;	// Clock source Fosc = 64MHz
    NCO1PFM = 0; 	// Fixed Duty Cycle mode
    NCO1OUT = 1; 	// NCO output enable
    NCO1EN = 1;		// NCO enable

    // /WE (RA2) output pin
    ANSELA2 = 0;	// Disable analog function
    RA2PPS = 0x00;	// LATA2 -> RA2
    LATA2 = 1;		//
    TRISA2 = 0;		// Set as output

    // /OE (RA4) output pin
    ANSELA4 = 0;	// Disable analog function
    LATA4 = 1;
    TRISA4 = 0;		// Set as output

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


    // Upload dummy -> CPU I/O RAM
    // /IORQ (RA0) output pin
    ANSELA0 = 0; // Disable analog function
    LATA0 = 0;   // enable I/O RAM
    TRISA0 = 0;  // Set as output
    for (i = 0x0020; i <= 0x7f20; i += 0x100) {
        ab.w = i;
        LATE2 = ((ab.h>>6) & 1);
        LATD = (ab.h & 0x3f);
        LATB = ab.l;
        LATA2 = 0;   // /WE = 0
        LATC = 0xdb; // debug
        LATA2 = 1;   // /WE = 1
    }
    LATA0 = 1;   // disable I/O RAM

    // Upload PIC Flash ROM -> CPU main RAM
    for (i = 0; i < ROM_SIZE; i++) {
        ab.w = i;
        LATE2 = ((ab.h>>6) & 1);
        LATD = (ab.h & 0x3f);
        LATB = ab.l;
        LATA2 = 0;     // /WE = 0
        LATC = rom[i]; // from PIC Flash ROM
        LATA2 = 1;     // /WE = 1
    }

    // Address bus (A15:/WAIT, A14:/RFSH) A13-A8 pin
    ANSELD = 0x00;	// Disable analog function
    WPUD = 0xff;	// Weak pull up
    TRISD = 0xff;	// Set as input

    // A14 (RE2) input pin
    ANSELE2 = 0;	// Disable analog function
    WPUE2 = 1;		// Weak pull up
    TRISE2 = 1;	// Set as input

    // Address bus A7-A0 pin
    ANSELB = 0x00;	// Disable analog function
    WPUB = 0xff;	// Weak pull up
    TRISB = 0xff;	// Set as input

    // Data bus D7-D0 input pin
    ANSELC = 0x00;	// Disable analog function
    WPUC = 0xff;	// Weak pull up
    TRISC = 0xff;	// Set as input

    // /IORQ (RA0) input pin
    ANSELA0 = 0;	// Disable analog function
    WPUA0 = 1;		// Weak pull up
    TRISA0 = 1;		// Set as input

    // /MREQ (RA1) input pin
    ANSELA1 = 0;	// Disable analog function
    WPUA1 = 1;		// Weak pull up
    TRISA1 = 1;		// Set as input

    // /RD (RA5) input pin
    ANSELA5 = 0;	// Disable analog function
    WPUA5 = 1;		// Weak pull up
    TRISA5 = 1;		// Set as input

    // /RFSH (RD6) input pin
    ANSELD6 = 0;	// Disable analog function
    WPUD6 = 1;		// Weak pull up
    TRISD6 = 1;		// Set as input

    // /WAIT (RD7) output pin
    ANSELD7 = 0;	// Disable analog function
    LATD7 = 1;		// not WAIT
    TRISD7 = 0;		// Set as output

    //==========    CPU info    ===========
    const char *cpu_info = "\r\n" CPU_CLK_STR "\r\n";
    while (*cpu_info != '\0') {
        putch(*cpu_info);
        ++cpu_info;
    }

    //========== Clear all CLC outs ==========
    CLCDATA = 0x2d; // 0b0010_1101

    //========== CLC input pin assign ===========
    // CLCx Input 1,2,5,6: only PortA or C
    // CLCx Input 3,4,7,8: only PortB or D
    // PortA: 0b00_000_xxx
    // PortB: 0b00_001_xxx
    // PortC: 0b00_010_xxx
    // PortD: 0b00_011_xxx
    // PortE: cannot be connected to CLC inputs/outputs
    CLCIN0PPS = 0x01; // CLCIN0PPS <- RA1 <- /MREQ
    CLCIN1PPS = 0x00; // CLCIN1PPS <- RA0 <- /IORQ

    CLCIN2PPS = 0x1e; // CLCIN2PPS <- RD6 <- /RFSH
    CLCIN3PPS = 0x0d; // CLCIN3PPS <- RB5 <- A5

    CLCIN4PPS = 0x05; // CLCIN4PPS <- RA5 <- /RD
    //CLCIN5PPS = 0x0c; // CLCIN5PPS <- RB4 <- A4

    CLCIN6PPS = 0x0e; // CLCIN6PPS <- RB6 <- A6
    CLCIN7PPS = 0x0f; // CLCIN7PPS <- RB7 <- A7

    //========== CLC4 address dec ==========
    // 0: port
    // 1: mem
    CLCSELECT = 3;   // Select CLC4

    CLCnSEL0 = 5;    // CLCIN5PPS <- A4
    CLCnSEL1 = 3;    // CLCIN3PPS <- A5
    CLCnSEL2 = 6;    // CLCIN6PPS <- A6
    CLCnSEL3 = 7;    // CLCIN7PPS <- A7

    CLCnGLS0 = 0xa8; // A7 | A6 | A5 (| A4)
    CLCnGLS1 = 0x00; // not gated
    CLCnGLS2 = 0x00; // not gated
    CLCnGLS3 = 0x00; // not gated

    CLCnPOL = 0x0e;  // The gate2,3 and 4 output '1'.
    CLCnCON = 0x82;  // 4 input AND

    //========== CLC5 IOMEM ==========
    CLCSELECT = 4;   // Select CLC5

    CLCnSEL0 = 1;    // CLCIN1PPS <- /IORQ
    CLCnSEL1 = 54;   // CLC4: address dec
    CLCnSEL2 = 127;  // N/C
    CLCnSEL3 = 127;  // N/C

    CLCnGLS0 = 0x01; // !/IORQ
    CLCnGLS1 = 0x08; // address dec (0:port, 1:mem)
    CLCnGLS2 = 0x00; // not gated
    CLCnGLS3 = 0x00; // not gated

    CLCnPOL = 0x0c;  // The gate3 and 4 output '1'.
    CLCnCON = 0x82;  // 4 input AND

    //========== CLC1 RAM /OE ==========
    CLCSELECT = 0;   // Select CLC1

    CLCnSEL0 = 0;    // CLCIN0PPS <- /MREQ
    CLCnSEL1 = 2;    // CLCIN2PPS <- /RFSH
    CLCnSEL2 = 4;    // CLCIN4PPS <- /RD
    CLCnSEL3 = 55;   // CLC5: IOMEM

    CLCnGLS0 = 0x81; // IOMEM | !/MREQ
    CLCnGLS1 = 0x08; // /RFSH
    CLCnGLS2 = 0x10; // !/RD
    CLCnGLS3 = 0x00; // not gated

    CLCnPOL = 0x88;  // inverted the output of the logic cell. The gate4 outputs '1'.
    CLCnCON = 0x82;  // 4 input AND

    //========== CLC2 RAM WE2 ==========
    CLCSELECT = 1;   // Select CLC2

    CLCnSEL0 = 0;    // CLCIN0PPS <- /MREQ
    CLCnSEL1 = 2;    // CLCIN2PPS <- /RFSH
    CLCnSEL2 = 4;    // CLCIN4PPS <- /RD
    CLCnSEL3 = 55;   // CLC5: IOMEM

    CLCnGLS0 = 0x81; // IOMEM | !/MREQ
    CLCnGLS1 = 0x08; // /RFSH
    CLCnGLS2 = 0x20; // /RD
    CLCnGLS3 = 0x00; // not gated

    CLCnPOL = 0x08;  // The gate4 outputs '1'.
    CLCnCON = 0x82;  // 4 input AND

    //========== CLC7 MIORQ ==========
    CLCSELECT = 6;   // Select CLC7

    CLCnSEL0 = 0;    // CLCIN0PPS <- /MREQ
    CLCnSEL1 = 1;    // CLCIN1PPS <- /IORQ
    CLCnSEL2 = 127;  // N/C
    CLCnSEL3 = 127;  // N/C

    CLCnGLS0 = 0x02; // /MREQ
    CLCnGLS1 = 0x08; // /IORQ
    CLCnGLS2 = 0x00; // not gated
    CLCnGLS3 = 0x00; // not gated

    CLCnPOL = 0x8c;  // inverted the output of the logic cell. The gate3 and 4 output '1'.
    CLCnCON = 0x82;  // 4 input AND

    //========== CLC6 RAM /WE ==========
    CLCSELECT = 5;   // Select CLC6

    CLCnSEL0 = 56;   // CLC6: /WE
    CLCnSEL1 = 57;   // CLC7: MIORQ
    CLCnSEL2 = 52;   // CLC2: WE2
    CLCnSEL3 = 42;   // NCO1

    CLCnGLS0 = 0x40; // D-FF CLK <- !NCO1 (falling edge)
    CLCnGLS1 = 0x15; // D-FF D   <- (WE2 & MIORQ & /WE) = inv(!WE2 | !MIORQ | !/WE )
    CLCnGLS2 = 0x00; // D-FF R   <- not gated ('0')
    CLCnGLS3 = 0x00; // D-FF S   <- not gated ('0')

    CLCnPOL = 0x82;  // inverted the output of the logic cell. inverted D-FF D.
    CLCnCON = 0x84;  // Select D-FF

    //========== CLC3 IO /WAIT ==========
    CLCSELECT = 2;   // Select CLC3

    CLCnSEL0 = 1;    // CLCIN1PPS <- /IORQ
    CLCnSEL1 = 3;    // CLCIN3PPS <- A5
    CLCnSEL2 = 6;    // CLCIN6PPS <- A6
    CLCnSEL3 = 7;    // CLCIN7PPS <- A7

    CLCnGLS0 = 0x01; // D-FF CLK <- !/IORQ (falling edge)
    CLCnGLS1 = 0xa8; // D-FF D   <- A7 | A6 | A5
    CLCnGLS2 = 0x00; // D-FF R   <- not gated ('0') <- G3POL
    CLCnGLS3 = 0x00; // D-FF S   <- not gated ('0')

    CLCnPOL = 0x82;  // inverted the output of the logic cell. inverted D-FF D.
    CLCnCON = 0x8c;  // Select D-FF, falling edge inturrupt

    //========== CLC output pin assign ===========
    // CLCxOUT: 0x01-0x08
    // CLCx Output 1,2,5,6: only PortA or C
    // CLCx Output 3,4,7,8: only PortB or D
    RA4PPS = 0x01;    // CLC1OUT -> RA4 -> /OE
    RA2PPS = 0x06;    // CLC6OUT -> RA2 -> /WE
    RD7PPS = 0x03;    // CLC3OUT -> RD7 -> /WAIT

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
    CLC3IF = 0; // Clear the CLC3 interrupt flag
    CLC3IE = 1; // Enabling CLC3 interrupt

    // Start CPU
    GIE = 1;   // Enable global interrupt
    LATE0 = 1; // Release /BUSREQ
    LATE1 = 1; // Release reset

    // All things come to those who wait
    while(1);
}
