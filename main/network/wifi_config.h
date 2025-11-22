
/**
 * @file    wifi_config.h
 * @brief   Wi-Fi configuration and connection handling for ESP8266
 * @author  Umesh Puri
 * @date    2025-06-12
 * @version 1.0
 *
 * This file contains routines to configure and manage Wi-Fi connections
 * on the ESP8266 platform. It includes SSID/Password setup, connection
 * attempts, and event-based status tracking.
 *
 * @license MIT License
 * Copyright (c) 2025 Umesh
 */


#if 0
#ifndef _WIFI_CONFIG_H
#define _WIFI_CONFIG_H

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_netif_ip_addr.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_http_server.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "per_intr.h"
#include "mqtt_app.h"
#include "esp_sntp.h"

#include <stdbool.h>

#define WIFI_NAMESPACE "wifi_cfg"

extern char Edev_id[12]; // unique device id
extern char Ebroker[64]; // broker address
extern char Eusername[32]; // username
extern char Epsdconn[32]; //password
extern char Egroup[18]; //group ID

//void wifi_start_softap_mode(void);
esp_netif_t *wifi_start_softap_mode(void);
void wifi_start_sta_mode(void);
bool wifi_sta_if_credentials_saved(void);
void clear_saved_wifi_credentials();

void initialize_sntp(void); 
void wait_for_time_sync();
#endif

#else
#ifndef _WIFI_CONFIG_H
#define _WIFI_CONFIG_H

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_netif_ip_addr.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_http_server.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "per_intr.h"
#include "mqtt_app.h"
#include "esp_sntp.h"

#include <stdbool.h>

#define WIFI_NAMESPACE "wifi_cfg"

extern char Edev_id[12]; // unique device id
extern char Ebroker[64]; // broker address
extern char Eusername[32]; // username
extern char Epsdconn[32]; //password
extern char Egroup[18]; //group ID

//void wifi_start_softap_mode(void);
esp_netif_t *wifi_start_softap_mode(void);
void wifi_start_sta_mode(void);
bool wifi_sta_if_credentials_saved(void);
void clear_saved_wifi_credentials();

void initialize_sntp(void); 
void wait_for_time_sync(void);
#endif


#endif
