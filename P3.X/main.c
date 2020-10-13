/* 
 * File:   main.c
 * Author: Jeff Roszell
 *
 * Created on October 11, 2020, 2:47 PM
 */

#include <xc.h>

#include "IOs.h"


// Main is the first thing that the MCU will load

void main(void) {

    IOinit();

    IOcheck();

    return;
}

