/* 
 * File:   IOcheck.c
 * Author: Jeff R, Long T
 *
 * Created on November 3, 2020, 2:03 AM
 */
#include <p24fxxxx.h>
#include <p24F16KA101.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include "UART2.h"

#include "IOs.h"

//Preprocessor directives - Configuration bits for MCU start up
#pragma config FCKSM = CSECMD // Clock switching is enabled, clock monitor disabled

// MACROS for Idle, Sleep modes
#define Nop() {__asm__ volatile ("nop");}
#define ClrWdt() {__asm__ volatile ("clrwdt");}
#define Sleep() {__asm__ volatile ("pwrsav #0");}   //Sleep() - put MCU in sleep mode - CPU and some peripherals off
#define Idle() {__asm__ volatile ("pwrsav #1");}    //Idle() - put MCU in idle mode - only CPU off
#define dsen() {__asm__ volatile ("BSET DSCON, #15");} //

static unsigned int c = 111;

//clkval = 8 for 8MHz; 
//clkval = 500 for 500kHz; 
//clkval = 32 for 32kHz; 

void NewClk(unsigned int clkval) {
    char COSCNOSC;
    if (clkval == 8) //8MHz
    {
        COSCNOSC = 0x00;
    } else if (clkval == 500) // 500 kHz
    {
        COSCNOSC = 0x66;
    } else if (clkval == 32) //32 kHz
    {
        COSCNOSC = 0x55;
    } else // default 32 kHz
    {
        COSCNOSC = 0x55;
    }
    SRbits.IPL = 7; //Disable interrupts
    CLKDIVbits.RCDIV = 0; // CLK division = 0
    __builtin_write_OSCCONH(COSCNOSC); // (0x00) for 8MHz; (0x66) for 500kHz; (0x55) for 32kHz;
    __builtin_write_OSCCONL(0x01);
    OSCCONbits.OSWEN = 1;
    while (OSCCONbits.OSWEN == 1) {
    }
    SRbits.IPL = 0; //enable interrupts
}

void IOinit(void) {
    AD1PCFG = 0xFFFF; // Turn all analog pins as digital  
    //Digital Output
    TRISBbits.TRISB8 = 0; //set RB8 to be output
    //Digital Inputs
    TRISAbits.TRISA2 = 1; //set RA2 to be input
    TRISBbits.TRISB4 = 1; //set RB4 to be input
    TRISAbits.TRISA4 = 1; //set RA4 to be input

    //CN Interrupt
    CNEN1bits.CN0IE = 1;
    CNEN1bits.CN1IE = 1;
    CNEN2bits.CN30IE = 1;

    //Set pull up
    CNPU1bits.CN0PUE = 1; //RA4 pullup
    CNPU1bits.CN1PUE = 1; //RB4 pullup
    CNPU2bits.CN30PUE = 1; //RA2 pullup

    //Interrupt priority
    IPC4bits.CNIP2 = 1;
    IPC4bits.CNIP1 = 1;
    IPC4bits.CNIP0 = 1;

    //Enable interrupt
    IEC1bits.CNIE = 1;
    //Interrupt nesting enabled
    INTCON1bits.NSTDIS = 1;
    //Clear CN Flag
    IFS1bits.CNIF = 0;
    NewClk(8);

}

void Delay_ms(uint16_t time_ms) {
    //Configure clock
    NewClk(32);

    //Configure Timer2
    T2CONbits.TSIDL = 0; //Continue module operation in idle mode
    T2CONbits.T32 = 0; //Timer2 and 3 act as two 16-bit timers
    T2CONbits.TCS = 0; //Use internal clock

    TMR2 = 0; //Clear TMR2

    //Interrupt configuration
    IEC0bits.T2IE = 1; //Enable interrupt
    IFS0bits.T2IF = 0; //clear interrupt flag

    T2CONbits.TON = 1; //Start 16-bit Timer2

    PR2 = 16 * time_ms; //Calculate PR2
    Idle(); //Switch to idle mode

    return;
}

//Timer2 interrupt sub routine

void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void) {
    IFS0bits.T2IF = 0; //clear interrupt flag
    T2CONbits.TON = 0; //stop the timer
}

//CNInterrupt interrupt sub routine

void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void) {
    IEC1bits.CNIE = 0;
    //8Mhz clock
    NewClk(8);

    if (IFS1bits.CNIF == 1) {
        //        c=PORTAbits.RA2*100+PORTAbits.RA4*10+PORTBbits.RB4; //generate a case number
        if (PORTAbits.RA2 == 0 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 1) {
            Disp2String("\rPB1 is pressed        "); //print the required string
            c=0;
        } else if (PORTAbits.RA2 == 1 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 1) {
            Disp2String("\rPB2 is pressed        "); //print the required string
            c=1;
        } else if (PORTAbits.RA2 == 1 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 0) {
            Disp2String("\rPB3 is pressed        "); //print the required string
            c=2;
        } else if (PORTAbits.RA2 == 0 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 1) {
            Disp2String("\rPB1 AND PB2 are pressed"); //print the required string
            c=3;
        } else if (PORTAbits.RA2 == 0 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 0) {
            Disp2String("\rPB1 AND PB3 are pressed"); //print the required string
            c=4;
        } else if (PORTAbits.RA2 == 1 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 0) {
            Disp2String("\rPB2 AND PB3 are pressed"); //print the required string
            c=5;
        } else if (PORTAbits.RA2 == 0 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 0) {
            Disp2String("\rAll PBs pressed        "); //print the required string
            c=6;
        } else {
            Disp2String("\rNothing pressed        "); //print the required string
            c=7;
        }
    }
    IFS1bits.CNIF = 0;
    IEC1bits.CNIE = 1;
    //8Mhz clock
    NewClk(32);
    Nop();
}

void IOcheck(void) {
    switch (c) {
        case 0: //only PB1 is pressed
            LATBbits.LATB8 = 1; //LED on
            Delay_ms(500);//delay for 333 since it takes 667ms to send the above string
            LATBbits.LATB8 = 0; //LED off
            Delay_ms(500); //delay for 500
            break;
        case 1: //only PB2 is pressed
            LATBbits.LATB8 = 1; //LED on
            Delay_ms(2000); //delay for 1333 since it takes 667ms to send the above string
            LATBbits.LATB8 = 0; //LED off
            Delay_ms(2000); //delay for 1333 since it takes 667ms to send the above string
            break;
        case 2: //only PB3 is pressed
            LATBbits.LATB8 = 1; //LED on
            Delay_ms(3000); //delay for 333 since it takes 667ms to send the above string
            LATBbits.LATB8 = 0; //LED off//LED off
            Delay_ms(3000); //delay for 2333 since it takes 667ms to send the above string
            break;
        case 3: //PB1 AND PB2
            LATBbits.LATB8 = 1; //LED on
            break;
        case 4: //PB1 AND PB3
            LATBbits.LATB8 = 1; //LED on
            break;
        case 5: //PB2 AND PB3
            LATBbits.LATB8 = 1; //LED on
            break;
        case 6: //All
            LATBbits.LATB8 = 1; //LED on
            break;
        case 7: //No Press
            LATBbits.LATB8 = 0; //LED off
            break;
    }
    return;
}



