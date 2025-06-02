/**
 * @file    delay.h
 * @brief   Delay utilities for STM32F103 microcontroller
 * @author  Umesh Puri
 * @date    2025-06-02
 * @version 1.0
 * 
 * This file provides functions for implementing accurate delays using 
 * timers or CPU cycle counting on the STM32F103 series.
 * 
 * @license MIT License
 * Copyright (c) 2025 Umesh
 */


#ifndef _DELAY_H
#define _DELAY_H
#include "system_stm32f1xx.h"
#include "stm32f1xx.h"
void TIM2_Init(void);
void _us_delay(uint32_t us);

#endif