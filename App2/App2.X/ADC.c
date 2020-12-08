#include <xc.h>
#include "ADC.h"
#include "UART2.h"
#include "ChangeClk.h"


void initADC(void)
{
	AD1CON1bits.ADON   = 0;      // Turn off ADC module for now
	AD1CON1bits.ADSIDL = 0;      // Operate in idle mode
	AD1CON1bits.FORM0  = 0b00;   // Unsigned integer output format
	AD1CON1bits.SSRC   = 0b111;  // Internal counter ends sampling and starts conversion
	AD1CON1bits.ASAM   = 0;      // Auto sampling off
	AD1CON1bits.SAMP   = 0;      // Set to sample, clear to hold

	AD1CON2bits.VCFG  = 0b000;   // Selects AVdd, AVss (supply voltage to PIC) as Vref
	AD1CON2bits.CSCNA = 0;       // Configure ADC by setting bits in AD1CON2 as shown in slides 15-16
	// TODO: Add another input
	// Interrupt at the completion of conversion for each sample/convert sequence
	AD1CON2bits.SMPI = 0b0000;
	AD1CON2bits.BUFM = 0;        // Buffer configured as one 16-word buffer
	AD1CON2bits.ALTS = 0;        // Always uses MUX A input multiplexer settings

	AD1CON3bits.ADRC = 0;        // Use system clock
	// Sampling time = 5*2/fclk
	// Ensure sample time is 1/10th of signal being sampled
	// TODO: Make sample time depend on clock frequency
	AD1CON3bits.SAMC = 0b00101;

	AD1CHSbits.CH0NA = 0;        // Channel 0 negative input is Vr- for MuxA
	AD1CHSbits.CH0SA = 0b0101;   // Channel 0 positive input is AN5

	AD1PCFG = 0xFFFF;            // Set all bits as digital
	AD1PCFGbits.PCFG5 = 0;       // Set pin8/AN5/RA3 as Analog input for ADC
	AD1CSSL = 0;                 // Disable input scan
	// TODO: Add another input

	// ADC Interrupt settings
	// TODO: choose ADC interrupt priority
	IPC3bits.AD1IP = 6;          // 7 is highest priority, 1 is lowest, 0 is disabled
	IFS0bits.AD1IF = 0;          // Clear interrupt flag
//	IEC0bits.AD1IE = 1;          // Enable ADC interrupts
	// TODO: Figure out where to enable ADC interrupts
}


uint16_t readADC(void)
{
	uint16_t ADCvalue;     // 16 bit register used to hold ADC converted digital output ADC1BUF0

	AD1CON1bits.SAMP = 1;  // Start Sampling, conversion starts automatically after SSRC and SAMC settings
//	while (AD1CON1bits.DONE == 0) ;
	Idle();                // Wait for the ADC interrupt to trigger
	ADCvalue = ADC1BUF0;   // ADC output is stored in ADC1BUF0 as this point
	AD1CON1bits.SAMP = 0;  // Stop sampling
	AD1CON1bits.ADON = 0;  // Turn off ADC, ADC value stored in ADC1BUF0;
	return(ADCvalue);      // Returns 10 bit ADC output stored in ADC1BIF0 to calling function
}


void drawGraph(uint16_t value)
{
	// If voltage exceeds 2000, set it to 2000
	if (value > 1024) {
		value = 1024;
	}

	// Set value to 0 if the value is negative
	if (value < 0) {
		value = 0;
	}

	NewClk(8);

	// Draw the graph
	Disp2String("\r");
	uint16_t maxLength = 50;  // Set the max length of the bar graph to 50
	uint16_t barLength = value * 50 / 1024;
	XmitUART2('>', barLength);
	XmitUART2(' ', maxLength - barLength);

	Disp2Hex(value);  // Display the current voltage in hex format
	NewClk(32);
}


void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void)
{
	IFS0bits.AD1IF = 0;  // Clear interrupt flag
}
