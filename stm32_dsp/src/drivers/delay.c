
/**
 * @file    delay.c
 * @brief   UART driver interface for STM32F103 microcontroller series and variants like STM32F103C8T6/CBT6 etc.
 * @author  Umesh Puri
 * @date    2025-06-02
 * @version 1.0
 *
 * This file provides function prototypes for initializing and using the USART
 * peripheral for serial communication on STM32F103 microcontrollers.
 *
 * @license MIT License
 * Copyright (c) 2025 Umesh
 */

#include "stm32f1xx.h"
#include "delay.h"
void TIM2_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->CR1 &= ~TIM_CR1_CEN; // Stop timer
    TIM2->PSC = (36000) - 1;   // 72 MHz / 72000 = 1 kHz → 1 ms per tick
    TIM2->CR1 |= TIM_CR1_OPM;  // One Pulse Mode
}

void _us_delay(uint32_t ms)
{
    TIM2->ARR = ms - 1;
    TIM2->CNT = 0;
    TIM2->SR &= ~TIM_SR_UIF;
    TIM2->CR1 |= TIM_CR1_CEN;

    while (!(TIM2->SR & TIM_SR_UIF))
        ;
    TIM2->SR &= ~TIM_SR_UIF;
}
