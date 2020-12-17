/*
 * File:   UART2.c
 * Author: Rushi V
 *
 * Created on October 18, 2020, 9:32 PM
 */

#include <xc.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

#include "UART2.h"

unsigned int clkval;
bool doneflag = false;


void initUART2(void)
/* Initialization of UART 2 module.  Call this before any function below*/
{
	// Configure UART2 module on pins RB0 (Tx) and RB1 (Rx) on PIC24F16KA101.
	// Enable UART2.
	// Set to Baud 4800 with 500kHz clock on PIC24F.

	doneflag = false;
//	if (U2MODEbits.UARTEN) return;  // Skip if it's already on

	TRISBbits.TRISB0 = 0;
	TRISBbits.TRISB1 = 1;
	LATBbits.LATB0   = 1;

	// Configure U2MODE
	U2MODE = 0b0000000000001000;
	/*
	U2MODEbits.UARTEN = 0;  // Bit15 Tx, Rx DISABLED, ENABLE at end of func
	U2MODEbits.USIDL  = 0;  // Bit13 Continue in Idle
	U2MODEbits.IREN   = 0;  // Bit12 No IR translation
	U2MODEbits.RTSMD  = 0;  // Bit11 Simplex Mode
	U2MODEbits.UEN    = 0;  // Bits8,9 Tx, Rx enabled; CTS, RTS not
	U2MODEbits.WAKE   = 0;  // Bit7 No Wake up (since we don't sleep here)
	U2MODEbits.LPBACK = 0;  // Bit6 No Loop Back
	U2MODEbits.ABAUD  = 0;  // Bit5 No Autobaud (would require sending '55')
	U2MODEbits.RXINV  = 0;  // Bit4 IdleState = 1
	U2MODEbits.BRGH   = 1;  // Bit3 16 clocks per bit period
	U2MODEbits.PDSEL  = 0;  // Bits1,2 8bit, No Parity
	U2MODEbits.STSEL  = 0;  // Bit0 One Stop Bit
	 */

	setBaudUART2();

	// Load all values in for U1STA SFR
	U2STA = 0b0010000000000000;
	/*
	U2STAbits.UTXISEL1 = 0;  // Bit15 Int when Char is transferred (1/2 config!)
	U2STAbits.UTXISEL0 = 1;  // Generate interrupt with last character shifted out of U2TXREG buffer.
	U2STAbits.UTXINV   = 0;  // Bit14 N/A, IRDA config
	U2STAbits.UTXBRK   = 0;  // Bit11 Disabled
	U2STAbits.UTXEN    = 0;  // Bit10 Tx pins controlled by periph
	U2STAbits.UTXBF    = 0;  // Bit9 *Read Only Bit*
	U2STAbits.TRMT     = 0;  // Bit8 *Read Only bit*
	U2STAbits.URXISEL  = 0;  // Bits6,7 Int. on character received
	U2STAbits.ADDEN    = 0;  // Bit5 Address Detect Disabled
	U2STAbits.RIDLE    = 0;  // Bit4 *Read Only Bit*
	U2STAbits.PERR     = 0;  // Bit3 *Read Only Bit*
	U2STAbits.FERR     = 0;  // Bit2 *Read Only Bit*
	U2STAbits.OERR     = 0;  // Bit1 *Read Only Bit*
	U2STAbits.URXDA    = 0;  // Bit0 *Read Only Bit*
	 */

	IFS1bits.U2TXIF = 0;     // Clear the Transmit Interrupt Flag
	IPC7bits.U2TXIP = 3;     // UART2 Tx interrupt has interrupt priority 3-4th highest priority.
	setTxIntrModeUART2(TX_BUF_AVAIL); // Interrupt when there is room in the transmit buffer

	IFS1bits.U2RXIF = 0;     // Clear the Receive Interrupt Flag
	IPC7bits.U2RXIP = 4;     // UART2 Rx interrupt has 2nd highest priority
	IEC1bits.U2RXIE = 0;     // Disable Receive Interrupts

	U2MODEbits.UARTEN = 1;   // And turn the peripheral on
	U2STAbits.UTXEN = 1;
	return;
}


inline void setTxIntrModeUART2(const enum UART_Tx_Intr_Mode mode)
/* Set the transmit interrupt mode bits for UART2 */
{
	switch (mode) {
	case TX_BUF_EMPTY:
		U2STAbits.UTXISEL0 = 0;
		U2STAbits.UTXISEL1 = 1;
		break;
	case TX_BUF_AVAIL:
		U2STAbits.UTXISEL0 = 0;
		U2STAbits.UTXISEL1 = 0;
		break;
	case TX_DONE:
		U2STAbits.UTXISEL0 = 1;
		U2STAbits.UTXISEL1 = 0;
	}
	IEC1bits.U2TXIE = 1;  // Enable Transmit Interrupts
}


