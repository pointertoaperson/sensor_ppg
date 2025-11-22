#ifndef _INTERR_EVENT_H
#define _INTERR_EVENT_H

#include "esp_timer.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdbool.h>
#include <stdint.h>


#define SAMPLING_INTERVAL_US 1500 //1ms
#define PERIOD_OFF  1000//SAMPLING_INTERVAL_US / 2
#define ADC_BUFFER_SIZE 3072


extern int ppg_adc;
extern int16_t adc_buffer[ADC_BUFFER_SIZE];

void  adc_timer_callback(void* arg);

extern TaskHandle_t delay_task_handle ;
extern TaskHandle_t oled_task_handle ;

void _adc_dma_init(void);
void start_adc_task(void);



#endif