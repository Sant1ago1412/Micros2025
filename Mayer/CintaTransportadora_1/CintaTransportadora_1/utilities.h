#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <stdio.h>

#define TRUE				1
#define FALSE				0

typedef union{
	struct{
		uint8_t bit0: 1;
		uint8_t bit1: 1;
		uint8_t bit2: 1;
		uint8_t bit3: 1;
		uint8_t bit4: 1;
		uint8_t bit5: 1;
		uint8_t bit6: 1;
		uint8_t bit7: 1;
	}bits;
	struct{
		uint8_t nibbleL: 4;
		uint8_t nibbleH: 4;
	}nibbles;
	uint8_t byte;
}flags;

typedef union{
	int32_t i32;
	int16_t i16[2];
	int8_t i8[4];
	uint32_t ui32;
	uint16_t ui16[2];
	uint8_t ui8[4];
}u_dat;

uint8_t iterator; 
uint8_t aux; 
uint8_t timeAux;
#endif