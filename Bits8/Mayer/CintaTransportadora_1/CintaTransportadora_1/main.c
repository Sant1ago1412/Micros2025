/* Includes ------------------------------------------------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "utilities.h"
#include "communication_atmega328.h"
#include "HCSRforATMEGA328.h"
#include "servo.h"
#include "box.h"
#include "debouncer.h"
#include "delay.h"
/* END Includes --------------------------------------------------------------*/


/* typedef -------------------------------------------------------------------*/

/* END typedef ---------------------------------------------------------------*/


/* define --------------------------------------------------------------------*/
#define IS10MS				flag0.bits.bit0
/*------PIN DECLARATION------*/
#define LED_HB				PB5
#define SW0					PB4
#define ECHO				PB0
#define TRIGGER				PB1
#define SERVO2				PD7
#define SERVO1				PD6
#define SERVO0				PD5
#define PIR0				PB4
#define PIR1				PD2
#define PIR2				PD3
#define PIR3				PD4
/* END define ----------------------------------------------------------------*/


/* Function prototypes -------------------------------------------------------*/
/**
*@brief Definición de entradas y salidas
*
 */
void initialize_ports();

/**
*@brief Inicialización de timer 1 configurado en un ciclo de 500ns y dos interrupciones por comparación
*		la primera a 1ms (COMPA): Se utiliza como timer general de 16bits y contador para 10ms generales
*		y la otra (COMPB): en función de lo que precise el HCSR04 			
*		Tambien se utiliza la interrupción por input en el PIN PB0
* 		*Se puede pensar mejor el COMPA*
 */
void initialize_timer1();

/**
*@brief Inicialización del timer 0 unicamente en la funcion de overflow a 128us para la funcion de los servomotores
*
 */
void initialize_timer0();

/**
*@brief Funcion para la inicialización de los actuadores (combinación de IR con Servomotor)
*
 */
void initialize_Actuators();

/**
*@brief Función llamada cada 10ms determinados por la bandera "IS10MS"
*
 */
void every10ms();

/**
*@brief Funcion utilizada para escribir sobre el pin definido como trigger para el HCSR
*
*@param value: valor en el que se debe escribir el pin: 1 encendido; 0 apagado.
 */
void WritePin_HCSR(uint8_t value);

/**
*@brief Funcion utilizada para leer el buffer de recepcion del UART, realiza una acción según el comando recibido
*
*@param datosCom: Puntero a la estructura de datos para la comunicación. En este caso utilizamos unicamente el puerto serie
 */
void decodeData(_sDato *datosCom);

/**
*@brief Funcion llamada cada vez que el HCSR termina de realizar una medición, es llamada directamente desde la librería
*
*@param distance: recibe como parametro la ultima distancia.
 */
void newMeasure(uint16_t distance);

/**
*@brief Funcion llamada cada vez que se detecta un flanco en un IR, llamada directamente desde la librería
*
*@param state: recibe como parametro el estando en el que se encuentra el sensor (para definir el flanco)
 */
void IR0stateChanged(e_Estados state);
void IR1stateChanged(e_Estados state);
void IR2stateChanged(e_Estados state);
void IR3stateChanged(e_Estados state);
/* END Function prototypes ---------------------------------------------------*/


/* Global variables ----------------------------------------------------------*/
flags				flag0;
volatile uint8_t	count10ms=		10;

uint8_t		count60ms		= 6;
uint8_t		count40ms		= 4;
u_dat		coder;

_sDato		datosComSerie;
uint8_t 	auxBuffTx[50];
uint8_t 	indiceAux			=0;
uint8_t 	cheksum;

/**
*@brief Combinacion de infrarrojo con un servomotor.
*
 */
s_Actuators		ACT[3];
uint8_t 		IR_IN;
/* END Global variables ------------------------------------------------------*/


/* Constant in Flash ---------------------------------------------------------*/

/* END Constant in Flash -----------------------------------------------------*/


/* Function ISR --------------------------------------------------------------*/
ISR(USART_RX_vect){
	datosComSerie.bufferRx[datosComSerie.indexWriteRx++] = UDR0;				
}

ISR(TIMER1_COMPA_vect){
	OCR1A += 2000;									
	count10ms--;
	if(!count10ms){
		IS10MS = TRUE;							// Flag that indicates that 10 ms have passed
		count10ms = 10;
	}
	task_delay();
}

ISR(TIMER1_COMPB_vect){
	on_timer1_compb_hcsr();
}

ISR(TIMER1_CAPT_vect){
	on_timer1_capt_hcsr();
}

ISR(TIMER0_OVF_vect){
	servo_interrupt();
}
/* END Function ISR ----------------------------------------------------------*/


