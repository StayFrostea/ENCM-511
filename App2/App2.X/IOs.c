/*
 * File:   IOs.c
 * Author: Long Tran, Jeff Roszell, Toshiro Taperek
 */

#include <xc.h>
#include <stdbool.h>
#include "ChangeClk.h"
#include "TimeDelay.h"
#include "IOs.h"
#include "UART2.h"
#include "meter.h"


void initIO(void)
/* Initialize GPIO and CN interrupts */
{
	AD1PCFG = 0xFFFF;		// Set all analog pins to digital
	TRISBbits.TRISB8  = 0;  // Set GPIO RB8 as a digital output (LED)
	LED               = 0;  // Turn the LED off

	TRISAbits.TRISA2  = 1;  // Set GPIO RA2 as a digital input (PB1)
	CNPU2bits.CN30PUE = 1;  // Enable pull up resistor on RA2/CN30
	CNEN2bits.CN30IE  = 1;  // Enable CN on CN30

	TRISAbits.TRISA4  = 1;  // Set GPIO RA4 as a digital input (PB2)
	CNPU1bits.CN0PUE  = 1;  // Enable pull up resistor on RA4/CN0
	CNEN1bits.CN0IE   = 1;  // Enable CN on CN0

	TRISBbits.TRISB4  = 1;  // Set GPIO RB4 as a digital input (PB3)
	CNPU1bits.CN1PUE  = 1;  // Enable pull up resistor on RB4/CN1
	CNEN1bits.CN1IE   = 1;  // Enable CN on CN1

	// CN Interrupt settings
	IPC4bits.CNIP = 2;      // 7 is highest priority, 1 is lowest, 0 is disabled
	IFS1bits.CNIF = 0;      // Clear interrupt flag
	IEC1bits.CNIE = 1;      // Enable CN interrupts
}


/*void initRefOsc(void)
{
	// Clock output on REFO/RB15
	TRISBbits.TRISB15  = 0;       // Set RB15 as output for REFO
	REFOCONbits.ROSSLP = 1;       // Ref oscillator is disabled in sleep
	REFOCONbits.ROSEL  = 0;       // Output base clock showing clock switching
	REFOCONbits.RODIV  = 0b1001;
	REFOCONbits.ROEN   = 1;       // Ref oscillator is enabled
	// TODO: Use a macro or something to set this to what's required
}*/

void initRefOsc(int test_setting)
/* Set up the clock and reference oscillator */
{

	//Clock output on REFO/RB15
	TRISBbits.TRISB15 = 0; // Set RB15 as output for REFO
	REFOCONbits.ROSSLP = 1; // Ref oscillator is disabled in sleep
	REFOCONbits.ROSEL = 0; // Output base clk showing clock switching

	switch(test_setting) {
		case 1:
			///////////////PULSE TEST SETTING 1
			NewClk(8); // 8 for 8 MHz; 500 for 500 kHz; 32 for 32 kHz
			REFOCONbits.RODIV = 0b1001;
			break;

		case 2:
			///////////////PULSE TEST SETTING 2
			NewClk(8);
			REFOCONbits.RODIV = 0b1111;
			break;

		case 3:
			///////////////PULSE TEST SETTING 3
			NewClk(500);
			REFOCONbits.RODIV = 0b1001;
			break;

		case 4:
			///////////////PULSE TEST SETTING 4
			NewClk(500);
			REFOCONbits.RODIV = 0b0111;
	}
	REFOCONbits.ROEN = 1; // Ref oscillator is enabled
}


void debounceCallback(void)
/* Record the state of the buttons after they are finished bouncing.
 * This function is called by the timer's ISR some time after the first
 * change is detected. */
{
	// Freeze the button states so that nothing changes while we're looking at it.
	bool button1 = PB1;
	bool button2 = PB2;
	bool button3 = PB3;

	// Look at the button state and choose which mode to switch to.
	if (button1 && !button2 && !button3) {
		mode = VOLT;
	} else if (!button1 && button2 && !button3) {
		mode = OHM;
	} else if (!button1 && !button2 && button3) {
		mode = PULSE;
	}

	IEC1bits.CNIE = 1;  // Re-enable CN interrupts
}


void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void)
/* Change Notification interrupt subroutine */
{
	IFS1bits.CNIF = 0;  // Clear IF flag
	IEC1bits.CNIE = 0;  // Disable CN interrupts

	/* Start a 50ms timer and return from this ISR.
	 * When the timer goes off, its ISR will record the state of
	 * the buttons and switch the state variable accordingly.
	 */
	delay_ms_callback(50, debounceCallback);
}
