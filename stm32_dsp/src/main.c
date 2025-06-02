/**
 * @file    main.c
 * @brief   Test application entry point for connectHeart peripheral and DSP library STM32 project
 * @author  Umesh Puri
 * @date    2025-06-02
 * @version 1.0
 *
 * This is a simple test main file for initial development and verification
 * of connectHeart peripherals and DSP library functionality on STM32F103.
 *
 * @license MIT License
 * Copyright (c) 2025 Umesh
 */

#define ARM_MATH_CM3
#include "stm32f1xx.h"
#include "core_cm3.h"
#include "arm_math.h"
#include <stdint.h>
#include <stdio.h>

#include "HSE_CLK.h"
#include "delay.h"
#include "uart.h"
#include "spi.h"
#include "trig_tim_adc.h"

#define FFT_SIZE 256

#include "adc_20ms_log.h"
volatile q15_t input[FFT_SIZE * 2];
volatile q15_t output[FFT_SIZE];
volatile q15_t maxVal = 0;
volatile uint32_t maxIndex = 0;
uint32_t mean_value = 0;

arm_cfft_radix4_instance_q15 fft_inst;
// arm_cfft_instance_q15 fft_inst;
int main(void)
{

   for (uint16_t i = 0; i < FFT_SIZE; i++)
   {
      input[i * 2] = (signed)(sample[i] - (q15_t)512);
      input[(i * 2) + 1] = 0;
   }
   arm_shift_q15(input, 8, input, FFT_SIZE * 2);

   X_Clock_Init();
   spi_init();
   TIM2_Init();
   uart1_init();

   // uart1_init();
   // adc_init_cont();

   // 1. Enable clock for GPIOC
   RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

   // 2. Configure PC13 as output (General-purpose output push-pull)
   GPIOC->CRH &= ~GPIO_CRH_MODE13;  // Clear previous mode for PC13
   GPIOC->CRH |= GPIO_CRH_MODE13_1; // Set as output (2 MHz)
   GPIOC->CRH &= ~GPIO_CRH_CNF13;   // General-purpose output (push-pull)

   // Initialize FFT
   arm_cfft_radix4_init_q15(&fft_inst, FFT_SIZE, 0, 1); // forward FFT, bit reverse = 1

   arm_cfft_radix4_q15(&fft_inst, input);

   arm_cmplx_mag_q15(input, output, FFT_SIZE);

   output[0] = 0; // ignoring the DC or the zeroth bin

   arm_max_q15(output, FFT_SIZE / 2, &maxVal, &maxIndex);

   uint32_t data = 1234;
   uint32_t data2 = 6666;
   spi_transmit(0);
   while (1)
   {
      // input check

      GPIOC->BSRR = GPIO_BSRR_BS13; // LED low

      _us_delay(500);

      spi_transmit(maxIndex);
      _us_delay(500);
   }
   return 0;
}
