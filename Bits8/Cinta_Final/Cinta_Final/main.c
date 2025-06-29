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
#include "util.h"

#include <stdio.h>

int USART_putchar(char c, FILE *stream) {
	if (c == '\n') USART_putchar('\r', stream);  // salto de l�nea para terminal
	while (!(UCSR0A & (1 << UDRE0)));  // Espera a que el buffer est� vac�o
	UDR0 = c;
	return 0;
}

FILE usart_output = FDEV_SETUP_STREAM(USART_putchar, NULL, _FDEV_SETUP_WRITE);
/* END Includes --------------------------------------------------------------*/


/* typedef -------------------------------------------------------------------*/

/* END typedef ---------------------------------------------------------------*/


/* define --------------------------------------------------------------------*/

#define bufferBox		15
#define bufferIrn		4
#define Cm6				348
#define Cm8				464
#define Cm9				522
#define Cm11			638
#define Cm12			696
#define Cm13			754
#define Cm15			870
#define Cm18			1100

#define	TRUE			1
#define FALSE			0
#define IS10MS			flag0.bits.bit0
#define MEASURINGBOX	flag0.bits.bit1
#define Kick1			flag0.bits.bit2
#define Kick2			flag0.bits.bit3
#define Kick3			flag0.bits.bit4
#define needtoReset		flag0.bits.bit5

#define RXBUFSIZE           256
#define TXBUFSIZE           256

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
void ini_USART(uint8_t ubrr);

void sensorMeasure(uint16_t distance);
void every10ms();

void IR_Init(IRDebounce *ir);
void IR_Update(IRDebounce *ir, uint8_t sample);
uint8_t IR_GetState( IRDebounce *ir);

void serialTask(_sRx *dataRx, _sTx *dataTx);
void decodeCommand(_sRx *dataRx, _sTx *dataTx);
void decodeHeader(_sRx *dataRx);
uint8_t putHeaderOnTx(_sTx  *dataTx, _eCmd ID, uint8_t frameLength);
uint8_t putByteOnTx(_sTx *dataTx, uint8_t byte);

void newBox(uint16_t distance);
void addBox(uint16_t distance);
void kickBox();
void servoreset();
/* END Function prototypes ---------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

flags flag0;

_sRx		dataRx;
_sTx		dataTx;
_uWord		myWord;
	 
uint8_t raw_input[bufferIrn];
uint8_t state1;
uint8_t	count100ms	= 10;
uint8_t count40ms = 4;
uint8_t countservoReset = 200;
uint8_t boxToTx;
s_boxType Cajita[bufferBox];
uint8_t CajitaArr[3][50];
uint8_t Numbox=0;
IRDebounce ir_sensor[bufferIrn];

volatile uint8_t buffRx[RXBUFSIZE];
uint8_t buffTx[TXBUFSIZE];

uint16_t globalDistance=0;

s_sizeConfig boxSizeconfig;

uint8_t sIR1=0, sIR2=0, sIR3=0;
/* END Global variables ------------------------------------------------------*/


/* Constant in Flash ---------------------------------------------------------*/

/* END Constant in Flash -----------------------------------------------------*/


