#ifndef HCSRforATMEGA328_H_
#define HCSRforATMEGA328_H_

#include "HCSR04.h"

#define TRIGGER		PB1

uint32_t	HCSR_1;
uint16_t	aux16 = 0;

static inline void on_reset_hcsr(){
	aux16 = TCNT1;								// Loads actual time in TCNT1 into aux16
	aux16 += 20;								// 20*500ns = 10us (tiempo del trigger), Adds 10 (us) to the previously saved time at TCNT1
	OCR1B = aux16;								// Then loads the value into OCR1B, generating an Output Compare Interrupt
	TIFR1 = (1<<OCF1B) | (1<<OCF1A);			// Flag set after the counter value in TCNT1 equals OCR1A and OCR1B
	TIMSK1 = (1<<OCIE1B);			// Habilita interrupciones B 
	HCSR04_Start(HCSR_1);						// Trigger en HIGH
}

static inline void on_timer1_compb_hcsr(){
	HCSR04_TriggerReady(HCSR_1);					//Pongo en LOW el Trigger
	TIFR1 |= (1<<ICF1);								// Timer/Counter1 Output Compare A Match Flag enabled
	TCCR1B = (1 << ICNC1) | (1 << ICES1);			// Input Capture Noise Canceler and Input Capture Edge Select activated
	TCCR1B |= (1 << CS11);							// Prescaler definition (x8): CS12 = 0 and CS10 = 0
	TIMSK1 = (1<<ICIE1) | (1<<OCIE1A);				// Input Capture Interrupt and Output Compare A Match Interrupt enabled <---
}

static inline void on_timer1_capt_hcsr(){
	if (TCCR1B & (1<<ICES1)){						// Si ICES1 = 1  el Timer va a capturar en el flanco de subida (rising edge).
		
		TCCR1B = (1 << ICNC1) | (1 << CS11);		//preparo para capturar el flanco de bajada
		HCSR04_RiseEdgeTime(HCSR_1, ICR1 >> 1);		//[[ ICR1 >> 1  ==  ICR1 / 2 ]] -> En ICR1 almacena el valor de TCNT1, Es decir, guardo el momento en que llego el RISE. Como ICR1 obtiene un tick cada 500ns, si contó 3000 ticks, se hace la cuenta 3000 * 0,5us = 1500us. (500ns = 0,5us = 1/2).
		}else{										// Falling edge is used as trigger

		TIMSK1 &= ~_BV(ICIE1);						//desactivo la interrupcion por input capture
		HCSR04_FallEdgeTime(HCSR_1, ICR1 >> 1);		//guardo el momento en que llego el FALLING
	}
}

void WritePin_HCSR(uint8_t value){
	if (value)
	PORTB |= (1<<TRIGGER);				// Sets a HIGH state (1) in the TRIGGER pin
	else
	PORTB &= ~(1<<TRIGGER);				// Sets a LOW state (0) in the TRIGGER pin
}

#endif /* HCSRforATMEGA328_H_ */