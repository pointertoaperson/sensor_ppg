
#include "sensor_ppg.h"
#include "display.h"
#include "i2c.h"
#include "adc.h"
#include "process_sequence.h"

uint16_t samples[SAMPLE_LEN];
uint16_t filled_data = 0 ;
// uint16_t ma_buffer[MA_WINDOW] = {0};
// uint16_t bpm_list[SAMPLE_LEN / MA_WINDOW] = {0};

int main(void)
{
  // pointers to memories

  uint16_t *pSampler = (uint16_t *)&samples;
  // uint16_t pBpm = bpm_list;

  // initializations
  do_sample = true;
  bool do_display = true;
  timer1_init();
  sei();
  adc_init();
  ssd1306_init();
  ssd1306_clearDisplay_banner();
  ssd1306_clearDisplay();
  // sample ADC
  DDRD |= (1 << PD2);
  PORTD |= (1 << PD2);
  while (1)
  {
      int data_len = pSampler-samples;
      filled_data = data_len;
    if (do_sample)
    {
      sample_adc(&pSampler, pSampler-1);
      _delay_ms(20);

    }
    else
    {
       uint16_t bpm = get_bpm(samples, data_len);
        ssd1306_clearDisplay();
        ssd1306_clearDisplay_banner();
        char bstr[30] ;
       sprintf(bstr, "BPM %d ", bpm );

        draw_text(bstr, 0, 0);
        ssd1306_set_address_range(3, 3);
        ssd1306_updateDisplay_text();
      
        draw_waveform(samples, data_len);
      ssd1306_set_address_range(0, 2);
      ssd1306_updateDisplay_wave(); // Update the display to show the waveform
      // do_display = false;
      // should renable sampling
      //PORTB |= (1 << PB5);
      pSampler = samples;
      do_sample = enable_sample();
    }
  }
}
