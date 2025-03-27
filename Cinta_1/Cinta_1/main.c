/*
 * Cinta_1.c
 *
 * Created: 26/3/2025 17:08:49
 * Author : Baccon
 */ 
/* Includes ------------------------------------------------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
/* END Includes --------------------------------------------------------------*/


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

typedef union{
	int32_t i32;
	uint32_t ui32;
	uint16_t ui16[2];
	uint8_t ui8[4];
	int8_t i8[4];
}u_dat;
/* END typedef ---------------------------------------------------------------*/


/* define --------------------------------------------------------------------*/

/* END define ----------------------------------------------------------------*/


/* Function prototypes -------------------------------------------------------*/
void setServoAngle();
void setupPWM();
void ini_ports();
void ini_timer1();
/* END Function prototypes ---------------------------------------------------*/


/* Global variables ----------------------------------------------------------*/

/* END Global variables ------------------------------------------------------*/


/* Constant in Flash ---------------------------------------------------------*/

/* END Constant in Flash -----------------------------------------------------*/


/* Function ISR --------------------------------------------------------------*/
ISR(TIMER0_COMPA_vect){
	
}
/* END Function ISR ----------------------------------------------------------*/


/* Function prototypes user code ----------------------------------------------*/
void ini_timer1(){
	
	//inicializar timer de 1ms
	TCCR0A = (1<<WGM01);
	TCCR0B = ((1<<CS01)|(1<<CS00));
	OCR0A = 249;
	
}

/* END Function prototypes user code ------------------------------------------*/

int main()
{
	/* Local variables -----------------------------------------------------------*/

	/* END Local variables -------------------------------------------------------*/




	/* User code Init ------------------------------------------------------------*/
	/* END User code Init --------------------------------------------------------*/


	while (1)
	{
		/* User Code loop ------------------------------------------------------------*/
	
		/* END User Code loop --------------------------------------------------------*/
	}

	return 0;
}