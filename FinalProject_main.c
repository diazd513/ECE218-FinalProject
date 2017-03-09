/* 
 * File:   proj3_main.c
 * Author: Dariel & Kevin 
 *
 * Created on February 2, 2017, 2:45 PM
 */
#define FCY 3685000L

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "p24hxxxx.h"	
#include <libpic30.h>
#include "ser_2.h"
#include "adc.h"

#pragma config FNOSC=FRC // set oscillator mode for FRC ~ 8 Mhz
#pragma config FCKSM=CSDCMD //Both clock switching and fail-safe clock monitor disabled
#pragma config OSCIOFNC=ON // use OSCIO pin for digital I/O (RA3)
#pragma config POSCMD=NONE //Primary Oscillator disabled
#pragma config FWDTEN = OFF // turn off watchdog timer

#define p_min 188
#define p_max 376
#define PWM_PERIOD 376
#define led LATAbits.LATA0 
#define led2 LATAbits.LATA1
#define buzzer PORTBbits.RB1
#define switch PORTBbits.RB15
#define trig1 PORTBbits.RB7
#define echo1 PORTBbits.RB6

int pulse_width_A1 = 200;
int pulse_width_A2 = 127;
int echoTimer;

uint16_t soundval;
uint16_t lightval;
uint16_t clockInter;


void configIO(void){
        __builtin_write_OSCCONL(OSCCON & 0xbf) ; //clear bit 6 unlock
        RPOR0bits.RP1R = 0b10010; //Map pin1 (RP1R) (RP1=pin5)to OC1(0b10010))
        RPOR0bits.RP0R = 0b10011; 
        RPINR19bits.U2RXR = 10;
        RPOR5bits.RP11R = 5;
        RPINR7bits.IC1R = 6; //Connect IC1 to RP6 (pin 15)
        __builtin_write_OSCCONL(OSCCON | 0x40) ; //set bit 6 lock
    }

/*
 * Timer 2 based interrupt service routine this sets the OC1RS to be the pulse width 
 * defined by the potentiometer
 */
void _ISR _T2Interrupt(void){ 
     OC1RS = pulse_width_A1;
     OC2RS = 0;
    _T2IF = 0;    
}

void initOC12(){
    T2CONbits.TON = 0;
    PR2 = PWM_PERIOD;
    OC1RS = p_min;
    OC1R = p_max;
    OC2RS = p_min;
    OC2R = p_max;
    T2CONbits.TCKPS0 = 0;
    T2CONbits.TCKPS1 = 1;
    T2CONbits.TCS = 0;
    OC1CONbits.OCTSEL = 0;
    OC1CONbits.OCM= 0b110;
    OC2CONbits.OCTSEL = 0;
    OC2CONbits.OCM= 0b110;
    T2CONbits.TON = 1;
}
  



void initIC(){
   T3CONbits.TON = 0;
   IC1CONbits.ICTMR = 0;
   IC1CONbits.ICI = 0b00;
   IC1CONbits.ICM = 0b001;
   T3CONbits.TCKPS0 = 0;
   T3CONbits.TCKPS1 = 0;
   T3CONbits.TCS = 0;
   PR3 = 0xFFFF;
   T3CONbits.TON = 1;
}

/*
 This is the interrupt that happens after every fourth 
 * rising edge on the clock cycle 
 * it sets the previous edge as the last edge gets 
 * a new recent edge from the IC1BUF then does calculations 
 * to find the count
 */
void _ISR _IC1Interrupt(void){
    clockInter = IC1BUF;
    _IC1IF = 0;  
    
    
     
}

void sendPulse1(void){
    trig1 = 1;
    __delay_ms(0.01);
    trig1 = 0;
    
}
uint16_t readDistance1(void){
    uint16_t time = 0;
    while(1){
        sendPulse1();
        
    }
}

/*
 *  
 */
int main(void) {
    
    configIO();
    initOC12();
    initIC();
    initUART2();
    
    AD1PCFGL = 0xffff;
    TRISA = 0x0000;
    TRISB = 0x0000;
    LATA = 0x0000;
    TRISAbits.TRISA0 = 0; //led
    TRISAbits.TRISA1 = 0; //led
    TRISBbits.TRISB1 = 0; //buzzer
    TRISBbits.TRISB15 = 1; // switch is input
    TRISBbits.TRISB6 = 1; //echo is an input
    TRISBbits.TRISB7 = 0; //trig is output
    _IC1IF = 0;
    _IC1IE = 1; 
    _T2IF = 0;
    _T2IE = 1; 
    
    
 
    char stringLight[100];
    char stringSound[100];
    char ping[25];
    while(1){
        
        led = ~led;
        __delay_ms(1000);
        led2 = ~led2;
        initADC(11);
        lightval = readADC(11);
        sprintf(stringLight, "light sensor %i " ,lightval);
        putsU2(stringLight);
        putU2('\n');
        

        initADC(10);
        soundval = averagevalue(10);
        sprintf(stringSound,"sound sensor %i", soundval);
        putsU2(stringSound);
        putU2('\n');
        
        sprintf(ping,"time %i", clockInter);
     //   buzzer = 1;
        
        
        
    }
            
    
}