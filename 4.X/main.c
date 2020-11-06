/* 
 * File:   main.c
 * Author: Jeff R, Long T
 *
 * Created on November 3, 2020, 2:03 AM
 */

// User header files
#include "IOs.h"

void main(void) {
    IOinit();
    Disp2String("Nothing pressed        "); //print the required string
    while(1){
        IOcheck();
    }
}


