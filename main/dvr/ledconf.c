#include "ledconf.h"

#include "esp_attr.h"
#include "esp32/rom/ets_sys.h"


static uint8_t state = 0x01;

#if 0

void led_init(void)
{
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << RED_LED_PIN) | (1ULL << IR_LED_PIN) | (1ULL << GREEN_LED_PIN),
        .pull_down_en = 0,
        .pull_up_en = 0
    };
    gpio_config(&io_conf);

    // Default to Green LED on
    gpio_set_level(RED_LED_PIN, 0);
    gpio_set_level(IR_LED_PIN, 0);
    gpio_set_level(GREEN_LED_PIN, 1);
}

uint8_t led_state(void)
{
    state <<= 1;

    switch (state)
    {
        case 0x02: // IR LED
            gpio_set_level(GREEN_LED_PIN, 0);
            gpio_set_level(IR_LED_PIN, 1);
            gpio_set_level(RED_LED_PIN, 0);
            break;

        case 0x04: // RED LED
            gpio_set_level(GREEN_LED_PIN, 0);
            gpio_set_level(IR_LED_PIN, 0);
            gpio_set_level(RED_LED_PIN, 1);
            break;

        case 0x08: // RESET to GREEN
            state = 0x01;
            gpio_set_level(GREEN_LED_PIN, 1);
            gpio_set_level(IR_LED_PIN, 0);
            gpio_set_level(RED_LED_PIN, 0);
            break;
    }

    return state;
}
#else

void led_init(void)
{
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << RED) | (1ULL << IR) | (1ULL << GREEN),
        .pull_down_en = 0,
        .pull_up_en = 0
    };
    gpio_config(&io_conf);

    // Default to GREEN LED ON
    GPIO.out_w1tc = (1 << RED) | (1 << IR);   // clear RED and IR
    GPIO.out_w1ts = (1 << GREEN);             // set GREEN
}

// Returns new state
uint8_t IRAM_ATTR led_state(void)
{
    // Shift state to the next LED
    state <<= 1;
    //turn on the led constantly
GPIO.out_w1ts = (1 << GREEN);
    // Turn off all LEDs first (clear bits)
   // GPIO.out_w1tc = (1 << GREEN) | (1 << IR) | (1 << RED);
ets_delay_us(100);
    

    switch (state)
    {
        case 0x02: // IR LED
        
          //  GPIO.out_w1ts = (1 << IR);
                       
            break;

        case 0x04: // RED LED
            
//            GPIO.out_w1ts = (1 << RED);
                 
            break;

        case 0x08: // RESET to GREEN
            state = 0x01;
            
//            GPIO.out_w1ts = (1 << GREEN);
                    
            break;
    }

    return state;
}

void led_off( void )
{

    GPIO.out_w1tc = (1 << GREEN) | (1 << IR) | (1 << RED);
}

#endif