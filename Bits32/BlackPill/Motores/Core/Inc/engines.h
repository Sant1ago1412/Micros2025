/*
 * engines.h
 *
 *  Created on: May 21, 2025
 *      Author: Baccon
 */

#ifndef INC_ENGINES_H_
#define INC_ENGINES_H_

#include "stdint.h"

typedef enum{
	FRONT,
	BACK,
	FREE,
	BRAKE
}_eState;

typedef struct{
	uint16_t speed;
	_eState estado;
	uint16_t maxSpeed;
}_sEng;

void en_InitENG(_sEng *engines,uint16_t maxSpeed);
void en_HandlerENG(_sEng *engines,int32_t newspeed,uint8_t freno);

#endif /* INC_ENGINES_H_ */
