/* 
 * File:   ADC.c
 * Author: Jeff, Long
 *
 * Created on November 27, 2020, 1:52 PM
 */

#include "xc.h"
#include "UART2.h"

///// ADC interrupt subroutine
void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void)
{
IFS0bits.AD1IF = 0; // Clear the ADC1 Interrupt Flag
}

uint16_t do_ADC(void)
{
uint16_t ADCvalue; // 16 bit register used to hold ADC converted digital output ADC1BUF0
uint16_t numLines;
/* ------------- ADC INITIALIZATION ------------------*/
AD1CON1bits.ADON = 1; // turn on ADC module
AD1CON1bits.FORM = 00; //This will set up integer
AD1CON1bits.SSRC = 111; //Use internal counter for start/stop
AD1CON1bits.ASAM = 0;

AD1CON2bits.VCFG = 0;//System Clock
AD1CON2bits.CSCNA = 000;//Do not scan results
AD1CON2bits.SMPI = 0000;//Interupt at end of each conversion
AD1CON2bits.BUFM = 0;//16 bit buffer
AD1CON2bits.ALTS = 0;//Always use MUX A

AD1CON3bits.ADRC = 0;//Clock for system derived on the system clock
AD1CON3bits.SAMC = 1010;//Divide system clock by ten
AD1CON3bits.ADCS = 00000;

AD1CHSbits.CH0NB = 0;//Channel 0 Vr-
AD1CHSbits.CH0SB = 0101;//The positive for channel 0 is on AN5
AD1CHSbits.CH0NA = 0;//Channel 0 Vr-
AD1CHSbits.CH0SA = 0101;//The positive for channel 0 is on AN5

AD1PCFG = 0; //Enables use of ADC on AN5

IFS0bits.AD1IF = 0;//Clear flag for ADC
IPC3bits.AD1IP = 4;//Set priority level for interrupt
IEC0bits.AD1IE = 1;//Enable the interrupt

/* ------------- ADC SAMPLING AND CONVERSION ------------------*/
AD1CON1bits.SAMP=1; //Start Sampling, Conversion starts automatically after SSRC and SAMC settings
while(AD1CON1bits.DONE==0)
{}
ADCvalue = ADC1BUF0; // ADC output is stored in ADC1BUF0 as this point
AD1CON1bits.SAMP=0; //Stop sampling
AD1CON1bits.ADON=0; //Turn off ADC, ADC value stored in ADC1BUF0;
return (ADCvalue); //returns 10 bit ADC output stored in ADC1BIF0 to calling function
}

