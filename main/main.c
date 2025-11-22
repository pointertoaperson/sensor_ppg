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
#include <stdbool.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_system.h"
#include "esp_spi_flash.h" //deprecated
#include "i2c_helper.h"
#include "display.h"
#include "inttypes.h"
#include "esp_log.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"

#include "interr_event.h"

#include "ledconf.h"
#include "network.h"
#include "process.h"





TimerHandle_t periodic_timer = NULL;
TaskHandle_t oled_task_handle = NULL;
TimerHandle_t reset_timer = NULL;


volatile uint32_t ppg_val = 0;  
volatile uint32_t spo2_val = 0;

char bstr[64];

void oled_task(void *pvParameters);
void network_task(void *pvParameter); // network task
//static void timer_callback(TimerHandle_t xTimer);
void processing_task(void *pv);
//void start_20ms_timer(void);

void app_main()
{

   
    // i2c initialization
    i2c_master_init();

    // ssd1306 initialization
    ssd1306_init();
    ssd1306_clear();
    led_init();
    _adc_dma_init();
    // DISPLAY TASK
     // FFT init
esp_err_t ret = dsps_fft2r_init_sc16(NULL, FFT_SIZE);
if (ret != ESP_OK) {
    printf("FFT init error: %d\n", ret);

}
    xTaskCreatePinnedToCore(oled_task, "oled_task", 4096, NULL, configMAX_PRIORITIES - 2, &oled_task_handle, 1);
    // Start the hardware timer that triggers every 20ms
    //start_20ms_timer();
    // Create the delay task pinned to CPU 1
    xTaskCreatePinnedToCore(processing_task, "processing_task", 4096, NULL, configMAX_PRIORITIES - 1, &delay_task_handle, 1);

  
    // NETWORK TASK
    xTaskCreatePinnedToCore(network_task, "Network Task", 4096, NULL, configMAX_PRIORITIES - 2, NULL, 0);

}

// oled task for diaplaying
void oled_task(void *pvParameters)
{
    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // clear on exit, block forever until notified
                                                 // read and display adc value
        // ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_0, &adc_val)); // read adc and fill in adc_val

        // mqtt publish value
        mqtt_buff_int = ppg_val;

        sprintf(bstr, "HR:%" PRIu32 "   bpm", ppg_val);
        ssd1306_draw_text(0, 0, bstr);

       // ESP_LOGI("adc_task", "adc_val, %u", adc_val);

        animate((ppg_adc * 24) / 4096);
       
    };
}

void network_task(void *pvParameter)
{
    network_app();
    vTaskDelete(NULL); // optional, if task has no loop
}


void processing_task(void *pv)
{
    while (1)
    {
        // Wait until notified to start

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // Wait for notification

      
        dsp_process();
        // Processing done, reset buffer_full
        ACC_COMP = false;

        // Notify ADC process (if needed)
        start_adc_task();
    }
}