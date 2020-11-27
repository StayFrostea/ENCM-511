/*
 * File:   main.c
 * Author: Long Tran, Jeff Roszell
 *
 * Created on November 18, 2020, 10:36 PM
 */

// MPLAB header libraries
#include <xc.h>

// User header files

#include "ChangeClk.h"
#include "TimeDelay.h"
#include "IOs.h"
#include "UART2.h"
#include "ADC.h"

#pragma config FCKSM = CSECMD // Clock switching is enabled, clock monitor disabled

//MAIN
void main(void) {
     
    // Change Clock
    NewClk(500); // 8 for 8 MHz; 500 for 500 kHz; 32 for 32 kHz
    
    uint16_t rV;//For string the result of the conversion
    
    InitUART2();

    while(1){
        rV = do_ADC();
    }             
    
    return;
}

