#ifndef _LEDCONF_H
#define _LEDCONF_H

#include <stdint.h>
#include <stdbool.h>
#include "driver/gpio.h"
#include "soc/gpio_struct.h"
#include "interr_event.h"

// Define LED pins (adjust as per your ESP32 board)
#define RED    GPIO_NUM_5
#define IR     GPIO_NUM_12
#define GREEN  GPIO_NUM_13

void led_init(void);
uint8_t led_state(void);
void led_off( void );

extern volatile bool ACC_COMP ;



#endif
