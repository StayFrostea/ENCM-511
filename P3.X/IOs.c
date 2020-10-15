/*
 * File:   IOs.c
 * Author: Jeff Roszell
 *
 * Created on October 13, 2020, 3:22 PM
 */


#include "xc.h"
#include <time.h>

//unsigned int GlobalVar;

void IOinit(void) {
    //Digital Output
    TRISBbits.TRISB8 = 0;
    //Digital Inputs
    TRISAbits.TRISA2 = 1;
    TRISBbits.TRISB4 = 1;
    TRISAbits.TRISA4 = 1;
    //Set pull up or down
    CNPU1bits.CN0PUE = 1;
    CNPU2bits.CN30PUE = 1;
}

//void IOcheck(void) {
//
//    while (1) {
//        while (PORTAbits.RA4 == 0) {
//            LATBbits.LATB8 = 1;
//        }
//        LATBbits.LATB8 = 0;
//    }
//
//    return;
//}

void delay(int n) {

    //Standard Delay for C
    unsigned int ms = 1000 * n;
    clock_t start_time = clock();
    while (clock() < start_time + ms);

}

void flashOne(void){
    //1 second on/off pattern
            LATBbits.LATB8 = 1;
            delay(1);
            LATBbits.LATB8 = 0;
            delay(1);
}

void flashTwo(void){
    //2 second on/off pattern
            LATBbits.LATB8 = 1;
            delay(2);
            LATBbits.LATB8 = 0;
            delay(2);
}

void flashThree(void){
    //3 second on/off pattern
            LATBbits.LATB8 = 1;
            delay(3);
            LATBbits.LATB8 = 0;
            delay(3);
}

void flashConstant(void){
    //stays on
            LATBbits.LATB8 = 0;
}
void IOrun(void) {

    while (1) {
        LATBbits.LATB8 = 0;
        if (PORTAbits.RA2 == 1 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 0) {
            while(PORTAbits.RA2 == 1 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 0){
                flashOne();
            }
            
        } else if (PORTAbits.RA2 == 0 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 0) {
            while(PORTAbits.RA2 == 0 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 0){
                flashTwo();
            }
            
        } else if (PORTAbits.RA2 == 0 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 1) {
            while(PORTAbits.RA2 == 0 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 1){
                flashThree();
            }
            
        } else if (PORTAbits.RA2 == 0 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 1) {
            while(PORTAbits.RA2 == 0 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 1){
                flashConstant();
            }
           
        } else if (PORTAbits.RA2 == 1 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 0) {
            while(PORTAbits.RA2 == 0 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 1){
                flashConstant();
            }
            
        } else if (PORTAbits.RA2 == 1 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 1) {
            while(PORTAbits.RA2 == 0 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 1){
                flashConstant();
            }
            
        } else
            LATBbits.LATB8 = 1;
    }
    return;
}