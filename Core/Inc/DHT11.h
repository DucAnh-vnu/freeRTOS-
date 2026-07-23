/*
 * DHT11.h
 *
 *  Created on: Jul 22, 2026
 *      Author: Acer
 */

#ifndef INC_DHT11_H_
#define INC_DHT11_H_
#include <stdint.h>
typedef struct{
	uint8_t temp_int;
	uint8_t temp_dec;
	uint8_t hum_int;
	uint8_t hum_dec;
	uint8_t checksum;

} DHT11_Data;

void DHT11_Init();

uint8_t readDHT11(DHT11_Data *data);


#endif /* INC_DHT11_H_ */
