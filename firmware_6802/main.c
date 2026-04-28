/*
    PIC18F47Q84 ROM, RAM and UART emulation firmware

    Target: EMUZ80 with 6802
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
#define CPU_CLK_MHz 8UL
#define CPU_CLK_STR "MEZ6802 2.0MHz"
// INC = (cpu[MHz] * 2^20) * 2 / 64[MHz] = (cpu * 2^20) / 32 = cpu * 1,048,576/32 = cpu * 32768
#define CPU_CLK_INC (CPU_CLK_MHz * 32768UL)

// ROM equivalent: Max 16KB
#define ROM_TOP  0xf000
#define ROM_SIZE 0x2000
const unsigned char rom[ROM_SIZE] __at(0xe000) = {
#include "../ROM/ROM8KB.h"
};

// RAM
#define RAM_TOP  0x0000
#define RAM_SIZE 0x1000
unsigned char ram[RAM_SIZE];

// UART
#define UART_SREG 0xe000 // Status REG
#define UART_DREG 0xe001 // Data REG
//#define UART_BPS  0x1a0  //  9600bps @ 64MHz
//#define UART_BPS  0xd0   // 19200bps @ 64MHz
#define UART_BPS  0x68   // 38400bps @ 64MHz

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
    if (!RA1) {
        if (ab.w == UART_DREG) {
            // Write into U3TXB
            U3TXB = PORTC;
        } else if (ab.w != UART_SREG) {
            // Write into RAM
            ram[ab.w & (RAM_SIZE - 1)] = PORTC;
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
    else if (ab.w >= ROM_TOP)   // Read from ROM
        LATC = rom[ab.w - (ROM_TOP - RAM_SIZE)];
    else                        // Read from RAM
        LATC = ram[ab.w & (RAM_SIZE - 1)];

    // Detect CLK rising edge
    while(!RE0);
    NOP(); // wait 62.5ns

    // Release MRDY (D-FF reset)
    G3POL = 1;
    G3POL = 0;

#if 0
    NOP();      // wait 62.5ns
#else
    while(RA0); // Detect E falling edge <2.75MHz (11MHz)
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

    // /NMI (RE1) output pin
    ANSELE1 = 0;	// Disable analog function
    LATE1 = 1;		// NMI inactive (High)
    TRISE1 = 0;		// Set as output

    // /IRQ (RA4) output pin
    ANSELA4 = 0;	// Disable analog function
    RA4PPS = 0x00;	// LATA4 -> RA4
    LATA4 = 1;		// IRQ inactive (High)
    TRISA4 = 0;		// Set as output

    // Address bus A15-A8 pin
    ANSELD = 0x00;	// Disable analog function
    WPUD = 0xff;    // Weak pull up
    TRISD = 0xff;	// Set as input

    // Address bus A7-A0 pin
    ANSELB = 0x00;	// Disable analog function
    WPUB = 0xff;	// Weak pull up
    TRISB = 0xff;	// Set as input

    // Data bus D7-D0 pin
    ANSELC = 0x00; // Disable analog function
    WPUC = 0xff;   // Weak pull up
    TRISC = 0xff;  // Set as input

    // CPU clock(RE0) by NCO FDC mode
    RE0PPS = 0x41;	// RE0 assign NCO3
    ANSELE0 = 0;	// Disable analog function
    TRISE0 = 0;		// NCO output pin
    NCO3INC = CPU_CLK_INC; // full speed
    NCO3CLK = 0x00;	// Clock source Fosc = 64MHz
    NCO3PFM = 0; 	// Fixed Duty Cycle mode
    NCO3OUT = 1; 	// NCO output enable
    NCO3EN = 1;		// NCO enable

    // MRDY (RA5) output pin (Low = Halt)
    ANSELA5 = 0;	// Disable analog function
    RA5PPS = 0x00;	// LATA5 -> RA5
    LATA5 = 1;		// MRDY ready
    TRISA5 = 0;		// Set as output

    // E (RA0) input pin
    ANSELA0 = 0;	// Disable analog function
    WPUA0 = 1;		// Weak pull up
    TRISA0 = 1;		// Set as input

    // R/W (RA1) input pin
    ANSELA1 = 0;	// Disable analog function
    WPUA1 = 1;		// Weak pull up
    TRISA1 = 1;		// Set as input

    // VMA (RA2) input pin
    ANSELA2 = 0;	// Disable analog function
    WPUA2 = 1;		// Weak pull up
    TRISA2 = 1;		// Set as input

    // Bank (RA3) output pin
    ANSELA3 = 0;	// Disable analog function
    RA3PPS = 0x00;	// LATA3 -> RA3
    LATA3 = 0;		// Bank 0
    TRISA3 = 0;		// Set as output

    // UART3 initialize
    U3BRG = UART_BPS;
    U3RXEN = 1;		// Receiver enable
    U3TXEN = 1;		// Transmitter enable

    // UART3 (RA7) Receiver
    ANSELA7 = 0;	// Disable analog function
    TRISA7 = 1;		// RX set as input
    U3RXPPS = 0x07;	// RA7->UART3:RX3;

    // UART3 (RA6) Transmitter
    ANSELA6 = 0;	// Disable analog function
    LATA6 = 1;		// Default level
    TRISA6 = 0;		// TX set as output
    RA6PPS = 0x26;	// RA6->UART3:TX3;

    U3ON = 1;		// Serial port enable

    // Copy PIC Flash ROM -> RAM
    for (unsigned int i = 0; i < RAM_SIZE; i++) {
        ram[i] = rom[i];
    }

    //==========    CPU info    ===========
    const char *cpu_info = "\r\n" CPU_CLK_STR "\r\n";
    while (*cpu_info != '\0') {
        putch(*cpu_info);
        ++cpu_info;
    }

    //========== Clear all CLC outs ==========
    CLCDATA = 0xc4; // 0b1100_0100

    //========== CLC input pin assign ===========
    // CLCx Input 1,2,5,6: only PortA or C
    // CLCx Input 3,4,7,8: only PortB or D
    // PortA: 0b00_000_xxx
    // PortB: 0b00_001_xxx
    // PortC: 0b00_010_xxx
    // PortD: 0b00_011_xxx
    // PortE: cannot be connected to CLC inputs/outputs
    CLCIN0PPS = 0x00; // CLCIN0PPS <- RA0 <- E
    CLCIN1PPS = 0x02; // CLCIN1PPS <- RA2 <- VMA

    CLCIN2PPS = 0x1f; // CLCIN2PPS <- RD7 <- A15
    CLCIN3PPS = 0x1e; // CLCIN3PPS <- RD6 <- A14

    CLCIN4PPS = 0x01; // CLCIN4PPS <- RA1 <- R/W
    CLCIN5PPS = 0x10; // CLCIN5PPS <- RC0 <- D0

    CLCIN6PPS = 0x1d; // CLCIN6PPS <- RD5 <- A13
    CLCIN7PPS = 0x1c; // CLCIN7PPS <- RD4 <- A12

    // ----------------------------------------------------------------------
    // address decoder:
    //
    // 0000-0fff: RAM
    // e000-efff: I/O regs
    // f000-ffff: ROM
    // ----------------------------------------------------------------------

    //========== CLC3 I/O /IORQ ==========
    CLCSELECT = 2;   // Select CLC3

    CLCnSEL0 = 2;    // CLCIN2PPS <- A15
    CLCnSEL1 = 3;    // CLCIN3PPS <- A14
    CLCnSEL2 = 6;    // CLCIN6PPS <- A13
    CLCnSEL3 = 7;    // CLCIN7PPS <- A12

    // $exxx
    CLCnGLS0 = 0x02; // A15=1
    CLCnGLS1 = 0x08; // A14=1
    CLCnGLS2 = 0x20; // A13=1
    CLCnGLS3 = 0x40; // A12=0

    CLCnPOL = 0x80;  // inverted the output of the logic cell.
    CLCnCON = 0x82;  // 4 input AND

    //========== CLC7 /RAM ==========
    CLCSELECT = 6;   // Select CLC7

    CLCnSEL0 = 2;    // CLCIN2PPS <- A15
    CLCnSEL1 = 3;    // CLCIN3PPS <- A14
    CLCnSEL2 = 6;    // CLCIN6PPS <- A13
    CLCnSEL3 = 7;    // CLCIN7PPS <- A12

    // $0xxx
    CLCnGLS0 = 0x01; // A15=0
    CLCnGLS1 = 0x04; // A14=0
    CLCnGLS2 = 0x10; // A13=0
    CLCnGLS3 = 0x40; // A12=0

    CLCnPOL = 0x80;  // inverted the output of the logic cell.
    CLCnCON = 0x82;  // 4 input AND

    //========== CLC8 /ROM ==========
    CLCSELECT = 7;   // Select CLC8

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

    // ----------------------------------------------------------------------
    // D-FF
    // ----------------------------------------------------------------------

    //========== CLC5 MRDY ==========
    CLCSELECT = 4;   // Select CLC5

    CLCnSEL0 = 0;    // CLCIN0PPS <- E
    CLCnSEL1 = 1;    // CLCIN1PPS <- VMA
    CLCnSEL2 = 127;  // N/C
    CLCnSEL3 = 127;  // N/C

    CLCnGLS0 = 0x01; // D-FF CLK <- inv!E = E (pos edge)
    CLCnGLS1 = 0x08; // D-FF D   <- VMA
    CLCnGLS2 = 0x00; // D-FF R   <- not gated ('0') <- G3POL
    CLCnGLS3 = 0x00; // D-FF S   <- not gated ('0')

    CLCnPOL = 0x81;  // inverted the output of the logic cell. inverted D-FF CLK.
    CLCnCON = 0x8c;  // Select D-FF, falling edge inturrupt

    //========== CLC output pin assign ===========
    // CLCxOUT: 0x01-0x08
    // CLCx Output 1,2,5,6: only PortA or C
    // CLCx Output 3,4,7,8: only PortB or D
    RA5PPS = 0x05;    // CLC5OUT -> RA5 -> MRDY

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
