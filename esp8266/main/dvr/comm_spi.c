/**
 * @file    comm_spi.h
 * @brief   SPI slave communication wrapper for ESP8266
 * @author  Umesh Puri
 * @date    2025-06-02
 * @version 1.0
 *
 * This file implements SPI slave-mode communication support for ESP8266,
 * providing a wrapper around  SPI peripheral access.
 * It includes initialization routines and data transfer helpers for
 * communicating as an SPI slave with a master controller or device.
 *
 * @license MIT License
 * Copyright (c) 2025 Umesh
 */

#include "comm_spi.h"

volatile uint32_t ppg_val = 0;
volatile uint32_t spo2_val = 0;

// SPI initilization function
void comm_spi_init(void)
{
    spi_config_t spi_config;

    // CS_EN:1, MISO_EN:1, MOSI_EN:1, BYTE_TX_ORDER:1, BYTE_TX_ORDER:1, BIT_RX_ORDER:0, BIT_TX_ORDER:0, CPHA:0, CPOL:0
    spi_config.interface.val = SPI_DEFAULT_INTERFACE;

    spi_config.intr_enable.val = SPI_SLAVE_DEFAULT_INTR_ENABLE;
    // Set SPI to slave mode
    spi_config.mode = SPI_SLAVE_MODE;
    // Register SPI event callback function
    spi_config.event_cb = NULL;


    spi_init(HSPI_HOST, &spi_config);

    hspi_slave_logic_device_create(SPI_SLAVE_HANDSHARK_GPIO, 1, SPI_WRITE_BUFFER_MAX_SIZE, SPI_READ_BUFFER_MAX_SIZE);
}

void IRAM_ATTR spi_slave_read_master_task(void *arg)
{
    uint8_t read_data[8]; // Just 8 bytes i.e. 2 integers

    for (;;)
    {
        taskYIELD();

        int read_len = hspi_slave_logic_read_data(read_data, 8, 1); // blocking read

        if (read_len == 8)
        {
            spo2_val = *((uint32_t *)&read_data[0]);
            ppg_val = *((uint32_t *)&read_data[4]);

          //  ESP_LOGI("spi_task", "PPG: %u, SpO2: %u", ppg_val, spo2_val);
        }

       // vTaskDelay(pdMS_TO_TICKS(1));
    }
}
