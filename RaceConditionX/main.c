/*
 * main.c
 *
 * Created: 20.03.2018 18:32:07
 * Author : chaos
 */ 

//#include <avr/io.h>
#include "avr_compiler.h"
#include "pmic_driver.h"
#include "TC_driver.h"
#include "clksys_driver.h"
#include "sleepConfig.h"
#include "port_driver.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "stack_macros.h"

#include "mem_check.h"

#include "init.h"
#include "utils.h"
#include "errorHandler.h"
#include "NHD0420Driver.h"

#define KEY_QUEUE_SIZE 20


extern void vApplicationIdleHook( void );
void vDisplayTask(void *pvParameters);
void vButtonHandler1(void *pvParameters);
void vButtonHandler2(void *pvParameters);
void vButtonHandler3(void *pvParameters);
void vButtonHandler4(void *pvParameters);

SemaphoreHandle_t buttonUpdate;		//ButtonUpdate Semaphore to signal new Buttonpress
SemaphoreHandle_t buttondataKey;	//A-Resource for buttonData
xQueueHandle xKeyQueue;									// Queue for Keys

void vApplicationIdleHook( void )
{	
	
}

int main(void)
{
    resetReason_t reason = getResetReason();

	vInitClock();
	vInitDisplay();
	
	buttonUpdate = xSemaphoreCreateBinary();
	buttondataKey = xSemaphoreCreateMutex();

	xTaskCreate( vDisplayTask, (const char *) "dTask", configMINIMAL_STACK_SIZE+10, NULL, 1, NULL);
	xTaskCreate( vButtonHandler1, (const char *) "bHandler", configMINIMAL_STACK_SIZE+10, NULL, 2, NULL);
	xTaskCreate( vButtonHandler2, (const char *) "bHandler", configMINIMAL_STACK_SIZE+10, NULL, 2, NULL);
	xTaskCreate( vButtonHandler3, (const char *) "bHandler", configMINIMAL_STACK_SIZE+10, NULL, 2, NULL);
	xTaskCreate( vButtonHandler4, (const char *) "bHandler", configMINIMAL_STACK_SIZE+10, NULL, 2, NULL);

	xKeyQueue = xQueueCreate(KEY_QUEUE_SIZE, sizeof(uint8_t));

	vDisplayClear();
	vTaskStartScheduler();
	return 0;
}

void vDisplayTask(void *pvParameters) {
	uint32_t b1 = 0, b2 = 0, b3 = 0, b4 = 0;
	for(;;) {
		
		uint8_t buttonData;			//P-Resource

		if (uxQueueMessagesWaiting(xKeyQueue)) {
			xQueueReceive(xKeyQueue, &buttonData, portMAX_DELAY);
		
			switch(buttonData) {
				case 1:
					b1++;
				break;
				case 2:
					b2++;
				break;
				case 3:
					b3++;
				break;
				case 4:
					b4++;
				break;
			}
			buttonData = 0;
			vDisplayWriteStringAtPos(0,0,"B1: %d", b1);
			vDisplayWriteStringAtPos(1,0,"B2: %d", b2);
			vDisplayWriteStringAtPos(2,0,"B3: %d", b3);
			vDisplayWriteStringAtPos(3,0,"B4: %d", b4);
		}
		vTaskDelay(100 / portTICK_RATE_MS);
	}
}

void vButtonHandler1(void *pvParameters) { //Buttonhandler to debounce Button and send to DisplayTask
	PORTF.DIRCLR = 0x10;
	for(;;) {
		if((PORTF.IN & 0x10) == 0x00) {
			while((PORTF.IN & 0x10) == 0x00) { //crude debouncing
				vTaskDelay(10);
			}
			uint8_t buttonData = 1;
			xQueueSendToBack(xKeyQueue, &buttonData, portMAX_DELAY);
			vTaskDelay(50 / portTICK_RATE_MS);
		}
	}
}
void vButtonHandler2(void *pvParameters) {
	PORTF.DIRCLR = 0x20;
	for(;;) {
		if((PORTF.IN & 0x20) == 0x00) {
			while((PORTF.IN & 0x20) == 0x00) {
				vTaskDelay(10);
			}
			uint8_t buttonData = 2;
			xQueueSendToBack(xKeyQueue, &buttonData, portMAX_DELAY);
			vTaskDelay(50 / portTICK_RATE_MS);
		}
	}
}
void vButtonHandler3(void *pvParameters) {
	PORTF.DIRCLR = 0x40;
	for(;;) {
		if((PORTF.IN & 0x40) == 0x00) {
			while((PORTF.IN & 0x40) == 0x00) {
				vTaskDelay(10);
			}
			uint8_t buttonData = 3;
			xQueueSendToBack(xKeyQueue, &buttonData, portMAX_DELAY);
			vTaskDelay(50 / portTICK_RATE_MS);
		}
	}
}
void vButtonHandler4(void *pvParameters) {
	PORTF.DIRCLR = 0x80;
	for(;;) {
		if((PORTF.IN & 0x80) == 0x00) {
			while((PORTF.IN & 0x80) == 0x00) {
				vTaskDelay(10);
			}
			uint8_t buttonData = 4;
			xQueueSendToBack(xKeyQueue, &buttonData, portMAX_DELAY);
			vTaskDelay(50 / portTICK_RATE_MS);
		}
	}
}