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
#include "esp_timer.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "i2c_helper.h"
#include "display.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "comm_spi.h"
#include "network.h"

TimerHandle_t periodic_timer = NULL;

TaskHandle_t oled_task_handle = NULL;
TimerHandle_t reset_timer = NULL;

volatile uint16_t adc_val;
char *bstr[64];

void oled_task(void);
void network_task(void *pvParameter); // network task
static void IRAM_ATTR timer_callback(void *arg);
void start_20ms_timer(void);

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

  // DISPLAY TASK

  xTaskCreate(oled_task, "oled_task", 4096, NULL, configMAX_PRIORITIES - 2, &oled_task_handle);
  // Start the hardware timer that triggers every 20ms
  start_20ms_timer();

  // SPI TASK
  xTaskCreate(spi_slave_read_master_task, "spi_task", 4096, NULL, configMAX_PRIORITIES - 1, NULL);

  // NETWORK TASK
  xTaskCreate(&network_task, "Network Task", 4096, NULL, configMAX_PRIORITIES - 2, NULL);
}

// oled task for diaplaying
void oled_task()
{
  while (1)
  {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // clear on exit, block forever until notified
    // read and display adc value
    adc_read(&adc_val); // read adc and fill in adc_val

    // mqtt publish value
    mqtt_buff_int = ppg_val;
    sprintf(bstr, "HR:%u   conf:%u", ppg_val, spo2_val);
    draw_text(bstr, 0, 0);

    // ESP_LOGI("adc_task", "adc_val, %u", adc_val);

    animate((adc_val * 24) / 1024);
    // taskYIELD();
    // ssd1306_updateDisplay();

    vTaskDelay(pdMS_TO_TICKS(10)); // to be updated at every 6ms
  };
}

void network_task(void *pvParameter)
{
  network_app();
  vTaskDelete(NULL); // optional, if task has no loop
}

static void IRAM_ATTR timer_callback(void *arg)
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  // Notify the oled_task that 20ms passed
  vTaskNotifyGiveFromISR(oled_task_handle, &xHigherPriorityTaskWoken);

  if (xHigherPriorityTaskWoken)
  {
    portYIELD_FROM_ISR();
  }
}

void start_20ms_timer(void)
{
  periodic_timer = xTimerCreate("20ms_timer", pdMS_TO_TICKS(10), pdTRUE, NULL, timer_callback);
  if (periodic_timer != NULL)
  {
    xTimerStart(periodic_timer, 0);
  }
  else
  {
    printf("Failed to create timer\n");
  }
}
