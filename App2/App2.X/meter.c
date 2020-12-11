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

const float Vdd = 3.0;  // Supply voltage


void initVoltmeter(void)
{
	initUART2();
	initADC(AN5);
	beginADC();
	clearLine();
}


void voltmeter(bool do_init)
/* Measure the voltage on AN5 and print over UART. */
{
	int i;
	float voltage;
	uint16_t avg = 0;  // Sample is only 10 bits, so 16 bits is enough to hold their sum

	if (do_init) initVoltmeter();

	/* Wait for the 16-sample buffer to be full.
	 * Check if the ADC ISR has cleared ASAM yet. */
	while(AD1CON1bits.ASAM) Idle();

	// Average the samples
	for (i = 0; i < ADC_BUF_LEN; i++)
		avg += (&ADC1BUF0)[i];

	AD1CON1bits.ASAM = 1;  // We can begin autosampling now that we've read the buffer
	avg >>= 4;             // Divide by 16

	voltage = (avg * Vdd) / 1024;  // Convert to voltage

	printUART2("\rVOLTMETER Voltage = ");
	printFloat(voltage);
	writeUART2('V');
}


void initOhmmeter(void)
{
	initUART2();
	initADC(AN11);
//	initADC(AN5);
	beginADC();
	clearLine();
}


void ohmmeter(bool do_init)
/* Measure resistance and send via UART */
{
	int i;
	uint32_t avg = 0;
	uint32_t r;

	if (do_init) initOhmmeter();

	while(AD1CON1bits.ASAM) Idle();

	// Average the samples
	for (i = 0; i < ADC_BUF_LEN; i++)
		avg += (&ADC1BUF0)[i];

	AD1CON1bits.ASAM = 1;  // We can begin autosampling now that we've read the buffer
	avg >>= 4;             // Divide by 16

	r = (1000 * avg) / (1024 - avg);  // Convert to resistance

	printUART2("\rOHMMETER Resistance = ");
	printUint(r, 6, false);
	printUART2(" Ohm     ");
}

void initPulsemeter(void)
{
	initUART2();
	clearLine();
}


void pulsemeter(bool do_init)
/* Measure frequency and amplitude and send via UART */
{
	if (do_init) initPulsemeter();

	/* Copy the data into the working buffer and begin
	 * autosampling immediately while we process the old data. */
//	memcpy(ADC_buf, (void*)&ADC1BUF0, sizeof(ADC_buf[0])*ADC_BUF_LEN);

	printUART2("\rPULSEMETER Freq = ___kHz, Amplitude = ____V");
}


void initIdle(void)
/* Set up for idle mode. */
{
	clearLine();
	printUART2("IDLE...");
	AD1CON1bits.ADON  = 0;   // Turn off ADC module
	U2MODEbits.UARTEN = 1;   // Turn off UART
}