/**
 * @file    ledconf.h
 * @brief   LED control logic for STM32F103 using GPIOA and GPIOC
 * @author  Umesh Puri
 * @date    2025-06-02
 * @version 1.0
 *
 * This file implements LED initialization and state control for a 3-LED setup
 * (Green, IR, Red) using STM32F103 GPIO. The LEDs cycle through a predefined pattern.
 * 
 * @license MIT License
 * Copyright (c) 2025 Umesh
 */


#ifndef _LEDCONF_H
#define _LEDCONF_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f1xx.h"
#include "core_cm3.h"
#include "adc.h"

#define RED_LED_PIN    GPIO_ODR_ODR5  // PA5
#define IR_LED_PIN     GPIO_ODR_ODR6  // PA6
#define GREEN_LED_PIN  GPIO_ODR_ODR7  // PA7


void led_init(void);
uint8_t led_state();

#endif
