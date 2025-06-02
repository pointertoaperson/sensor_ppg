#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "i2c_helper.h"
#include "display.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "comm_spi.h"

volatile uint16_t adc_val ;
volatile char *bstr[20];

void oled_task(void);

  void app_main()
  {
    //spi initialization
    comm_spi_init();
    
    
    
    // adc initialization
    adc_config_t adc_config;
    adc_config.mode = ADC_READ_TOUT_MODE;
    adc_config.clk_div = 8; //10MHz sampling time
    ESP_ERROR_CHECK(adc_init(&adc_config));
   
   
   
    //i2c initialization
    i2c_init();
   
   
   
    //ssd1306 initialization
    ssd1306_init();
    ssd1306_clearDisplay();
    ssd1306_updateDisplay(0);
    
    //create tasks
xTaskCreate(oled_task, "oled_task",1024,NULL,4,NULL);      

xTaskCreate(spi_slave_read_master_task, "spi_task",2048,NULL,4,NULL);

}


  //oled task for diaplaying
void oled_task(){
while(1) {
//read and display adc value
adc_read(&adc_val); // read adc and fill in adc_val

sprintf(bstr, "adc:%d  spi:%" PRIu32, adc_val, spi_val);
draw_text(bstr, 0, 0);

        

uint16_t arr = 24;
 animate((adc_val*17)/1024);
// ssd1306_updateDisplay();
 
    vTaskDelay(20/portTICK_PERIOD_MS);// to be updated at every 20ms
 };
  }

         
