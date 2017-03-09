//revision thk 11/25/09 N+V simplified library
//comments modified and all A/D pins added - CT 12/23/16
#ifndef __ADCDRV_H__
#define __ADCDRV_H__ 

// expansion bus pin to ADC channel number
//only pins 2-7 (AN0-AN5)and 23-26 (AN12-AN9) can support analog ADC on this part
extern char bcd10000;
extern char bcd1000;
extern char bcd100;
extern char bcdtens;
extern char bcdunits; 
#define AN0 0
#define AN1 1
#define AN2 2
#define AN3 3
#define AN4 4
#define AN5 5
#define AN9 9
#define AN10 10
#define AN11 11
#define AN12 12


int readADC( int ANpin);   //parameter I/O expansion pin number this function does conversion
void initADC( int ANpin);	//initialize ADC operation of this I/O pin
int mask_translate(int IOpin);
void binary_to_ASCIIconvert( int n);
int averagevalue (int ANpin);

#endif








