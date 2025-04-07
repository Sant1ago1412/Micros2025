/*
 * HCSR.c
 *
 * Created: 19/3/2025 16:13:04
 * Author : Baccon
 */ 

/* Includes ------------------------------------------------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
/* END Includes --------------------------------------------------------------*/

//variables en interrupciones deben ser volatiles 
/* typedef -------------------------------------------------------------------*/
typedef union{ //union sirve para compartir bites entre 2 variables
	struct{
		uint8_t b0: 1;
		uint8_t b1: 1;
		uint8_t b2: 1;
		uint8_t b3: 1;
		uint8_t b4: 1;
		uint8_t b5: 1;
		uint8_t b6: 1;
		uint8_t b7: 1;
	}bit;
	uint8_t byte;
}_uflag;

typedef struct{
	uint8_t indexWriteTx;    //!< Indice de escritura del buffer circular de transmisión
	uint8_t indexReadTx;     //!< Indice de lectura del buffer circular de transmisión
	uint8_t bufferTx[32];   //!< Buffer circular de transmisión
}_sDato;

typedef enum {
	
	MEAS_IDLE,         // Sin medición en curso
	MEAS_TRIGGER,      // Disparando el pulso (TRIG en alto)
	MEAS_WAIT_FOR_ECHO,// Esperando el pulso de ECHO
	MEAS_DONE,          // Medición completada
	
} meas_state_t;

typedef struct{
	
    volatile  uint16_t startEcho;
	volatile uint16_t stopEcho;
	uint16_t distance;
	
}TimeHcsr;

typedef union{
	int32_t i32;
	uint32_t ui32;
	uint16_t ui16[2];
	uint8_t ui8[4];
	int8_t i8[4];
}u_dat;

/* END typedef ---------------------------------------------------------------*/


/* define --------------------------------------------------------------------*/
#define LASTSTATESW0	flag1.bit.b0
#define IS10MS			flag1.bit.b1
#define ISNEWSTATESWO	flag1.bit.b2
#define ECHOREADY		flag1.bit.b3

#define FALSE			0
#define TRUE			1
#define Led				PORTB5
//#define SW0				PRTB4
#define Led1			PORTB4
#define Led2			PORTB2
#define Led3			PORTB3
#define Echo			PORTB0
#define Trigger			PORTB1
/* END define ----------------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/

void ini_ports();
void ini_timer1();
void Measure();
void ini_UART();
void sendUart(_sDato* datosCom);

/* END Function prototypes ---------------------------------------------------*/


/* Global variables ----------------------------------------------------------*/
volatile meas_state_t meas_state = MEAS_IDLE;
TimeHcsr timehc;
_uflag flag1;
_sDato DatoSerie;
u_dat Converse;
/* END Global variables ------------------------------------------------------*/


/* Constant in Flash ---------------------------------------------------------*/

/* END Constant in Flash -----------------------------------------------------*/


/* Function ISR --------------------------------------------------------------*/
ISR(TIMER1_COMPB_vect){
	
	IS10MS=TRUE;
	OCR1B += 20000;
	
}

//entro aca cuando devuelve el echo 
ISR(TIMER1_COMPA_vect){
	meas_state = MEAS_WAIT_FOR_ECHO;
}

ISR(TIMER1_CAPT_vect){
	
	//ices1 es el input capture edge , me decta el falco desendente en caso de ser 0
	if(TCCR1B & _BV(ICES1)){
		timehc.startEcho = ICR1;
		TCCR1B &= ~_BV(ICES1);
		//rising
	}else{
		//falling
		timehc.stopEcho = ICR1;
		meas_state = MEAS_DONE;
	}
}
/* END Function ISR ----------------------------------------------------------*/


/* Function prototypes user code ----------------------------------------------*/
void ini_ports(){
	
	//DDRB = ((1 << Led) | (0 << SW0) | (0<<Echo) | (1<<Trigger));
	//OUTPUTS
	DDRB = ((1 << Led) | (1 << Led1) | (1 << Led2) | (1 << Led3) | (1 << Trigger));
	//INPUTS
	DDRB &= ~(1<<Echo);
	//PORTB = ((1 << SW0) | (1<<Trigger));
	PORTB = (1<<Echo);
}

