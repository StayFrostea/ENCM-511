/*
 * File:   main.c
 * Author: Toshiro Taperek, Long Tran, Jeff Roszell
 */

// MPLAB header libraries
#include <xc.h>
#include <p24F16KA101.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>

// User header files
#include "ChangeClk.h"
#include "TimeDelay.h"
#include "IOs.h"
#include "UART2.h"

#pragma config FCKSM = CSECMD // Clock switching is enabled, clock monitor disabled

// Global variables
uint8_t CNflag = 0;

// MACROS for Idle, Sleep modes
#define Nop() {__asm__ volatile ("nop");}
#define ClrWdt() {__asm__ volatile ("clrwdt");}
#define Sleep() {__asm__ volatile ("pwrsav #0");}   //Sleep() - put MCU in sleep mode - CPU and some peripherals off
#define Idle() {__asm__ volatile ("pwrsav #1");}    //Idle() - put MCU in idle mode - only CPU off
#define dsen() {__asm__ volatile ("BSET DSCON, #15");} //

void main(void)
{
	// Clock output on REFO/RB15 - Testing purposes only
	TRISBbits.TRISB15 = 0; // Set RB15 as output for REFO
	REFOCONbits.ROEN = 1; // Ref oscillator is enabled
	REFOCONbits.ROSSLP = 0; // Ref oscillator is disabled in sleep
	REFOCONbits.ROSEL = 0; // Output base clk showing clock switching
	REFOCONbits.RODIV = 0b0000;

	// Change Clock
	NewClk(32); // 8 for 8 MHz; 500 for 500 kHz; 32 for 32 kHz

	// Initialize IOs for low-power wake-up
	AD1PCFG = 0xFFFF; // Turn all analog pins as digital
	IOinit(); // Enable IO and CN interrupts on Push buttons

	while (1) {
		IOmain();
	}

}

/*
int main(void) {
    // Change Clock
    NewClk(32); // 8 for 8 MHz; 500 for 500 kHz; 32 for 32 kHz
    
    while(1) 
    {
        drawGraph(do_ADC());//Display to terminal the bar graph along with the value
    }
    
    return 0;
}
*/