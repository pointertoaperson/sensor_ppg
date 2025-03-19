#ifndef _SENSOR_PPG_
#define _SENSOR_PPG_
// std includes
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

// samples to read
#define SAMPLE_LEN 125
// moving average params
#define MA_WINDOW 5
#define MIN_PEAK_DIST  40

// detection for signals
#define THRESHOLD 750 // Adjust based on signal amplitude

// TIMER setting
//#define TM1_CMP 62499
#define TM1_CMP 39062
// #define TM1_PRESCL 

extern bool do_sample;
extern uint16_t filled_data;

#endif