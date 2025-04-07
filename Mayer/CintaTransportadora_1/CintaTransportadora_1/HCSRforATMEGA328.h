#ifndef HCSRforATMEGA328_H_
#define HCSRforATMEGA328_H_

#include "HCSR04.h"

#define TRIGGER		PB1

uint32_t	HCSR_1;
uint16_t	aux16 = 0;

static inline void on_timer1_compb_hcsr(){
	HCSR04_TriggerReady(HCSR_1);
	TIFR1 |= (1<<ICF1);								// Timer/Counter1 Output Compare A Match Flag enabled
	TCCR1B = (1 << ICNC1) | (1 << ICES1);			// Input Capture Noise Canceler and Input Capture Edge Select activated
	TCCR1B |= (1 << CS11);							// Prescaler definition (x8): CS12 = 0 and CS10 = 0
	TIMSK1 = (1<<ICIE1) | (1<<OCIE1A);				// Input Capture Interrupt and Output Compare A Match Interrupt enabled
}

static inline void on_timer1_capt_hcsr(){
	if (TCCR1B & (1<<ICES1)){						// Rising edge will trigger the capture
	
		TCCR1B = (1 << ICNC1) | (1 << CS11);
		HCSR04_RiseEdgeTime(HCSR_1, ICR1 >> 1);
	}else{											// Falling edge is used as trigger

		TIMSK1 &= ~_BV(ICIE1);
		HCSR04_FallEdgeTime(HCSR_1, ICR1 >> 1);
	}
}

static inline void on_reset_hcsr(){
	aux16 = TCNT1;								// Loads actual time in TCNT1 into aux16
	aux16 += 15;								// Adds 15 (us) to the previously saved time at TCNT1
	OCR1B = aux16;								// Then loads the value into OCR1B, generating an Output Compare Interrupt
	TIFR1 = (1<<OCF1B) | (1<<OCF1A);			// Flag set after the counter value in TCNT1 equals OCR1A and OCR1B
	TIMSK1 = (1<<OCIE1B) | (1<<OCIE1A);			// Timer/Counter1 Output Compare A and B Match interrupts are enabled.
	HCSR04_Start(HCSR_1);						// Counter decrements
}

void WritePin_HCSR(uint8_t value){
	if (value)
		PORTB |= (1<<TRIGGER);				// Sets a HIGH state (1) in the TRIGGER pin
	else
		PORTB &= ~(1<<TRIGGER);				// Sets a LOW state (0) in the TRIGGER pin
}

#endif /* HCSRforATMEGA328_H_ */