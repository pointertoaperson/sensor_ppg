/**
 * @file    network.h
 * @brief   Main network initialization and mode selection logic for ESP8266
 * @author  Umesh Puri
 * @date    2025-06-02
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

#ifndef _NETWORK_H
#define _NETWORK_H
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "wifi_config.h"
#include "ssid_reset.h"
#include "mqtt_app.h"

void network_app(void);
#endif