/* Function ISR --------------------------------------------------------------*/
ISR(TIMER1_COMPA_vect){
	
	/*		Con el OCR1B en 20000 , cuento cada 10ms		*/
	IS10MS=TRUE;
	OCR1A += 19999;
	
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

ISR(USART_RX_vect){
	dataRx.buff[dataRx.indexW++] = UDR0;
	dataRx.indexW &= dataRx.mask;
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
	/* Configuro noise canceler del input capture, el flanco del input capture y prescaler en 8 (f = 16MHz / 8 = 2MHz ? 1 tick = 0.5 �s) */
	TCCR1B = 0xC2;
	TCNT1 = 0x00;
	/*	Activo la interrupcion por comparador b	*/
	TIMSK1 = (1<<OCIE1A);
	/* Le doy un valor al comparador B	*/
	OCR1A = 19999;
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

void ini_USART(uint8_t ubrr){
	UBRR0H = 0;
	UBRR0L = ubrr;
	UCSR0A = 0xFE; //inicializo todas las banderas excepto el multiprocesor
	UCSR0B =   0x98; // (1<<RXCIE0) | (1<<RXEN0)|(1<<TXEN0); //Activo las banderas de interrupcion de recepcion y la habilitacion del rx y tx
	UCSR0C =  0x06; // (0<<UCSZ02) | (1<<UCSZ01) | (1<<UCSZ00); //se setea como asincrono, paridad desactivada, 1 stop bit, 8 data bits
	
}

void IR_Init(IRDebounce *ir) {
	for(int globalIndex = 0;globalIndex<bufferIrn;globalIndex++){
		ir[globalIndex].state = IR_DOWN;
		ir[globalIndex].last_sample = 0;
		ir[globalIndex].stateConfirmed = 0;
	}
}

void IR_Update(IRDebounce *ir, uint8_t sample) {
	switch (ir->state) {
		case IR_RISING:
		if (sample == 1 && ir->last_sample == 1){
			ir->state = IR_UP;
			ir->stateConfirmed = 0x01;
			}else{
			ir->state = IR_DOWN;
		}
		break;

		case IR_UP:
		if (sample == 0){
			ir->state = IR_FALLING;
			}else{
			ir->state = IR_UP;
		}
		break;

		case IR_FALLING:
		if (sample == 0 && ir->last_sample == 0){
			ir->state = IR_DOWN;
			ir->stateConfirmed = 0x00;
			}else{
			ir->state = IR_UP;
		}
		break;

		case IR_DOWN:
		if (sample == 1){
			ir->state = IR_RISING;
			}else{
			ir->state = IR_DOWN;
		}
		break;
		default:
		ir->state = IR_UP;
		break;
	}

	ir->last_sample = sample;
}

uint8_t IR_GetState( IRDebounce *ir) {
	return ir->stateConfirmed;
}

void serialTask(_sRx* dataRx, _sTx* dataTx){
	if(dataRx->isComannd){
		dataRx->isComannd=FALSE;
		decodeCommand(dataRx,dataTx);
	}
	if(dataRx->indexR!=dataRx->indexW){
		
		decodeHeader(dataRx);
	}
	if (dataTx->indexR!= dataTx->indexW) {
		if (UCSR0A & (1 << UDRE0)) { // Si el buffer de transmisi�n est� vac�o
			UDR0 = dataTx->buff[dataTx->indexR++]; // Enviar el dato
			dataTx->indexR &= dataTx->mask;
		}
	}
}

uint8_t putHeaderOnTx(_sTx  *dataTx, _eCmd ID, uint8_t frameLength){
	dataTx->chk = 0;
	dataTx->buff[dataTx->indexW++]='U';
	dataTx->indexW &= dataTx->mask;
	dataTx->buff[dataTx->indexW++]='N';
	dataTx->indexW &= dataTx->mask;
	dataTx->buff[dataTx->indexW++]='E';
	dataTx->indexW &= dataTx->mask;
	dataTx->buff[dataTx->indexW++]='R';
	dataTx->indexW &= dataTx->mask;
	dataTx->buff[dataTx->indexW++]=frameLength+1;
	dataTx->indexW &= dataTx->mask;
	dataTx->buff[dataTx->indexW++]=':';
	dataTx->indexW &= dataTx->mask;
	dataTx->buff[dataTx->indexW++]=ID;
	dataTx->indexW &= dataTx->mask;
	dataTx->chk ^= (frameLength+1);
	dataTx->chk ^= ('U' ^'N' ^'E' ^'R' ^ID ^':') ;
	return  dataTx->chk;
}
uint8_t putByteOnTx(_sTx *dataTx, uint8_t byte)
{
	dataTx->buff[dataTx->indexW++]=byte;
	dataTx->indexW &= dataTx->mask;
	dataTx->chk ^= byte;
	return dataTx->chk;
}
void decodeCommand(_sRx *dataRx, _sTx *dataTx){
	switch(dataRx->buff[dataRx->indexData]){
		
		case ALIVE:
		
			putHeaderOnTx(dataTx, ALIVE, 2);
			putByteOnTx(dataTx, ACK );
			putByteOnTx(dataTx, dataTx->chk);
		
		break;
		case FIRMWARE:
		break;

		case DATA:
			myWord.ui16[0]	= globalDistance;
			putHeaderOnTx(dataTx, DATA, 7);
			putByteOnTx(dataTx, myWord.ui8[0]);
			putByteOnTx(dataTx, myWord.ui8[1]);
			putByteOnTx(dataTx, boxToTx);
			myWord.ui16[0] = IR_GetState(&ir_sensor[0]);
			putByteOnTx(dataTx, myWord.ui8[0]);
			putByteOnTx(dataTx, myWord.ui8[1]);
			putByteOnTx(dataTx, Numbox);
			putByteOnTx(dataTx, dataTx->chk);
		break;
		
		//case NEWBOX:
		//myWord.ui8[0]=boxToTx;
		//putHeaderOnTx(dataTx, NEWBOX, 8);
		//putByteOnTx(dataTx, myWord.ui8[0]);
		//myWord.ui16[0]	= globalDistance;
		//putByteOnTx(dataTx, myWord.ui8[0]);
		//putByteOnTx(dataTx, myWord.ui8[1]);
		//myWord.ui16[0] = IR_GetState(&ir_sensor[0]);
		//putByteOnTx(dataTx, myWord.ui8[0]);
		//putByteOnTx(dataTx, myWord.ui8[1]);
		//myWord.ui16[0] = Numbox;
		//putByteOnTx(dataTx, myWord.ui8[0]);
		//putByteOnTx(dataTx, myWord.ui8[1]);
		//putByteOnTx(dataTx, dataTx->chk);
		//
		//break;

		default:
			putHeaderOnTx(dataTx, (_eCmd)dataRx->buff[dataRx->indexData], 2);
			putByteOnTx(dataTx,UNKNOWN );
			putByteOnTx(dataTx, dataTx->chk);
		break;
		
	}
}
void decodeHeader(_sRx *dataRx){
	uint8_t auxIndex=dataRx->indexW;
	while(dataRx->indexR != auxIndex){
		switch(dataRx->header)
		{
			case HEADER_U:
			if(dataRx->buff[dataRx->indexR] == 'U'){
				dataRx->header = HEADER_N;
			}
			break;
			case HEADER_N:
			if(dataRx->buff[dataRx->indexR] == 'N'){
				dataRx->header = HEADER_E;
				}else{
				if(dataRx->buff[dataRx->indexR] != 'U'){
					dataRx->header = HEADER_U;
					dataRx->indexR--;
				}
			}
			break;
			case HEADER_E:
			if(dataRx->buff[dataRx->indexR] == 'E'){
				dataRx->header = HEADER_R;
				}else{
				dataRx->header = HEADER_U;
				dataRx->indexR--;
			}
			break;
			case HEADER_R:
			if(dataRx->buff[dataRx->indexR] == 'R'){
				dataRx->header = NBYTES;
				}else{
				dataRx->header = HEADER_U;
				dataRx->indexR--;
			}
			break;
			case NBYTES:
			dataRx->nBytes=dataRx->buff[dataRx->indexR];
			dataRx->header = TOKEN;
			break;
			case TOKEN:
			if(dataRx->buff[dataRx->indexR] == ':'){
				dataRx->header = PAYLOAD;
				dataRx->indexData = dataRx->indexR+1;
				dataRx->indexData &= dataRx->mask;
				dataRx->chk = 0;
				dataRx->chk ^= ('U' ^'N' ^'E' ^'R' ^dataRx->nBytes ^':') ;
				}else{
				dataRx->header = HEADER_U;
				dataRx->indexR--;
			}
			break;
			case PAYLOAD:
			dataRx->nBytes--;
			if(dataRx->nBytes>0){
				dataRx->chk ^= dataRx->buff[dataRx->indexR];
				}else{
				dataRx->header = HEADER_U;
				if(dataRx->buff[dataRx->indexR] == dataRx->chk)
				dataRx->isComannd = TRUE;
			}
			break;
			default:
			dataRx->header = HEADER_U;
			break;
		}
		dataRx->indexR++;
		dataRx->indexR &= dataRx->mask;
	}
}

void every10ms(){
	
	if (!count100ms){		//Si pasaron 100ms
		on_reset_hcsr();
		count100ms = 10;
		PORTB ^= (1<<LED_BI);
	}
	
	if (!count40ms){
		for(int i=0;i<4;i++){
			IR_Update(&ir_sensor[i], raw_input[i]);
		}
		count40ms = 4;
		newBox(globalDistance);
		kickBox();
	}
	if (!countservoReset && needtoReset){
		servoreset();
	}
	raw_input[0] = (PIND & (1<<IR0)) ? 1 : 0;
	raw_input[1] = (PIND & (1<<IR1)) ? 1 : 0;
	raw_input[2] = (PIND & (1<<IR2)) ? 1 : 0;
	raw_input[3] = (PIND & (1<<IR3)) ? 1 : 0;
	
	IS10MS = FALSE;
	count100ms--;
	count40ms--;
	countservoReset--;
}

void sensorMeasure(uint16_t distance){
	
	globalDistance = distance;
	
}

void addBox(uint16_t distance){
	static uint8_t i = 1, j = 1, k = 1;
	
	if ((distance >= Cm13) && (distance < Cm15)) { //si la distancia se encuentra entre 15 y 13 cm es caja chica
		Cajita[Numbox].boxState=isOn;
		Cajita[Numbox].boxSize=SmallBox;
		boxToTx = 0x4;
		CajitaArr[0][i] = 1;
		i++;
			
		printf("Fila SmallBox: ");
		for (uint8_t n = 0; n < 20; n++) {
			printf("%u ", CajitaArr[0][n]);
		}
		printf("\n");
	}
	else if ((distance >= Cm11) && (distance < Cm13)){
		Cajita[Numbox].boxState=isOn;
		Cajita[Numbox].boxSize=MediumBox;
		boxToTx = 0x2;
		CajitaArr[1][j] = 1;
		CajitaArr[0][i] = 0;
		j++;
		i++;
			
		printf("Fila MediumBox: ");
		for (uint8_t n = 0; n < 20; n++) {
			printf("%u ", CajitaArr[1][n]);
		}
		printf("\n");
	}
	else if ((distance >= Cm9) && (distance < Cm11)){
		Cajita[Numbox].boxState=isOn;
		Cajita[Numbox].boxSize=LargeBox;
		boxToTx = 0x1;
		CajitaArr[0][i] = 0;
		CajitaArr[1][j] = 0;
		CajitaArr[2][k] = 1;
		j++;
		i++;
		k++;
					
		printf("Fila LargeBox: ");
		for (uint8_t n = 0; n < 20; n++) {
			printf("%u ", CajitaArr[2][n]);
		}
		printf("\n");
		
	}
	else if (((distance < Cm9) || (distance >= Cm15)) && distance>1 ){
		Cajita[Numbox].boxSize=NotSelected;
		CajitaArr[0][i] = 0;
		CajitaArr[1][j] = 0;
		CajitaArr[2][k] = 0;
		j++;
		i++;
		k++;
		Kick1 = 0;
		Kick2 = 0;
		Kick3 = 0;
		boxToTx = 0x0;
		printf("%i\n",distance);
		printf("NotSelected\n");
	}
	Numbox++;
	
	if(Numbox>=bufferBox) //reinicio el buffer
	Numbox=0;
}

void newBox(uint16_t distance){
	if(distance<Cm18){
		if((IR_GetState(&ir_sensor[0]) == 0) && !MEASURINGBOX){
			MEASURINGBOX=TRUE;
			addBox(distance);
			PORTB ^=(1<<LED_BI);
		}
		if ((IR_GetState(&ir_sensor[0]) == 1)) //si IR no mide
			MEASURINGBOX=FALSE;
	}
}

void kickBox(){
	

	if (ir_sensor[1].state== IR_FALLING){
		sIR1++;
		printf("sIR1:%u ", sIR1);
		if(CajitaArr[0][sIR1] == 1){
			printf("ENTRE: ");
			printf("\n");
			Kick1=1;
		}
	}
	
	if (ir_sensor[2].state== IR_FALLING){
		sIR2++;
		printf("sIR2:%u ", sIR2);
		if(CajitaArr[1][sIR2] == 1){
			printf("ENTRE: ");
			printf("\n");
			Kick2=1;
		}
	}
	
	if (ir_sensor[3].state== IR_FALLING){
		sIR3++;
		printf("sIR3:%u ", sIR3);
		if(CajitaArr[2][sIR3] == 1){
			printf("ENTRE: ");
			printf("\n");
			Kick3=1;
		}
	}
	if(sIR1>50)
		sIR1=0;
	if(sIR2>50)
		sIR1=0;
	if(sIR3>50)
		sIR1=0;
	if(ir_sensor[1].state== IR_RISING && Kick1){
		servo_Angle(0,0);
		Kick1=0;	
		needtoReset=1;
		countservoReset=50;
	}
		
	if(ir_sensor[2].state== IR_RISING && Kick2){
		servo_Angle(1,0);
		Kick2=0;
		needtoReset=1;
		countservoReset=50;
	}
	
	if(ir_sensor[3].state== IR_RISING && Kick3){
		servo_Angle(2,0);
		Kick3=0;
		needtoReset=1;
		countservoReset=50;
	}
		
}

void servoreset(){
	servo_Angle(0,120);
	servo_Angle(1,120);
	servo_Angle(2,120);
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
	ini_USART(16);
	
	IR_Init(&ir_sensor[0]);
	stdout = &usart_output;
	
	addServo(&PORTD,SV0);
	addServo(&PORTB,SV1);
	addServo(&PORTB,SV2);
	servo_Angle(0,90);
	servo_Angle(1,90);
	servo_Angle(2,90);
	HCSR_1 = HCSR04_AddNew(&WritePin_HCSR, 16);
	
	Numbox = 255;
	
	dataRx.buff = (uint8_t *)buffRx;
	dataRx.indexR = 0;
	dataRx.indexW = 0;
	dataRx.header = HEADER_U;
	dataRx.mask = RXBUFSIZE - 1;
	
	dataTx.buff = buffTx;
	dataTx.indexR = 0;
	dataTx.indexW = 0;
	dataTx.mask = TXBUFSIZE -1;
	
	boxSizeconfig.smallboxF=Cm15;
	boxSizeconfig.smallboxC=Cm13;
	boxSizeconfig.mediumboxF=Cm13;
	boxSizeconfig.mediumboxC=Cm11;
	boxSizeconfig.largeboxF=Cm11;
	boxSizeconfig.largeboxC=Cm9;
	
	ir_sensor[1].irType = SmallBox;
	ir_sensor[2].irType = MediumBox;
	ir_sensor[3].irType = LargeBox;
	
	for (uint8_t i = 0; i < 3; i++) {
		for (uint8_t j = 0; j < 20; j++) {
			CajitaArr[i][j] = 0;
		}
	}
	
	/* END User code Init --------------------------------------------------------*/
	sei();

	while (1){
		/* User Code loop ------------------------------------------------------------*/
		task_HCSR();
		serialTask(&dataRx,&dataTx);
		if (IS10MS)
			every10ms();
		/* END User Code loop --------------------------------------------------------*/
	}

	return 0;
}

