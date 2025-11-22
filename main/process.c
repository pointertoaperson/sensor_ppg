

/**
 * @file    process.c
 * @brief   PPG signal processing logic 
 * @author  Umesh Puri
 * @date    2025-06-12
 * @version 1.0
 *

 *
 * @license MIT License
 * Copyright (c) 2025 Umesh
 */

#include "process.h"
#include "inttypes.h"
#include "stdint.h"
#include "hamming.h"



int16_t *input;  // interleaved real/imag
int16_t mag[FFT_SIZE] = {0};

int16_t maxVal = 0;
uint32_t maxIndex = 0;
int16_t noisefloor = 0;
volatile int16_t confidence = 0;
volatile float hr_values = 0.0f;
int16_t sampleMean = 0;

 static inline int16_t saturate_q15(int32_t x) {
    if (x > 32767) return 32767;
    if (x < -32768) return -32768;
    return (int16_t)x;
}

static inline int16_t q15_sqrt(int32_t x)
{
    // simple binary search integer sqrt
    int32_t res = 0;
    int32_t bit = 1L << 30; // The second-to-top bit
    while (bit > x) bit >>= 2;

    while (bit != 0) {
        if (x >= res + bit) {
            x -= res + bit;
            res = (res >> 1) + bit;
        } else {
            res >>= 1;
        }
        bit >>= 2;
    }
    return (int16_t)res;  // result in Q15
}



void dsp_process(void)
{
     // FFT init & run
    static bool fft_initialized = false;

    if (!fft_initialized) {
      input = (int16_t *)heap_caps_malloc(FFT_SIZE * 2 * sizeof(int16_t), MALLOC_CAP_8BIT);
        dsps_fft2r_init_sc16(NULL, FFT_SIZE);
        fft_initialized = true;
    } 
     
    // Prepare interleaved input: real = PPG, imag = 0
     int16_t sum = 0;
    for (uint16_t i = 0; i < FFT_SIZE /2; i++) {
       int32_t temp = adc_buffer[i * 3] * hamming1024[i]; // Real
        input[i << 1] = saturate_q15(temp >> 15);  
        sum += input[i<<1];
        input[(i << 1) + 1] = 0;           // Imag
    }
     sampleMean = sum / (FFT_SIZE / 2);

   
// Zero padding to 512
    memset(&input[FFT_SIZE], 0, FFT_SIZE * sizeof(int16_t));
 
   

    // Remove DC and apply Hamming window
/*

   for (uint16_t i = 0; i < FFT_SIZE; i++)
   {
      input[i << 1] = (i < FFT_SIZE / 2) ? input[i << 1] - (sampleMean << 1) : 0; // compensate the means of interleved array multiplying by 2
     // input[i << 1] = input[i << 1] - (sampleMean << 1) ; 
     // int32_t temp = (int32_t)input[i << 1] * hamming2048[i];                         // Q15 * Q15 = Q30
     // input[i << 1] = saturate_q15(temp >> 15);                            // Back to Q15 and saturate to 16-bit
      input[(i << 1) + 1] = 0;
   }
*/
  

    dsps_fft2r_sc16(input, FFT_SIZE);
   dsps_bit_rev_sc16_ansi(input, FFT_SIZE);

    // Compute magnitude
    for (uint16_t i = 0; i < FFT_SIZE; i++) {
        int32_t real = input[i << 1];
        int32_t imag = input[(i << 1) + 1];
   mag[i] = q15_sqrt(real * real + imag * imag);
    }

    // Noise floor
    int16_t noise_sum = 0;
    for (uint16_t i = 0; i < FFT_SIZE / 2; i++) {
        noise_sum += mag[i];
    }
    noisefloor = noise_sum / (FFT_SIZE / 2);

    // Remove noise
    for (uint16_t i = 0; i < FFT_SIZE; i++) {
        mag[i] = (i < FFT_SIZE / 2) ? mag[i] - noisefloor : 0;
    }

    // Bandpass filtering
    memset(&mag[0], 0, 4 * sizeof(int16_t));
    memset(&mag[50], 0, (FFT_SIZE - 50) * sizeof(int16_t));

    // Find max
    maxVal = 0;
    maxIndex = 0;
    for (uint16_t i = 0; i < FFT_SIZE / 2; i++) {
        if (mag[i] > maxVal) {
            maxVal = mag[i];
            maxIndex = i;
        }
    }

    confidence = (noisefloor > 0) ? maxVal / noisefloor : 0;
    
         printf("maxIDX =%"PRIu32 "maxval =%d\n", maxIndex, maxVal);
    spo2_val = (uint32_t)confidence;

    if (confidence < 10 || maxVal < 20) {
        ppg_val= 0;
        return;
    }

    // Quadratic / log-parabolic interpolation
    if (maxIndex != 0 && maxIndex != FFT_SIZE - 1) {
        float mag_km1 = (mag[maxIndex - 1] <= 0) ? 0.001f : mag[maxIndex - 1];
        float mag_k = (mag[maxIndex] <= 0) ? 0.001f : mag[maxIndex];
        float mag_kp1 = (mag[maxIndex + 1] <= 0) ? 0.001f : mag[maxIndex + 1];

        float delta = 0.5f * (logf(mag_km1) - logf(mag_kp1)) /
                      (logf(mag_km1) - 2 * logf(mag_k) + logf(mag_kp1));

        float refined_freq = ((float)maxIndex + delta) * 0.108f;
        hr_values = 60.0f * refined_freq;
        printf("HR float =  %f", hr_values);
        ppg_val = (uint32_t)(hr_values+0.5f);
   
    } else {
        ppg_val = 0;
        return;
    }
}
