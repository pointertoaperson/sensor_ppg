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
#define PPG_SAMP (&adc_buffer[0])
#define SPO2_IR (&adc_buffer[1])
#define SPO2_RED (&adc_buffer[2])

int16_t *pPPG_Sample = PPG_SAMP; // pointer for Green
int16_t *pSPO2_IR = SPO2_IR;     // pointer for IR
int16_t *pSPO2_RED = SPO2_RED;   // pointer for RED
q15_t input[FFT_SIZE * 2];
q15_t output[FFT_SIZE];
static q15_t mag[FFT_SIZE] = {0};
static int16_t peak[FFT_SIZE] = {0};
static q15_t reals[FFT_SIZE] = {0};
q15_t maxVal = 0;
uint32_t maxIndex = 0;
uint32_t mean_value = 0;
q15_t noisefloor = 0;
volatile q15_t confidence = 0;
volatile float32_t hr_values = 0.0f;
q15_t sampleMean = 0;
int16_t j = 0;

arm_cfft_radix4_instance_q15 fft_inst;
arm_cfft_radix4_instance_q15 Ifft_inst;
void DSP_process(void); // DSP_process function prototype
float32_t calculate_heart_rate(const int16_t *peak_indices);

void DSP_process(void); // DSP_process function prototype

int main(void)
{
   // Peripheral and process initialization
   X_Clock_Init();  // 72MHz CLK
   spi_init();      // SPI initialization
   TIM2_Init();     // TIM2 Delay initialization
   adc_init_cont(); // ADC  initialization
   led_init();      // LEDs initialization

   arm_cfft_radix4_init_q15(&fft_inst, FFT_SIZE, 0, 1);  // forward FFT
   arm_cfft_radix4_init_q15(&Ifft_inst, FFT_SIZE, 1, 1); // IFFT

   while (1)
   {
      if (ACC_COMP)
      {

         ACC_COMP = false;
         DSP_process(); // call DSP_process
         // after completion of DSP start the process again
         start_process();
      }

      _us_delay(500);
      spi_transmit(hr_values, confidence);
      _us_delay(500);
   }
   return 0;
}

void DSP_process(void)
{

   for (uint16_t i = 0; i < FFT_SIZE; i++)
   {
      input[i * 2] = (adc_buffer[i * 3]);
      input[(i * 2) + 1] = 0;
   }
   // get mean of input
   arm_mean_q15(&input[0], FFT_SIZE * 2, &sampleMean);

   // remove dc component
   for (uint16_t i = 0; i < FFT_SIZE; i++)
   {
      input[i * 2] = input[i * 2] - (sampleMean << 1); // compensate the means of interleved array multiplying by 2
      input[(i * 2) + 1] = 0;
   }

   arm_shift_q15(input, 8, input, FFT_SIZE * 2);
   arm_cfft_radix4_q15(&fft_inst, input);
   arm_cmplx_mag_q15(input, mag, FFT_SIZE);

   // get noise floor before removing high bins
   arm_mean_q15(&mag[0], FFT_SIZE, &noisefloor);

   // filtering: zero out high frequencies
   memset(&mag[21], 0, (FFT_SIZE - 21) * sizeof(q15_t)); // zero bins 21 to end

   arm_max_q15(mag, FFT_SIZE / 2, &maxVal, &maxIndex);

   confidence = (noisefloor > 0) ? maxVal / noisefloor : 0;

   if (confidence < 10) // threshold confidence
   {
      hr_values = 0;
      return;
   }
   // Zero all bins except max bin and symmetric bin
   if (maxIndex != 0 && maxIndex != FFT_SIZE - 1)
   {
      for (uint16_t i = 0; i < FFT_SIZE; i++)
      {
         if ((i != maxIndex) && (i != maxIndex + 1) && (i != maxIndex - 1))
         {
            input[2 * i] = 0;
            input[2 * i + 1] = 0;
         }
      }
   }
   else
   {
      hr_values = 0;
      return;
   }

   // prform Inverse FFT
   arm_shift_q15(input, 8, input, FFT_SIZE * 2);
   arm_cfft_radix4_q15(&Ifft_inst, input);

   // Extract real part
   for (int i = 0; i < FFT_SIZE; i++)
   {
      reals[i] = input[2 * i];
   }

   // Find peaks in the real part
   int16_t j = 0;
   for (int i = 1; i < FFT_SIZE - 1; i++)
   {
      if ((reals[i - 1] < reals[i]) && (reals[i] > reals[i + 1]))
      {
         peak[j++] = i;
         if (j >= FFT_SIZE - 1)
            break; // avoid overflow
      }
   }
   peak[j] = 256; // sentinel with bit 8 set
   j = 0;

   // Calculate heart rates from peaks
   hr_values = calculate_heart_rate(peak);

   // clear peak for next frame
   memset(peak, 0, sizeof(peak));
}

float32_t calculate_heart_rate(const int16_t *peak_indices)
{
   q15_t mean_diff = 0;
   int16_t i = 0;
   for (; !((peak_indices[i] & (1 << 8))) && !((peak_indices[i + 1] & (1 << 8))); i++)
   {
      q15_t diff_samples = peak_indices[i + 1] - peak_indices[i];
      mean_diff += diff_samples;
   }

   if (i > 0 && mean_diff > 0)
   {
      return (3333.33f) / ((float32_t)mean_diff / i);
   }
   else
   {
      return 0;
   }
}
