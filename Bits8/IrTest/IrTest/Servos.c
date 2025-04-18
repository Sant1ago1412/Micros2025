/*
 * Servos.c
 *
 * Created: 06/04/2025 16:04:53
 *  Author: Baccon
 */ 
#include "Servos.h"

typedef struct sServo{
	volatile uint8_t* port;
	uint8_t pin;
	volatile uint8_t cycle;
	uint8_t dutyCycle;
	
}s_servo;

static s_servo servoHandle[3];
static uint8_t ServN = 0;
volatile static uint8_t is20ms=156;
 
void addServo(volatile uint8_t* PORT,uint8_t Pin){
	
	servoHandle[ServN].pin=Pin;
	servoHandle[ServN].port=PORT;
	ServN++;
}
void servo_Angle(uint8_t servo,uint8_t angulo){
	
	uint16_t riseValue;
	
	riseValue = (uint16_t)angulo * ANGLE_TO_PWM_SLOPE;
	riseValue += ANGLE_TO_PWM_OFFSET;
	riseValue /= 180;
	riseValue += ROUNDING_CORRECTION;
	riseValue /= 10;
		
	servoHandle[servo].dutyCycle = (uint8_t)riseValue;
}

void writeServo(){
	is20ms--;
	
	uint8_t counter=0;
	
	for(counter = 0; counter < ServN; counter++){
		servoHandle[counter].cycle--;
		if(!servoHandle[counter].cycle)
		*servoHandle[counter].port &= ~(1 << servoHandle[counter].pin);
	}
	if(!is20ms){
		for(counter = 0; counter < ServN; counter++){
			servoHandle[counter].cycle = servoHandle[counter].dutyCycle;
			*servoHandle[counter].port |= (1 << servoHandle[counter].pin);
		}
		is20ms = 156;
	}
}
