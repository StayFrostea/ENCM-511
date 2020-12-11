#ifndef ADC_H
#define ADC_H

#include <xc.h>

#define ADC_BUF_LEN 16

enum AN_pin {
	AN5  = 0b0101,
	AN11 = 0b1011,
	AN12 = 0b1100
};

extern uint16_t ADC_buf[ADC_BUF_LEN];

void initADC(enum AN_pin input);
void beginADC(void);

void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void);

#endif /* ADC_H */
