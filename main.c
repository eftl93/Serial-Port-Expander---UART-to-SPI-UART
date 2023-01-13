/*
 * File:   main.c
 * Author: Eder Torres
 *
 * This code is for a PIC18F46k22 that controls the signal distribution board
 * this board, receives character commands through UART1 from a Beaglebone Black
 * and (if not a repeated character or a disconnect from BBB and this board will
 * forward the received character
 * 
 * This board also controls a "2 Coil Latch relay" to turn on and off beam lights
 * the relay was chosen in order to save power and not overheat the relay coil
 * 
 * This board also has 3 on-board led lights to debug and diagnose the board 
 * in order to see if the board is received the right character and be able to 
 * trace mis-communications
 */

#include <xc.h>
#include <stdio.h>
#include <string.h>
#include "spi.h"
#include "uart.h"
#include "main.h"
#include "gpio.h"

#define _XTAL_FREQ 64000000


extern volatile uint8_t *rx_str_interrupt;

uint8_t text1[] = "Hello, Welcome!";
uint8_t instructions1[] = "Use left joystick to move left wheel";
uint8_t instructions2[] = "Use right joystick to move right wheel";
uint8_t instructions3[] = "Press 'q' and 'e' to turn light beam off and on";
uint8_t wii_classic_packet[] = "z1111!!!y";
uint8_t servo_controller_tx = 0x00;
uint8_t debouncing_counter = 0x00;
uint8_t debouncing_flag = 0x01;
uint8_t demuxed_controller_signals = 0x00;

struct uart_package
{
    uint8_t lx_joystick;
    uint8_t ly_joystick;
    uint8_t rx_joystick;
    uint8_t ry_joystick;
    uint8_t d_pad;
    uint8_t action_buttons;
    uint8_t shoulder_plus_minus;
};

struct ctrl_buttons
{
    uint8_t a;
    uint8_t b;
    uint8_t x;
    uint8_t y;
};

struct dpad_buttons
{
    uint8_t up;
    uint8_t down;
    uint8_t left;
    uint8_t right;
};

struct shoulder_plus_minus_buttons
{
    uint8_t zl;
    uint8_t zr;
    uint8_t lt;
    uint8_t rt;
    uint8_t minus;
    uint8_t plus;
};

struct uart_package classic_ctrl;
struct ctrl_buttons act_buttons;
struct dpad_buttons arrow_buttons;
struct shoulder_plus_minus_buttons shoulder_buttons;
struct shoulder_plus_minus_buttons aux_buttons;


