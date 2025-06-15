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

// #define ARM_MATH_CM3
#include "stm32f1xx.h"
#include "core_cm3.h"
#include "arm_math.h"
#include <stdint.h>
#include <stdio.h>

#include "HSE_CLK.h"
#include "delay.h"
#include "spi.h"
#include "adc.h"
#include "ledconf.h"
#define FFT_SIZE 256
#define PPG_SAMP (adc_buffer)
#define SPO2_IR (adc_buffer + 1)
#define SPO2_RED (adc_buffer + 2)
#include "adc_20ms_log.h"
uint16_t *pPPG_Sample = PPG_SAMP; // pointer for Green
uint16_t *pSPO2_IR = SPO2_IR;     // pointer for IR
uint16_t *pSPO2_RED = SPO2_IR;    // pointer for RED
q15_t input[FFT_SIZE * 2];
q15_t output[FFT_SIZE];
q15_t maxVal = 0;
uint32_t maxIndex = 0;
uint32_t mean_value = 0;

arm_cfft_radix4_instance_q15 fft_inst;

void DSP_process(void); // DSP_process function prototype

int main(void)
{
   // Peripheral and process initialization
   X_Clock_Init();  // 72MHz CLK
   spi_init();      // SPI initialization
   TIM2_Init();     // TIM2 Delay initialization
   adc_init_cont(); // ADC  initialization
   led_init();      // LEDs initialization

   

   while (1)
   {
      if (ACC_COMP)
      {

         ACC_COMP = false;
         _us_delay(10000);
         DSP_process(); // call DSP_process
         // after completion of DSP start the process again
         start_process();
        
      }

      _us_delay(500);

      spi_transmit(maxIndex);
      _us_delay(500);
   }
   return 0;
}

void DSP_process(void)
{

   for (uint16_t i = 0; i < FFT_SIZE; i++)
   {
      // input[i * 2] = (signed)(*(pPPG_Sample + (i * 3)) - (q15_t)512);
      input[i * 2] = (signed)(sample[i] - (q15_t)512);
      input[(i * 2) + 1] = 0;
   }
   arm_shift_q15(input, 8, input, FFT_SIZE * 2);

   // Initialize FFT
   arm_cfft_radix4_init_q15(&fft_inst, FFT_SIZE, 0, 1); // forward FFT, bit reverse = 1

   arm_cfft_radix4_q15(&fft_inst, input);

   arm_cmplx_mag_q15(input, output, FFT_SIZE);

   output[0] = 0; // ignoring the DC or the zeroth bin

   arm_max_q15(output, FFT_SIZE / 2, &maxVal, &maxIndex);
}
