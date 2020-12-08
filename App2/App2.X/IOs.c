/* 
 * File:   IOs.c
 * Author: Long Tran, Jeff Roszell, Toshiro Taperek
 */

#include <xc.h>
#include "ChangeClk.h"
#include "TimeDelay.h"
#include "IOs.h"
#include "UART2.h"


void IOinit(void)
/* Initialize IO and interrupts */
{
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
	IPC4bits.CNIP = 6;      // 7 is highest priority, 1 is lowest, 0 is disabled
	IFS1bits.CNIF = 0;      // Clear interrupt flag
	IEC1bits.CNIE = 1;      // Enable CN interrupts

	NewClk(32);             // Set clock to 32kHz
	return;
}


uint16_t time = 0;       // Time displayed on the terminal
uint16_t pressTime = 0;  // Time RB4 is pressed in ms
uint16_t resetFlag = 1;  // Flag to avoid printing ALARM when the
                         //   timer is reset to 0
uint16_t runFlag = 0;    // Flag to indicate that the timer is running

void IOcheck(void)
{
	NewClk(32);
	// Pushbutton debouncing
	IEC1bits.CNIE = 0;  // Disable CN interrupts
	delay_ms(400, 1);   // 400ms delay to filter out bounces
	IEC1bits.CNIE = 1;  // Enable CN interrupts to detect PB release

	// PB1 Instructions:
	while (PB1 && !PB2 && !PB3) {  // While only PB1 is pressed
		pressTime = 0;      // Reset pressTime
		resetFlag = 0;
		time += 60;         // Add 1 min to timer
		NewClk(8);          // Speed up clock just for display
		Disp2String("\r");  // Print time to terminal
		Disp2Dec(time / 60 * 1000);
		Disp2String("m : ");
		Disp2Dec(time % 60 * 1000);
		Disp2String("s         ");
		NewClk(32);        // Slow down clock for delay and other tasks
		delay_ms(800, 1);  // 0.8 sec delay
	}

	// PB2 Instructions:
	while (!PB1 && PB2 && !PB3) {  // While only PB2 is pressed
		time += 1;          // Add 1 sec to timer
		pressTime = 0;      // Reset pressTime
		resetFlag = 0;
		NewClk(8);
		Disp2String("\r");  // Print time to terminal
		Disp2Dec(time / 60 * 1000);
		Disp2String("m : ");
		Disp2Dec(time % 60 * 1000);
		Disp2String("s         ");
		NewClk(32);         // Slow down clock for delay and other tasks
		delay_ms(800, 1);   // 0.8 sec delay
	}

	// PB3 Instructions:
	while (!PB1 && !PB2 && PB3) {  // While only PB3 is pressed
		delay_ms(100, 1);  // 0.2 sec delay
		pressTime += 100;  // Update pressTime
	}
	return;
}


void IOmain()
{
	// Depending on the value of pressTime, do the following
	if (pressTime > 3000) {
		time = 0;                // Reset timer
		pressTime = 0;           // Reset pressTime
		resetFlag = 1;
		runFlag = 0;
	} else if (pressTime > 0) {  // 0<pressTime<3000
		// Set pressTime to 0 so multiple short presses
		//   won't increase the pressTime
		pressTime = 0;
		resetFlag = 0;
		runFlag = !runFlag;      // Flip the run flag
	}

	if (time > 0 && runFlag == 1) {
		time -= 1;          // Decrement timer
		delay_ms(1000, 1);  // 1 sec delay
	}

	if (runFlag == 1) {     // Control the LED
		if (time == 0)
			LED = 1;        // LED on
		else
			LED = !LED;     // Toggle the LED
	} else {
		LED = 0;            // LED off
	}

	NewClk(8);
	Disp2String("\r");           // Print time to terminal
	Disp2Dec(time / 60 * 1000);
	Disp2String("m : ");
	Disp2Dec(time % 60 * 1000);

	if (time == 0 && resetFlag == 0) {
		Disp2String("s -- ALARM");
	} else {
		Disp2String("s         ");
	}

	NewClk(32);  // Slow down clock for delay and other tasks
}


void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void)
/* Change Notification interrupt subroutine */
{
	CNflag = 1;         // Global user defined flag - use only if needed
	IFS1bits.CNIF = 0;  // Clear IF flag
	T2CONbits.TON = 0;  // Disable timer
	IEC0bits.T2IE = 0;  // Disable timer interrupt
	IOcheck();
	Nop();
	return;
}
