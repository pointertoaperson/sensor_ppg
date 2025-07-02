/**
 * @file    network.c
 * @brief   Main network initialization and mode selection logic for ESP8266
 * @author  Umesh Puri
 * @date    2025-06-12
 * @version 1.0
 *
 * This file handles the initialization of non-volatile storage (NVS),
 * Wi-Fi configuration selection based on stored credentials, and
 * switching between STA and SoftAP modes for the ESP8266. It serves
 * as the application entry point for setting up networking.
 *
 * @license MIT License
 * Copyright (c) 2025 Umesh
 */

#include "network.h"

static const char *TAG = "networ_main";

void network_app(void)
{
    ESP_LOGI(TAG, "Starting...");
    //initialize interrupt pins
    intr_pin_init();
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Try STA connection if saved credentials exist
    if (!wifi_sta_if_credentials_saved())
    {
        // Start SoftAP + HTTPS config server
        wifi_start_softap_mode();
    }
    else
    {
        wifi_start_sta_mode(); // Attempt connection
    }
}
