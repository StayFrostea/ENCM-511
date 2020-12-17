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
#include "ChangeClk.h"
#include "TimeDelay.h"

const float Vdd = 3.0;  // Supply voltage


void initVoltmeter(void)
{
	initUART2();
	initADC(AN5, false);
	beginADC();
	delay_ms(100, 1);
	clearLine();
}


//	uint32_t avg = 0;
//	int i;
///* Wait for the 16-sample buffer to be full.
//	 * Check if the ADC ISR has cleared ASAM yet. */
//	while(AD1CON1bits.ASAM) Idle();
//
//	// Average the samples
//	for (i = 0; i < ADC_BUF_LEN; i++)
//		avg += (&ADC1BUF0)[i];
//
//	AD1CON1bits.ASAM = 1;  // We can begin autosampling now that we've read the buffer
//	avg >>= 4;             // Divide by 16


void voltmeter(bool do_init)
/* Measure the voltage on AN5 and print over UART. */
{
	float voltage;
	uint16_t val;  // Sample is only 10 bits, so 16 bits is enough to hold their sum

	if (do_init) initVoltmeter();

	/* Wait for the 16-sample buffer to be full.
	 * Check if the ADC ISR has cleared ASAM yet. */
	while(AD1CON1bits.ASAM) Idle();
	val = ADC1BUF0;
	AD1CON1bits.ASAM = 1;  // We can begin autosampling now that we've read the buffer

	voltage = (val * Vdd) / 1024;  // Convert to voltage

	printUART2("VOLTMETER Voltage = ");
	printFloat(voltage);
	printUART2(" V\r");
}


void initOhmmeter(void)
{
	initUART2();
	initADC(AN11, false);
	beginADC();
	delay_ms(100, 1);
	clearLine();
}


void ohmmeter(bool do_init)
/* Measure resistance and send via UART */
{
	uint32_t val;
	uint32_t r;

	if (do_init) initOhmmeter();

	while(AD1CON1bits.ASAM) Idle();
	val = ADC1BUF0;
	AD1CON1bits.ASAM = 1;  // We can begin autosampling now that we've read the buffer

	r = (1000 * val) / (1024 - val);  // Convert to resistance

	printUART2("OHMMETER Resistance = ");
	printUint(r, 6, false);
	printUART2(" Ohm     \r");
}

void initPulsemeter(void)
{
	initUART2();
	initADC(AN12, true);
	beginADC();
	delay_ms(100, 1);
	clearLine();
}


void pulsemeter(bool do_init)
/* Measure frequency and amplitude and send via UART */
{
	int i;
	if (do_init) initPulsemeter();
	while(AD1CON1bits.ASAM) Idle();  // Wait for the buffer to be full

	uint16_t buf[16];
	uint16_t max = 0, min = UINT16_MAX;

	for (i = 0; i < ADC_BUF_LEN; i++) {
		uint16_t s = buf[i] = (&ADC1BUF0)[i];
		if (s > max) max = s;
		else if (s < min) s = min;
	}

	AD1CON1bits.ASAM = 1;  // Begin sampling again

	/* Assuming this is a square wave pulse, divide all samples
	 * into peak or trough category, and find an average for each.
	 * The difference between the two averages is the amplitude.
	 *
	 * For frequency, we take an average of peak lengths and an average
	 * of trough lengths and add them together to get average period.
	 * This allows measurement of waves whose period is greater than the
	 * sample window but not greater than double, and of pulses with
	 * duty cycles other than 50%.
	 *
	 * Integer division is presumed to be precise enough for the initial averages.
	 */
	uint16_t threshold = (min + max) / 2;

	uint32_t peak_avg       = 0;
	uint8_t  peak_avg_cnt   = 0;
	uint32_t trough_avg     = 0;

	uint16_t peak_len_avg   = 0;
	uint16_t peak_len_cnt   = 0;
	uint16_t trough_len_avg = 0;
	uint16_t trough_len_cnt = 0;

	uint16_t last_edge_i    = 0;
	bool prev_was_peak      = false;

	for (i = 0; i < ADC_BUF_LEN; i++) {
		uint16_t s = buf[i];

		if (s > threshold) {
			peak_avg += s;
			++peak_avg_cnt;

			if (!prev_was_peak && i != 0) {
				if (last_edge_i != 0) {
					peak_len_avg += i - last_edge_i;
					++peak_len_cnt;
				}
				last_edge_i = i;
			}
			prev_was_peak = true;
		} else {

			trough_avg += s;
			if (prev_was_peak && i != 0) {
				if (last_edge_i != 0) {
					trough_len_avg += i - last_edge_i;
					++trough_len_cnt;
				}
				last_edge_i = i;
			}
			prev_was_peak = false;
		}
	}

	// Calculate amplitude
	peak_avg /= peak_avg_cnt;
	trough_avg /= 16-peak_avg_cnt;
	uint16_t diff = peak_avg - trough_avg;
	float amp = (diff * Vdd) / 1024;        // Convert to voltage

	// Calculate frequency
	if (peak_len_cnt != 0)  // Prevent divide-by-zero
		peak_len_avg /= peak_len_cnt;
	else
		peak_len_avg = 0;

	if (trough_len_cnt != 0)  // Prevent divide-by-zero
		trough_len_avg /= trough_len_cnt;
	else
		trough_len_avg = 0;

	/* If we can see one or more cycles in our snapshot, we add the average peak and trough
	 * half periods to get one full period.
	 * If we didn't get a view of a full cycle, one of the peak/trough averages will be zero,
	 * As per the constraints in the previous section.  In this case, the sum can be taken,
	 * but we will assume a duty cycle of 50% and double the resulting period.
	 * If we didn't even capture all of a half cycle, both averages will be zero and the
	 * frequency cannot be calculated.
	 */

	uint32_t period_ns;
	float freq_kHz;
	if (peak_len_avg == 0 && trough_len_avg == 0) {
		freq_kHz = 0;
	} else {
		period_ns = (peak_len_avg + trough_len_avg) *
				get_instruction_period_ns() *
				(AD1CON3bits.ADCS + 1) *
				(AD1CON3bits.SAMC + 12);
		freq_kHz = (1000000.0F / period_ns);

		if (peak_len_avg == 0 || trough_len_avg == 0)
			freq_kHz /= 2;
	}

	printUART2("PULSEMETER Freq = ");
	printFloat(freq_kHz);
	printUART2(" kHz, Amplitude = ");
	printFloat(amp);
	printUART2(" V\r");
}


inline uint32_t get_instruction_period_ns(void)
/* Returns the instruction cycle period for
 * the current clock frequency in nanoseconds.
 * Returns 0 if unknown or unimplemented. */
{
	switch (OSCCONbits.COSC) {
		case 0b110:       // 500kHz clock
			return 4000;
		case 0b101:       // 32kHz clock
			return 62500;
		case 0b000:       // 8MHz clock
			return 250;
		default:
			return 0;
	}
}


void initIdle(void)
/* Set up for idle mode. */
{
	clearLine();
	printUART2("IDLE...\r");
	AD1CON1bits.ADON  = 0;   // Turn off ADC module
	endUART2();
}