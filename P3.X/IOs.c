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

void IOinit(void)
{
    AD1PCFG = 0xFFFF; // Turn all analog pins as digital
    //Digital Output
    TRISBbits.TRISB8 = 0; //set RB8 to be output
    //Digital Inputs
    TRISAbits.TRISA2 = 1; //set RA2 to be input
    TRISBbits.TRISB4 = 1; //set RB4 to be input
    TRISAbits.TRISA4 = 1; //set RA4 to be input
    //Set pull up
    CNPU1bits.CN0PUE = 1;  //RA4 pullup
    CNPU1bits.CN1PUE = 1;  //RB4 pullup
    CNPU2bits.CN30PUE = 1; //RA2 pullup
}

//create a delay of n seconds
void delay(int n)
{
    unsigned int ms = 1000 * n;   //convert n to milliseconds
    clock_t start_time = clock(); //get current time
    while (clock() < (start_time + ms))
        ; //loop until n seconds has passed
}

void IOcheck(void)
{
    while (1)
    {
        unsigned int c = PORTAbits.RA2 * 100 + PORTAbits.RA4 * 10 + PORTBbits.RB4; //generate a case number
        switch (c)
        {
        case 011:               //only PB1 is pressed
            LATBbits.LATB8 = 1; //LED on
            delay(1);           //delay for 1 second
            LATBbits.LATB8 = 0; //LED off
            delay(1);           //delay for 1 second
            break;
        case 101:               //only PB2 is pressed
            LATBbits.LATB8 = 1; //LED on
            delay(2);           //delay for 2 second
            LATBbits.LATB8 = 0; //LED off
            delay(2);           //delay for 2 second
            break;
        case 110:               //only PB3 is pressed
            LATBbits.LATB8 = 1; //LED on
            delay(3);           //delay for 3 second
            LATBbits.LATB8 = 0; //LED off//LED off
            delay(3);           //delay for 3 second
            break;
        case 111:               //no button is pressed
            LATBbits.LATB8 = 0; //LED off
            break;
        default:                //any other cases
            LATBbits.LATB8 = 1; //LED on
            break;
        }
    }
    return;
}
