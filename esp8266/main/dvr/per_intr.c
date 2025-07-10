/**
 * @file    ssid_reset.c
 * @brief   Wi-Fi SSID reset handler for ESP8266 devices via user interaction or timeout
 * @author  Umesh Puri
 * @date    2025-06-12
 * @version 1.0
 *
 * This file handles SSID reset mechanisms for ESP8266, including
 * button-based resets or automatic fallback to default configuration
 * after a timeout. Useful for resetting the device to configuration mode.
 *
 * @license MIT License
 * Copyright (c) 2025 Umesh
 */

#include "per_intr.h"
#include "wifi_config.h"

#include <stdio.h>
#include "esp_log.h"
#include "esp_system.h"

static const char *TAG = "ssid_reset";
static bool first_intr = true;

// TIMER CALLBACK FUNCTION

void reset_timer_callback(TimerHandle_t xTimer)
{

    if (gpio_get_level(RESET_PIN) == 0)
    {
        ESP_LOGI(TAG, "Reset pin held low for 20s. Restarting in SoftAP mode...");

        // clear saved wifi credential
        clear_saved_wifi_credentials();

        esp_restart();
    }
    else
    {
        ESP_LOGI(TAG, "Reset pin released before 20s, aborting reset.");
    }
}

// GPIO reset pin ISR ROUTINE
static void IRAM_ATTR reset_isr_handler(void *arg)
{

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
   if (first_intr) {
        first_intr = false;
        return;  // Ignore first interrupt
    }
        int rst_level = gpio_get_level(RESET_PIN);
        ;
       
        if (rst_level == 0)
        {
            // Pin went HIGH — start 5s timer
            xTimerStartFromISR(reset_timer, &xHigherPriorityTaskWoken);
        }
        else
        {
            // Pin went LOW — stop timer if running
            xTimerStopFromISR(reset_timer, &xHigherPriorityTaskWoken);
        }

        

        if (xHigherPriorityTaskWoken)
        {
            portYIELD_FROM_ISR();
        }
       
    }

    // GPIO reset pin ISR ROUTINE
static void IRAM_ATTR disp_isr_handler(void *arg)
{

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
   if (first_intr) {
        first_intr = false;
        return;  // Ignore first interrupt
    }
       
        int disp_level = gpio_get_level(DISP_INTR_PIN);
        
        
        if (disp_level == 1)
        {
            
            // Notify the OLED task
          // vTaskNotifyGiveFromISR(oled_task_handle, &xHigherPriorityTaskWoken);
        }

        if (xHigherPriorityTaskWoken)
        {
           // portYIELD_FROM_ISR();
        }
       
    }
    


void intr_pin_init()
{
    gpio_config_t io_conf_reset = {
        .intr_type = GPIO_INTR_NEGEDGE, // Detect falling edges
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << RESET_PIN),
        .pull_up_en = GPIO_PULLUP_ENABLE, // enable pull up
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };
/*
    gpio_config_t io_conf_disp = {
        .intr_type = GPIO_INTR_POSEDGE, // Detect  rising edges
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << DISP_INTR_PIN),
        .pull_up_en = GPIO_PULLUP_DISABLE,    // disable pull up
        .pull_down_en = GPIO_PULLDOWN_ENABLE, // enable pull down
    };
*/
    gpio_config(&io_conf_reset);
   // gpio_config(&io_conf_disp);

    // Create one-shot software timer for 5 seconds
    reset_timer = xTimerCreate("reset_timer", pdMS_TO_TICKS(5000), pdFALSE, NULL, reset_timer_callback);
    if (reset_timer == NULL)
    {
        ESP_LOGE(TAG, "Failed to create reset timer");
        return;
    }

    // Install GPIO ISR service
    gpio_install_isr_service(0);

    // Add ISR handler for the reset pin
    gpio_isr_handler_add(RESET_PIN, reset_isr_handler, NULL);
    //gpio_isr_handler_add(DISP_INTR_PIN, disp_isr_handler, NULL);

    ESP_LOGI(TAG, " pin interrupt initialized on GPIO %d & %d", RESET_PIN, DISP_INTR_PIN);
}
