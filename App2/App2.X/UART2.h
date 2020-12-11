/*
 * File:   UART2.h
 * Author: rvyas
 *
 * Created on November 19, 2016, 9:22 PM
 */

#ifndef UART2_H
#define UART2_H

void initUART2(void);
void setBaudUART2(void);
void endUART2(void);

void nWriteUART2(char c, unsigned int n);
void writeUART2(char c);
void printUART2(char *str);

void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt(void);
void __attribute__((interrupt, no_auto_psv)) _U2TXInterrupt(void);

void Disp2Hex(unsigned int);
void Disp2Hex32(unsigned long int);
void printU16(uint16_t val, int digits);
void printFloat(float val);
void clearLine(void);

enum UART_Tx_Intr_Mode {
	TX_BUF_AVAIL,
	TX_DONE,
	TX_BUF_EMPTY
};

inline void setTxIntrModeUART2(const enum UART_Tx_Intr_Mode mode);

#endif /* UART2_H */
