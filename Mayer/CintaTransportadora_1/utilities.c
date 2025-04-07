#include "utilities.h"

static volatile uint8_t is20ms = 156;
static volatile uint8_t servo0Cycle = 8;
static volatile uint8_t servo1Cycle = 8;
static volatile uint8_t servo2Cycle = 8;

void servo_interrupt(){
	is20ms--;
	servo0Cycle--;
	servo1Cycle--;
	servo2Cycle--;
	
	if(!servo0Cycle){
		PORTD &= ~(1 << SERVO0);
	}
	if(!servo1Cycle){
		PORTD &= ~(1 << SERVO1);
	}
	if(!servo2Cycle){
		PORTD &= ~(1 << SERVO2);
	}
	if(!is20ms){
		is20ms = 156;
		servo0Cycle = 8;
		servo1Cycle = 11;
		servo2Cycle = 15;
		PORTD |= (1 << SERVO0) | (1 << SERVO2) | (1 << SERVO1);
	}
}