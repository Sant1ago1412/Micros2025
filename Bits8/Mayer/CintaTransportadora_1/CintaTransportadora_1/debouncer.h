#ifndef DEBOUNCER_H_
#define DEBOUNCER_H_

#include <stdio.h>

#define MAX_INPUTS_DEBOUNCED		4

#define FALLING_EDGE				0
#define RISING_EDGE					1

#define SETED						1
#define CLEARED						0

typedef enum{
	DOWN,
	UP,
	FALLING,
	RISING
}e_Estados;

typedef struct{
	uint8_t value;
	e_Estados state;
	uint8_t val;
	volatile uint8_t* pin;
	uint8_t  pos;
	void (*stateChanged)(e_Estados estado);
}s_Input;

s_Input debouncerBuff[MAX_INPUTS_DEBOUNCED];
uint8_t	inputIndex;

/**
 * @brief Inicializa el sistema de debounce.
 * 
 * Esta función configura las variables y ajustes necesarios
 * para que el mecanismo de debounce funcione correctamente.
 */
void initialize_debounce();

/**
 * @brief Ejecuta la tarea de debounce.
 * 
 * Esta función debe ser llamada periódicamente para procesar
 * la lógica de debounce y actualizar el estado de las entradas.
 */
void task_debouncer();

/**
 * @brief Agrega una nueva entrada al sistema de debounce.
 * 
 * @param PIN Puntero al registro de entrada (por ejemplo, PINx).
 * @param POS Posición del bit de la entrada en el registro.
 * @param STATECHANGED Función de callback que se llama cuando el estado de la entrada cambia.
 * @return uint8_t Devuelve un identificador para la entrada agregada o un código de error si falla.
 */
uint8_t add_to_debounce(volatile uint8_t* PIN, uint8_t POS, void (*STATECHANGED)(e_Estados estado));

/**
 * @brief Actualiza el estado de una entrada específica.
 * 
 * @param stateInput Puntero a la estructura de estado de la entrada que se va a actualizar.
 */
void inputState(s_Input *stateInput);

/**
 * @brief Obtiene el estado actual de una entrada específica.
 * 
 * @param input Identificador de la entrada a consultar.
 * @return uint8_t Devuelve el estado actual de la entrada (por ejemplo, HIGH o LOW).
 */
uint8_t getState(uint8_t input);


#endif //DEBOUNCER_H_