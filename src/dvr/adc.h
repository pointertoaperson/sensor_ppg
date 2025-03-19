#ifndef _ADC_H
#define _ADC_H
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "adc.h"


void adc_init(void);
uint16_t read_adc(uint8_t channel);

#endif