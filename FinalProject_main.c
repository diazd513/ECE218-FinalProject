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
#include "ser.h"
#include "adc.h"

#pragma config FNOSC=FRC // set oscillator mode for FRC ~ 8 Mhz
#pragma config FCKSM=CSDCMD //Both clock switching and fail-safe clock monitor disabled
#pragma config OSCIOFNC=ON // use OSCIO pin for digital I/O (RA3)
#pragma config POSCMD=NONE //Primary Oscillator disabled
#pragma config FWDTEN = OFF // turn off watchdog timer


#define redLED LATAbits.LATA0 
#define blueLED LATAbits.LATA1
#define buzzer PORTBbits.RB1
#define switch PORTBbits.RB15
#define trig1 PORTBbits.RB7
#define echo1 PORTBbits.RB6
#define trig2 PORTBbits.RB5
#define echo2 PORTBbits.RB4
#define trig3 PORTBbits.RB9
#define echo3 PORTBbits.RB8

typedef int boolean;
#define true 1
#define false 0
int tripped = 0; 

char enterPassword[] = "Please enter the password: ";
char passwordEntered[] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\0'};
char password[] = "password";


int i;

uint16_t soundVal, lightVal, clockInter1, clockInter2,clockInter3,risingEdge1 = 0, fallingEdge1 = 0,risingEdge2 = 0, fallingEdge2= 0,risingEdge3 = 0, fallingEdge3 = 0;
uint16_t soundValLimit = 100, lightValLimit = 20, proximityLimit = 200;
int tester = 0;
char variable= '0';


void configIO(void){
        __builtin_write_OSCCONL(OSCCON & 0xbf) ; //clear bit 6 unlock
        RPOR0bits.RP1R = 0b10010; //Map pin1 (RP1R) (RP1=pin5)to OC1(0b10010))
        RPOR0bits.RP0R = 0b10011; 
        RPINR19bits.U2RXR = 10;
        RPOR5bits.RP11R = 5;
        RPINR7bits.IC1R = 6; //Connect IC1 to RP6 (pin 15)
        RPINR7bits.IC2R = 8;
        RPINR10bits.IC7R = 4;
        __builtin_write_OSCCONL(OSCCON | 0x40) ; //set bit 6 lock
    }

/*
 * Timer 2 based interrupt service routine this sets the OC1RS to be the pulse width 
 * defined by the potentiometer
 */
void _ISR _T2Interrupt(void){ 
     OC2RS = 0;
    _T2IF = 0;    
}

void initOC12(){
    T2CONbits.TON = 0;
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
   T3CONbits.TCKPS1 = 1;
   T3CONbits.TCS = 0;
   PR3 = 0xFFFF;
   T3CONbits.TON = 1;
   
   IC2CONbits.ICTMR = 0;
   IC2CONbits.ICI = 0b00;
   IC2CONbits.ICM = 0b001;
   
   IC7CONbits.ICTMR = 0;
   IC7CONbits.ICI = 0b00;
   IC7CONbits.ICM = 0b001;
   
}

/*
 This is the interrupt that happens after every fourth 
 * rising edge on the clock cycle 
 * it sets the previous edge as the last edge gets 
 * a new recent edge from the IC1BUF then does calculations 
 * to find the count
 */
void _ISR _IC1Interrupt(void){
    fallingEdge1 = risingEdge1;
    risingEdge1 = IC1BUF;
    clockInter1 = risingEdge1 - fallingEdge1;
    _IC1IF = 0;  
    
     
}

void _ISR _IC2Interrupt(void){
    fallingEdge2 = risingEdge2;
    risingEdge2 = IC2BUF;
    clockInter2 = risingEdge2 - fallingEdge2;
    _IC2IF = 0;  
    
     
}
void _ISR _IC7Interrupt(void){
    fallingEdge3 = risingEdge3;
    risingEdge3 = IC7BUF;
    clockInter3 = risingEdge3 - fallingEdge3;
    _IC7IF = 0;  
    
     
}
void sendPulse1(void){
    trig1 = 0;
    trig1 = 1;
    __delay_us(10);
    trig1 = 0;
    
}
void sendPulse3(void){
    trig3 = 0;
    trig3 = 1;
    __delay_us(10);
    trig3 = 0;
    
}
void sendPulse2(void){
    trig2 = 0;
    trig2 = 1;
    __delay_us(10);
    trig2 = 0;
    
}

void __attribute__((interrupt,no_auto_psv))U2RXInterrupt(){
    
    variable =(char) U2RXREG & 0x00FF;
    _U2RXIF = 0;
    tester ++;
}

