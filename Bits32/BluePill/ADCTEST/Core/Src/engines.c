/*
 * engines.c
 *
 *  Created on: May 21, 2025
 *      Author: Baccon
 */
#include "engines.h"
#include "UnerProtocol.h"

_sEng engine;

void en_Init(_sEng *engines,uint16_t maxSpeed){

//	engines->direction.pin1=
//	engines->direction.pin2=
}
uint16_t pwm_value(){

	int16_t pwm=0;
	pwm=ret_eng_Values();

	if(pwm>maxspeedValue){
		pwm=maxspeedValue;
	}
//	else
//	if{
//
//	}
	return pwm;
}
