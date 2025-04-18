/*
 * GccApplication1.c
 *
 * Created: 20/3/2025 13:01:09
 * Author : Baccon
 */ 

/* Includes ------------------------------------------------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
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

/**
 * @brief Estructura de datos para el puerto serie
 * 
 */
typedef struct{
    //uint8_t timeOut;         //!< TiemOut para reiniciar la máquina si se interrumpe la comunicación
    //uint8_t indexStart;      //!< Indice para saber en que parte del buffer circular arranca el ID
    //uint8_t cheksumRx;       //!< Cheksumm RX
    //uint8_t indexWriteRx;    //!< Indice de escritura del buffer circular de recepción
    //uint8_t indexReadRx;     //!< Indice de lectura del buffer circular de recepción
    uint8_t indexWriteTx;    //!< Indice de escritura del buffer circular de transmisión
    uint8_t indexReadTx;     //!< Indice de lectura del buffer circular de transmisión
    //uint8_t bufferRx[RINGBUFFLENGTH];   //!< Buffer circular de recepción
    uint8_t bufferTx[32];   //!< Buffer circular de transmisión
}_sDato;

typedef union{
	int32_t i32;
	uint32_t ui32;
	uint16_t ui16[2];
	uint8_t ui8[4];
	int8_t i8[4];
}u_dat;
/* END typedef ---------------------------------------------------------------*/


/* define --------------------------------------------------------------------*/
#define TRUE		1
#define FALSE		0
#define IS10MS		flag0.bits.bit0
#define ECHOREADY   flag0.bits.bit1

#define SW0			PB4
#define ECHO		PB0
#define TRIGGER		PB1
/* END define ----------------------------------------------------------------*/


/* Function prototypes -------------------------------------------------------*/
void initialize_timer1();
void task_100ms();
void task_HCSR();
void initialize_USART( unsigned int ubrr);
void task_USART();
/* END Function prototypes ---------------------------------------------------*/


/* Global variables ----------------------------------------------------------*/
flags flag0;
uint8_t counter = 10;
struct{
	//uint8_t state;	
	volatile enum estados{
		TRIGGER_SET_ON,
		TRIGGER_SET_OFF,
		WAITING_ECHO_RISE,
		WAITING_ECHO_FALL,
		CALCLE,
		WAITING
	}state;
	struct{
		volatile uint16_t start;
		volatile uint16_t stop;
		uint16_t distance;
	}timer;
}HCSR;
_sDato datosComSerie;
u_dat coder;
/* END Global variables ------------------------------------------------------*/


/* Constant in Flash ---------------------------------------------------------*/

/* END Constant in Flash -----------------------------------------------------*/


/* Function ISR --------------------------------------------------------------*/

ISR(TIMER1_COMPB_vect){
	//do something c/ 10ms
	IS10MS = TRUE; 
	OCR1B += 20000;
}

ISR(TIMER1_CAPT_vect){
	if(TCCR1B & _BV(ICES1)){ //RISING 3: cuando recibo el echo empiezo a tomar el tiempo y paso a esperar fall
		HCSR.timer.start = ICR1; //tomo el tiempo
		TCCR1B &= ~(1<< ICES1); //paso a esperar el fall
		HCSR.state = WAITING_ECHO_FALL;
	}else{ //falling
		if(HCSR.state == TRIGGER_SET_OFF){ //2: pasan 10us y apago el trigger, paso a esperar el rise del echo
			PORTB &= ~(1 << TRIGGER);
			TIMSK1 &= ~(1<<OCIE1A); //desactivo el ocr1a
			//TIMSK1 |= (1 << ICIE1);
			TCCR1B |= (1<<ICES1);//paso a esperar el rise
			datosComSerie.bufferTx[datosComSerie.indexWriteTx++]='2';
			HCSR.state = WAITING_ECHO_RISE;
		}
		if(HCSR.state == WAITING_ECHO_FALL){//4: cuando llega el fall tomo el otro tiempo y desactivo el hcsr
			HCSR.timer.stop = ICR1; //tomo el tiempo 
			//ECHOREADY = TRUE; 
			//TIMSK1 = (1 << OCIE1B);  //nada, no? 
			HCSR.state = CALCLE;//todo listo para calcular
		}
	}
}
/* END Function ISR ----------------------------------------------------------*/


/* Function prototypes user code ----------------------------------------------*/
void initialize_ports(){
	DDRB = (0 << SW0) | (0 << ECHO) | (1 << TRIGGER);
	PORTB = (1 << SW0) | (0 << ECHO);
}

void initialize_timer1(){ //CONFIGURAR PARA QUE CUENTE CADA 500ns
	TCCR1A = 0x00;
	TCCR1B = 0xC2; //0xC2
	TIFR1 = TIFR1;
	TCNT1 = 0; 
	TIMSK1 = (1 << OCIE1B) | (1 << ICIE1);
	OCR1B = 19999;
}

