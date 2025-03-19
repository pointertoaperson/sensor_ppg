
#include <avr/io.h>

#include "FreeRTOS.h"
#include "task.h"


#define mainLED_TASK_PRIORITY			(tskIDLE_PRIORITY)
//#define mainLED_TASK_PRIORITY 			(tskIDLE_PRIORITY+1)
// #define mainNEXT_TASK_2				(tskIDLE_PRIORITY+2)
// #define mainNEXT_TASK_3				(tskIDLE_PRIORITY+3)


// TODO: Research moving vLEDFlashTask method to apptasks.c
void vGPIOD5(void *pvParms)
{
for(;;){
	PORTD = PORTD | (1 << PORTD5);
	vTaskDelay(400/portTICK_PERIOD_MS);
	PORTD = PORTD  & ~(1 <<PORTD5);
		vTaskDelay(400/portTICK_PERIOD_MS);	 
}
}
void vGPIOD6(void *pvParms)
{
for(;;){
	PORTD = PORTD | (1 << PORTD6);
	vTaskDelay(200/portTICK_PERIOD_MS);
	PORTD = PORTD  & ~(1 <<PORTD6);
	vTaskDelay(200/portTICK_PERIOD_MS);
	 
}
}

void init(void);

void vApplicationIdleHook( void );

int  main(void)
{
DDRD |= (1 << DDD5);
DDRD |= (1<<DDD6);
	xTaskCreate(vGPIOD5, (int8_t*) "LED", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(vGPIOD6, (int8_t*) "LED", configMINIMAL_STACK_SIZE, NULL, 1/*mainLED_TASK_PRIORITY*/, NULL);


	vTaskStartScheduler();

	return 0;
}


void vApplicationIdleHook( void ){

}
