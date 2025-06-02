/**
 * @file    uart.h
 * @brief   UART driver interface for STM32F103 microcontroller series and variants like STM32F103C8T6/CBT6 etc.
 * @author  Umesh Puri
 * @date    2025-06-02
 * @version 1.0
 *
 * This file provides function prototypes for initializing and using the USART
 * peripheral for serial communication on STM32F103 microcontrollers.
 *
 * @license MIT License
 * Copyright (c) 2025 Umesh
 */

#ifndef UART_H_
#define UART_H_

int uart1_init(void);
void uart1_write(int ch);
char uart1_read(void);

#endif
