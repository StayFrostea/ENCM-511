/*
 * File:   main.c
 * Author: Toshiro Taperek, Long Tran, Jeff Roszell
 */

#include <xc.h>
#include <p24F16KA101.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>

#include "ChangeClk.h"
#include "TimeDelay.h"
#include "IOs.h"
#include "UART2.h"

// Clock control
#pragma config IESO     = OFF     // 2 Speed Startup disabled
#pragma config FNOSC    = FRC     // Start up CLK = 8 MHz
#pragma config FCKSM    = CSECMD  // Clock switching is enabled,
                                  //     clock monitor disabled.
#pragma config SOSCSEL  = SOSCLP  // Secondary oscillator for
                                  //     Low Power Operation.
#pragma config POSCFREQ = MS      // Primary Oscillator/External clock
                                  //     frequency between 100kHz and 8 MHz.
                                  //     Options: LS, MS, HS.
#pragma config OSCIOFNC = ON      // CLKO output disabled on pin 8, use as IO.
#pragma config POSCMOD  = NONE    // Primary oscillator mode is disabled

// Global variables
uint8_t CNflag = 0;

// Macros for power-saving modes
#define Nop()    {__asm__ volatile ("nop");}
#define ClrWdt() {__asm__ volatile ("clrwdt");}
//      Sleep() - put MCU in sleep mode - CPU and some peripherals off
#define Sleep()  {__asm__ volatile ("pwrsav #0");}
//      Idle()  - put MCU in idle mode  - only CPU off
#define Idle()   {__asm__ volatile ("pwrsav #1");}
#define dsen()   {__asm__ volatile ("BSET DSCON, #15");}


int main(void)
{
	// Clock output on REFO/RB15 - Testing purposes only
	TRISBbits.TRISB15  = 0;       // Set RB15 as output for REFO
	REFOCONbits.ROEN   = 1;       // Ref oscillator is enabled
	REFOCONbits.ROSSLP = 0;       // Ref oscillator is disabled in sleep
	REFOCONbits.ROSEL  = 0;       // Output base clock showing clock switching
	REFOCONbits.RODIV  = 0b0000;

	// Change Clock
	NewClk(32);  // 8 for 8 MHz; 500 for 500 kHz; 32 for 32 kHz

	// Initialize IOs for low-power wake-up
	AD1PCFG = 0xFFFF;  // Turn all analog pins as digital
	IOinit();          // Enable IO and CN interrupts on Push buttons

	while (1) {
		IOmain();
	}
	return 0;
}

/*
int main(void) {
	// Change Clock
	NewClk(32);  // 8 for 8 MHz; 500 for 500 kHz; 32 for 32 kHz

	while(1) {
		// Display to terminal the bar graph along with the value
		drawGraph(do_ADC() );
	}

	return 0;
}
*/
