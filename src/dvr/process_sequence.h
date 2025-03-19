#ifndef _PROCESS_SEQUENCE_H
#define _PROCESS_SEQUENCE_H
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "adc.h"
#include <stdbool.h>
extern volatile uint8_t display_update_flag;
void timer1_init();
void sample_adc(uint16_t **samples, uint16_t *prev);
uint16_t  get_bpm(uint16_t *samples, uint16_t data_len);
inline bool enable_sample(void) {

    // enable all the interrupts
    //sei();
    TIMSK1 |= (1 << OCIE1A);  // Enable interrupt on compare match A for Timer1
    return  true;
}
#endif
