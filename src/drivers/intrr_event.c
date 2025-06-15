
#include "ledconf.h"
#include "adc.h"

uint16_t *pADC_BUFF = adc_buffer; // assign a pointer to the buffer array

void DMA1_Channel1_IRQHandler(void)
{
    if ((DMA1->ISR & DMA_ISR_TCIF1)) // Transfer complete flag for channel 1
    {

        if (pADC_BUFF <= &adc_buffer[767])
        {

            *pADC_BUFF = adc_val;
            led_state(); // change led state
            pADC_BUFF++;
        }
        else
        {
            stop_process(); // stop buffer filling and process
            ACC_COMP = true;
            pADC_BUFF = adc_buffer; // reset the pointer
        }
        DMA1->IFCR = DMA_IFCR_CTCIF1; // Clear the transfer complete flag
    }
}
