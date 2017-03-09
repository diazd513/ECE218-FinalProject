/* 
 * File:   ser.h
 * Author: ReidKnapp
 *
 * Created on February 9, 2017, 3:10 PM
 */

#ifndef SER_H
#define	SER_H

#ifdef	__cplusplus
extern "C" {
#endif



void initUART2(void);
void putU2(char c);
char getU2(void);
void putsU2(char *s);


#ifdef	__cplusplus
}
#endif

#endif	/* SER_H */

