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
/* ------------- ADC INITIALIZATION ------------------*/
ADCON1bits.ADON = 1; // turn on ADC module

AD1CON3bits.ADRC = 0;

/* ------------- ADC SAMPLING AND CONVERSION ------------------*/
AD1CON1bits.SAMP=1; //Start Sampling, Conversion starts automatically after SSRC and SAMC settings
while(AD1CON1bits.DONE==0)
{}
ADCvalue = ADC1BUF0; // ADC output is stored in ADC1BUF0 as this point
AD1CON1bits.SAMP=0; //Stop sampling
AD1CON1bits.ADON=0; //Turn off ADC, ADC value stored in ADC1BUF0;
return (ADCvalue); //returns 10 bit ADC output stored in ADC1BIF0 to calling function
}

