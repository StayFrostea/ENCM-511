#ifndef TIME_DELAY_H
#define TIME_DELAY_H

#include <xc.h>

typedef void (*DelayCallbackPtr)();

void delay_ms(uint16_t time_ms, uint8_t idle_on);
void delay_ms_callback(uint16_t time_ms, DelayCallbackPtr cb);

void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void);

#endif /* TIME_DELAY_H */
