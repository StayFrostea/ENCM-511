/* 
 * File: ChangeClk.h
 * Author: Jeff, Long
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef CHANGE_CLK
#define	CHANGE_CLK

#include <xc.h> // include processor files - each processor file is guarded.  

void NewClk(unsigned int clkval);

#endif	/* CHANGE_CLK */