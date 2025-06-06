/*
 * engines.h
 *
 *  Created on: May 21, 2025
 *      Author: Baccon
 */

#ifndef INC_ENGINES_H_
#define INC_ENGINES_H_

#include <stdint.h>

typedef struct{
	uint8_t pin1;
	uint8_t pin2;
}_sEpins;

typedef enum{
	FRONT,
	BACK,
	FREE,
	BRAKE
}_eState;

typedef struct{
	_sEpins direction;
	uint16_t speed;
	_eState estado;
	uint8_t nMotor;
}_sEng;

uint16_t pwm_value();

#endif /* INC_ENGINES_H_ */
