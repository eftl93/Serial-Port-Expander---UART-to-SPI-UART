/*
 * File:   uart.c
 * Author: Eder Torres
 *
 * 
 */
#include <xc.h>
#include "uart.h"
#include "main.h"
#include <stdio.h>

volatile uint8_t *rx_str_interrupt;
volatile uint8_t rx_char = 0;
volatile uint8_t recording_on = 0;
extern uint8_t wii_classic_packet[];

void uart_init() {
    TXSTA1bits.BRGH = 1; //for ASYNC: Highs baud rate selected
    BAUDCON1bits.BRG16 = 1; //16 bit baud rate generator is used (because of the HS clock)
    TXSTA2bits.BRGH = 1; //for ASYNC: Highs baud rate selected
    BAUDCON2bits.BRG16 = 1; //16 bit baud rate generator is used (because of the HS clock)
    SPBRG1 = 0x8A; //Set the baud rate to 115200 (spbrgh1:spbrg1 = 138)
    SPBRGH1 = 0x00; //Set the baud rate to 115200 (spbrgh1:spbrg1 = 138)
    //SPBRG2=0x82;            //Set the baud rate to 9600 (spbrgh1:spbrg1 = 1666)
    //SPBRGH2=0x06;           //Set the baud rate to 9600 (spbrgh1:spbrg1 = 1666)
    SPBRG2 = 0x8A; //Set the baud rate to 115200 (spbrgh1:spbrg1 = 138)
    SPBRGH2 = 0x00; //Set the baud rate to 115200 (spbrgh1:spbrg1 = 138)
    RX1_DIR = 1; //RX must be set as an input for uart
    TX1_DIR = 1; //TX must be set as an input for uart
    RX2_DIR = 1; //RX must be set as an input for uart
    TX2_DIR = 1; //TX must be set as an input for uart
    TXSTA1bits.SYNC = 0; //Async mode
    TXSTA2bits.SYNC = 0; //Async mode
    RCSTA1bits.SPEN = 1; //Serial port enabled
    RCSTA2bits.SPEN = 1; //Serial port enabled
    TXSTA1bits.TXEN = 1; //Transmit enabled
    TXSTA2bits.TXEN = 1; //Transmit enabled

#ifndef UART1_INTERRUPT     //if interrupt macro is not defined, disable uart1_rx interrupt
    PIE1bits.RC1IE = 0;
#endif
#ifdef UART1_INTERRUPT      //if interrupt macro is defined, enable uart1_rx interrupt
    PIE1bits.RC1IE = 1;
#endif

    RCSTA1bits.CREN = 1; //Receiver enabled
    RCSTA2bits.CREN = 1; //Receiver enabled
}

//Functions that sends one byte through uart1

void tx1(char data1) {
    while (!PIR1bits.TX1IF); //keep checking until the txbuffer is empty
    TXREG1 = data1;
    __delay_us(1);
}

//function used to transmit commands to the servo controller

void tx2(char data2) {
    while (!PIR3bits.TX2IF); //wait until the tx is not full
    TXREG2 = data2;
}

//First arguments choose either rx1 or rx2 (1 or 2)
//Second argument takes a pointer to a char
//This function will keep sending chars until the character to be
//sent is the nul character (\n)

void uart_wr_str(uint8_t port, uint8_t *str) {
    switch (port) {
        case(1):
            while (*str != '\0') {
                tx1(*str++);
            }
            break;
        case(2):
            while (*str != '\0') {
                tx2(*str++);
            }
            break;
        default:
            while (*str != '\0') {
                tx1(*str++);
            }
            break;
    }
    tx1('\0');
}

//resets the uart1 module in case that another byte is
//received while the buffer still holds the previous byte

void rx1_overrun_detect_reset(void) {
    if (RCSTA1bits.OERR) {
        RCSTA1bits.CREN = 0;
        __delay_us(4);
        RCSTA1bits.CREN = 1;
    }
}

#ifndef UART1_INTERRUPT
//This is the polling method, it will run if UART1_INTERRUPT is not defined

uint8_t rx1() {
    uint8_t x;
    if (PIR1bits.RC1IF) //keep checking untill rcbuffer is full
    {
        x = RCREG1;
        PIR1bits.RC1IF = 0;
        __delay_us(10);
    } else {
        x = 'x';
        __delay_us(10);
    }
    return x;
}

void uart_rd_str(uint8_t port, uint8_t *str) {
    switch (port) {
        case (1):
            while (*str != '\0') {
                *str++ = rx1();
            }
            break;

        case(2): //UART2 RX is pending to be implemented
            while (*str != '\0') {
                *str++ = rx1();
            }
            break;

        default: //by default read RX1
            while (*str != '\0') {
                *str++ = rx1();
            }
            break;
    }
}

uint8_t uart_rd_custom_block(uint8_t *str, uint8_t start_char, uint8_t end_char) {
    uint8_t length = 0;
    rx_char = rx1();
    __delay_us(5);
    if (rx_char == start_char) {
        while (*str != end_char) {
            rx_char = rx1();
            __delay_us(5);
            rx1_overrun_detect_reset();
            if (rx_char != 'x') {
                *str++ = rx_char;
                length++;
            }
        }
    }
    return length;
}
#endif


//This ISR takes care of RX1, this signal comes from the SBC (Beaglebone Black in this case)
#ifdef UART1_INTERRUPT

void __interrupt() UART_ISR(void) {
    if (PIR1bits.RC1IF) {
        rx_char = RCREG1; //save content into global variable
        PIR1bits.RC1IF = 0;
        if (rx_char == 'z') {
            recording_on = 1;
        } else if (rx_char == 'y') {
            recording_on = 0;
        } else {
            recording_on = recording_on;
        }

        switch (recording_on) {
            case (0):
                rx_str_interrupt = &wii_classic_packet;
                break;
            case (1):
                *rx_str_interrupt++ = rx_char;
                break;
            default:
                *rx_str_interrupt = *rx_str_interrupt;
                break;
        }

    }
    PIR1bits.RC1IF = 0;
}
#endif