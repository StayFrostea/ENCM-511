/* 
 * File:   TimeDelay.c
 * Author: longt
 *
 * Created on November 19, 2020, 6:25 PM
 */

#include <xc.h>
#include "TimeDelay.h"


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
	TMR2 = 0;
	T2CONbits.TON = 1;    // Start timer

	if (idle_on == 1) {
		Idle();
	}
	T2CONbits.TON = 0;    // Stop timer
	return;
}


void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void)
/* Used in HW IO control Driver project 3 */
{
	IFS0bits.T2IF = 0;  // Clear timer 2 interrupt flag
//	TMR2flag = 1;       // Optional global variable created by user
	return;
}