void alarmOn(void){
    putsU2("The alarm has been tripped!");
    putU2('\n');
    putU2('\r');
    tripped = true;
    
    while(tripped == true){
        i = 0;
        putsU2(enterPassword);
        blueLED = ~blueLED;
        __delay_ms(150);
        redLED = ~redLED;

        while (i < 8){
            while(!U2STAbits.URXDA){
                blueLED = ~blueLED;
                __delay_ms(150);
                redLED = ~redLED;
            }
            passwordEntered[i] = getU2();
            putU2(passwordEntered[i]);
            i++;
            blueLED = ~blueLED;
            __delay_ms(150);
            redLED = ~redLED;
        }
        putU2('\n');
        putU2('\r');
        if (passwordEntered[0] == password[0]){
            blueLED = ~blueLED;
            __delay_ms(150);
            redLED = ~redLED;
            if (passwordEntered[1] == password[1]){
                blueLED = ~blueLED;
                __delay_ms(150);
                redLED = ~redLED;
                if (passwordEntered[2] == password[2]){
                     blueLED = ~blueLED;
                    __delay_ms(150);
                     redLED = ~redLED;
                    if (passwordEntered[3] == password[3]){
                        blueLED = ~blueLED;
                        __delay_ms(150);
                        redLED = ~redLED;
                        if (passwordEntered[4] == password[4]){
                            blueLED = ~blueLED;
                            __delay_ms(150);
                            redLED = ~redLED;
                            if (passwordEntered[5] == password[5]){
                                blueLED = ~blueLED;
                                __delay_ms(150);
                                redLED = ~redLED;
                                if (passwordEntered[6] == password[6]){
                                     blueLED = ~blueLED;
                                     __delay_ms(150);
                                     redLED = ~redLED;
                                    if (passwordEntered[7] == password[7]){
                                        putsU2("Password Accepted! The alarm will now be disarmed");
                                        tripped = 0;
                                        blueLED = 0; 
                                        redLED = 0;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else{
            blueLED = ~blueLED;
            __delay_ms(150);
            redLED = ~redLED;
            putsU2("Incorrect Password. Please try again");
            putU2('\n');
            putU2('\r');
            blueLED = ~blueLED;
            __delay_ms(150);
            redLED = ~redLED;
        }
    }


}






void blinkLEDs(){
    int count = 0;
    while (1){
        if(redLED == 0){
            blueLED = ~blueLED;
        }
        if (blueLED == 0){
            redLED = ~redLED;
        }
       count++;
        __delay_ms(150);

    }
}



int main(void) {
    
    configIO();
    initOC12();
    initIC();
    initUART2();
    
    AD1PCFGL = 0xffff;
    TRISA = 0x0000;
    TRISB = 0x0000;
    LATA = 0x0000;
    _IC1IF = 0;
    _IC1IE = 1;
    _IC2IE = 1;
    _IC2IF = 0;
    _IC7IF = 0;
    _IC7IE = 1;
    _T2IF = 0;
    _T2IE = 1; 
//    alarmOn();
    TRISAbits.TRISA0 = 0; //led
    TRISAbits.TRISA1 = 0; //led
    TRISBbits.TRISB15 = 1; // switch is input
    char testTT[25];
    char stringLight[100];
    char stringSound[100];
    char ping[25];
    
    while(1){
//        if (switch == 0){
//            putsU2("Alarm is Disabled.");
//            blueLED = 0;
//            redLED = 0;
//        }
//        else {
//          TRISBbits.TRISB1 = 0; //buzzer
            TRISBbits.TRISB6 = 1; //echo is an input
            TRISBbits.TRISB7 = 0; //trig is output
            TRISBbits.TRISB4 = 1; 
            TRISBbits.TRISB5 = 0;
            TRISBbits.TRISB8 = 1;
            TRISBbits.TRISB9 = 0;
       //     putsU2("Alarm is Enabled!");
            sendPulse1();
            sendPulse2();
            sendPulse3();           
            initADC(11);
            lightVal = readADC(11);
            initADC(10);
            soundVal = readADC(10);
       // }

        
        sprintf(stringLight, "Light: %i" ,lightVal);
        putsU2(stringLight);
        putU2('\n');
        putU2('\n');
//        
//

        sprintf(stringSound,"Sound: %i", soundVal);
        putsU2(stringSound);
        putU2('\n');
        putU2('\n');
        
                
        
        sprintf(ping,"Prox1: %i, Prox2: %i, Prox3: %i", clockInter1, clockInter2, clockInter3);
        putsU2(ping);
        putU2('\n');
        putU2('\n');
//        
//        
//        sprintf(testTT,"IC interrupts %i", tester);
//        putsU2(testTT);
//        putU2('\n');
//        putU2('\n');
        
//        if(soundVal > soundValLimit){
//            putsU2("I checked Sound!");
//            putU2('\n');
//            putU2('\n');
//            alarmOn();
//            
//        }
//        else if (lightVal > lightValLimit){
//            putsU2("I checked Light!");
//            putU2('\n');
//            putU2('\n');
//            alarmOn();
//        }
//        else if (clockInter1 > 0 || clockInter2 > 0 || clockInter3 > 0){
//                if (clockInter1 < proximityLimit || clockInter2 < proximityLimit || clockInter3 < proximityLimit){
//                    putsU2("I checked Proximity");
//                    putU2('\n');
//                    putU2('\n');
//                    alarmOn();
//                }
//        }
        __delay_ms(10000);
//     //   buzzer = 1;
//        
//        
        
    
        
 //   }  
    }
}