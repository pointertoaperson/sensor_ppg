#ifndef _PROCESS_H
#define _PROCESS_H
#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"

#include "math.h"
#include "esp_dsp.h"
#include "interr_event.h"


#define FFT_SIZE 2048
#define SAMPLE_RATE 222.222f // Hz

extern volatile uint32_t ppg_val;  
extern volatile uint32_t spo2_val;
void dsp_process(void); 




#endif