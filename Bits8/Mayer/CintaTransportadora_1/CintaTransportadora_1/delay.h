#ifndef DELAY_H_
#define DELAY_H_

#include <stdio.h>

#define LOOP			0	
#define ONESHOT			1
#define STOP_ONESHOT	2	

typedef struct{
	int16_t    startTime;   /*!< Almacena el tiempo leido del timer*/
	uint16_t    interval;   /*!< intervalo de comparación para saber si ya transcurrio el tiempo leido*/
	uint8_t     isRunnig;   /*!< Indica si el delay está activo o no*/
	uint8_t		type;
}s_delay;

void task_delay();

void delayConfig(s_delay *delay, uint16_t intervalo, uint8_t type);

uint8_t delayRead(s_delay *delay);

void delayWrite(s_delay *delay, uint16_t intervalo, uint8_t type);

#endif //DELAY_H_