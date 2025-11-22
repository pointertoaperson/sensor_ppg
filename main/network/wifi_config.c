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

static bool sntp_initialized = false;
static bool time_sync_started = false;
void time_sync_and_start_mqtt_task(void *pv);
static char *TAG = "TIME and MQTT";

char Edev_id[12] = {0};   // extern to mqttapp
char Ebroker[64] = {0};   // extern to mqttapp
char Eusername[32] = {0}; // extern to mqttapp
char Epsdconn[32] = {0};  // extern to mqttapp
char Egroup[18] = {0};    // extern to mqttapp
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
static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    char ip_str[16];
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        ESP_LOGI(TAG, "STA started, connecting to AP...");
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        ESP_LOGI(TAG, "Disconnected from AP, retrying...");
        esp_wifi_connect();
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;

        esp_ip4addr_ntoa(&event->ip_info.ip, ip_str, sizeof(ip_str));

        ESP_LOGI(TAG, "Connected, got IP: %s", ip_str);
        if (!time_sync_started)
        {
            time_sync_started = true;
            ESP_LOGI("alert", "got IP and reached to the time sync");
            BaseType_t x = xTaskCreate(time_sync_and_start_mqtt_task,
                                       "time_sync_task",
                                       4096, /* stack in bytes (words on esp-idf) */
                                       NULL,
                                       tskIDLE_PRIORITY + 1,
                                       NULL);
            if (x != pdPASS)
            {
                ESP_LOGE(TAG, "Failed to create time_sync task");
                time_sync_started = false;
            }
        }
    }
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
    char ssid[32] = {0};     // ssid
    char password[64] = {0}; // password

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

    // Read dev_id
    len = sizeof(Edev_id);
    err = nvs_get_str(handle, "dev_id", Edev_id, &len);
    if (err != ESP_OK)
    {
        ESP_LOGW(TAG, "Failed to read dev_id from NVS: %s", esp_err_to_name(err));
        Edev_id[0] = '\0'; // dev_id might be empty
    }
    else
    {
        Edev_id[sizeof(Edev_id) - 1] = '\0'; // ensure null-termination
    }

    // Read broker_address
    len = sizeof(Ebroker);
    err = nvs_get_str(handle, "broker", Ebroker, &len);
    if (err != ESP_OK)
    {
        ESP_LOGW(TAG, "Failed to read broker address from NVS: %s", esp_err_to_name(err));
        Ebroker[0] = '\0'; // broker might be empty
    }
    else
    {
        Ebroker[sizeof(Ebroker) - 1] = '\0'; // ensure null-termination
    }

    // Read broker_address
    len = sizeof(Egroup);
    err = nvs_get_str(handle, "group", Egroup, &len);
    if (err != ESP_OK)
    {
        ESP_LOGW(TAG, "Failed to read group from NVS: %s", esp_err_to_name(err));
        Egroup[0] = '\0'; // broker might be empty
    }
    else
    {
        Egroup[sizeof(Egroup) - 1] = '\0'; // ensure null-termination
    }
    nvs_close(handle);

    ESP_LOGI(TAG, "Read SSID from NVS: '%s'", ssid);
    ESP_LOGI(TAG, "Read Password from NVS: '%s'", password);
    ESP_LOGI(TAG, "Read device id from NVS: '%s'", Edev_id);
    ESP_LOGI(TAG, "Read broker address from NVS: '%s'", Ebroker);
    ESP_LOGI(TAG, "Read group ID from NVS: '%s'", Egroup);

    esp_netif_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {0};
    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    wifi_config.sta.ssid[sizeof(wifi_config.sta.ssid) - 1] = '\0';

    strncpy((char *)wifi_config.sta.password, password, sizeof(wifi_config.sta.password) - 1);
    wifi_config.sta.password[sizeof(wifi_config.sta.password) - 1] = '\0';

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "STA connecting to saved SSID: %s", ssid);
}

esp_err_t root_get_handler(httpd_req_t *req)
{
    httpd_resp_send(req, html_form, strlen(html_form)); // Fixed here
    return ESP_OK;
}

esp_err_t setwifi_post_handler(httpd_req_t *req)
{
    char buf[512] = {0};
    int ret, remaining = req->content_len;
    char ssid[32] = {0}, password[64] = {0}, dev_id[12] = {0}, broker[64] = {0}, username[32] = {0}, psdconn[32] = {0}, group[18] = {0};

    while (remaining > 0)
    {
        if ((ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)))) <= 0)
        {
            return ESP_FAIL;
        }
        remaining -= ret;
    }

    sscanf(buf, "ssid=%31[^&]&password=%61[^&]&broker=%61[^&]&device_id=%12[^&]&username=%31[^&]&psdconn=%31[^&]&group=%16[^&]&", ssid, password, broker, dev_id, username, psdconn, group); // delimeter - '&'
    ESP_LOGI("buf", "buff = %s", buf);
    ESP_LOGI(TAG, "Received SSID: %s, Password: %s, Device ID: %s, Broker: %s, Username = %s, PASDCON = %s, GROUP =%s", ssid, password, dev_id, broker, username, psdconn, group);

    nvs_handle_t handle;
    ESP_ERROR_CHECK(nvs_open(WIFI_NAMESPACE, NVS_READWRITE, &handle));
    ESP_ERROR_CHECK(nvs_set_str(handle, "ssid", ssid));
    ESP_ERROR_CHECK(nvs_set_str(handle, "password", password));
    ESP_ERROR_CHECK(nvs_set_str(handle, "dev_id", dev_id));
    ESP_ERROR_CHECK(nvs_set_str(handle, "broker", broker));
    ESP_ERROR_CHECK(nvs_set_str(handle, "username", username));
    ESP_ERROR_CHECK(nvs_set_str(handle, "psdconn", psdconn));
    ESP_ERROR_CHECK(nvs_set_str(handle, "group", group));
    ESP_ERROR_CHECK(nvs_commit(handle));
    nvs_close(handle);

    httpd_resp_send(req, "Saved. Restarting...", strlen("Saved. Restarting..."));
    vTaskDelay(pdMS_TO_TICKS(2000));
    esp_restart();
    return ESP_OK;
}

