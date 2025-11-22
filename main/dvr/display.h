/**
 * @file    display.h
 * @brief   SSD1306 OLED display driver implementation for ESP8266 
 * @author  Umesh Puri
 * @date    2025-06-02
 * @version 1.0
 * 
 * This file implements functions to control the SSD1306 OLED display
 * over I2C/SPI interface, including initialization, drawing, and updating the screen.
 * 
 * @license MIT License
 * Copyright (c) 2025 Umesh
 */


#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <string.h>
//#include "i2c_helper.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_log.h"

#define I2C_MASTER_SCL_IO           16
#define I2C_MASTER_SDA_IO           17
#define I2C_MASTER_NUM              I2C_NUM_0
#define I2C_MASTER_FREQ_HZ          100000
#define I2C_MASTER_TX_BUF_DISABLE   0
#define I2C_MASTER_RX_BUF_DISABLE   0

#define SSD1306_ADDR                0x3C

#define SSD1306_ADDRESS 0x3C 

// SSD1306 128x64 display size
#define SSD1306_WIDTH               128
#define SSD1306_HEIGHT             32
#define SSD1306_PAGES               (SSD1306_HEIGHT / 8)

#define BUFFER_SIZE 512       // 128x32 = 512 pixels
#define PI 3.14159265
extern  char* text;


/*
void ssd1306_init(void);
static esp_err_t ssd1306_write_cmd(uint8_t cmd);
void ssd1306_updateDisplay(uint8_t page);
void ssd1306_updateDisplay_text(void);
void ssd1306_clearDisplay(void);
void ssd1306_clearDisplay_banner(void);
void ssd1306_set_address_range(uint8_t start_page, uint8_t end_page);
*/


 void i2c_master_init(void);
 void ssd1306_init(void);
 void ssd1306_clear(void); 
 void ssd1306_update(void);
 void ssd1306_draw_pixel(int x, int y, bool color);
 void ssd1306_draw_char(int x, int y, char c);
 void ssd1306_draw_text(int x, int y, const char* str);

void animate(uint16_t adc_val);
void draw_waveform(uint16_t *frame, uint16_t data_len);
void draw_pixel(uint8_t x, uint8_t y);
void draw_text( char* text, uint8_t x, uint8_t y);

#define OLED_I2C_ADDRESS   0x3C

// Control bytes
#define OLED_CONTROL_BYTE_CMD_SINGLE    0x80
#define OLED_CONTROL_BYTE_CMD_STREAM    0x00
#define OLED_CONTROL_BYTE_DATA_STREAM   0x40


#define OLED_CMD_SET_CONTRAST           0x81    
#define OLED_CMD_DISPLAY_RAM            0xA4
#define OLED_CMD_DISPLAY_ALLON          0xA5
#define OLED_CMD_DISPLAY_NORMAL         0xA6
#define OLED_CMD_DISPLAY_INVERTED       0xA7
#define OLED_CMD_DISPLAY_OFF            0xAE
#define OLED_CMD_DISPLAY_ON             0xAF


#define OLED_CMD_SET_MEMORY_ADDR_MODE   0x20 
#define OLED_CMD_SET_COLUMN_RANGE       0x21  
#define OLED_CMD_SET_PAGE_RANGE         0x22  

#define OLED_CMD_SET_DISPLAY_START_LINE 0x40
#define OLED_CMD_SET_SEGMENT_REMAP      0xA1  
#define OLED_CMD_SET_MUX_RATIO          0xA8  
#define OLED_CMD_SET_COM_SCAN_MODE      0xC8  
#define OLED_CMD_SET_DISPLAY_OFFSET     0xD3  
#define OLED_CMD_SET_COM_PIN_MAP        0xDA  
#define OLED_CMD_NOP                    0xE3  


#define OLED_CMD_SET_DISPLAY_CLK_DIV    0xD5  
#define OLED_CMD_SET_PRECHARGE          0xD9  
#define OLED_CMD_SET_VCOMH_DESELCT      0xDB  


#define OLED_CMD_SET_CHARGE_PUMP        0x8D 




#endif