#ifndef METER_H
#define METER_H

#include <xc.h>

enum Mode {
	VOLT,
	OHM,
	PULSE,
	IDLE
};

extern enum Mode mode;

void voltmeter(void);
void ohmmeter(void);
void pulsemeter(void);

#endif /* METER_H */