/* Function prototypes user code ----------------------------------------------*/
void initialize_ports(){
	DDRB = (1 << TRIGGER) | (1 << LED_HB);								    //salidas B
	DDRB &= ~( (1 << SW0) | (1 << ECHO) | (1 << PIR0));				//entradas B
	PORTB = (1 << SW0) | (1 << ECHO) | (1 << PIR0);
	
	DDRD = (1 << SERVO0) | (1 << SERVO1) | (1 << SERVO2);
	DDRD &= ~((1 << PIR1) | (1 << PIR2) | (1 << PIR3));
	PORTD |= ((1 << PIR1) | (1 << PIR2) | (1 << PIR3));
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
}

void initialize_Actuators(){
	ACT[0].servo = servo_add(&PORTD, SERVO0);
	ACT[1].servo = servo_add(&PORTD, SERVO1);
	ACT[2].servo = servo_add(&PORTD, SERVO2);
	
	IR_IN = add_to_debounce(&PINB, PIR0, &IR0stateChanged);
	ACT[0].IR = add_to_debounce(&PIND, PIR1, &IR1stateChanged);
	ACT[1].IR = add_to_debounce(&PIND, PIR2, &IR2stateChanged);
	ACT[2].IR = add_to_debounce(&PIND, PIR3, &IR3stateChanged);
	
	ACT[0].selectedType=TYPE_A;
	ACT[0].dropIt=FALSE;
	ACT[0].readIndex=0;
	ACT[0].nigger = 100;
	ACT[1].selectedType=TYPE_B;
	ACT[1].dropIt=FALSE;
	ACT[1].readIndex=0;
	ACT[1].nigger = 100;
	ACT[2].selectedType=TYPE_C;	
	ACT[2].dropIt=FALSE;
	ACT[2].readIndex=0;
	ACT[2].nigger = 100;
	
}

void sendDistance(uint16_t distancia){
	datosComSerie.bufferRx[datosComSerie.indexWriteRx+POSID]=DEBUGER;
	datosComSerie.indexStart=datosComSerie.indexWriteRx;
	decodeData(&datosComSerie);
}

void decodeData(_sDato *datosCom){
	#define RX datosCom->bufferRx
	#define INDEX datosCom->indexStart+POSID
	indiceAux=0;
	auxBuffTx[indiceAux++]='U'; //start
	auxBuffTx[indiceAux++]='N'; //header_1
	auxBuffTx[indiceAux++]='E'; //header_2
	auxBuffTx[indiceAux++]='R'; //header_3
	auxBuffTx[indiceAux++]=0; //this indexStart
	auxBuffTx[indiceAux++]=':'; //token

	switch (RX[INDEX]){
		case GETALIVE:
			auxBuffTx[indiceAux++]=GETALIVE;
			auxBuffTx[indiceAux++]=ACK;
			auxBuffTx[NBYTES] = 0x03;
		break;
		case FIRMWARE:
		// 			auxBuffTx[indiceAux++]=FIRMWARE;
		// 			for(uint8_t i=0; i<strFirm; i++){
		// 				auxBuffTx[indiceAux++]=FIRMVERSION[i];
		// 			}
		// 			auxBuffTx[NBYTES]=0x10;
		break;
		case DISTANCE:
			auxBuffTx[indiceAux++]=DISTANCE;
			coder.ui16[0] = HCSR04_Read(HCSR_1);
			auxBuffTx[indiceAux++]= coder.ui8[0];
			auxBuffTx[indiceAux++]= coder.ui8[1];
			auxBuffTx[NBYTES] = 0x04;
		break;
		case IR:
			auxBuffTx[indiceAux++]=IR;
			coder.ui16[0] = getState(IR_IN);
			auxBuffTx[indiceAux++] = coder.ui8[0];
			auxBuffTx[indiceAux++] = coder.ui8[1];
			coder.ui16[0] = getState(ACT[0].IR);
			auxBuffTx[indiceAux++] = coder.ui8[0];
			auxBuffTx[indiceAux++] = coder.ui8[1];
			coder.ui16[0] = getState(ACT[1].IR);
			auxBuffTx[indiceAux++] = coder.ui8[0];
			auxBuffTx[indiceAux++] = coder.ui8[1];
			auxBuffTx[NBYTES] = 0x08;
		break;
		case DEBUGER:
			auxBuffTx[indiceAux++]=DEBUGER;
			auxBuffTx[indiceAux++] = ACT[0].dropIt;
			auxBuffTx[indiceAux++] = ACT[1].dropIt;
			auxBuffTx[indiceAux++] = ACT[2].dropIt;
			auxBuffTx[NBYTES] = 0x05;
		break;
		case ADDBOX:
			auxBuffTx[indiceAux++]=ADDBOX;
			auxBuffTx[indiceAux++] = getBoxType(indicepepe++);
			auxBuffTx[NBYTES] = 0x03;
		break;
		default:
		auxBuffTx[indiceAux++]=0xDD;
		auxBuffTx[indiceAux++]=RX[INDEX];
		auxBuffTx[NBYTES] = 0x03;
		break;
	}
	cheksum=0;
	//auxBuffTx[NBYTES]+=1;
	for(iterator=0; iterator<indiceAux; iterator++){
		cheksum ^= auxBuffTx[iterator];
		datosCom->bufferTx[datosCom->indexWriteTx++]=auxBuffTx[iterator];
	}
	datosCom->bufferTx[datosCom->indexWriteTx++]=cheksum;
}

