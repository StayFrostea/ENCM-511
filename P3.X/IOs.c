/*
 * File:   IOs.c
 * Author: Jeff Roszell
 *
 * Created on October 13, 2020, 3:22 PM
 */


#include "xc.h"

//unsigned int GlobalVar;

void IOinit(void) {
    //Digital Output
    TRISBbits.TRISB8 = 0;
    //Digital Input
    TRISAbits.TRISA4 = 1;
    //Set pull up or down
    CNPU1bits.CN0PUE = 1;
}

void IOcheck(void) {

    while (1) {
        while (PORTAbits.RA4 == 0) {
            LATBbits.LATB8 = 1;
        }
        LATBbits.LATB8 = 0;
    }

    return;
}