/* Initialize soft AP */

esp_netif_t *wifi_start_softap_mode(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Create default AP network interface
    esp_netif_t *esp_netif_ap = esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t ap_config = {
        .ap = {
            .ssid = "connectHeart",
            .ssid_len = strlen("connectHeart"),
            .channel = 1,
            .password = "password123",
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .required = false,
            },
        },
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

    esp_netif_ip_info_t ip_info;
    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");

    if (netif && esp_netif_get_ip_info(netif, &ip_info) == ESP_OK)
    {
        char ip_str[16];
        esp_ip4addr_ntoa(&ip_info.ip, ip_str, sizeof(ip_str));
        ESP_LOGI(TAG, "SoftAP IP address: %s", ip_str);
    }
    else
    {
        ESP_LOGE(TAG, "Failed to get SoftAP IP address");
    }

    // Start HTTP server
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_uri_t root = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = root_get_handler};
        httpd_uri_t setwifi = {
            .uri = "/SetConfig",
            .method = HTTP_POST,
            .handler = setwifi_post_handler};

        httpd_register_uri_handler(server, &root);
        httpd_register_uri_handler(server, &setwifi);
    }
    else
    {
        ESP_LOGE(TAG, "Failed to start HTTP server");
    }

    return esp_netif_ap;
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

    err = nvs_erase_key(handle, "dev_id");
    if (err == ESP_OK)
    {
        ESP_LOGI("NVS", "dev_id erased");
    }
    else
    {
        ESP_LOGW("NVS", "dev_id not found or failed to erase: %s", esp_err_to_name(err));
    }

    err = nvs_erase_key(handle, "broker");
    if (err == ESP_OK)
    {
        ESP_LOGI("NVS", "broker erased");
    }
    else
    {
        ESP_LOGW("NVS", "broker not found or failed to erase: %s", esp_err_to_name(err));
    }

    err = nvs_erase_key(handle, "username");
    if (err == ESP_OK)
    {
        ESP_LOGI("NVS", "username erased");
    }
    else
    {
        ESP_LOGW("NVS", "username not found or failed to erase: %s", esp_err_to_name(err));
    }

    err = nvs_erase_key(handle, "psdconn");
    if (err == ESP_OK)
    {
        ESP_LOGI("NVS", "psdconn erased");
    }
    else
    {
        ESP_LOGW("NVS", "psdconn not found or failed to erase: %s", esp_err_to_name(err));
    }
    err = nvs_erase_key(handle, "group");
    if (err == ESP_OK)
    {
        ESP_LOGI("NVS", "group erased");
    }
    else
    {
        ESP_LOGW("NVS", "group not found or failed to erase: %s", esp_err_to_name(err));
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

void time_sync_and_start_mqtt_task(void *pv)
{
    ESP_LOGI("alert! ", " time sync task entered");
    /* set timezone if you want local time; use UTC for cert checks too */
    setenv("TZ", "UTC0", 1);
    tzset();

    /* init SNTP if not already inited */
    initialize_sntp();

    /* wait for sync (this blocks but it's ok inside a dedicated task) */
    wait_for_time_sync();

    /* after time is OK, start MQTT */
    ESP_LOGI(TAG, "Starting MQTT (time sync done)");
    ESP_LOGI("alert! ", " reached mqtt start");

    mqtt_app_start();
    /* task done */
    vTaskDelete(NULL);
}

void initialize_sntp(void)
{
    if (sntp_initialized)
        return;

    ESP_LOGI("time", "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    // esp_sntp_init();
    sntp_init();

    sntp_initialized = true;
}

void wait_for_time_sync(void)
{
    time_t now = 0;
    struct tm timeinfo = {0};

    initialize_sntp();

    int retry = 0;
    const int retry_count = 5;
    while (timeinfo.tm_year < (2022 - 1900) && ++retry < retry_count)
    {
        ESP_LOGI("time", "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    if (timeinfo.tm_year < (2022 - 1900))
    {
        ESP_LOGW("time", "System time not set correctly!");
    }
    else
    {

        ESP_LOGI("time", "System time is set %d", timeinfo.tm_year);
    }
}

