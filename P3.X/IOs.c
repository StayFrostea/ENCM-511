/*
 * File:   IOs.c
 * Author: Jeff Roszell
 *
 * Created on October 13, 2020, 3:22 PM
 */


#include "xc.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
//This CPU is 8MHz
//Clock speed / speed of PC incrementation / amount of cycles of clock per instruction
//8MHz / 2 cycles per incrementation / 2 cycles per instruction = 2MHz
#define FCY 2000000UL
#include <libpic30.h>

void IOinit(void) {
    AD1PCFG = 0xFFFF; // Turn all analog pins as digital  
    //Digital Output
    TRISBbits.TRISB8 = 0;//set RB8 to be output
    //Digital Inputs
    TRISAbits.TRISA2 = 1;//set RA2 to be input
    TRISBbits.TRISB4 = 1;//set RB4 to be input
    TRISAbits.TRISA4 = 1;//set RA4 to be input
    //Set pull up
    CNPU1bits.CN0PUE = 1;//RA4 pullup
    CNPU1bits.CN1PUE = 1;//RB4 pullup
    CNPU2bits.CN30PUE = 1;//RA2 pullup
}

void IOcheck(void) {
    while(1){
        unsigned int c=PORTAbits.RA2*100+PORTAbits.RA4*10+PORTBbits.RB4; //generate a case number
        switch(c){
            case 11: //only PB1 is pressed
                LATBbits.LATB8 = 1;//LED on
                __delay_ms(1000);//delay for 1 second
                LATBbits.LATB8 = 0;//LED off
                __delay_ms(1000);//delay for 1 second
                break;
            case 101: //only PB2 is pressed
                LATBbits.LATB8 = 1;//LED on
                __delay_ms(2000);//delay for 2 second
                LATBbits.LATB8 = 0;//LED off
                __delay_ms(2000);//delay for 2 second
                break;
            case 110: //only PB3 is pressed
                LATBbits.LATB8 = 1;//LED on
                __delay_ms(3000);//delay for 3 second
                LATBbits.LATB8 = 0;//LED off//LED off
                __delay_ms(3000);//delay for 3 second
                break;
            case 111: //no button is pressed
                LATBbits.LATB8 = 0;//LED off
                break;
            default: //any other cases
                LATBbits.LATB8 = 1;//LED on
                break;
        }
    }
    return;
}
