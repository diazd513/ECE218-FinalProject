#include "ser.h"
#include "p24hxxxx.h"



#define FCY 3685000L //define the instruction clock used in BRG calculation
#define BAUDRATE 9600 //a standard baud rate available to most serial terminals
#define BRGVAL ((FCY/BAUDRATE/16) - 1) //value for Baud Rate Generator register

void initUART2(void){
    U2BRG = BRGVAL;
    U2MODEbits.UARTEN = 1;
    U2STAbits.UTXEN = 1;
}

void putU2(char c){
    
    while(U2STAbits.UTXBF != 0){}
    U2TXREG = c;
}

char getU2(void){
    while(U2STAbits.URXDA != 1){ }
    char c = U2RXREG;
    return c;
}


void putsU2(char *s){
    while(*s!='\0'){
        putU2(*s);
        s++;
    }
    putU2('\r');
}

