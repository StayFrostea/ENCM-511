/* 
 * File:   main.c
 * Author: Jeff Roszell
 *
 * Created on October 11, 2020, 2:47 PM
 */

#include <xc.h>

void main(int argc, char** argv) {
    
    TRISBbits.TRISB8 = 0;
    while(1){
        LATBbits.LATB9 = 1;
    }

    return;
}

