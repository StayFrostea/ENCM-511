#include "xc.h"
#include "ADC.h"
#include "UART2.h"
#include "ChangeClk.h"

uint16_t do_ADC(void)
{
	uint16_t ADCvalue; // 16 bit register used to hold ADC converted digital output ADC1BUF0
	
	/* ------------- ADC INITIALIZATION ------------------*/
	AD1CON1bits.ADON = 1; // turn on ADC module
	AD1CON1bits.FORM0 = 0; // output format bit
	AD1CON1bits.FORM1 = 0; // output format bit
	// Internal counter ends sampling and starts conversion
	AD1CON1bits.SSRC0 = 1;
	AD1CON1bits.SSRC1 = 1;
	AD1CON1bits.SSRC2 = 1;

	AD1CON1bits.SAMP = 0;
	AD1CON2bits.VCFG = 0b000; // Selects AVDD, AVSS (supply voltage to PIC) as Vref
	AD1CON2bits.CSCNA = 0; // Configure ADC by setting bits in AD1CON2 as shown in slides 15-16

	// Interrupt at the completion of conversion for each sample/convert sequence
	AD1CON2bits.SMPI0 = 0;
	AD1CON2bits.SMPI1 = 0;
	AD1CON2bits.SMPI2 = 0;
	AD1CON2bits.SMPI3 = 0;

	AD1CON2bits.BUFM = 0; // Buffer configured as one 16-word buffer
	AD1CON2bits.ALTS = 0; // Always uses MUX A input multiplexer settings

	AD1CON3bits.ADRC = 0; // Use system clock

	// Slowest sampling time = 5*2/fclk
	AD1CON3bits.SAMC0 = 1;
	AD1CON3bits.SAMC1 = 0;
	AD1CON3bits.SAMC2 = 1;
	AD1CON3bits.SAMC3 = 0;
	AD1CON3bits.SAMC4 = 0;
	// Ensure sample time is 1/10th of signal being sampled

	AD1CHSbits.CH0NA = 0; // Channel 0 negative input is Vr-

	// Channel 0 positive input is AN5
	AD1CHSbits.CH0SA0 = 1;
	AD1CHSbits.CH0SA1 = 0;
	AD1CHSbits.CH0SA2 = 1;
	AD1CHSbits.CH0SA3 = 0;

	AD1PCFGbits.PCFG5 = 0;
	AD1CSSLbits.CSSL5 = 0;
	
	
	/* ------------- ADC SAMPLING AND CONVERSION ------------------*/
	AD1CON1bits.SAMP = 1; //Start Sampling, Conversion starts automatically after SSRC and SAMC settings
	while (AD1CON1bits.DONE == 0) {
	}
	ADCvalue = ADC1BUF0; // ADC output is stored in ADC1BUF0 as this point
	AD1CON1bits.SAMP = 0; // Stop sampling
	AD1CON1bits.ADON = 0; // Turn off ADC, ADC value stored in ADC1BUF0;
	return(ADCvalue); // Returns 10 bit ADC output stored in ADC1BIF0 to calling function
}

void drawGraph(uint16_t value)
{
	// If voltage exceeds 2000 set it to 2000
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
	uint16_t maxLength = 50; // Set the max length of the bar graph to 50
	uint16_t barLength = value * 50 / 1024;
	XmitUART2('>', barLength);
	XmitUART2(' ', maxLength - barLength);

	Disp2Hex(value); // Display the current voltage in hex format
	NewClk(32);
}