/*
 * DHT11.c
 *
 *  Created on: Jul 23, 2026
 *      Author: Acer
 */

#include "dht11.h"
#include "stm32f4xx.h"
#include <stdint.h>


void DHT11_SetOutput(){

	GPIOC->MODER &= ~(3 << 6);

	GPIOC->MODER |= (1 << 6);
}


void DHT11_SetInput(){

	GPIOC->MODER &= ~(3 << 6);
}


void DHT11_Init(){

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

	DHT11_SetOutput();

	GPIOC->OTYPER &= ~(1 << 3);

	GPIOC->PUPDR &= ~(3 << (3 * 2));

	GPIOC->BSRR |= GPIO_BSRR_BS3;
}

void delay_ms(uint8_t time){

	TIM2->CNT = 0;

	while(TIM2->CNT < (time * 1000)){

	}
}

void delay_us(uint8_t time){

	TIM2->CNT = 0;

	while(TIM2->CNT < (time)){

	}
}

void init_Timer(){

	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	TIM2->PSC = 15;

	TIM2->ARR = 0xFFFFFFFF;

	TIM2->CNT = 0;

	TIM2->CR1 |= TIM_CR1_CEN;

}

void DHT11_Start(){

	DHT11_SetOutput();

    GPIOC->BSRR = GPIO_BSRR_BR3;

	delay_ms(18);

	GPIOC->BSRR = GPIO_BSRR_BS3;

	delay_us(30);

	DHT11_SetInput();
}

uint8_t wait_Response(){

	TIM2->CNT = 0;

	while(GPIOC->IDR & (1 << 3)){
		 if(TIM2->CNT > 100){
		        return 0;
		 }
	}

	TIM2->CNT = 0;

	while(!(GPIOC->IDR& (1 << 3))){
		 if(TIM2->CNT > 100){
		        return 0;
		 }
	}

	TIM2->CNT = 0;

	while(GPIOC->IDR & (1 << 3))
	{
	    if(TIM2->CNT > 100)
	        return 0;
	}

	return 1;
}

uint8_t read_bit_DHT11(){


	while(!(GPIOC->IDR & (1 << 3)));

	TIM2->CNT = 0;

	while(GPIOC->IDR & (1 << 3));

	if (TIM2->CNT > 50) return 1;

	else return 0;

}


uint8_t read_byte_DHT11(){
	uint8_t data = 0;
	for (int i = 0 ; i < 8 ; i++){
		data <<= 1;
		data |= read_bit_DHT11();
	}
	return data;
}

uint8_t readDHT11(DHT11_Data *data){

	uint8_t raw[5];

	DHT11_Start();

	if(!wait_Response()){
		return 0;
	}

	for (int i = 0 ; i < 5 ; i++){
		raw[i] = read_byte_DHT11();
	}

	if (raw[4] != (uint8_t)(raw[0] + raw[1] + raw[2] + raw[3])) return 0;

	data->hum_int = raw[0];

	data->hum_dec = raw[1];

	data->temp_int = raw[2];

	data->temp_dec = raw[3];

	data->checksum = raw[4];

	return 1;

}


