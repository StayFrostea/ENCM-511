/*
 * File:   main.c
 * Author: longt , Jeff Roszell
 *
 * Created on Nov 27, 2020, 03:47 PM
 */

// MPLAB header libraries
#include <xc.h>

#include <p24fxxxx.h>
#include <p24F16KA101.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>

#include "ChangeClk.h"
#include "ADC.h"

#pragma config FCKSM = CSECMD 

int main(void) {
    // Change Clock
    NewClk(32); // 8 for 8 MHz; 500 for 500 kHz; 32 for 32 kHz
    
    while(1) 
    {
        drawGraph(do_ADC());//Display to terminal the bar graph along with the value
    }
    
    return 0;
}
