/* 
 * File:   main.c
 * Author: longt
 *
 * Created on November 3, 2020, 2:03 AM
 */

// User header files
#include "IOs.h"

void main(void) {
    IOinit();
    while(1){
        IOcheck();
    }
}


