#ifndef UART_H
#define UART_H


#include <xc.h>
#include <stdio.h>
#include <string.h>
#include "main.h"

#define _XTAL_FREQ 64000000
#define RX1 PORTCbits.RC6
#define TX1 PORTCbits.RC7
#define RX2 PORTDbits.RD6
#define TX2 PORTDbits.RD7
#define RX1_DIR TRISCbits.RC6
#define TX1_DIR TRISCbits.RC7
#define RX2_DIR TRISDbits.RD6
#define TX2_DIR TRISDbits.RD7



void uart_init(void);
void tx1(char data1);
void tx2(char data2);
void uart_wr_str(uint8_t port, uint8_t *str);
void uart_rd_str(uint8_t port, uint8_t *str);
uint8_t uart_rd_custom_block(uint8_t *str, uint8_t start_char, uint8_t end_char);
void rx1_overrun_detect_reset(void);

#ifndef UART1_INTERRUPT
uint8_t rx1(void);
#endif




#endif