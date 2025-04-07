#include "debouncer.h"
#include <stdlib.h>
#include "utilities.h"

void inputState(s_Input *stateInput){
	switch(stateInput->state){
		case UP:
			if(stateInput->value == DOWN){
				stateInput->state = FALLING;
			}
			
		break;
		case DOWN:
			if(stateInput->value == UP){
				stateInput->state = RISING;
			}
			
		break;
		case RISING:
			if(stateInput->value == UP){
				stateInput->state = UP;
				/*------------------------------*/
				stateInput->val = UP;
				stateInput->stateChanged(RISING);
				/*------------------------------*/
			}else{
				stateInput->state = DOWN;
			}
		break;
		case FALLING:
			if(stateInput->value == DOWN){
				stateInput->state = DOWN;
				/*------------------------------*/
				stateInput->val = DOWN;
				stateInput->stateChanged(FALLING);
				/*------------------------------*/
			}else{
				stateInput->state = UP;
			}
		break;
		default:
		stateInput->state = UP;
	}
}

void initialize_debounce(){
	for(inputIndex=0; inputIndex<MAX_INPUTS_DEBOUNCED; inputIndex++){
		debouncerBuff[inputIndex].pin = NULL;
		debouncerBuff[inputIndex].pos = 0;
		debouncerBuff[inputIndex].stateChanged = NULL;
	}
	inputIndex = 0;
}

uint8_t add_to_debounce(volatile uint8_t* PIN, uint8_t  POS, void (*STATECHANGED)(e_Estados estado)){
	if(inputIndex >= MAX_INPUTS_DEBOUNCED)
		return 0;
	debouncerBuff[inputIndex].pin = PIN;
	debouncerBuff[inputIndex].pos = POS;
	debouncerBuff[inputIndex].stateChanged = STATECHANGED;
	return inputIndex++;
}

void task_debouncer(){
	for(iterator = 0; iterator < MAX_INPUTS_DEBOUNCED; iterator++){
		if(*debouncerBuff[iterator].pin & (1 << debouncerBuff[iterator].pos)){
			debouncerBuff[iterator].value = SETED;
		}else{
			debouncerBuff[iterator].value = CLEARED;
		}
		inputState(&debouncerBuff[iterator]);
	}
	
}
uint8_t getState(uint8_t input){
	return debouncerBuff[input].val;
}