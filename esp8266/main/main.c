/**
 * @file    main.c
 * @brief   Demo main application for ESP8266 with SPI, ADC, OLED, and network tasks
 * @author  Umesh Puri
 * @date    2025-06-02
 * @version 1.0
 *
 * This file contains the main application entry point for testing various
 * modules on the ESP8266 platform. It sets up SPI slave communication,
 * ADC sampling, I2C display rendering, and Wi-Fi initialization. Tasks
 * are created to manage OLED display updates, SPI communication, and
 * network boot logic.
 *
 * @license MIT License
 * Copyright (c) 2025 Umesh
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "i2c_helper.h"
#include "display.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "comm_spi.h"
#include "network.h"

volatile uint16_t adc_val;
volatile char *bstr[20];

void oled_task(void);
void network_task(void *pvParameter); // network task

void app_main()
{
  // spi initialization
  comm_spi_init();

  // adc initialization
  adc_config_t adc_config;
  adc_config.mode = ADC_READ_TOUT_MODE;
  adc_config.clk_div = 8; // 10MHz sampling time
  ESP_ERROR_CHECK(adc_init(&adc_config));

  // i2c initialization
  i2c_init();

  // ssd1306 initialization
  ssd1306_init();
  ssd1306_clearDisplay();
  ssd1306_updateDisplay(0);

  // Dummy mqtt publish value
  mqtt_buff_int = 777;
  // DISPLAY TASK
  xTaskCreate(oled_task, "oled_task", 1024, NULL, 4, NULL);

  // SPI TASK
  xTaskCreate(spi_slave_read_master_task, "spi_task", 2048, NULL, 4, NULL);

  // NETWORK TASK
  xTaskCreate(&network_task, "Network Task", 4096, NULL, 4, NULL);
}

// oled task for diaplaying
void oled_task()
{
  while (1)
  {
    // read and display adc value
    adc_read(&adc_val); // read adc and fill in adc_val

    sprintf(bstr, "adc:%d  spi:%" PRIu32, adc_val, spi_val);
    draw_text(bstr, 0, 0);

    uint16_t arr = 24;
    animate((adc_val * 17) / 1024);
    // ssd1306_updateDisplay();

    vTaskDelay(20 / portTICK_PERIOD_MS); // to be updated at every 20ms
  };
}

void network_task(void *pvParameter)
{
  network_app();
  vTaskDelete(NULL); // optional, if task has no loop
}
