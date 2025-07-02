
/**
 * @file    spi.c
 * @brief   SPI driver interface for STM32F103
 * @author  Umesh Puri
 * @date    2025-06-02
 * @version 1.0
 *
 * This file provides the interface for configuring and using the SPI peripheral
 * on the STM32F103. It includes functions for initialization and data transfer.
 *
 * @license MIT License
 * Copyright (c) 2025 Umesh
 */

#ifndef _SPI_H
#define _SPI_H
#include "stm32f1xx.h"
#include "stdint.h"
#include "HSE_CLK.h"
#include "delay.h"
#include "uart.h"


void spi_init(void);
void spi_transmit(uint32_t dataH, uint32_t dataL);
void cs_low();
void cs_high();

#endif
