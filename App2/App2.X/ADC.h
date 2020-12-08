#ifndef ADC_H
#define ADC_H

#include <xc.h>

void initADC(void);
uint16_t readADC(void);
void drawGraph(uint16_t value);

void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void);

#endif /* ADC_H */
