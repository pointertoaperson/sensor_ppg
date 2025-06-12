/**
 * @file    wifi_config.c
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

#include "wifi_config.h"

static const char *TAG = "wifi_config";

// Define MIN if not defined
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

// http form
static const char *html_form =
    "<!DOCTYPE html><html><body>"
    "<h2>WiFi Configuration</h2>"
    "<form action=\"/setwifi\" method=\"post\">"
    "SSID: <input type=\"text\" name=\"ssid\" maxlength=\"32\"><br>"
    "Password: <input type=\"password\" name=\"password\" maxlength=\"64\"><br>"
    "<input type=\"submit\" value=\"Save\">"
    "</form></body></html>";

// Event handler with connect/disconnect handling
esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id)
    {
    case SYSTEM_EVENT_STA_START:
        ESP_LOGI(TAG, "STA started, connecting to AP...");
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "Connected, got IP: %s", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        ESP_LOGI(TAG, "Disconnected from AP, retrying...");
        esp_wifi_connect();
        break;
    default:
        break;
    }
    return ESP_OK;
}

bool wifi_sta_if_credentials_saved()
{
    nvs_handle_t handle;
    size_t len;
    esp_err_t err = nvs_open(WIFI_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK)
        return false;

    err = nvs_get_str(handle, "ssid", NULL, &len);
    nvs_close(handle);
    return (err == ESP_OK && len > 0);
}

void wifi_start_sta_mode()
{
    char ssid[32] = {0};
    char password[64] = {0};
    size_t len;

    nvs_handle_t handle;
    ESP_ERROR_CHECK(nvs_open(WIFI_NAMESPACE, NVS_READONLY, &handle));

    // Read SSID
    len = sizeof(ssid);
    esp_err_t err = nvs_get_str(handle, "ssid", ssid, &len);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to read SSID from NVS: %s", esp_err_to_name(err));
        nvs_close(handle);
        return;
    }
    ssid[sizeof(ssid) - 1] = '\0'; // ensure null-termination

    // Read Password
    len = sizeof(password);
    err = nvs_get_str(handle, "password", password, &len);
    if (err != ESP_OK)
    {
        ESP_LOGW(TAG, "Failed to read password from NVS: %s", esp_err_to_name(err));
        password[0] = '\0'; // password might be empty
    }
    else
    {
        password[sizeof(password) - 1] = '\0'; // ensure null-termination
    }

    nvs_close(handle);

    ESP_LOGI(TAG, "Read SSID from NVS: '%s'", ssid);
    ESP_LOGI(TAG, "Read Password from NVS: '%s'", password);

    // For ESP8266 RTOS SDK, use older TCP/IP stack init
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {0};
    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    wifi_config.sta.ssid[sizeof(wifi_config.sta.ssid) - 1] = '\0';

    strncpy((char *)wifi_config.sta.password, password, sizeof(wifi_config.sta.password) - 1);
    wifi_config.sta.password[sizeof(wifi_config.sta.password) - 1] = '\0';

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "STA connecting to saved SSID: %s", ssid);

    // initialize MQTT APP after connection
    mqtt_app_start();
}

esp_err_t root_get_handler(httpd_req_t *req)
{
    httpd_resp_send(req, html_form, strlen(html_form)); // Fixed here
    return ESP_OK;
}

esp_err_t setwifi_post_handler(httpd_req_t *req)
{
    char buf[256];
    int ret, remaining = req->content_len;
    char ssid[32] = {0}, password[64] = {0};

    while (remaining > 0)
    {
        if ((ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)))) <= 0)
        {
            return ESP_FAIL;
        }
        remaining -= ret;
    }

    sscanf(buf, "ssid=%31[^&]&password=%31[^&]", ssid, password);

    ESP_LOGI(TAG, "Received SSID: %s, Password: %s", ssid, password);

    nvs_handle_t handle;
    ESP_ERROR_CHECK(nvs_open(WIFI_NAMESPACE, NVS_READWRITE, &handle));
    ESP_ERROR_CHECK(nvs_set_str(handle, "ssid", ssid));
    ESP_ERROR_CHECK(nvs_set_str(handle, "password", password));
    ESP_ERROR_CHECK(nvs_commit(handle));
    nvs_close(handle);

    httpd_resp_send(req, "Saved. Restarting...", strlen("Saved. Restarting..."));
    vTaskDelay(pdMS_TO_TICKS(2000));
    esp_restart();
    return ESP_OK;
}

void wifi_start_softap_mode()
{
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t ap_config = {
        .ap = {
            .ssid = "connectHeart",
            .password = "password123",
            .ssid_len = 0,
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK},
    };

    if (strlen((const char *)ap_config.ap.password) == 0)
    {
        ap_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Wi-Fi SoftAP started. SSID:%s password:%s",
             ap_config.ap.ssid, ap_config.ap.password);

    // initiate https server

    httpd_handle_t server = NULL;

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_uri_t root = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = root_get_handler};
        httpd_uri_t setwifi = {
            .uri = "/setwifi",
            .method = HTTP_POST,
            .handler = setwifi_post_handler};

        httpd_register_uri_handler(server, &root);
        httpd_register_uri_handler(server, &setwifi);
    }
    else
    {
        ESP_LOGE(TAG, "Failed to start HTTP server");
    }
}

// Clear WiFi credentials
void clear_saved_wifi_credentials()
{
    nvs_handle_t handle;
    esp_err_t err;

    err = nvs_open(WIFI_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK)
    {
        ESP_LOGE("NVS", "Failed to open NVS: %s", esp_err_to_name(err));
        return;
    }

    err = nvs_erase_key(handle, "ssid");
    if (err == ESP_OK)
    {
        ESP_LOGI("NVS", "SSID erased");
    }
    else
    {
        ESP_LOGW("NVS", "SSID not found or failed to erase: %s", esp_err_to_name(err));
    }

    err = nvs_erase_key(handle, "password");
    if (err == ESP_OK)
    {
        ESP_LOGI("NVS", "Password erased");
    }
    else
    {
        ESP_LOGW("NVS", "Password not found or failed to erase: %s", esp_err_to_name(err));
    }

    err = nvs_commit(handle);
    if (err != ESP_OK)
    {
        ESP_LOGE("NVS", "Failed to commit changes: %s", esp_err_to_name(err));
    }
    else
    {
        ESP_LOGI("NVS", "WiFi credentials cleared successfully");
    }

    nvs_close(handle);
}
