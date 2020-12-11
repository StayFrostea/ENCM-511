/*
 * File:   main.c
 * Author: Toshiro Taperek, Long Tran, Jeff Roszell
 */

#include <xc.h>
#include <p24F16KA101.h>
#include <math.h>
#include <errno.h>

#include "ChangeClk.h"
#include "TimeDelay.h"
#include "IOs.h"
#include "UART2.h"
#include "ADC.h"
#include "meter.h"

// Conditional compilation of the pulse meter functionality
#define MIDTERM 1

// Clock control
#pragma config IESO     = OFF     // 2 Speed Startup disabled
#pragma config FNOSC    = FRC     // Start up CLK = 8 MHz
#pragma config FCKSM    = CSECMD  // Clock switching is enabled, clock monitor disabled.
#pragma config SOSCSEL  = SOSCLP  // Secondary oscillator for Low Power Operation.
#pragma config POSCFREQ = MS      /* Primary Oscillator/External clock frequency between
                                     100kHz and 8 MHz. Options: LS, MS, HS. */
#pragma config OSCIOFNC = ON      // CLKO output disabled on pin 8, use as IO.
#pragma config POSCMOD  = NONE    // Primary oscillator mode is disabled

// Macros for power-saving modes
#define Nop()    {__asm__ volatile ("nop");}
#define ClrWdt() {__asm__ volatile ("clrwdt");}
//      Sleep() - put MCU in sleep mode - CPU and some peripherals off
#define Sleep()  {__asm__ volatile ("pwrsav #0");}
//      Idle()  - put MCU in idle mode  - only CPU off
#define Idle()   {__asm__ volatile ("pwrsav #1");}
#define dsen()   {__asm__ volatile ("BSET DSCON, #15");}

// Global variables
enum Mode mode = VOLT;

// Local helper functions for main()
void loop(void);
void init(void);


int main(void)
{
	init();
	while (1)
		loop();
	return 0;
}


void init(void)
{
	initRefOsc();
	// TODO: Control clock according to macros
	NewClk(8);  // 8 for 8 MHz; 500 for 500 kHz; 32 for 32 kHz

	initUART2();
	initIO();

	// TODO: Move the following to the mode-switch code
//	initVoltmeter();
}


void loop(void)
{
	switch (mode) {
		case VOLT:
			voltmeter();
			break;
		case OHM:
			ohmmeter();
			break;
		case PULSE:
			#if MIDTERM
				pulsemeter();
				break;
			#endif
		case IDLE:
			// TODO: Disable other peripherals
			Idle();
	}
}
