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

char TOPIC[128] = {0};
char payload[64] ={0};

int mqtt_buff_int = 0;
static const char *TAG = "mqtt_example";
static esp_mqtt_client_handle_t client = NULL;
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT connected");
        
   

            static bool task_created = false;
  
        xTaskCreatePinnedToCore(publish_task, "publish_task", 4096, NULL, configMAX_PRIORITIES - 2, NULL, 0);
        task_created = true;

       
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
    esp_mqtt_client_config_t mqtt_cfg = {
    .broker = {
        .address.uri = BROKER,   
    },
    .credentials = {
        .client_id = Edev_id,    
    },
    .network.timeout_ms = 5000,   
};

        
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

// Use this to publish values
void mqtt_publish_value(int val)
{
    

    snprintf(TOPIC,sizeof(TOPIC),"%s/%s/heart",Egroup ,Edev_id);
    ESP_LOGI(TAG, "Group: %s", Egroup);
     ESP_LOGI(TAG, "TOPIC: %s", TOPIC);
  
    snprintf(payload, sizeof(payload), "Heart Rate  : %d", val);

       ESP_LOGI(TAG,"payload %s", payload);
        ESP_LOGI(TAG,"client pointer %p", client);
    esp_mqtt_client_enqueue(client, TOPIC, payload, 0, 1, 0,true);
   
}

void publish_task(void *pvParameter)
{
    while (1)

    {
        mqtt_publish_value(mqtt_buff_int);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}