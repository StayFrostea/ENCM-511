/*
 * File:   meter.c
 * Author: toshi
 *
 * Created on December 8, 2020, 5:38 PM
 */


#include <xc.h>
#include <string.h>
#include "meter.h"
#include "UART2.h"
#include "ADC.h"
#include "IOs.h"

void voltmeter()
/* Measure the voltage on AN5 and print over UART. */
{
	int i;

	/* Wait for the 16-sample buffer to be full.
	 * Check if the ADC ISR has cleared ASAM yet. */
	while(AD1CON1bits.ASAM) Idle();

	/* Copy the data into the working buffer and begin
	 * autosampling immediately while we process the old data. */
	memcpy(ADC_buf, (void*)&ADC1BUF0, sizeof(ADC_buf[0])*ADC_BUF_LEN);
	AD1CON1bits.ASAM = 1;

	for (i = 0; i < 16; i++) {
		Disp2Dec(ADC_buf[i]);
		printUART2(", ");
	}
	printUART2("\n");
}


void ohmmeter()
{
	// Measure resistance and send via UART
}


void pulsemeter()
{
	// Measure frequency and amplitude and send via UART
}
