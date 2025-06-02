/**
 * @file    trig_trim_adc.h
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

#ifndef _TRIG_TIM_ADC_H_
#define _TRIG_TIM_ADC_H_
#include "stm32f1xx.h"
extern volatile uint16_t adc_data_dma[10];
extern volatile uint8_t finished;
void triggered_adc_init(void);

#endif