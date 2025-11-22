/**
 * @file    adc.h
 * @brief   ADC driver implementation for STM32F103
 * @author  Umesh Puri
 * @date    2025-06-02
 * @version 1.0
 *
 * This file implements analog-to-digital conversion using the STM32 ADC peripheral.
 * It includes functions to initialize the ADC, read values, and manage channels.
 *
 * @license MIT License
 * Copyright (c) 2025 Umesh
 */


#ifndef __ADC_H
#define __ADC_H

#include "stm32f1xx.h"
#include "stdbool.h"

#define ADC_BUFFER (512*2)


extern volatile int16_t adc_buffer[ADC_BUFFER];
extern volatile int16_t adc_val;
extern bool ACC_COMP;
void adc_init_cont(void);
void stop_process(void);
void start_process(void);

#endif