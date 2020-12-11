#ifndef METER_H
#define METER_H

#include <xc.h>

enum Mode {
	VOLT,
	OHM,
	PULSE,
	IDLE
};

extern enum Mode mode;  // Global mode/state variable

void initVoltmeter(void);
void initOhmmeter(void);
void initPulsemeter(void);

void voltmeter(void);
void ohmmeter(void);
void pulsemeter(void);

#endif /* METER_H */
