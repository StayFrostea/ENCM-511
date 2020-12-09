#ifndef IO_H
#define IO_H

#include <xc.h>

/* Pin definitions for the pushbuttons (read-only).
 * Here they are logically negated because the buttons are normally open
 * and the pins are pulled up internally, and so are active-low.
 */
#define PB1 (!PORTAbits.RA2)
#define PB2 (!PORTAbits.RA4)
#define PB3 (!PORTBbits.RB4)

// LED output pin (RB8); active-high; read/write
#define LED (LATBbits.LATB8)

void initIO(void);
/*
void IOcheck(void);
void IOmain(void);
 */

void initRefOsc(void);

void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void);

#endif /* IO_H */
