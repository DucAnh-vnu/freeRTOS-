/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os2.h"
#define MASK(X) (1 << (X))

typedef struct {
	char text[64];
} Msg_t;

osMessageQueueId_t uartQueue;

void sendChar(char c){
	while (!(USART2->SR & USART_SR_TXE));
	USART2->DR = c;
}

void sendString(char *s)
{
	while(*s){
		sendChar(*s);
		s++;
	}
}

osThreadId_t ledTaskHanlder,buttonTaskHandler,logTaskHandler;
const osThreadAttr_t BlinkTask_attr = {
		.name = "BlinkTask",
		.stack_size = 128 * 4,
		.priority = (osPriority_t) osPriorityNormal,

};

const osThreadAttr_t ButtonTask_attr = {
		.name = "ButtonTask",
		.stack_size = 128 * 4,
		.priority = (osPriority_t) osPriorityNormal,
};


const osThreadAttr_t logTaskHandler_attr ={
		.name = "uartHandler",
		.stack_size = 256 * 4,
		.priority = (osPriority_t) osPriorityNormal,

};
void BlinkTask(void *argument){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER &= ~(3 << 10);
	GPIOA->MODER |= (1 << 10);
	while(1){
		osDelay(100);
	}
}

void ButtonTask(void *argument){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	GPIOC->MODER &= ~(3 << 26);
	GPIOC->PUPDR &= ~(3U << (13 * 2));
	GPIOC->PUPDR |=  (1U << (13 * 2));

	Msg_t msg;



	uint8_t lastState = 1;

	for (;;){
		    uint8_t state = (GPIOC->IDR & GPIO_IDR_ID13) ? 1 : 0;

		    if (state != lastState)
		    {
		        lastState = state;

		        if (state) {
		            snprintf(msg.text, sizeof(msg.text), "Released\r\n");
		        	osDelay(20);
		        }
		        else {
		            snprintf(msg.text, sizeof(msg.text), "Pressed\r\n");

		        osMessageQueuePut(uartQueue, &msg, 0, 0);
		        }
		    }
		}

}

void uartHandler(void *argument){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	GPIOA->MODER &= ~( (3 << 2*2) | (3 << 2*3));
	GPIOA->MODER |= (2 << 2*2) | (2 << 2*3);
	GPIOA->AFR[0] &= ~((0xF << (4*2)) | (0xF << (4*3)));
	GPIOA->AFR[0] |=  ((7 << (4*2)) | (7 << (4*3)));
	USART2->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
	USART2->BRR = 139;

	Msg_t msg;
	for (;;){
		if (osMessageQueueGet(uartQueue, &msg, NULL, osWaitForever) == osOK) {
					sendString(msg.text);
		}
	}
}

void MX_FREERTOS_Init(){
	uartQueue = osMessageQueueNew(10,sizeof(Msg_t),NULL);
	ledTaskHanlder = osThreadNew(BlinkTask,NULL,&BlinkTask_attr);
	buttonTaskHandler = osThreadNew(ButtonTask,NULL,&ButtonTask_attr);
	logTaskHandler = osThreadNew(uartHandler,NULL,&logTaskHandler_attr );
}