void main()
{
    uint8_t dummy_spi_tx;
    rx_str_interrupt = &wii_classic_packet; //copy the address of wii_classic_packet[] to rx_str_interrupt 
    IPEN=0; //disable priority levels on interrupts (pic16cxxx compatibility mode)
    INTCON=0b00000000;      //disables all interrupts
    gpio_init();            //initialize GPIOs, set leds and relay controller as output. Turn off all the lights
    spi_master_init();      //initialize SPI in master mode
    uart_init();            //initialize both UART1 and UART2 module
    //timer1_init(60000,8);    //initialize timer1 to every 2000 timer1 cycles. Asynchrounous, source clk is fosc/4. Pre-scaler is 1/8
    INTCONbits.GIE = 1; //enables all unmasked interrupts
    INTCONbits.PEIE = 1; //enables all unmasked peripherals interrupts  
    
    dummy_spi_tx=spi_data(3,0x6F); //send an 'o' to the motor controller board to turn off the motors
    uart_wr_str(1, text1);
    tx1('\n'); //new line
    tx1('\r'); //return to the beginning of the same line
    uart_wr_str(1, instructions1);
    tx1('\n');
    tx1('\r');
    uart_wr_str(1, instructions2);
    tx1('\n');
    tx1('\r');
    uart_wr_str(1, instructions3);
    tx1('\n');
    tx1('\r');

    while(1)
    {
        //remove the offset given by the HID keyboard controller and save the values to a struct
        classic_ctrl.lx_joystick = wii_classic_packet[1] - 33;
        classic_ctrl.ly_joystick = wii_classic_packet[2] - 33;
        classic_ctrl.rx_joystick = wii_classic_packet[3] - 33;
        classic_ctrl.ry_joystick = wii_classic_packet[4] - 33;
        classic_ctrl.d_pad       = wii_classic_packet[5] - 33;
        classic_ctrl.action_buttons = wii_classic_packet[6] - 33;
        classic_ctrl.shoulder_plus_minus = wii_classic_packet[7] - 33;
        wii_classic_packet[8] = '\0';
        uart_wr_str(1,wii_classic_packet); //sending the whole string received on uart1_rx back to uart1_tx for debugging purposes
        tx1('\r');

        //sending set of data to motor controller board(3) through spi, starting with 'z' and ending with 'y' to set start and end of package
        //sending a copy to channel device 0 for debugging
        dummy_spi_tx = spi_data(3,'z');
        dummy_spi_tx = spi_data(0,'z');
        dummy_spi_tx = spi_data(3,classic_ctrl.lx_joystick);
        dummy_spi_tx = spi_data(0,classic_ctrl.lx_joystick);
        dummy_spi_tx = spi_data(3,classic_ctrl.ly_joystick);
        dummy_spi_tx = spi_data(0,classic_ctrl.ly_joystick);
        dummy_spi_tx = spi_data(3,classic_ctrl.rx_joystick);
        dummy_spi_tx = spi_data(0,classic_ctrl.rx_joystick);
        dummy_spi_tx = spi_data(3,classic_ctrl.ry_joystick);
        dummy_spi_tx = spi_data(0,classic_ctrl.ry_joystick);
        dummy_spi_tx = spi_data(3,'y');
        dummy_spi_tx = spi_data(0,'y');
         
        //read the status of each of the action buttons
        act_buttons.a = ((classic_ctrl.action_buttons & 0b00001000) >> 3);
        act_buttons.b = ((classic_ctrl.action_buttons & 0b00000100) >> 2);
        act_buttons.x = ((classic_ctrl.action_buttons & 0b00000010) >> 1);
        act_buttons.y = ((classic_ctrl.action_buttons & 0b00000001) >> 0);
        
        //read the status of each of the d-pad
        arrow_buttons.up    = ((classic_ctrl.d_pad & 0b00001000) >> 3);
        arrow_buttons.down  = ((classic_ctrl.d_pad & 0b00000100) >> 2);
        arrow_buttons.left  = ((classic_ctrl.d_pad & 0b00000010) >> 1);
        arrow_buttons.right = ((classic_ctrl.d_pad & 0b00000001) >> 0);
        
        //read the status of each of the shoulder buttons
        shoulder_buttons.zl = ((classic_ctrl.shoulder_plus_minus & 0b00100000) >> 5);
        shoulder_buttons.zr = ((classic_ctrl.shoulder_plus_minus & 0b00010000) >> 4);
        shoulder_buttons.lt = ((classic_ctrl.shoulder_plus_minus & 0b00001000) >> 3);
        shoulder_buttons.rt = ((classic_ctrl.shoulder_plus_minus & 0b00000100) >> 2);
        
        //read the status of the '+' and '-' buttons
        aux_buttons.minus   = ((classic_ctrl.shoulder_plus_minus & 0b00000010) >> 1);
        aux_buttons.plus    = ((classic_ctrl.shoulder_plus_minus & 0b00000001) >> 0);
        
        //preparing byte with action buttons, d_pad and send status of a counter variable when overflown (for "debouncing")
        //a timer could be used to send the servo data every so often.
        //Pretty much, this part of the code sets how many times to loop in the while(1), before marking data as "valid".
        //This is done so a single tap on the controller won't move the servos to the extreme values, instead, they can move
        //one step at a time
        debouncing_counter++;
        if(debouncing_counter == 0x00)
        {
            debouncing_flag = (0x03 << 6); //sets the two Most significant bits to "11" (0xC0) after the counter has overflown
        }
        else
        {
            debouncing_flag = 0x00;
            if(debouncing_counter == 0x5F) //set how many cycles to ignore before setting the "debouncing_flag" = to 0xC0
            {
                debouncing_counter = 0xFF;
            }
        }
        
        demuxed_controller_signals = (shoulder_buttons.zl << 7) | (shoulder_buttons.zr << 6) | (act_buttons.a << 5) | (act_buttons.b << 4) | (classic_ctrl.d_pad);
        switch(demuxed_controller_signals)
        {
            case(0x00): //no button pressed
                servo_controller_tx = 0x00 | debouncing_flag;
                break;
            case(0x01): //"right" button pressed
                servo_controller_tx = 0x01 | debouncing_flag;
                break;
            case(0x02): //"left"  button pressed
                servo_controller_tx = 0x02 | debouncing_flag;
                break;
            case(0x04): //"down" button pressed
                servo_controller_tx = 0x03 | debouncing_flag;
                break;
            case(0x08): //"up" button pressed
                servo_controller_tx = 0x04 | debouncing_flag;
                break;
            case(0x10): //"b" button Pressed
                servo_controller_tx = 0x05 | debouncing_flag;
                break;
            case(0x20): //"a" button pressed
                servo_controller_tx = 0x06 | debouncing_flag;
                break;
            case(0x40): //"zr" button pressed
                servo_controller_tx = 0x07 | debouncing_flag;
                break;
            case(0x80): //"zl" button Pressed
                servo_controller_tx = 0x08 | debouncing_flag;
                break;
            default :
                servo_controller_tx = 0x00;
                break;
        }
        tx2(servo_controller_tx);

        //Interpret the commands and turn on the LEDs in a pattern depending on 
        //the data received
        RED_LED = act_buttons.a;
        GREEN_LED = act_buttons.b;
        if((act_buttons.x) & (!act_buttons.y))
        {
            high_beams_on();
        }
        else if((!act_buttons.x) & (act_buttons.y))
        {
            high_beams_off();
        }
        else
        {
            
        }
        

      rx1_overrun_detect_reset();
                
    }
}
