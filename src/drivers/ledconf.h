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
