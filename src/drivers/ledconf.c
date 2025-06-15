#include "ledconf.h"

void led_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    // PA5, PA6, PA7 output push-pull 10 MHz
    GPIOA->CRL &= ~(GPIO_CRL_CNF5 | GPIO_CRL_MODE5 |
                    GPIO_CRL_CNF6 | GPIO_CRL_MODE6 |
                    GPIO_CRL_CNF7 | GPIO_CRL_MODE7);
    GPIOA->CRL |= (GPIO_CRL_MODE5_0 | GPIO_CRL_MODE6_0 | GPIO_CRL_MODE7_0);

    // default to green LED
    GPIOA->ODR &= ~RED_LED_PIN;
    GPIOA->ODR &= ~IR_LED_PIN;
    GPIOA->ODR |= GREEN_LED_PIN;

    GPIOC->CRH &= ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13);
    
  // GPIOC->ODR |= GPIO_ODR_ODR13; //test led off remove later
    
}

uint8_t state =  0x01;

uint8_t led_state()
{
     state <<= 1;

    switch (state)
    {

    case 0x02:
        // Turn on IR only
        GPIOA->ODR &= ~GREEN_LED_PIN;
        GPIOA->ODR |= IR_LED_PIN;
        GPIOA->ODR &= ~RED_LED_PIN;

        break;

    case 0x04:

        // turn of RED only
        GPIOA->ODR &= ~GREEN_LED_PIN;
        GPIOA->ODR &= ~IR_LED_PIN;
        GPIOA->ODR |= RED_LED_PIN;

        break;
    case 0x08:
        //RESET state
        state = 0x01;
         GPIOA->ODR |= GREEN_LED_PIN;
        GPIOA->ODR &= ~IR_LED_PIN;
        GPIOA->ODR &= ~RED_LED_PIN;
        
        break;

        
    }
    return state;
}
