
/**
 * @file    mqtt_app.h
 * @brief   MQTT client wrapper and event handling for ESP8266
 * @author  Umesh Puri
 * @date    2025-06-12
 * @version 1.0
 *
 * This file implements MQTT client setup and event handling functionality
 * for the ESP8266 platform. It provides initialization, publishing, and
 * subscription management for MQTT protocol communication with a broker.
 *
 * @license MIT License
 * Copyright (c) 2025 Umesh
 */

#ifndef _MQTT_APP_H
#define _MQTT_APP_H

#include "mqtt_client.h"
#include "esp_log.h"
#include "esp_system.h"
#include "wifi_config.h"


#define BROKER  "mqtt://broker.hivemq.com:1883"



void mqtt_app_start(void);
void mqtt_publish_value(int val);
void publish_task(void *pvParameter);


extern int mqtt_buff_int;
#endif

/*BROKER
ws://broker.hivemq.com:8000/mqtt
*/