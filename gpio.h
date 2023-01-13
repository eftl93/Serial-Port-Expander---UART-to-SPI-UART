
/* 
 * File:gpio.h   
 * Author: Eder Torres
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef GPIO_H
#define	GPIO_H

#define _XTAL_FREQ 64000000

#include <xc.h>  
#include <stdio.h>
#include "main.h"

void gpio_init(void);
void high_beams_on(void);
void high_beams_off(void);
void debug_leds_on(void);
void debug_leds_off(void);

#endif	

