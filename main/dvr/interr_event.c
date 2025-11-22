#include "ledconf.h"
#include "interr_event.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp32/rom/ets_sys.h"

int adc_val = 0;

int ppg_adc = 0;

uint8_t ifPPGled = 0x01;

static int16_t *pADC_BUFF = &adc_buffer[0];

static esp_timer_handle_t adc_timer;
static adc_oneshot_unit_handle_t adc_handle;
TaskHandle_t delay_task_handle = NULL;

static void display_callback(void);
static inline int16_t read_adc(void);

volatile bool ACC_COMP = false;
uint8_t disp_toggle = 0x01;
uint8_t scan_rate = 0;

int16_t adc_buffer[ADC_BUFFER_SIZE] = {0};

// ADC read function
static IRAM_ATTR int16_t read_adc()
{

    esp_err_t ret = adc_oneshot_read(adc_handle, ADC_CHANNEL_0, &adc_val);
    if (ret != ESP_OK)
        return 0;

    return (int16_t)adc_val;
}

void IRAM_ATTR adc_timer_callback(void *arg)
{

    if (!ACC_COMP)
    {

        *pADC_BUFF = (read_adc());

        // ets_delay_us(PERIOD_OFF);

        if (ifPPGled == 0x01)
        {
            scan_rate += 1;
        }
        if (scan_rate == 8)
        {
            scan_rate = 0;
            ppg_adc = adc_val;
            display_callback();
        }
    }
    ifPPGled = led_state(); // toggle LED

    pADC_BUFF++;

    if (pADC_BUFF >= &adc_buffer[ADC_BUFFER_SIZE])
    {
        ACC_COMP = true;
        esp_timer_stop(adc_timer); // Stop filling until DSP finishes
        if (delay_task_handle != NULL)
        {
            xTaskNotifyGive(delay_task_handle);
        }
    }
}


// Initialize ADC
void _adc_dma_init(void)
{
    esp_timer_create_args_t adc_timer_args = {
        .callback = &adc_timer_callback,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "adc_timer"};

    ESP_ERROR_CHECK(esp_timer_create(&adc_timer_args, &adc_timer));

    adc_oneshot_unit_init_cfg_t unit_cfg = {.unit_id = ADC_UNIT_1};
    adc_oneshot_new_unit(&unit_cfg, &adc_handle);

    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_6,
    };
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_0, &chan_cfg);
    // start the timer
    start_adc_task();
}

// Start ADC sampling task pinned to CPU 1
void start_adc_task(void)
{
    ACC_COMP = false;
    pADC_BUFF = adc_buffer;
    esp_timer_start_periodic(adc_timer, SAMPLING_INTERVAL_US);
}

void IRAM_ATTR display_callback(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (oled_task_handle != NULL)
    {
        // Notify the oled_task that 20ms passed
        vTaskNotifyGiveFromISR(oled_task_handle, &xHigherPriorityTaskWoken);

        if (xHigherPriorityTaskWoken)
        {
            portYIELD_FROM_ISR();
        }
    }
}