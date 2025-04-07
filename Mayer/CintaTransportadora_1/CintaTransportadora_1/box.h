#ifndef BOX_H_
#define BOX_H_

#include "utilities.h"
#include "delay.h"

#define MAX_BOX_BUFFER			16

#define IS_OLD_BOX				boxFlag.bits.bit0

#define COUNTINGFOR_FALL_IR0	s_Belt.flag.bits.bit0
#define COUNTINGFOR_RISE_IR1	s_Belt.flag.bits.bit1
#define IS_SPEED_MEASURING		s_Belt.flag.bits.bit2

typedef enum e_type{
	NO_TYPE = 0xFF,
	TYPE_A = 0x01,
	TYPE_B = 0x02,
	TYPE_C = 0x03,
	DISCARD = 0xDD
}eType;

typedef struct{
	uint8_t IR;
	uint8_t servo;
	eType selectedType;
	uint8_t readIndex;
	uint8_t dropIt;
	s_delay servoHitTime;
	uint8_t nigger;
}s_Actuators;




typedef struct sBox{
	eType type;
}s_Box;

struct{
	struct HCSR_data{
		uint8_t prom_quant;
		uint8_t prom_iter;
		uint16_t auxHeight; 
	}HCSR;
	struct{
		uint16_t hA;
		uint16_t hB;
		uint16_t hC;
		uint16_t width;
		uint8_t margin;
	}BOX;
}sConfigBox;


/*struct{
	uint16_t speed;
	flags flag;
}s_Belt;*/

flags boxFlag;
uint8_t indexBox;
uint8_t indicepepe;

/**
 * @brief Inicializa las estructuras necesarias para la gestión de las cajas.
 * 
 * Esta función debe ser llamada antes de realizar cualquier operación con cajas.
 */
void initialize_boxes();

/**
 * @brief Agrega una nueva caja con una altura específica.
 * 
 * @param altura Altura de la caja en unidades de medida.
 */
void addBox(uint16_t altura);

/**
 * @brief Obtiene el tipo de una caja específica.
 * 
 * @param ind Índice de la caja en la lista de cajas.
 * @return El tipo de la caja como un valor de tipo uint8_t.
 */
uint8_t getBoxType(uint8_t ind);

/**
 * @brief Establece el tipo de una caja específica.
 * 
 * @param ind Índice de la caja en la lista de cajas.
 * @param type Tipo de la caja a establecer (debe ser de tipo eType).
 */
void setBoxType(uint8_t ind, eType type);

#endif // BOX_H_