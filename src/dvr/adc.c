

#include "adc.h"

// --- ADC Functions ---
void adc_init(void) {
 // Set reference voltage to VCC, left adjust ADC result, enable ADC
    ADMUX = (1 << REFS0); //  AREF internal 
    ADCSRA = (1 << ADEN)  // Enable ADC
             | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescaler 128 (for 16 MHz clock)




}

uint16_t read_adc(uint8_t channel) {
    // Select the channel (0-7)
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    
    // Start conversion
    ADCSRA |= (1 << ADSC);
    
    // Wait for conversion to finish
    while (ADCSRA & (1 << ADSC));
    
    // Return ADC value
    return ADC;
}
