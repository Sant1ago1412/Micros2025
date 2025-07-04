#ifndef UTIL_H_
#define UTIL_H_
#define RINGBUFFLENGTH		256
#define POSID               2
#define POSDATA             5
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
	struct{
		uint8_t nibbleL: 4;
		uint8_t nibbleH: 4;
	}nibbles;
	uint8_t byte;
}flags;
typedef union{
	uint32_t    ui32;
	int32_t     i32;
	uint16_t    ui16[2];
	int16_t     i16[2];
	uint8_t     ui8[4];
	int8_t      i8[4];
}_uWord;
typedef struct{
	uint8_t *buff;      /*!< Puntero para el buffer de recepci�n*/
	uint8_t indexR;     /*!< indice de lectura del buffer circular*/
	uint8_t indexW;     /*!< indice de escritura del buffer circular*/
	uint8_t indexData;  /*!< indice para identificar la posici�n del dato*/
	uint8_t mask;       /*!< m�scara para controlar el tama�o del buffer*/
	uint8_t chk;        /*!< variable para calcular el checksum*/
	uint8_t nBytes;     /*!< variable para almacenar el n�mero de bytes recibidos*/
	uint8_t header;     /*!< variable para mantener el estado dela MEF del protocolo*/
	uint8_t timeOut;    /*!< variable para resetear la MEF si no llegan m�s caracteres luego de cierto tiempo*/
	uint8_t isComannd;
}_sRx;

typedef struct{
	uint8_t *buff;      /*!< Puntero para el buffer de transmisi�n*/
	uint8_t indexR;     /*!<indice de lectura del buffer circular*/
	uint8_t indexW;     /*!<indice de escritura del buffer circular*/
	uint8_t mask;       /*!<m�scara para controlar el tama�o del buffer*/
	uint8_t chk;        /*!< variable para calcular el checksum*/
}_sTx;
/**
 * @brief Enumeraci�n para la maquina de estados
 * que se encarga de decodificar el protocolo
 * de comunicaci�n
 *  
 */
typedef enum{
    HEADER_U,
    HEADER_N,
    HEADER_E,
    HEADER_R,
    NBYTES,
    TOKEN,
    PAYLOAD
}_eDecode;
/**
 * @brief Enumeraci�n de los comandos del protocolo
 * 
 */
typedef enum{
    ALIVE = 0xF0,
    FIRMWARE= 0xF1,
	DATA = 0xA5,
    ACK = 0x0D,
    UNKNOWN = 0xFF
}_eCmd;
typedef enum {
	
	MEAS_IDLE,         // Sin medici�n en curso
	MEAS_TRIGGER,      // Disparando el pulso (TRIG en alto)
	MEAS_WAIT_FOR_ECHO,// Esperando el pulso de ECHO
	MEAS_DONE,          // Medici�n completada
	
} meas_state_t;

typedef struct{
	
	volatile  uint16_t startEcho;
	volatile uint16_t stopEcho;
	uint16_t distance;
	
}TimeHcsr;
typedef enum {
	IR_RISING = 0,
	IR_UP,
	IR_FALLING,
	IR_DOWN
} IRState;

typedef enum Tamano{
	SmallBox=1,
	MediumBox,
	LargeBox,
	NotSelected
}s_boxSize;

typedef struct {
	IRState state;
	uint8_t stateConfirmed;
	uint8_t last_sample;
	s_boxSize irType;
} IRDebounce;


typedef enum State{
	isOn=0,
	Push,
	isOut
}s_boxState;

typedef struct boxAtributte{
	
	s_boxSize  boxSize;
	s_boxState boxState;
	
}s_boxType;

typedef struct boxSize{
	uint16_t smallboxF;
	uint16_t smallboxC;
	uint16_t mediumboxF;
	uint16_t mediumboxC;
	uint16_t largeboxF;
	uint16_t largeboxC;
}s_sizeConfig;

#endif