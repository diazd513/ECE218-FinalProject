#include "p24hxxxx.h"
#include "adc.h"
char bcd10000 =0;
char bcd1000 =0;
char bcd100 =0 ;
char bcdtens =0 ;
char bcdunits =0;
int average =0;

int readADC( int ANpin)
{
    AD1CHS0  = ANpin;               // select analog input channel based upon I/o expansion pin
    AD1CON1bits.SAMP = 1;       // start sampling, automatic conversion will follow
    while (!AD1CON1bits.DONE);   // wait to complete the conversion
    return ADC1BUF0;            // read the conversion result
} // readADC

void initADC( int ANpin) 
{  //recheck bit settings for 12 bits
    AD1PCFGL =mask_translate(ANpin) ;    // select analog input pins
    AD1CON1 = 0x00E0;   // auto convert after end of sampling
  	 AD1CON1bits.AD12B =1;			//set to 12 bits 
    AD1CSSL = 0;        // no scanning required 
    AD1CON3 = 0x1F02;   // system clock,max sample time = 31Tad, Tad = 2 x Tcy = 125ns >75ns
    AD1CON2 = 0;        // use MUXA, AVss and AVdd are used as Vref+/-
    AD1CON1bits.ADON = 1; // turn on the ADC
} //initADC


int mask_translate(int ANpin) {
	//recheck pin setting for Microstick
	int tempmask =0xfffe;   //default to pin 2 AN0
	if (ANpin==0) tempmask= 0xfffe;
	if (ANpin==1) tempmask= 0xfffd;
    if (ANpin==2) tempmask= 0xfffb;
    if (ANpin==3) tempmask= 0xfff7;
	if (ANpin==4) tempmask= 0xffef;
	if (ANpin==5) tempmask= 0xffdf;
	if (ANpin==9) tempmask= 0xfdff;
	if (ANpin==10) tempmask= 0xfbff;
	if (ANpin==11) tempmask= 0xf7ff;
	if (ANpin==12) tempmask= 0xefff;
	return tempmask;
	
}
void binary_to_ASCIIconvert( int n) {
bcd10000 =0x00;
bcd1000=0x00;
bcd100 =0x00;
bcdtens =0x00;
bcdunits =0x00;
while (n>=10000) {
n = n-10000;
bcd10000 =bcd10000+1;
}
while (n>=1000) {
n = n-1000;
bcd1000 =bcd1000+1;
}
while (n>=100) {
n = n-100;
bcd100 =bcd100+1;
}
while (n>=10) {
n = n-10;
bcdtens =bcdtens+1;
}

 bcdunits =n;
bcd10000 =bcd10000+0x30;
bcd1000 =bcd1000+0x30;
bcd100 =bcd100+0x30;
bcdtens =bcdtens+0x30;
bcdunits =bcdunits+0x30;
}
//performs running average
int averagevalue (int ANpin) {
	int pin = ANpin;
	average =0;
	average = readADC(pin);
	average = average +readADC(pin);
	average = average +readADC(pin);
	average = average +readADC(pin);
	average = average/4;
	return average;
	
	
}
		