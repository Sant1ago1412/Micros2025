#ifndef SERVO_H_
#define	SERVO_H_

#include <stdint.h>

#define MAX_SERVOS 3

/**
 * @brief Configura el ángulo de un servo.
 * 
 * Esta función permite establecer un ángulo entre 0 y 180 grados para un servo específico.
 * 
 * @param servo Número del servo a configurar.
 * @param angle Ángulo deseado en grados (0 a 180).
 */
void servo_setAngle(uint8_t servo, uint8_t angle);

/**
 * @brief Agrega un servo al sistema.
 * 
 * Esta función registra un nuevo servo especificando el puerto y el pin al que está conectado.
 * 
 * @param PORT Puntero al registro del puerto donde está conectado el servo.
 * @param PIN Número del pin donde está conectado el servo.
 * @return uint8_t Identificador del servo agregado.
 */
uint8_t servo_add(volatile uint8_t* PORT, uint8_t PIN);

/**
 * @brief Maneja la interrupción de desbordamiento del temporizador para controlar los servos.
 * 
 * Configura un temporizador de 8 bits con un desbordamiento cada 128 microsegundos. 
 * Se requieren 156 ciclos (~20ms) para generar una señal de 50Hz, adecuada para controlar servos.
 * Esta función debe ser llamada dentro de la rutina de interrupción de desbordamiento del temporizador.
 */
void servo_interrupt();

#endif