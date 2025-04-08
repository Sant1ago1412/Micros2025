/*
 * Cinta_Final.c
 *
 * Created: 07/04/2025 14:45:21
 * Author : Baccon
 */ 

/* Includes ------------------------------------------------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "Servos.h"
#include "HCSRforATMega328.h"
#include "HCSR04.h"
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

typedef struct{
	
	enum sizeBox{
		SmallBox,
		MediumBox,
		LargeBox
	}boxSize;
	enum State{
		isOn,
		Push,
		isOut,
	}boxState;
	
	uint16_t  Numbox;
	
}s_boxType;
/* END typedef ---------------------------------------------------------------*/


/* define --------------------------------------------------------------------*/
#define	TRUE			1
#define FALSE			0
#define IS10MS				flag0.bits.bit0

/*------PIN DECLARATION------*/
#define LED_BI			PB5

/************************************************************************/
/*							Pin Declaration Servos                      */
/************************************************************************/
#define SV0				PD7
#define SV1				PB4
#define SV2				PB3

/************************************************************************/
/*							Pin Declaration IR's                        */
/************************************************************************/
#define IR0				PD2
#define IR1				PD3
#define IR2				PD4
#define IR3				PD5

/************************************************************************/
/*							Pin Declaration HCSR                        */
/************************************************************************/
#define ECHO			PB0

/* END define ----------------------------------------------------------------*/


/* Function prototypes -------------------------------------------------------*/
void ini_ports();
void ini_timer1();
void ini_timer0();
void sensorMeasure();
void every10ms();
/* END Function prototypes ---------------------------------------------------*/


/* Global variables ----------------------------------------------------------*/
flags flag0;
uint8_t	count100ms	= 10;
/* END Global variables ------------------------------------------------------*/


/* Constant in Flash ---------------------------------------------------------*/

/* END Constant in Flash -----------------------------------------------------*/


/* Function ISR --------------------------------------------------------------*/
ISR(TIMER1_COMPA_vect){
	
	/*		Con el OCR1B en 20000 , cuento cada 10ms		*/
	IS10MS=TRUE;
	OCR1B += 20000;
	
}

ISR(TIMER1_COMPB_vect){
	on_timer1_compb_hcsr();
}

ISR(TIMER1_CAPT_vect){
	on_timer1_capt_hcsr();
}

ISR(TIMER0_OVF_vect){
	writeServo();
}
/* END Function ISR ----------------------------------------------------------*/


/* Function prototypes user code ----------------------------------------------*/
void ini_ports(){
	
	/************************************************************************/
	/*								OUTPUTS                                 */
	/************************************************************************/
	DDRB = ((1 << LED_BI)| (1 << SV1) | (1 << SV2) | (1<<TRIGGER));
	DDRD = (1 << SV0);
	
	/************************************************************************/
	/*								INPUTS                                  */
	/************************************************************************/
	DDRB &= ~(1<<ECHO);
	DDRD &= ~((1<<IR0) | (1<<IR1) | (1<<IR2) | (1<<IR3));
	
	/*						Activo Pull ups internos						*/
	PORTB = (1<<ECHO);
	PORTD = ((1<<IR0) | (1<<IR1) | (1<<IR2) | (1<<IR3));
}

/************************************************************************/
/*		Timer 1 es funcional al HCSR y a la accion cada 10ms            */
/************************************************************************/
void ini_timer1(){
	
	TCCR1A = 0x00;
	/* Configuro noise canceler del input capture, el flanco del input capture y prescaler en 8 (f = 16MHz / 8 = 2MHz ? 1 tick = 0.5 µs) */
	TCCR1B = 0xC2;
	TCNT1 = 0x00;
	/*	Activo la interrupcion por comparador b	*/
	TIMSK1 = (1<<OCIE1A);
	/* Le doy un valor al comparador B	*/
	OCR1B = 19999;
	TIFR1 = TIFR1;
	
}

/************************************************************************/
/*			Timer 0 funcional a generar el pwm de los servos			*/
/************************************************************************/
void ini_timer0(){
	
	TCCR0A = 0;
	TCNT0 = 0;
	/*	Pongo las banderas en 0 con TIFR	*/
	TIFR0 = 0x07;
	/*	Habilito la interrupcion por TOV	*/
	TIMSK0 = (1<<TOIE0);
	/*	Prescaler en 8 , obtengo el cuentas de 500ns, tov a 500*256 = 128us		*/
	TCCR0B = (1<<CS01);
	
}

void every10ms(){
	
	if (!count100ms){		//Si pasaron 100ms
		on_reset_hcsr();
		count100ms = 10;
	}
	count100ms--;
	
	IS10MS = FALSE;
}

void sensorMeasure(uint16_t distance){
	
	if(distance < 1176)			//20cm
	PORTB |= (1 << LED_BI);
	else
	PORTB &= ~(1 << LED_BI);
	
}

/* END Function prototypes user code ------------------------------------------*/

int main(){
	
	cli();
	
	/* Local variables -----------------------------------------------------------*/

	/* END Local variables -------------------------------------------------------*/




	/* User code Init ------------------------------------------------------------*/
	flag0.byte = 0;
	
	ini_ports();
	ini_timer1();
	ini_timer0();
	
	addServo(&PORTD,SV0);
	addServo(&PORTD,SV1);
	addServo(&PORTD,SV2);
	HCSR_1 = HCSR04_AddNew(WritePin_HCSR, 16);
	HCSR04_AttachOnReadyMeasure(HCSR_1, sensorMeasure);	
	
	/* END User code Init --------------------------------------------------------*/
	sei();

	while (1){
		/* User Code loop ------------------------------------------------------------*/
		task_HCSR();
		if (IS10MS)
			every10ms();
		/* END User Code loop --------------------------------------------------------*/
	}

	return 0;
}

