/**
 * @file    ssid_reset.h
 * @brief   Wi-Fi SSID reset handler for ESP8266 devices via user interaction or timeout
 * @author  Umesh Puri
 * @date    2025-06-02
 * @version 1.0
 *
 * This file handles SSID reset mechanisms for ESP8266, including
 * button-based resets or automatic fallback to default configuration
 * after a timeout. Useful for resetting the device to configuration mode.
 *
 * @license MIT License
 * Copyright (c) 2025 Umesh
 */

#ifndef _SSID_RESET_H
#define _SSID_RESET_H
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "freertos/timers.h"

#include "ssid_reset.h"

#include <stdbool.h>

#define RESET_PIN GPIO_NUM_12 // GPIO12 as reset PIN nodemcu D6

void reset_pin_init();
static TimerHandle_t reset_timer;

#endif