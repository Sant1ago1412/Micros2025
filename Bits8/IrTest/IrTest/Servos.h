/*
 * Servos.h
 *
 * Created: 06/04/2025 16:05:16
 *  Author: Baccon
 */ 


#ifndef SERVOS_H_
#define SERVOS_H_

#include <stdint.h>
#define ANGLE_TO_PWM_SLOPE			117			// Pendiente de conversión (78 unidades PWM por grado)
#define ANGLE_TO_PWM_OFFSET			10546		// Offset para 0°
#define ROUNDING_CORRECTION			5			// Corrección para redondeo (+5/10 = redondeo aritmético)

//agrego un servo 
void addServo(volatile uint8_t* PORT,uint8_t Pin);

void servo_Angle(uint8_t servo,uint8_t angulo);

void writeServo();


#endif /* SERVOS_H_ */