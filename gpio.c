/*
 * File:   gpio.c
 * Author: eder0
 *
 * Created on July 23, 2022, 9:02 PM
 */
#include <xc.h>
#include <stdio.h>
#include "gpio.h"
#include "main.h"

void gpio_init(void)
{
    ANSELA=0;           //disable analog
    ANSELB=0;           //disable analog
    ANSELC=0;           //disable analog
    ANSELD=0;           //disable analog
    ANSELE=0;           //disable analog
   CM1CON0bits.C1ON=0; //disable comparator1 module
   CM2CON0bits.C2ON=0; //disable comparator2 module
   RED_LED_DIR = 0;     //set output for debugging led
   GREEN_LED_DIR = 0;   //set output for debugging led
   YELLOW_LED_DIR = 0;  //set output for debugging led
   TRISD    &= 0xFC;  //output to control the relay for the beam lights
   debug_leds_off();
   high_beams_off();
}

//set the relay to turn on the lights
void high_beams_on(void)
{
    LATD=0b00000010; //set relay to turn on the lights
    __delay_ms(10);
    __delay_ms(5);
    LATD=0; 
}

//reset the relay to turn on off the lights
void high_beams_off(void)
{
    LATD = 0b00000001; 
    __delay_ms(10);
    __delay_ms(5);
    LATD=0;
}

//turns all the debug leds on
void debug_leds_on(void)
{
    LATA |= 0x07;    
}

//Turn off the onboard diagnostic LEDs  
void debug_leds_off(void)
{
   LATA &= 0xF8;   
}
