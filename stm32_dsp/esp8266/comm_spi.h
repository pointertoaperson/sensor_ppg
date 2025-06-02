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

#ifndef _COMM_SPI_H
#define _COMM_SPI_H

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

#include "driver/spi.h"
#include "driver/hspi_logic_layer.h"

#define SPI_SLAVE_HANDSHARK_GPIO 2

#define SPI_WRITE_BUFFER_MAX_SIZE 4
#define SPI_READ_BUFFER_MAX_SIZE 4
extern volatile uint32_t spi_val;
void comm_spi_init(void);
void IRAM_ATTR spi_slave_read_master_task(void *arg);

#endif
