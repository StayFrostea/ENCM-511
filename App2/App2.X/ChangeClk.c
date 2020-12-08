#include <xc.h>
#include "ChangeClk.h"

/* clkval = 8 for 8MHz;
 * clkval = 500 for 500kHz;
 * clkval = 32 for 32kHz;
 */

void NewClk(unsigned int clkval)
{
	char COSCNOSC;
	if (clkval == 8) {
		COSCNOSC = 0x00;         // 8MHz
	} else if (clkval == 500) {
		COSCNOSC = 0x66;         // 500 kHz
	} else if (clkval == 32) {
		COSCNOSC = 0x55;         // 32 kHz
	} else {
		COSCNOSC = 0x55;         // Default 32 kHz
	}

	// Switch clock to 500 kHz
	SRbits.IPL = 7;	       // Disable interrupts
	CLKDIVbits.RCDIV = 0;  // CLK division = 0

	// (0x00) for 8MHz; (0x66) for 500kHz; (0x55) for 32kHz;
	__builtin_write_OSCCONH(COSCNOSC);
	__builtin_write_OSCCONL(0x01);
	OSCCONbits.OSWEN = 1;
	while (OSCCONbits.OSWEN == 1) ;
	SRbits.IPL = 0;  // Enable interrupts
}
