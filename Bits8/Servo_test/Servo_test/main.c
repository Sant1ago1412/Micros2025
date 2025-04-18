/* Includes ------------------------------------------------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "Servos.h"
/* END Includes --------------------------------------------------------------*/


/* typedef -------------------------------------------------------------------*/
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
/* END typedef ---------------------------------------------------------------*/


/* define --------------------------------------------------------------------*/
#define IS10MS				flag0.bits.bit0
/*------PIN DECLARATION------*/
#define LED_HB				PB5
#define SERVO0				PD5
#define SERVO1				PD6
/* END define ----------------------------------------------------------------*/


/* Function prototypes -------------------------------------------------------*/

void initialize_ports();

void initialize_timer0();
void initialize_timer1();

void every10ms();

/* END Function prototypes ---------------------------------------------------*/


/* Global variables ----------------------------------------------------------*/
flags flag0;

uint8_t	count40ms = 4;

/* END Global variables ------------------------------------------------------*/


/* Constant in Flash ---------------------------------------------------------*/

/* END Constant in Flash -----------------------------------------------------*/


/* Function ISR --------------------------------------------------------------*/
ISR(TIMER1_COMPA_vect){
	OCR1A += 2000;
	count10ms--;
	if(!count10ms){
		IS10MS = 1;							// Flag that indicates that 10 ms have passed
		count10ms = 10;
	}
}

ISR(TIMER0_OVF_vect){
	writeServo();
}
/* END Function ISR ----------------------------------------------------------*/


/* Function prototypes user code ----------------------------------------------*/
void initialize_ports(){
	DDRB = (1 << LED_HB);		
	
	DDRD = (1 << SERVO0)|(1<<SERVO1);
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
		PORTB ^= (1 << LED_HB);
		count40ms = 4;
	}

	count40ms--;
	
	IS10MS = 0;
}

/* END Function prototypes user code ------------------------------------------*/

int main(){
	cli();
	/* Local variables -----------------------------------------------------------*/
	
	/* END Local variables -------------------------------------------------------*/

	/* User code Init ------------------------------------------------------------*/
	flag0.byte = 0;
	initialize_ports();
	initialize_timer0();
	initialize_timer1();
	
	addServo(&PORTD,SERVO0);
	addServo(&PORTD,SERVO1);

	/* END User code Init --------------------------------------------------------*/
	sei();

	while (1){
		/* User Code loop ------------------------------------------------------------*/
		if(IS10MS)
			every10ms();
		/* END User Code loop --------------------------------------------------------*/
	}
	return 0;
}