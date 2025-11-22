#include <Arduino.h>
#include "esp_dsp.h"

#define FFT_SIZE 512
#define SAMPLE_RATE 55.3f  // your sample rate

float buffer[FFT_SIZE * 2];      // Complex buffer for FFT input/output
float window[FFT_SIZE];          // Window coefficients
float mag_dB[FFT_SIZE / 2];      // Magnitude spectrum in dB

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("FFT Example with ESP-DSP");

  // Initialize FFT
  esp_err_t ret = dsps_fft2r_init_fc32(NULL, FFT_SIZE);
  if (ret != ESP_OK) {
    Serial.printf("FFT init error: %d\n", ret);
    while (1);
  }

  // Generate Hann window coefficients once
  dsps_wind_hann_f32(window, FFT_SIZE);
}

void loop() {
  // Generate test signal: change this frequency to test
  int HR = 91;
  float test_freq = HR/60.0f; // Hz, change to see effect
  for (int i = 0; i < FFT_SIZE; i++) {
    float t = float(i) / SAMPLE_RATE;
    // Real input signal
    float val = sinf(2 * PI * test_freq * t);

    // Apply window here for better accuracy (or after copy)
    buffer[2 * i] = val * window[i];  // real part
    buffer[2 * i + 1] = 0.0f;         // imaginary part zero
  }

  // Perform FFT
  dsps_fft2r_fc32(buffer, FFT_SIZE);
  dsps_bit_rev_fc32(buffer, FFT_SIZE);

  // Compute magnitude in dB scale
  for (int i = 0; i < FFT_SIZE / 2; i++) {
    float re = buffer[2 * i];
    float im = buffer[2 * i + 1];
    float mag = sqrtf(re * re + im * im);
    mag_dB[i] = 20 * log10f(mag + 1e-6f);  // add epsilon to avoid log(0)
  }

  // Find peak magnitude bin in bandlimited range (e.g. ignore DC bin)
  int bin_start = 1;             // skip bin 0 (DC)
  int bin_end = FFT_SIZE / 2 - 1;
  int maxIndex = bin_start;
  float maxVal = mag_dB[bin_start];
  for (int i = bin_start + 1; i <= bin_end; i++) {
    if (mag_dB[i] > maxVal) {
      maxVal = mag_dB[i];
      maxIndex = i;
    }
  }

  // Parabolic interpolation for better frequency estimate
  float alpha = mag_dB[maxIndex - 1];
  float beta = mag_dB[maxIndex];
  float gamma = mag_dB[maxIndex + 1];
  float p = 0.5f * (alpha - gamma) / (alpha - 2 * beta + gamma);

  // Calculate frequency from peak bin and interpolation
  float peak_bin = maxIndex + p;
  float freq = peak_bin * SAMPLE_RATE / FFT_SIZE;
  float bpm = freq * 60.0f; // beats per minute

  Serial.printf("Peak bin: %d, Interp: %.3f, Freq: %.3f Hz, BPM: %.1f\n",
                maxIndex, p, freq, bpm);

  // Optional: show spectrum using dsps_view (text graph)
  dsps_view(mag_dB, FFT_SIZE / 2, 40, 10, -100, 0, '|');

  delay(2000);
}
