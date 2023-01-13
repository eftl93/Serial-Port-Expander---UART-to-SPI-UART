#ifndef SPI_H
#define SPI_H

#include <stdio.h>
#include <string.h>
#include <xc.h>

#define SS0 LATBbits.LATB2 //slave select 0 connected to LATB.2
#define SS1 LATBbits.LATB3 //slave select 1 connected to LATB.3
#define SS2 LATBbits.LATB4 //slave select 2 connected to LATB.4
#define SS3 LATBbits.LATB5 //slave select 3 connected to LATB.5
#define SCK1 PORTCbits.RC3 //serial clock 1 connected to PORTC.3
#define SDI1 PORTCbits.RC4 //serial data in 1 connected to PORTC.4
#define SDO1 PORTCbits.RC5 //serial data out 1 connected to PORTC.5

//defines name for the macro to select the direction of the above ports
#define SS0_DIR TRISBbits.RB2
#define SS1_DIR TRISBbits.RB3
#define SS2_DIR TRISBbits.RB4
#define SS3_DIR TRISBbits.RB5
#define SCK1_DIR TRISCbits.RC3
#define SDI1_DIR TRISCbits.RC4
#define SDO1_DIR TRISCbits.RC5

void spi_master_init(void);
unsigned char spi_data(unsigned char device, unsigned char tx_data);



#endif