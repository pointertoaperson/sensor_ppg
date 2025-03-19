#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


#include "adc.h"
#include "i2c.h"

#define SSD1306_ADDRESS 0x3C // I2C Address for SSD1306 (check your display's datasheet)

#define BUFFER_SIZE 512       // 128x32 = 512 pixels
#define PI 3.14159265
extern  char* text;

uint8_t buffer[BUFFER_SIZE];  // Buffer to store the screen data

void ssd1306_init(void);
void ssd1306_sendCommand(uint8_t cmd);
void ssd1306_updateDisplay_wave(void);
void ssd1306_updateDisplay_text(void);
void ssd1306_clearDisplay(void);
void ssd1306_clearDisplay_banner(void);
void ssd1306_set_address_range(uint8_t start_page, uint8_t end_page);

void draw_waveform(uint16_t *frame, int data_len);
void draw_pixel(uint8_t x, uint8_t y);
void draw_text( char* text, uint8_t x, uint8_t y);
void draw_pixel_text(uint8_t x, uint8_t y);




#endif