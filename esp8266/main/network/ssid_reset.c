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

#include "ssid_reset.h"
#include "wifi_config.h"

#include <stdio.h>
#include "esp_log.h"
#include "esp_system.h"

static const char *TAG = "ssid_reset";

// TIMER CALLBACK FUNCTION

void reset_timer_callback(TimerHandle_t xTimer)
{

    if (gpio_get_level(RESET_PIN) == 0)
    {
        ESP_LOGI(TAG, "Reset pin held HIGH for 20s. Restarting in SoftAP mode...");

        // clear saved wifi credential
        clear_saved_wifi_credentials();

        esp_restart();
    }
    else
    {
        ESP_LOGI(TAG, "Reset pin released before 20s, aborting reset.");
    }
}

// GPIO ISR ROUTINE
static void IRAM_ATTR gpio_isr_handler(void *arg)
{

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    int level = gpio_get_level(RESET_PIN);
    ets_printf("ISR: Reset pin level = %d\n", level);
    if (level == 0)
    {
        // Pin went HIGH — start 20s timer
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

void reset_pin_init()
{
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_ANYEDGE, // Detect both rising and falling edges
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << RESET_PIN),
        .pull_up_en = GPIO_PULLUP_ENABLE, // Use pull-up, adjust if needed
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };

    gpio_config(&io_conf);

    // Create one-shot software timer for 20 seconds
    reset_timer = xTimerCreate("reset_timer", pdMS_TO_TICKS(10000), pdFALSE, NULL, reset_timer_callback);
    if (reset_timer == NULL)
    {
        ESP_LOGE(TAG, "Failed to create reset timer");
        return;
    }

    // Install GPIO ISR service
    gpio_install_isr_service(0);

    // Add ISR handler for the reset pin
    gpio_isr_handler_add(RESET_PIN, gpio_isr_handler, NULL);

    ESP_LOGI(TAG, "Reset pin interrupt initialized on GPIO %d", RESET_PIN);
}
