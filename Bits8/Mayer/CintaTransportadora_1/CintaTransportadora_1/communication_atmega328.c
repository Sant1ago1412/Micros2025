#include <avr/io.h>
#include "communication_atmega328.h"

//void (*decode)(_sDato *datosCom);
_eProtocolo estadoProtocolo;



void initialize_USART(_sDato* datosCom, unsigned int ubrr,  void (*decodeAux)(_sDato *datosCom)){
	/*Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	UCSR0A = 0xFE; //inicializo todas las banderas excepto el multiprocesor
	/*Enable receiver and transmitter */
	UCSR0B = (1<<RXCIE0) | (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C =  (0<<UCSZ02) | (1<<UCSZ01) | (1<<UCSZ00);
	datosCom->timeOut = 0;				
	datosCom->indexStart = 0;					
	datosCom->cheksumRx = 0;					
	datosCom->indexWriteTx=0;
	datosCom->indexReadTx=0;
	datosCom->indexWriteRx=0;
	datosCom->indexReadRx=0;
	decode = decodeAux;
}

void task_communication(_sDato* datosCom){
	if(datosCom->indexReadRx!=datosCom->indexWriteRx ){
		decodeProtocol(datosCom);
		//datosCom->indexWriteRx &= 0x1F; //31
		//datosCom->indexReadRx &= 0x1F; //31
	}
	if (datosCom->indexReadTx != datosCom->indexWriteTx) {
		if (UCSR0A & (1 << UDRE0)) { // Si el buffer de transmisi�n est� vac�o
			UDR0 = datosCom->bufferTx[datosCom->indexReadTx]; // Enviar el dato
			datosCom->indexReadTx++;
			//datosCom->indexWriteTx &= 0x1F; //31
			//datosCom->indexReadTx &= 0x1F; //31
		}
	}
}

void decodeProtocol(_sDato *datosCom){
	static uint8_t nBytes=0;
	uint8_t indexWriteRxCopy=datosCom->indexWriteRx;

	while (datosCom->indexReadRx!=indexWriteRxCopy)
	{
		switch (estadoProtocolo) {
		case START:
			if (datosCom->bufferRx[datosCom->indexReadRx++]=='U'){
				estadoProtocolo=HEADER_1;
				datosCom->cheksumRx=0;
			}
			break;
		case HEADER_1:
			if (datosCom->bufferRx[datosCom->indexReadRx++]=='N')
			estadoProtocolo=HEADER_2;
			else{
				datosCom->indexReadRx--;
				estadoProtocolo=START;
			}
			break;
		case HEADER_2:
			if (datosCom->bufferRx[datosCom->indexReadRx++]=='E')
			estadoProtocolo=HEADER_3;
			else{
				datosCom->indexReadRx--;
				estadoProtocolo=START;
			}
			break;
		case HEADER_3:
			if (datosCom->bufferRx[datosCom->indexReadRx++]=='R')
				estadoProtocolo=NBYTES;
			else{
				datosCom->indexReadRx--;
				estadoProtocolo=START;
			}
			break;
		case NBYTES:
			datosCom->indexStart=datosCom->indexReadRx;
			nBytes=datosCom->bufferRx[datosCom->indexReadRx++];
			estadoProtocolo=TOKEN;
			break;
		case TOKEN:
			if (datosCom->bufferRx[datosCom->indexReadRx++]==':'){
				estadoProtocolo=PAYLOAD;
				datosCom->cheksumRx ='U'^'N'^'E'^'R'^ nBytes^':';
			}
			else{
				datosCom->indexReadRx--;
				estadoProtocolo=START;
			}
			break;
		case PAYLOAD:
			if (nBytes>1){
				datosCom->cheksumRx ^= datosCom->bufferRx[datosCom->indexReadRx++];
			}
			nBytes--;
			if(nBytes<=0){
				estadoProtocolo=START;
				if(datosCom->cheksumRx == datosCom->bufferRx[datosCom->indexReadRx]){
					if(decode != NULL)
						decode(datosCom);
				}
			}
			break;
		default:
			estadoProtocolo=START;
			break;
		}
	}
}