void setBaudUART2(void)
/* Set the baud rate for UART2 based on the CPU clock.
 * 32kHz  clock - Baud=300
 * 500kHz clock - Baud=4800
 * 8MHz   clock - Baud=9600
 */
{
	U2MODEbits.UARTEN = 0;  // Turn it off first

	if (OSCCONbits.COSC == 0b110) {
		// Gives a baud rate of 4807.7 Baud with 500kHz clock
		// Set Baud to 4800 on realterm
		U2BRG = 12;
	} else if (OSCCONbits.COSC == 0b101) {
		// Gives a baud rate of 300 Baud with 32kHz clock
		// Set Baud to 300 on realterm
		U2BRG = 12;
	} else if (OSCCONbits.COSC == 0b000) {
		U2BRG = 103;  // Gives a baud rate of 9600 with 8MHz clock
		// Set Baud to 9600 on realterm
	}
}

void endUART2(void)
/* Gracefully begin to close the UART2 module in a non-blocking way.
 * Call this after a block of writes is finished.
 */
{
	if (!U2MODEbits.UARTEN) return;
	doneflag = true;
	setTxIntrModeUART2(TX_DONE);
	while (!U2STAbits.TRMT) Idle();
	U2MODEbits.UARTEN = 0;
}


void writeUART2(char c)
/* Write one character c over the UART2 interface.
 * This function relies on the UART interface already running.
 */
{
	if (!c) return;
	while (U2STAbits.UTXBF) {
		setTxIntrModeUART2(TX_BUF_AVAIL);
		Idle();
	}
	U2TXREG = c;
}


void nWriteUART2(char c, unsigned int n)
/* Write character c over the UART2 interface, n times. */
{
	if (!n || !c) return;
	while (n-- > 0)
		writeUART2(c);
}


void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt(void)
{
//	LATA = U2RXREG;
	IFS1bits.U2RXIF = 0;
}


void __attribute__((interrupt, no_auto_psv)) _U2TXInterrupt(void)
{
	IFS1bits.U2TXIF = 0;  // Clear the interrupt flag
	IEC1bits.U2TXIE = 1;  // Disable transmit interrupts

	// If the TSR and Tx buffer are empty, turn off the module
	if (doneflag && U2STAbits.TRMT) {
		U2MODEbits.UARTEN = 0;
		LATBbits.LATB9 = 1;
		doneflag = false;
	}
}


void printUART2(char *str)
/* Display a string of characters */
{
	if (!str) return;

 //	nWriteUART2('\n', 2);
//	writeUART2('\r');

	unsigned int i = 0;
	while (str[i])
		writeUART2(str[i++]);

//	nWriteUART2('\n', 2);
//	writeUART2('\r');
}


void Disp2Hex(unsigned int DispData)
/* Display 16 bit number in Hex form using UART2 */
{
	char i;
	char nib = 0x00;
	writeUART2(' ');  // Disp Gap
	writeUART2('0');  // Disp Hex notation 0x
	writeUART2('x');

	for (i = 3; i >= 0; i--) {
		nib = ((DispData >> (4 * i)) & 0x000F);
		if (nib >= 0x0A) {
			nib = nib + 0x37;  // For Hex values A-F
		} else {
			nib = nib + 0x30;  // For hex values 0-9
		}
		writeUART2(nib);
	}

	writeUART2(' ');
	DispData = 0x0000;  // Clear DispData
	return;
}


void Disp2Hex32(unsigned long int DispData32)
/* Display 32 bit number in Hex form using UART2 */
{
	char i;
	char nib = 0x00;
	writeUART2(' ');  // Disp Gap
	writeUART2('0');  // Disp Hex notation 0x
	writeUART2('x');

	for (i = 7; i >= 0; i--) {
		nib = ((DispData32 >> (4 * i)) & 0x000F);
		if (nib >= 0x0A) {
			nib = nib + 0x37;  // For Hex values A-F
		} else {
			nib = nib + 0x30;  // For hex values 0-9
		}
		writeUART2(nib);
	}

	writeUART2(' ');
	DispData32 = 0x00000000;   // Clear DispData
	return;
}


void printUint(uint32_t val, int digits, bool pad_zeros)
/* Display a 32-bit unsigned int in decimal form */
{
	uint8_t rem;        // Remainder in div by 10
	uint16_t quot;

	// If we're not padding, print at least one zero
	if (!pad_zeros && val == 0) {
		writeUART2('0');
		return;
	}

	while (digits > 0) {
		quot = val / pow(10, --digits);
		rem = quot % 10;

		// Omit leading zeroes
		if (rem == 0) {
			if (pad_zeros)
				writeUART2('0');
		} else {
			pad_zeros = true;
			writeUART2(rem + 0x30);
		}
	}
}


void printFloat(float val)
/* Display a 32-bit floating point number in decimal form.
 * Currently supports only 1 digit before and 4 after the decimal. */
// TODO: improve this?
{
	uint32_t whole = (uint32_t)val;
	printUint((uint32_t)val, 10, false);  // Print the integer part first
	writeUART2('.');
	printUint((uint32_t)(1000*(val - whole) + 0.5), 3, true);  // Add 0.5 for rounding
}

void clearLine(void)
/* Clear the previous line on the terminal. */
{
	writeUART2('\r');
	nWriteUART2(' ', 80);  // Clear the previous line
	writeUART2('\r');
}