#include "delay.h"
#include "utilities.h"

static uint16_t time16 = 0;
uint8_t timeReach = FALSE;

void task_delay(){
	time16++;
}

void delayConfig(s_delay *delay, uint16_t intervalo, uint8_t type){
	delay->interval = intervalo;
	delay->type	= type;
	delay->isRunnig = FALSE;
}

uint8_t delayRead(s_delay *delay){
	timeReach = FALSE;
	if((!delay->isRunnig) && (delay->type != STOP_ONESHOT)){
		delay->isRunnig = TRUE;
		delay->startTime = time16;
	}else{
		if((time16 - delay->startTime) >= delay->interval){
			timeReach = TRUE;
			if(delay->type == ONESHOT){
				delay->type = STOP_ONESHOT;
			}
			delay->isRunnig = FALSE;
		}
	}
	return timeReach;
}

void delayWrite(s_delay *delay, uint16_t intervalo, uint8_t type){
	delay->interval = intervalo;
	delay->isRunnig = FALSE;
	delay->type = type;
}