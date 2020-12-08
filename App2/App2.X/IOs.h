#ifndef IO_H
#define IO_H

#include <xc.h>

// Pin definitions for the pushbuttons (read-only).
// Here they are logically negated because the buttons are normally open
//   and the pins are pulled up internally, and so are active-low.
#define PB1 (!PORTAbits.RA2)
#define PB2 (!PORTAbits.RA4)
#define PB3 (!PORTBbits.RB4)

// LED output pin (RB8); active-high; read/write
#define LED (LATBbits.LATB8)

extern uint8_t CNflag;

void IOinit(void);
void IOcheck(void);
void IOmain(void);

#endif /* IO_H */
