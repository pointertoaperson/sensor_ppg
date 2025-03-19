#include "sensor_ppg.h"
#include "process_sequence.h"

volatile uint8_t display_update_flag = 0;
volatile uint16_t ms_counter = 0;

bool do_sample = true;
// initialize timer 1 for interrupt
void timer1_init()
{
    DDRB |= (1 << PB5);
    TCCR1B |= (1 << WGM12);   // CTC mode (Clear Timer on Compare Match)
    TCCR1A &= ~(1 << COM1A0); // Clear COM1A0 to avoid toggling OC1A initially
                              // TCCR1A |= (1 << COM1A0);      // Toggle OC1A on compare match (PB1 pin)
    TCCR1B |= (1 << CS12);    
    TCCR1B |= (1 << CS10);    
    TCCR1B &= ~(1 << CS11);    
    OCR1A = TM1_CMP;             // Set the compare match value for 10ms interval
    TIMSK1 |= (1 << OCIE1A);  // Enable interrupt on compare match A for Timer1
}

// interrupt vector routine
int samp_count = 0;
int BPM_f = 0;


ISR(TIMER1_COMPA_vect)
{
    if (filled_data < SAMPLE_LEN)
    {
        return;
     }
    TIMSK1 &= ~(1 << OCIE1A);  // disable interrupt Timer1
    do_sample = false;
      
       // _delay_ms(5);
}

void sample_adc(uint16_t **samples, uint16_t *prev)
{

    uint16_t curr_read = read_adc(0);
   // _delay_ms(5);
    if ( (curr_read - *prev) > 100 )
    {
        curr_read -= 150;
    }

    **samples =  curr_read;
    (*samples)++;
    //*samples = curr_read <= 0 ? *prev : curr_read;

    if (curr_read > THRESHOLD)
    {
        PORTB |= (1 << PB5);
    }
    else
    {
        PORTB &= ~(1 << PB5);
    }
}

uint16_t count_peaks(int16_t *samples, const uint16_t length)
{
    uint16_t ma_buffer[MA_WINDOW] = {0};
    int16_t prev_sample = 0;
    uint32_t sum = 0;
    uint8_t ma_index = 0;
    uint16_t last_peak_index = 0;
    uint8_t in_peak = 0;
    uint16_t peak_count = 0;
    uint16_t sample_dist = 0;
    for (uint16_t i = 0; i < length; i++)
    {
        int16_t diff = samples[i] - prev_sample;
        prev_sample = samples[i];

        uint32_t squared = (int32_t)diff * (int32_t)diff;

        // Moving average
        sum = sum - ma_buffer[ma_index] + squared;
        ma_buffer[ma_index] = squared;
        ma_index = (ma_index + 1) % MA_WINDOW;

        uint32_t integrated = sum / MA_WINDOW;

        // short circuit

        if (!in_peak && (integrated > THRESHOLD) && ((i - last_peak_index) > MIN_PEAK_DIST))
        {
            in_peak = 1;
            sample_dist += (i - last_peak_index);
            last_peak_index = i;
            peak_count++;
        }
        else if (in_peak && (integrated < THRESHOLD))
        {
            in_peak = 0;
        }
    }

    return sample_dist / peak_count; // peak_count;
}
# if 0
        char bpm_str[20];
        sprintf(bpm_str, "BPM:%d   SPO2:%d %%", BPM, display_update_flag);

        ssd1306_clearDisplay_banner();
        draw_text(bpm_str, 0, 0);
        ssd1306_set_address_range(3, 3);
        ssd1306_updateDisplay_text();
#endif


uint16_t  get_bpm(uint16_t *samples, uint16_t data_len) {
        BPM_f = (count_peaks(samples, data_len) * 20);
        uint16_t BPM = 60000 / BPM_f;
     
    return BPM ;
}
            