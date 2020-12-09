/*
 * File:   TimeDelay.c
 * Author: longt
 *
 * Created on November 19, 2020, 6:25 PM
 */

#include <xc.h>
#include <stddef.h>
#include "TimeDelay.h"

// A file-global function pointer that the timer's ISR will call if non-null
DelayCallbackPtr callback = NULL;


void delay_ms(uint16_t time_ms, uint8_t idle_on)
{
	// T2CON config
	T2CONbits.TSIDL = 0;  // Operate in idle mode
	T2CONbits.T32 = 0;    // Operate timer 2 as 16 bit timer
	T2CONbits.TCS = 0;    // Use internal clock
//	T2CONbits.TGATE = 0;

	// Timer 2 interrupt config
	IPC1bits.T2IP = 2;    // 7 is highest and 1 is lowest priority
	IEC0bits.T2IE = 1;    // Enable timer interrupt
	IFS0bits.T2IF = 0;    // Clear timer 2 flag

	PR2 = time_ms << 4;   // After PR2 simplification
	TMR2 = 0;             // Reset the counter
	T2CONbits.TON = 1;    // Start timer

	if (idle_on)
		Idle();
}

void delay_ms_callback(uint16_t time_ms, DelayCallbackPtr cb)
/* Start a timer and return immediately, calling the given callback
 * function when the timer expires after time_ms milliseconds.
 * The pointer must point to a function which takes no parameters
 * and returns void.
 * Calling this function again will reset any current timers.
 */
{
	callback = cb;
	delay_ms(time_ms, 0);
}


void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void)
/* ISR for the delay timer */
{
	IFS0bits.T2IF = 0;  // Clear timer 2 interrupt flag
	T2CONbits.TON = 0;  // Stop timer

	if (callback) {
		callback();
		callback = NULL;
	}
	return;
}
