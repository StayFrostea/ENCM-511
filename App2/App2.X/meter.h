#ifndef METER_H
#define METER_H

#include <xc.h>
#include <stdbool.h>

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

void voltmeter(bool do_init);
void ohmmeter(bool do_init);
void pulsemeter(bool do_init);

#endif /* METER_H */
