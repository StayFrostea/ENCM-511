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
	initADC(AN5);
	beginADC();
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

	/* Copy the data into the working buffer and begin
	 * autosampling immediately while we process the old data. */
//	memcpy(ADC_buf, (void*)&ADC1BUF0, sizeof(ADC_buf[0])*ADC_BUF_LEN);

//	 Average the samples
	for (i = 0; i < ADC_BUF_LEN; i++)
		avg += (&ADC1BUF0)[i];

	AD1CON1bits.ASAM = 1;  // We can begin autosampling now that we've read the buffer
	avg >>= 4;  // Divide by 16


	// Convert to voltage
	voltage = avg * Vdd / 1024;

	printUART2("\rVOLTMETER Voltage = ");
//	printU16(avg, 4);
	printFloat(voltage);
	printUART2("V        \r");
}


void initOhmmeter(void)
{
	printUART2("OHMMETER\n");
}


void ohmmeter(bool do_init)
/* Measure resistance and send via UART */
{
	if (do_init) initOhmmeter();
}


void initPulsemeter(void)
{
	printUART2("PULSEMETER\n");
}


void pulsemeter(bool do_init)
/* Measure frequency and amplitude and send via UART */
{
	if (do_init) initPulsemeter();
}
