/*
 * IrTest.c
 *
 * Created: 06/04/2025 22:07:35
 * Author : Baccon
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "Servos.h"

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
	uint8_t byte;
}flags;

#define IS10MS				flag0.bits.bit0
#define SERVO0				PD5
#define SERVO1				PD6
#define IR1					PB2
#define LED_HB				PB5

void initialize_ports();

void initialize_timer0();
void initialize_timer1();

void every10ms();

flags flag0;
uint8_t	count40ms = 4;

ISR(TIMER1_COMPA_vect){
	OCR1A += 20000;
	IS10MS = 1;							// Flag that indicates that 10 ms have passed
}
ISR(TIMER0_OVF_vect){
	writeServo();
}

void initialize_ports(){
	DDRB = (0<<IR1)|(1 << LED_HB);
	DDRD = (1 << SERVO0)|(1<<SERVO1);
	PORTB = (1<<IR1);
}

void initialize_timer1(){
	TCCR1A = 0;
	OCR1A = 19999;
	TIFR1 = TIFR1;
	TIMSK1 = (1 << OCIE1A);
	TCCR1B = (1 << ICNC1) | (1 << CS11);
}

void initialize_timer0(){
	TCCR0A = 0;
	TCNT0 = 0;
	TIFR0 = 0x07;
	TIMSK0 = (1<<TOIE0);
	TCCR0B = (1<<CS01);
	//con el prescaler en 8, salta la tov cada 500ns
}

void every10ms(){
	if (!count40ms){								// If 1000 ms have passed
		count40ms = 4;

		if (PINB & (1<<IR1)){
			servo_Angle(0,90);
			PORTB ^= (1 << LED_HB);
		}else{
			servo_Angle(0,180);
			PORTB ^= (1 << LED_HB);
		}
	}
	count40ms--;
	IS10MS = 0;
}

int main(){
	cli();
   initialize_ports();
   initialize_timer0();
   initialize_timer1();
   	addServo(&PORTD,SERVO0);
   sei();
   
    while (1) {
		
		if(IS10MS)
			every10ms();
			
    }
	return 0;
}

