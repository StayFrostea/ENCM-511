/* 
 * File:   IOs.c
 * Author: longt, Jeff Roszell
 *
 * Created on November 19, 2020, 6:25 PM
 */

#include "xc.h"
#include "ChangeClk.h"
#include "TimeDelay.h"
#include "IOs.h"
#include "UART2.h"

//// IOinit() with IO interrupts
void IOinit(void)
{
    TRISBbits.TRISB8 = 0; // Make GPIO RB8 as a digital output
    LATBbits.LATB8 = 0; // Make GPIO RB8 as a digital output
    
    TRISAbits.TRISA4 = 1; // Makes GPIO RA4 as a digital input
    CNPU1bits.CN0PUE = 1; // Enables pull up resistor on RA4/CN0
    CNEN1bits.CN0IE = 1;  // enable CN on CN0
    
    TRISBbits.TRISB4 = 1; // Makes GPIO RB4 as a digital input
    CNPU1bits.CN1PUE = 1; // Enables pull up resistor on RA4/CN0
    CNEN1bits.CN1IE = 1;  // enable CN on CN1
    
    TRISAbits.TRISA2 = 1; // Makes GPIO RA2 as a digital input
    CNPU2bits.CN30PUE = 1; // Enables pull up resistor on RA4/CN0
    CNEN2bits.CN30IE = 1;  // enable CN on CN30
    
    //CN Interrupt settings
    IPC4bits.CNIP = 6; //7 is highest priority, 1 is lowest, 0 is disabled interrupt
    IFS1bits.CNIF = 0; // Clear interrupt flag
    IEC1bits.CNIE = 1; //Enable CN interrupts
    
    NewClk(32);
    return;
}

uint16_t time=0; //time displayed on the terminal
uint16_t pressTime=0; //time RB4 is pressed in ms
uint16_t resetFlag=1; //flag to avoid printing ALARM when timer is reset to 0
uint16_t runFlag=0; //flag to indicate that the timer is running

void IOcheck(void)
{
    NewClk(32);
    IEC1bits.CNIE = 0; //disable CN interrupts to avoid debounces
    delay_ms(400,1);   // 400 msec delay to filter out debounces 
    IEC1bits.CNIE = 1; //Enable CN interrupts to detect pb release
    
    //PB1 Instructions
    while((PORTAbits.RA4 == 1) && (PORTBbits.RB4 == 1) && (PORTAbits.RA2 == 0)) //While only RA2 pb1 is pressed
    {
        pressTime=0; // reset pressTime
        resetFlag=0;
        time+=60; // add 1 min to timer
        NewClk(8); // speed up clock just for display
        Disp2String("\r"); // print time to terminal
        Disp2Dec(time/60*1000);
        Disp2String("m : ");
        Disp2Dec(time%60*1000);
        Disp2String("s         ");
        NewClk(32); // slow down clock for delay and other tasks
        delay_ms(800,1);   // 0.8 sec delay
    }
    
    //PB2 Instructions
    while((PORTBbits.RB4 == 1) && (PORTAbits.RA4 == 0) &&  (PORTAbits.RA2 == 1)) //While only RA4 pb2 is pressed
    {
        time+=1; // add 1 sec to timer
        pressTime=0; // reset pressTime
        resetFlag=0;
        NewClk(8);
        Disp2String("\r"); // print time to terminal
        Disp2Dec(time/60*1000);
        Disp2String("m : ");
        Disp2Dec(time%60*1000);
        Disp2String("s         ");        
        NewClk(32); // slow down clock for delay and other tasks
        delay_ms(800,1);   // 0.8 sec delay
    }
    
    //PB3 Instructions
    while((PORTAbits.RA2 == 1) && (PORTAbits.RA4 == 1) && (PORTBbits.RB4 == 0)) //While only RB4 pb3 is pressed
    {
        delay_ms(100,1);   // 0.2 sec delay
        pressTime+=100; // update pressTime
    }
    return;
}

void IOmain(){
    //depends on value of pressTime do the following
    if(pressTime>3000) //pressTime > 3000
    {
        time=0; //reset timer
        pressTime=0; //reset pressTime
        resetFlag=1;
        runFlag=0;
    } 
    else if(pressTime>0) // 0<pressTime<3000
    {
        pressTime=0; // set pressTime to 0 so multiple short press wont increase the pressTime
        resetFlag=0;
        runFlag=runFlag^1; //Change run flag
    }
    if(time>0 && runFlag==1){
        time -= 1; // decrease timer
        delay_ms(1000,1);   // 1 sec delay
    }
    if(runFlag==1){ //control the LED
        if(time==0){
            LATBbits.LATB8 = 1; // LED on
        } else {
            LATBbits.LATB8 = LATBbits.LATB8 ^ 1; //blink the LED
        }
    } else {
        LATBbits.LATB8 = 0; // LED off       
    }
    NewClk(8);
    Disp2String("\r"); // print time to terminal
    Disp2Dec(time/60*1000);
    Disp2String("m : ");
    Disp2Dec(time%60*1000);
    if(time==0&&resetFlag==0){
        Disp2String("s -- ALARM");
    } else {
        Disp2String("s         ");
    }
    NewClk(32); // slow down clock for delay and other tasks    
}

///// Change of pin Interrupt subroutine
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void)
{
	CNflag = 1;  // global user defined flag - use only if needed
	IFS1bits.CNIF = 0;		// clear IF flag
    T2CONbits.TON = 0; // Disable timer
    IEC0bits.T2IE = 0; //Disable timer interrupt
    IOcheck();
	Nop();	
    
    return;
}

    