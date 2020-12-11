#include <xc.h>
#include "ADC.h"
#include "IOs.h"

uint16_t ADC_buf[ADC_BUF_LEN];  // Global ADC buffer

void initADC(enum AN_pin input)
{
	AD1CON1bits.ADON   = 0;              // Turn off ADC module for now
	AD1CON1bits.ADSIDL = 0;              // Operate while in idle mode
	AD1CON1bits.FORM0  = 0b00;           // Unsigned integer output format
	AD1CON1bits.SSRC   = 0b111;          // Internal counter ends sampling and starts conversion
	AD1CON1bits.SAMP   = 0;              // Set to sample, clear to convert

	AD1CON2bits.VCFG   = 0b000;          // Selects AVdd, AVss (supply voltage to PIC) as Vref
	AD1CON2bits.CSCNA  = 0;              // Use the CH0SA channel as input
	AD1CON2bits.SMPI   = ADC_BUF_LEN-1;  // Interrupt after the buffer is full with ADC_BUF_LEN samples
	AD1CON2bits.BUFM   = 0;              // Buffer configured as one 16-word buffer
	AD1CON2bits.ALTS   = 0;              // Always uses MUX A input multiplexer settings

	AD1CON3bits.ADRC   = 0;              // Use system clock
	AD1CON3bits.SAMC   = 0b00001;        // Number of A/D clock periods (T_ADs) to wait while sampling
	AD1CON3bits.ADCS   = 0;              // A/D clock prescaler. T_AD = ADCS * 2Tosc

	AD1CHSbits.CH0NA   = 0;              // Channel 0 negative input is Vr- for MuxA
	AD1CHSbits.CH0SA   = input;          // Set channel 0 positive input pin (input & 0b1111 would be safe)

	AD1PCFG = 0xFFFF;                    // Set all analog pins to digital
	AD1PCFGbits.PCFG5  = 0;              // Set pin8/AN5/RA3 as analog input
	AD1PCFGbits.PCFG11 = 0;              // Set pin16/AN11/RB13 as analog input
	AD1PCFGbits.PCFG12 = 0;              // Set pin15/AN12/RB12 as analog input
	AD1CSSL = 0;                         // Disable input scanning

	// ADC Interrupt settings
	IPC3bits.AD1IP = 6;           // 7 is highest priority, 1 is lowest, 0 is disabled
	IEC0bits.AD1IE = 1;           // Enable ADC interrupts

	// TODO: Choose ADC interrupt priority
	// TODO: Allow each mode to choose appropriate SAMC, ADCS, and SMPI values
}

void beginADC(void)
/* Start the automatic ADC sampling. */
{
	// Turns out the order of this sequence is important.
	AD1CON1bits.ADON = 1;  // Turn ADC ON
	IFS0bits.AD1IF   = 0;  // Clear interrupt flag
	AD1CON1bits.ASAM = 1;  // Auto sampling on.  Starts immediately
}


void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void)
/* ADC Interrupt ISR.  Triggered when the buffer is full (16 samples) */
{
	IFS0bits.AD1IF = 0;    // Clear interrupt flag
	AD1CON1bits.ASAM = 0;  // Stop autosampling.  This also indicates that the data is ready
}
