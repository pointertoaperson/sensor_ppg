/**
 * @file    mqtt_app.c
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

#include "mqtt_app.h"

int mqtt_buff_int = 0;
static const char *TAG = "mqtt_example";
static esp_mqtt_client_handle_t client = NULL;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT connected");
        // create a mqtt publishing task
        xTaskCreate(publish_task, "publish_task", 1024, NULL, configMAX_PRIORITIES - 2, NULL);
        // start MQTT PUBLISHING
        mqtt_publish_value(mqtt_buff_int);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "MQTT disconnected");
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "MQTT error occurred");
        break;
    default:
        break;
    }
}

void mqtt_app_start(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtt://broker.hivemq.com", // Or your own broker
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

// Use this to publish values
void mqtt_publish_value(int val)
{
    char payload[64];
    snprintf(payload, sizeof(payload), "Heart Rate estimate : %d", val);
    esp_mqtt_client_publish(client, "/esp8266/adc", payload, 0, 1, 0);
}

void publish_task(void)
{
    while (1)

    {
        mqtt_publish_value(mqtt_buff_int);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}