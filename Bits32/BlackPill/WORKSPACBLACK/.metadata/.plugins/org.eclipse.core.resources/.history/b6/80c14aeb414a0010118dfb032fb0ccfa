/*
 * ADC.c
 *
 *  Created on: Jun 15, 2025
 *      Author: santi
 */

#include "ADC.h"

void ADC_Filter(s_ADC *adcValues){

	static uint8_t index=0;
	static uint16_t Buffer[8][8];

	if(adcValues->newValue==1){
		adcValues->newValue=0;
		for(uint8_t channel = 0; channel < NUM_CHANNELS; channel++){
			adcValues->Sumatoria[channel] -= Buffer[index][channel];
			adcValues->Sumatoria[channel] += adcValues->raw_Data[channel];
			Buffer[index][channel] = adcValues->raw_Data[channel];
			adcValues->filteredData[channel] = (adcValues->Sumatoria[channel] / 8);
		}

		index++;
		if(index>7)
			index=0;
	}
}
