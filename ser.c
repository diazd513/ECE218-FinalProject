/*
 * File:   ser.c
 * Author: Kevin
 *
 * Created on February 9, 2017, 11:18 AM
 */

#include <p24HJ128GP502.h>



#include "p24hxxxx.h"
#include "ser.h"

#define FCY 3685000L//define the instruction clock used in BRG calculation
#define BAUDRATE 9600//a standard baud rate available to most serial terminals
#define BRGVAL ((FCY/BAUDRATE/16)-1)    //value for Baud Rate Generator register

void initUART2(void){
    
    U2BRG = BRGVAL;
    U2MODEbits.UARTEN = 1;
    U2STAbits.UTXEN = 1;
    
}

void putU2(char c){
    while(U2STAbits.UTXBF){
    
    }
    U2TXREG = c;
}   

char getU2(void){
    while(!U2STAbits.URXDA){
    }
    return U2RXREG;
}

void putsU2(char *s){
    
    while(*s ){
        putU2(*s++);
       
    }
    putU2('\n');
    putU2('\r');
    
}