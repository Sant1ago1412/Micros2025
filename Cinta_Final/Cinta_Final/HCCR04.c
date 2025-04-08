//
/*					HCSR04 Library (.c) - ATMEGA328P
					Ingenieria en Mecatronica - UNER					*/
//

#include "HCSR04.h"
#include <stdlib.h>

typedef struct sHandleHCSR04
{
	void (*WritePin)(uint8_t value);		
	void (*OnReadyMeasure)(uint16_t distance);	
	
	uint16_t usTimeRise;					//tiempo en el que llega la señal del ECHO
	uint16_t usTimeFall;					//tiempo en el que la señal de ECHO desaparece
	uint16_t lastDistanceUs;				//Guardo la última distancia medida en microsegundos
	uint32_t ticks;							//Indica el número de ciclos de reloj necesarios para medir correctamente la distancia (16)
	
	union
	{
		struct {
			uint8_t EDGEREADY: 1;			//Indica que se ha recibido la señal completa (subida y bajada)
			uint8_t spare: 7;				// 4 bits de reserva
		}bit;
		uint8_t byte;						//Accedo a todos los bits como un solo byte
	}flags;

	
} _sHCSR04Handle;

//variable interna estatica
static _sHCSR04Handle *mySensor;		//puntero a la estructura _sHCSR04Handle. Se usa como variable auxiliar para manipular los datos de los sensores ultrasónicos HCSR04.

unsigned int HCSR04_AddNew(void (*WritePin_HCSR04)(uint8_t value), uint32_t ticks)
{
	mySensor = (_sHCSR04Handle *)malloc(sizeof(_sHCSR04Handle));	//myHandleAux almacena un nuevo sensor dinámicamente, por lo que myHandleAux apunta a la nueva estructura creada.

	mySensor->WritePin = WritePin_HCSR04;
	mySensor->ticks = ticks;
	mySensor->flags.byte = 0;								
	mySensor->lastDistanceUs = 0;
	mySensor->usTimeRise = 0;
	mySensor->usTimeFall = 0;
	mySensor->OnReadyMeasure = NULL;	
	mySensor->WritePin(0);		
	
	return (unsigned int)mySensor;
}

//uint16_t HCSR04_Read(unsigned int handleHCSR04)	//leo para mandar por comunicacion. 
//{
	////Leo el valor de lastDistanceUs del HCSR_1 
	////myHandleAux apunta a la dirección de memoria de un sensor (En este caso HCSR_1), la convierte en un puntero tipo "_sHCSR04Handle", entonces myHandleAux ahora apunta a la estructura _sHCSR04Handle en esa dirección de memoria.
	////convierte un número (dirección de memoria) en un puntero usable.
	//myHandleAux = (_sHCSR04Handle *)handleHCSR04; 
//
	//if (myHandleAux != NULL)
		//return myHandleAux->lastDistanceUs; 
	//return 0;
//}


void HCSR04_Start(unsigned int handleHCSR04)
{
	mySensor = (_sHCSR04Handle *)handleHCSR04;
	
	mySensor->WritePin(1);					
	
}

void HCSR04_TriggerReady(unsigned int handleHCSR04)
{
	mySensor = (_sHCSR04Handle *)handleHCSR04;
	
	mySensor->WritePin(0);
}

void HCSR04_RiseEdgeTime(unsigned int handleHCSR04, uint16_t usTimeRise)
{
	mySensor = (_sHCSR04Handle *)handleHCSR04;

	mySensor->usTimeRise = usTimeRise;	//Cargo cuando se pone en RISE
	mySensor->flags.byte = 0;

}

void HCSR04_FallEdgeTime(unsigned int handleHCSR04, uint16_t usTimeFall)
{
	mySensor = (_sHCSR04Handle *)handleHCSR04;

	mySensor->usTimeFall = usTimeFall;	//Cargo cuando se pone en Fall 
	mySensor->flags.bit.EDGEREADY = 1;
	
}

void task_HCSR()
{
	if (mySensor != NULL)
	{
		if (mySensor->flags.bit.EDGEREADY) //¿el sensor midió? Esto indica que ya se recibieron los dos flancos (rising y falling) y se puede calcular el tiempo que tardó el eco en volver = la distancia.
		{
			mySensor->flags.byte = 0;
			
			if(mySensor->usTimeRise < mySensor->usTimeFall){ //si no hubo overflow
				mySensor->lastDistanceUs = mySensor->usTimeFall - mySensor->usTimeRise; //distancia medida
				}else{ //hubo overflow
				mySensor->lastDistanceUs = mySensor->usTimeFall - mySensor->usTimeRise + 0xFFFF; //para corregir
			}
			
			if(mySensor->lastDistanceUs > 11764) //2 metros
				mySensor->lastDistanceUs = 0xFFFF;
			
			if (mySensor->OnReadyMeasure != NULL)
				mySensor->OnReadyMeasure(mySensor->lastDistanceUs);

		}

	}
}

void HCSR04_AttachOnReadyMeasure(unsigned int handleHCSR04, void (*OnReadyMeasure)(uint16_t distance))
{
	mySensor = (_sHCSR04Handle *)handleHCSR04;
	mySensor->OnReadyMeasure = OnReadyMeasure;
}