void task_actuator(s_Actuators *actuador, e_Estados state){
	if(state == RISING){
		while(actuador->readIndex != indexBox && getBoxType(actuador->readIndex) == NO_TYPE){
			actuador->readIndex++;
			actuador->readIndex &= 0x0F;
		}
		if(getBoxType(actuador->readIndex) == actuador->selectedType){
			actuador->dropIt = TRUE;
		}else if(actuador->readIndex != indexBox){
			actuador->readIndex++;
		}
		servo_setAngle(actuador->servo, 180);
	}else{
		if(actuador->dropIt == TRUE){
			setBoxType(actuador->readIndex++, NO_TYPE);
			servo_setAngle(actuador->servo, 0);
			actuador->dropIt = FALSE;
			//delayConfig(&actuador->servoHitTime, 1000, ONESHOT);
		}
	}
}

void IR0stateChanged(e_Estados state){

}
void IR1stateChanged(e_Estados state){
	task_actuator(&ACT[0], state);
}
void IR2stateChanged(e_Estados state){
	task_actuator(&ACT[1], state);
}
void IR3stateChanged(e_Estados state){
	task_actuator(&ACT[2], state);
}

void newMeasure(uint16_t distance){
	if(getState(IR_IN) == DOWN && IS_OLD_BOX == FALSE){		
		sConfigBox.HCSR.prom_iter++;
		
		if(distance < 1160){ //20cm * 58
			distance = 1160 - distance;
		}else{
			return;
		}
		
		sConfigBox.HCSR.auxHeight += distance;
		
		if(sConfigBox.HCSR.prom_iter >= sConfigBox.HCSR.prom_quant){
			sConfigBox.HCSR.prom_iter = 0;
			sConfigBox.HCSR.auxHeight /= sConfigBox.HCSR.prom_quant;
			
			addBox(sConfigBox.HCSR.auxHeight);
			sConfigBox.HCSR.auxHeight = 0;
			
			datosComSerie.bufferRx[datosComSerie.indexWriteRx+POSID]=ADDBOX;
			datosComSerie.indexStart=datosComSerie.indexWriteRx;
			decodeData(&datosComSerie);
			
			IS_OLD_BOX = TRUE;
		}
	}
	if(getState(IR_IN) == UP){
		IS_OLD_BOX = FALSE; 
	}
	
}

void every10ms(){
	if (!count60ms){								// If 100 ms have passed
		on_reset_hcsr();
		count60ms = 10;
	}
	count60ms--;
	
	if(!count40ms){
		task_debouncer();
		count40ms = 4;
	}
	count40ms--;
	IS10MS = FALSE;
}
/* END Function prototypes user code ------------------------------------------*/

int main(){
	cli();
	/* Local variables -----------------------------------------------------------*/
	
	/* END Local variables -------------------------------------------------------*/

	/* User code Init ------------------------------------------------------------*/
	flag0.byte = 0;
	initialize_ports();
	initialize_timer1();
	initialize_timer0();
	initialize_USART(&datosComSerie, 16, &decodeData); // 16 PARA atmega328 para 115200
	initialize_debounce();
	initialize_boxes();
	initialize_Actuators();
	HCSR_1 = HCSR04_AddNew(&WritePin_HCSR, 16);
	HCSR04_AttachOnReadyMeasure(HCSR_1, &newMeasure);
	
	/* END User code Init --------------------------------------------------------*/
	sei();

	/* ----------- Test servos ------------- */
	servo_setAngle(ACT[0].servo, 90);
	servo_setAngle(ACT[1].servo, 90);
	servo_setAngle(ACT[2].servo, 90);
	/* ---------- End Test servos ---------- */
	while (1){
		/* User Code loop ------------------------------------------------------------*/
		task_communication(&datosComSerie);
		task_HCSR();
		if(IS10MS)
			every10ms();
		/* END User Code loop --------------------------------------------------------*/
	}
	return 0;
}