void ini_timer1(){
	
	TCCR1A = 0x00;
	//configuro el Prescaler 8 (f = 16MHz / 8 = 2MHz ? 1 tick = 0.5 µs)
	TCCR1B = 0xC2;
	// CONFIGURACION DE LA CANCELACION DE RUIDO
	TCNT1 = 0x00;
	TIMSK1 = (1<<OCIE1B); 
	OCR1B = 19999;
	TIFR1 = TIFR1;
	
}

void ini_UART(uint16_t Config){
	UBRR0H = (unsigned char)(Config>>8);
	UBRR0L = (unsigned char)Config;
	
	UCSR0A = 0xFE; //inicializo todas las banderas excepto el multiprocesor
	/*Enable receiver and transmitter */
	UCSR0B = (1<<RXCIE0) | (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C =  (0<<UCSZ02) | (1<<UCSZ01) | (1<<UCSZ00);
}

void Measure(){
	
	switch(meas_state){
		
		case MEAS_IDLE:
			if(ECHOREADY){
				meas_state = MEAS_TRIGGER;
			}
		break;
		
		case MEAS_TRIGGER:
		//aca debo esperar los 10us y pasar a wait for echo
			if(ECHOREADY){
			
				PORTB |= (1<<Trigger);//lanzo el trigger
				OCR1A = TCNT1 + 19;//hago una interrupcion a 20 ciclos que son 10us
				ECHOREADY = FALSE;
				TIMSK1 |= (1<<OCIE1A);
				TCCR1B |= (1<<ICES1);
			
			}
		
		break;
		
		case MEAS_WAIT_FOR_ECHO:
			
			//apago el trigger y espero a que devuelva el echo en la interrupcion del input caputre
			if (PORTB & (1<<Trigger)){
			
				PORTB &= ~(1<<Trigger);
				TIMSK1 |= (1 << ICIE1);
				TIMSK1 &= ~(1<<OCIE1A);
				
			}
		break;
		
		case MEAS_DONE: //obtengo la distancia en cm
			if(timehc.startEcho <= timehc.stopEcho){
				timehc.distance = timehc.stopEcho - timehc.startEcho;
				}else{
				timehc.distance = timehc.stopEcho - timehc.startEcho + 0xffff;
			}
			
			timehc.distance = timehc.distance/116;
			//lo divido por el doble de lo que seria
			
			Converse.ui16[0] = timehc.distance;
			DatoSerie.bufferTx[DatoSerie.indexWriteTx++]= Converse.ui8[1];
			DatoSerie.bufferTx[DatoSerie.indexWriteTx++]= Converse.ui8[0];
		
			
			DatoSerie.bufferTx[DatoSerie.indexWriteTx++]= 32;
			meas_state=MEAS_IDLE;
		
		break;
		
	}
	return;
}
/* END Function prototypes user code ------------------------------------------*/

void sendUart(_sDato* datosCom){
	if (datosCom->indexReadTx != datosCom->indexWriteTx) {
		if (UCSR0A & (1 << UDRE0)) { // Si el buffer de transmisión está vacío
			UDR0 = datosCom->bufferTx[datosCom->indexReadTx++]; // Enviar el dato
		}
	}
	//datosCom->indexWriteTx &= 0xF1; //31
	//datosCom->indexReadTx &= 0xF1; //31
}
int main()
{
	cli();
	/* Local variables -----------------------------------------------------------*/
	static uint8_t Mytimer = 10;
	/* END Local variables -------------------------------------------------------*/

	/* User code Init ------------------------------------------------------------*/
	ini_ports();
	ini_timer1();
	ini_UART(16);
	ECHOREADY=TRUE;
	timehc.distance=0;
	/* END User code Init --------------------------------------------------------*/

	sei();
	
	while (1){
		/* User Code loop ------------------------------------------------------------*/
		Measure();
		sendUart(&DatoSerie);
		if(IS10MS){
			Mytimer--;
			IS10MS=0;
			
			if (!Mytimer){
				Mytimer=10;
				ECHOREADY=1;
				meas_state=MEAS_IDLE;
				PORTB ^= (1<<Led);
			}
			
		}
		
		/* END User Code loop --------------------------------------------------------*/
	}

	return 0;
}