void initialize_HCSR(){
	HCSR.state = TRIGGER_SET_ON;
	//TIMSK1 &= ~(1 << ICIE1);
	HCSR.timer.distance = 0;
	//PORTB |= (1 << TRIGGER);
}

void initialize_USART( unsigned int ubrr)
{
	/*Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;

	UCSR0A = 0xFE; //inicializo todas las banderas excepto el multiprocesor
	/*Enable receiver and transmitter */
	UCSR0B = (1<<RXCIE0) | (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C =  (0<<UCSZ02) | (1<<UCSZ01) | (1<<UCSZ00);
}

void task_100ms(){
	//do something 100ms
	
	
	//resetear el HCSR
	HCSR.state = TRIGGER_SET_ON;
	//TIMSK1 &= ~(1 << ICIE1);
}

void task_HCSR(){
	switch(HCSR.state){
		case TRIGGER_SET_ON: //1: seteo el trigger en 1 y configuro la interrupción para  el falling dentro de 10us
			//habilito el trigger y activo la interrupción en 10us
			PORTB |= (1 << TRIGGER);
			OCR1A = TCNT1 + 18;  // Es 20 pero pongo 18 para compensar los ciclos perdidos
			TIMSK1 |= (1<<OCIE1A) | (1<<OCIE1B); 
			TCCR1B &= ~(1<< ICES1);
			HCSR.state = TRIGGER_SET_OFF;
			datosComSerie.bufferTx[datosComSerie.indexWriteTx++]='0';
		break;
		case TRIGGER_SET_OFF:
			datosComSerie.bufferTx[datosComSerie.indexWriteTx++]='1';
		break;
		case WAITING_ECHO_RISE:
			//datosComSerie.bufferTx[datosComSerie.indexWriteTx++]='2';
		break;
		case WAITING_ECHO_FALL:
			datosComSerie.bufferTx[datosComSerie.indexWriteTx++]='3';
		break;
		case CALCLE:
			if(HCSR.timer.start <= HCSR.timer.stop){
				HCSR.timer.distance = HCSR.timer.start - HCSR.timer.stop;
			}else{
				HCSR.timer.distance = (0xFFFF - HCSR.timer.start) + HCSR.timer.stop + 1;
			}
			if(HCSR.timer.distance > 11600) 
				HCSR.timer.distance = 0xFFFF;
			
			coder.ui16[0]=0x4142;
			
			datosComSerie.bufferTx[datosComSerie.indexWriteTx++]= coder.ui8[0];
			datosComSerie.bufferTx[datosComSerie.indexWriteTx++]= coder.ui8[1];
			datosComSerie.bufferTx[datosComSerie.indexWriteTx++]= 32;
			HCSR.state = WAITING;
		break;
		case WAITING:
		break;
		default:
		break;
	}
}

void task_USART(_sDato* datosCom){
	/*
	 if(datosCom->indexReadTx!=datosCom->indexWriteTx){
		 if(source){
			 if(pcCom.writeable()){
				 pcCom.putc(datosCom->bufferTx[datosCom->indexReadTx++]);
			 }
		 }

	for(uint8_t a=0; a<indiceAux; a++){
		cheksum ^= auxBuffTx[a];
		datosCom->bufferTx[datosCom->indexWriteTx++]=auxBuffTx[a];
	}
	*/
	if (datosCom->indexReadTx != datosCom->indexWriteTx) {
		if (UCSR0A & (1 << UDRE0)) { // Si el buffer de transmisión está vacío
			UDR0 = datosCom->bufferTx[datosCom->indexReadTx++]; // Enviar el dato
		}
	}
	datosCom->indexWriteTx &= 0xF1; //31
	datosCom->indexReadTx &= 0xF1; //31
	
}

void USART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	/* Put data into buffer, sends the data */
	UDR0 = data;
}
/* END Function prototypes user code ------------------------------------------*/

int main()
{
	cli();
	/* Local variables -----------------------------------------------------------*/

	/* END Local variables -------------------------------------------------------*/


	/* User code Init ------------------------------------------------------------*/
	flag0.byte = 0; 
	initialize_ports();
	initialize_timer1();
	initialize_HCSR();
	initialize_USART(16); // 16 PARA atmega328 para 115200

	/* END User code Init --------------------------------------------------------*/

	sei();
	while (1)
	{
		/* User Code loop ------------------------------------------------------------*/
		task_USART(&datosComSerie);
		task_HCSR();
		if (IS10MS){
			IS10MS = FALSE;
			counter--;
			
			//USART_Transmit(0x41);

			if(!counter){
				counter = 10; 
				task_100ms();

				//datosComSerie.bufferTx[datosComSerie.indexWriteTx++]=0x41;
			}
		}
		
		/* END User Code loop --------------------------------------------------------*/
	}

	return 0;
}

