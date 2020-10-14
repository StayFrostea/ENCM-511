/* 
 * File:   main.c
 * Author: Jeff Roszell
 *
 * Created on October 11, 2020, 2:47 PM
 */

#include <xc.h>

#include "IOs.h"

//unsigned int GlobalVar;


// Main is the first thing that the MCU will load

void main(void) {

    //Main is only for function calls
    //Create the functions in their own c files and accompany with a header

    IOinit();
    
    IOcheck();

    return;
}

