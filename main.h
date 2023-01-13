/* 
 * File: main.h  
 * Author: Eder Torres
 * Comments: adding header file to clean up main file
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef MAIN_H
#define	MAIN_H

#include <xc.h> // include processor files 
#include <stdio.h>

//Register: CONFIG1H @0x300001
#pragma config FOSC = HSHP      //High Speed Oscillator, High Power
#pragma config PLLCFG = ON      //4x PLL enabled
#pragma config PRICLKEN = ON    // Primary clock is always enabled
#pragma config FCMEN = OFF      //Fail-Safe Clock Monitor disabled
#pragma config IESO = OFF       //Oscillator Switchover mode disabled

//Register: CONFIG2L @0x300002
#pragma config PWRTEN = OFF     //Power up timer disabled
#pragma config BOREN = OFF      //Brown-out Reset disabled

//Register: CONFIG2H @0x300003
#pragma config WDTEN = OFF      //WDT is always disabled

//Register: CONFIG3H @0x30005
#pragma config CCP2MX = PORTC1  //CCP2 is muxed with RC1 (unused)
#pragma config PBADEN = OFF     //PORTB[5:0] pins are configured as Digital I/O on reset
#pragma config CCP3MX = PORTE0  //P3A/CCP3 is muxed with RE0
#pragma config HFOFST = OFF     //Fast startup is disabled
#pragma config T3CMX  = PORTC0  //T3CKI is on RC0
#pragma config P2BMX  = PORTC0  //P2b is on RC0
#pragma config MCLRE  = EXTMCLR //MCLR pin enabled

//Register: CONFIG4L @0x300006
#pragma config STVREN = ON      //Stack full/underflow will cause Reset
#pragma config LVP    = OFF     //single supply ICSP disabled
#pragma config DEBUG  = OFF     //background debug disabled

//Register: CONFIG5L @0x300008
#pragma config CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF
//Register: CONFIG5H @0x300009
#pragma config CPB = OFF, CPD = OFF
//Register: CONFIG6L @0x30000A
#pragma config WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF
//Register: CONFIG6L @0x30000B
#pragma config WRTC = OFF, WRTB = OFF, WRTD = OFF
//Register: CONFIG7L @0x30000C
#pragma config EBTR0 = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF
//Register: CONFIG7H @0x30000D
#pragma config EBTRB = OFF

#define RED_LED_DIR     TRISAbits.TRISA0
#define RED_LED         LATAbits.LATA0
#define GREEN_LED_DIR   TRISAbits.TRISA1
#define GREEN_LED       LATAbits.LATA1
#define YELLOW_LED_DIR  TRISAbits.TRISA2
#define YELLOW_LED      LATAbits.LATA2

#define UART1_INTERRUPT 1
//#define UART2_INTERRUPT 1
//#define SSP1_INTERRUPT 1
//#define TIMER1_INTERRUPT 1

#endif	

