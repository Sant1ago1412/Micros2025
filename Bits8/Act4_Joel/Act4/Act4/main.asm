;
; Act4.asm
;
; Created: 18/3/2025 18:08:11
; Author : Schiebert Joel
;

#include <m328pdef.inc>

;igualdades
.equ	softversion=0x01
.equ	build=0x04
.equ	centuria=20
.equ	anno=17
.equ	mes=1
.equ	dia=29

;igualdades
.equ	LEDBUILTIN = PB5
.equ	SW0 = PB4

; Definiciones para el buffer USART
.equ    BUFSIZERX = 32	//Tamaño del buffer de recepción
.equ    BUFSIZETX = 32	//Tamaño del buffer de transmisión

;-----------------------------------------------------------------------------------------------------------------------------------------------

.equ LASTSTATESW0	= 0		//GPIOR0<0>: ultimo estado del pulsador, Se usa para detectar flancos (cuando el usuario presiona o suelta el botón).
.equ IS10MS			= 5		//GPIOR0<5>: para lograr los 10ms
.equ ISNEWSTATESW0	= 7		//GPIOR0<7>: Indica si hubo un "nuevo estado" del botón. Se activa cuando el usuario presiona el botón (SW0).


;Estos valores representan los patrones de parpadeo del LED en cada estado.
;Estos valores se almacenan en ledstate y son usados para definir cuándo el LED debe encenderse o apagarse.
.equ	STATEIDLE	= 0x80000000	
.equ	STATEUNO	= 0xF1000000	
.equ	STATEDOS	= 0xF1100000	
.equ	STATETRES	= 0xF1110000	
.equ	STATECUATRO = 0xF1111000	
.equ	STATECINCO	= 0xF1111100	
.equ	STATESEIS	= 0xF1400000	
.equ	STATESIETE	= 0xF1408000	
.equ	STATEOCHO	= 0xF140A000	
.equ	STATENUEVE	= 0xF140A800	
.equ	STATEDIEZ	= 0xF140AA00	

;-----------------------------------------------------------------------------------------------------------------------------------------------

;definiciones - nombres simb?licos
.def	w=r16
.def	w1=r17
.def	saux=r18
.def	flag1=r19
.def	newButton=r21

;-----------------------------------------------------------------------------------------------------------------------------------------------

; Segmento de Datos SRAM
.dseg
statboot:		.BYTE	1
addrrx:			.BYTE	2
RXBUF:			.BYTE	24

bufRX:			.BYTE   BUFSIZERX	; Buffer circular de recepción
iwRX:			.BYTE   1			; Índice de escritura RX
irRX:			.BYTE   1			; Índice de lectura RX
bufTX:			.BYTE   BUFSIZETX	; Buffer circular de transmisión
iwTX:			.BYTE   1			; Índice de escritura TX
irTX:			.BYTE   1			; Índice de lectura TX

time10ms:		.BYTE	1
time100ms:		.BYTE	1
maskState:		.BYTE	4			;Máscara de estado del LED (4 bytes). Guarda una máscara de 4 bytes que se usa para definir el patrón de parpadeo del LED.
ledState:		.BYTE	4			;Estado actual del LED (4 bytes). Cuando el usuario cambia de estado, se actualiza con STATEIDLE o STATEUNO.
ledLastState:	.BYTE	1

dbSW0:			.BYTE	1			;Contador de debounce del botón. Evita que una sola pulsación sea detectada varias veces.

;-----------------------------------------------------------------------------------------------------------------------------------------------

; Segmento de Código
.cseg

.org    0x00		// El micro empieza en la direccion 0x00
    jmp start

.org	0x20		//pagina 74 de ATMega328P
	jmp TIMER0_OVF

.org    0x24		// Vector de interrupción para USART RX
    jmp USART_RXC

.org	0x34		//por el vector de interrupciones

;-----------------------------------------------------------------------------------------------------------------------------------------------

;******************* CONSTANTES EN FLASH *************************

version:		.db "20250314_01b01",'\n','\0'		;Se puede enviar por el puerto serie si se recibe un comando de diagnóstico.
msgTest:		.db "TEST",'\n','\0'				;Este mensaje se usa como respuesta si se recibe la letra "T" por USART.

msgStateIdle:	.db "STATE IDLE",'\n','\0'			;Este mensaje se imprime por serie cuando STATEIDLE está activo.
msgStateUno:	.db "STATE UNO",'\n','\0','\0'		;Este mensaje se imprime cuando STATEUNO está activo.
msgStateDos:	.db "STATE DOS",'\n','\0','\0'		
msgStateTres:	.db "STATE TRES",'\n','\0'	
msgStateCuatro:	.db "STATE CUATRO"
msgStateCinco:	.db "STATE CINCO",'\n'	
msgStateSeis:	.db "STATE SEIS",'\n','\0'	
msgStateSiete:	.db "STATE SIETE",'\n'	
msgStateOcho:	.db "STATE OCHO",'\n','\0'
msgStateNueve:	.db "STATE NUEVE",'\n'	
msgStateDiez:	.db "STATE DIEZ",'\n','\0'	

;-----------------------------------------------------------------------------------------------------------------------------------------------

;INTERRUPCIONES

TIMER0_OVF:					;interrupcion a la que va a entrar 1 de 256 clocks (prescaler)

	in		r2,SREG			;cargo en r2 todos los valores de SREG, el cual me guarda todos los estados de las banderas del micro (registro de estado) //SREG = STATUS REGISTER, En donde en cada bit tiene los registros del micro, como la C, Z, etc.
	push	r2				;guardo el estado de SREG en la pila para no tener alteraciones por la interrupcion 
	push	r16				;como es una interrupcion guardo el valor de r16 por si tenia algo guardado
	
	lds		r16,time10ms	;Cargo 1 byte (time10ms) directamente desde el espacio de datos 
	dec		r16				;Decremento en 1 el r16, que vale 5 (Ver en "START"). Como a la interrupcion entrará cada 2ms, 2x5=10ms
	sts		time10ms,r16	;Almaceno directamente en el espacio de datos, es decir, voy decrementando y cargandole el nuevo valor a time10ms, que irá desde 5 a 0

	brne	OUT_TIMER0_OVF	;entra a esta funcion siempre que time10ms no haya llegado a 0

	;si dio 0 hago lo siguiente ,osea que llego a los 10ms

	sbi		GPIOR0, IS10MS	;seteo en 1 el bit 5 de gpior0 (indica si pasaron los 10ms)
	ldi		r16,5			;Vuelvo a cargar el valor 5 para que se vuelvan a repetir los 10ms
	sts		time10ms,r16	;reincio el contador en 5 (cantidad de veces que necesito para que cuente 10ms)

OUT_TIMER0_OVF:				;vuelvo a recuperar los datos guardados en la pila para que se sigan utilzando en el programa
	
	pop		r16
	pop		r2
	out		SREG,r2			;devuelve el estado origianl de SREG (porque usamos la bandera z en la interrupcion)
	reti


/*Interrupción USART_RXC
Guarda el dato recibido en bufRX y actualiza el índice de escritura iwRX.
Usa un buffer circular para evitar sobreescrituras.*/

USART_RXC:
    in		r2, SREG ; Guardar el estado de SREG y agrego a la pila los registros que voy a utilizar en la interrupcion para no perderlos cuando termine la interrupcion
    push    r2
    push    r16
    push    r17
    push    r26
    push    r27

	; Calcular la dirección en bufRX para almacenar el dato recibido
    ldi		r26, low(bufRX)
    ldi		r27, high(bufRX)
    lds		r16, iwRX			;Leer índice de escritura RX
    clr		r17
    add		r26, r16
    adc		r27, r17 
	; Guardar el dato recibido en el buffer
    lds		r17, UDR0			; Leer dato del USART
    st		X, r17				; Almacenar en buffer RX

	; Incrementar índice circular de escritura
    inc		r16
    andi    r16, BUFSIZERX-1	; Mantener dentro del rango del buffer
    sts		iwRX, r16

OUT_USART_RXC:
    pop		r27
    pop		r26
    pop		r17
    pop		r16
    pop		r2
    out		SREG, r2	; Restaurar SREG
    reti				; Retornar de la interrupción


;constantes
//consts:		.DB 0, 255, 0b01010101, -128, 0xaa
varlist:	.DD 0, 0xfadebabe, -2147483648, 1 << 30

;************************************** Funciones **************************************

;****** Inicializar Puertos ******
ini_ports:
	
	ldi		r16, (0 << SW0) | (1 << LEDBUILTIN) 
	out		DDRB,r16 
	ldi		r16, (1 << SW0) 
	out		PORTB, r16 
	ret


;***** Inicializar Timer ******
ini_timer:

	ldi		r16, (1<<WGM01)	//0b00000010 - timer en modo CTC (WGM01 = 1, WGM00 = 0)
	out		TCCR0A, r16

	clr		r16				//Como TCNT0 y TIFR0 tienen el valor 0b00000000 (es decir 0), directamente limpio el registro para que quede en 0 y se los cargo
	out		TCNT0, r16		//TCNT0 reinicia el contador, por eso es necesario ponerlo en 0
	out		TIFR0, r16

	ldi		r16, (1 << OCIE0A)	//0b00000010. Habilitar interrupción por comparación OCR0A
	sts		TIMSK0, r16

	ldi		r16, (1<<CS02)	//0b00000100	cargo el prescaler en 256
	out		TCCR0B, r16

	ldi		r16, 124	//Valor para generar 2ms (ver en word)
	out		OCR0A ,r16

	ret

;********** Inicialización USART **********

/*Configura el USART a 115200 baudios, 8N1
Configura USART a 115200 baudios, 8 bits, sin paridad, 1 stop bit.
Habilita TX, RX e interrupción RX.*/

ini_USART0:
    ldi		r16, 0xFE		; Doble velocidad habilitada (U2X0)
    sts		UCSR0A, r16
    ldi		r16, (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0) ; Habilitar TX, RX e interrupción RX
    sts		UCSR0B, r16
    ldi		r16, 0x06		; Configurar 8N1 (8 bits, sin paridad, 1 stop bit)
    sts		UCSR0C, r16
    ldi		r16, 16			; Baud rate para 115200 con fclk=16MHz // Configurar baud rate (115200 baudios a 16MHz)
    sts		UBRR0L, r16
    sts		UBRR0L, r16
    ldi		r16, 0
    sts		UBRR0H, r16
    ret

;Enviar texto constante por USART 
PutConstTextOnBufTX:
    push    r16
    push    r17
    push    r26
    push    r27
    lds		r16, iwTX			; Leer índice de escritura TX

nextByteText:
    ldi		r27, high(bufTX)
    ldi		r26, low(bufTX)
    add		r26, r16
    brcc	PC+2
    subi	r27, -1
    inc		r16
    andi    r16, BUFSIZETX-1	; Mantener dentro del buffer

    lpm		r17, Z+				; Cargar siguiente carácter del mensaje
    cpi		r17, '\0'			; ¿Es el final de la cadena?
    breq	outPutConstText		; Si es el final de la cadena entonces voy a outputconstText
    st		X, r17				; Almacenar en buffer TX
    brne	nextByteText		; Repetir si no ha terminado

outPutConstText:
    dec		r16
    andi	r16, BUFSIZETX-1
    sts		iwTX, r16
    pop		r27
    pop		r26
    pop		r17
    pop		r16
    ret

;Procesar datos recibidos
/*Procesamiento de recepción doRX
Verifica si se recibió una 'T' y si es así, llama a PutConstTextOnBufTX para enviar "TEST". Y demas instrucciones */

doRX:
    lds		r16, irRX
    ldi		r26, low(bufRX)
    ldi		r27, high(bufRX)
    clr		r17
    add		r26, r16
    adc		r27, r17
    inc		r16
    andi	r16, BUFSIZERX-1
    sts		irRX, r16
    ld		r16, X		; Leer dato recibido

doRX_V:	;if 'V' is received, the firmware version is answered  
	cpi		r16, 'V'
	breq	PC+2
	rjmp	doRX_T
	ldi		r30, low(version << 1)
	ldi		r31, high(version << 1)
	call	PutConstTextOnBufTX
	ret

doRX_T: ; Si se recibe 'T', se responde con "TEST"
	cpi		r16, 'T'
	breq	PC+2
	rjmp	doRX_S
    ldi		r30, low(msgTest << 1)
    ldi		r31, high(msgTest << 1)
    call	PutConstTextOnBufTX
    ret

doRX_S: ;if 'S' is received, change ledState
	cpi		r16, 'S' 
	breq	PC+2 //Si el caracter recibido es S, voy 2 lineas mas adelante, es decir a LDS 
	ret

	lds		r16, ledState+1				//leo el estado actual del led. (En un principio ledstate+3 posee 0x80)
	cpi		r16, 0xAA					//Comparo, si el estado del led es DIEZ, entonces cambio a IDLE 
	brne	setNewStateRX_s					//si son iguales, seteo un nuevo estado. Es decir. Si estaba en IDLE paso a un nuevo estado. Sino. si es que no estaba en IDLE, voy a IDLE 

	ldi		r30, low(msgStateIdle << 1)	
	ldi		r31, high(msgStateIdle << 1)
	call	PutConstTextOnBufTX

	//Cargo el estado "IDLE"
	ldi		r16, low(STATEIDLE)		//STATEIDLE = 0x80000000 
	ldi		r17, high(STATEIDLE)
	ldi		r18, byte3(STATEIDLE)
	ldi		r19, byte4(STATEIDLE)
	rjmp	outNewState

; Cargo el nuevo estado "STATEx" a los registros
setNewStateRX_s:
	
	ldi		r30, low(msgStateUno << 1)
	ldi		r31, high(msgStateUno << 1)
	call	PutConstTextOnBufTX

	lds		r16, ledLastState
	inc		r16
	sts		ledLastState, r16

	cpi		r16, 1
	breq	SetStateUNO_RXs
	cpi		r16, 2
	breq	SetStateDOS_RXs
	cpi		r16, 3
	breq	SetStateTRES_RXs
	cpi		r16, 4
	breq	SetStateCUATRO_RXs
	cpi		r16, 5
	breq	SetStateCINCO_RXs
	cpi		r16, 6
	breq	SetStateSEIS_RXs
	cpi		r16, 7
	breq	SetStateSIETE_RXs
	cpi		r16, 8
	breq	SetStateOCHO_RXs
	cpi		r16, 9
	breq	SetStateNUEVE_RXs
	cpi		r16, 10
	breq	SetStateDIEZ_RXs

SetStateUNO_RXs:
	ldi		r16, low(STATEUNO)		
	ldi		r17, high(STATEUNO)
	ldi		r18, byte3(STATEUNO)
	ldi		r19, byte4(STATEUNO)

	rjmp	outNewState

SetStateDOS_RXs:
	ldi		r16, low(STATEDOS)		
	ldi		r17, high(STATEDOS)
	ldi		r18, byte3(STATEDOS)
	ldi		r19, byte4(STATEDOS)

	rjmp	outNewState

SetStateTRES_RXs:
	ldi		r16, low(STATETRES)		
	ldi		r17, high(STATETRES)
	ldi		r18, byte3(STATETRES)
	ldi		r19, byte4(STATETRES)

	rjmp	outNewState

SetStateCUATRO_RXs:
	ldi		r16, low(STATECUATRO)		
	ldi		r17, high(STATECUATRO)
	ldi		r18, byte3(STATECUATRO)
	ldi		r19, byte4(STATECUATRO)

	rjmp	outNewState
	
SetStateCINCO_RXs:
	ldi		r16, low(STATECINCO)		
	ldi		r17, high(STATECINCO)
	ldi		r18, byte3(STATECINCO)
	ldi		r19, byte4(STATECINCO)

	rjmp	outNewState

SetStateSEIS_RXs:
	ldi		r16, low(STATESEIS)		
	ldi		r17, high(STATESEIS)
	ldi		r18, byte3(STATESEIS)
	ldi		r19, byte4(STATESEIS)

	rjmp	outNewState

SetStateSIETE_RXs:
	ldi		r16, low(STATESIETE)		
	ldi		r17, high(STATESIETE)
	ldi		r18, byte3(STATESIETE)
	ldi		r19, byte4(STATESIETE)

	rjmp	outNewState

SetStateOCHO_RXs:
	ldi		r16, low(STATEOCHO)		
	ldi		r17, high(STATEOCHO)
	ldi		r18, byte3(STATEOCHO)
	ldi		r19, byte4(STATEOCHO)

	rjmp	outNewState

SetStateNUEVE_RXs:
	ldi		r16, low(STATENUEVE)		
	ldi		r17, high(STATENUEVE)
	ldi		r18, byte3(STATENUEVE)
	ldi		r19, byte4(STATENUEVE)

	rjmp	outNewState

SetStateDIEZ_RXs:
	ldi		r16, low(STATEDIEZ)		
	ldi		r17, high(STATEDIEZ)
	ldi		r18, byte3(STATEDIEZ)
	ldi		r19, byte4(STATEDIEZ)

	ldi		r25, 0
	sts		ledLastState, r25

	rjmp	outNewState

///////////////////////////////////////////////////////////////////////////////////////////////////////
;Manejo la detección de un botón con antirrebote (debounce)

doSW0:
	lds		r16, dbSW0
	sbic	GPIOR0, LASTSTATESW0	
	rjmp	testLowSW0
	sbis	PINB, SW0				
	rjmp	doSW0db1
	sbi		GPIOR0, LASTSTATESW0	
	rjmp	doSW0db0

testLowSW0:
	sbic	PINB, SW0
	rjmp	doSW0db1
	cbi		GPIOR0, LASTSTATESW0

;Anti-rebote
//Si detecta un cambio en el botón, inicia el debounce cargando r16 = 6.
doSW0db0:
	ldi		r16, 6

/*Cada vez que se llama a esta función, r16 se va decrementando.
Cuando r16 llega a 0, confirma el nuevo estado del botón y activa ISNEWSTATESW0.*/
doSW0db1:
	tst		r16				//Verifica si r16 es 0
	breq	outDoSW0
	dec		r16
	brne	outDoSW0
	sbi		GPIOR0, ISNEWSTATESW0	

outDoSW0:
	sts		dbSW0, r16
	ret

////////////////////////////////////////////////////////////////////////////////////////////////////////
	
; Cambio el estado del led cuando se presiona el boton. 
doNewStateSW0:
	cbi		GPIOR0, ISNEWSTATESW0		//limpio el bit 7 de GPIOR0
	sbic	GPIOR0, LASTSTATESW0		//Si el bit0 (LASTSTATESW0) es 0 (boton apretado) entonces salteo la siguiente instruccion y cambio el estado del led
	ret
	lds		r16, ledState+1				//leo el estado actual del led. (En un principio ledstate+3 posee 0x80)
	cpi		r16, 0xAA					//Comparo, si el estado del led es DIEZ, entonces cambio a IDLE 
	brne	setNewState					//si son iguales, seteo un nuevo estado. Es decir. Si estaba en IDLE paso a un nuevo estado. Sino. si es que no estaba en IDLE, voy a IDLE 

	ldi		r30, low(msgStateIdle << 1)	
	ldi		r31, high(msgStateIdle << 1)
	call	PutConstTextOnBufTX

	//Cargo el estado "IDLE"
	ldi		r16, low(STATEIDLE)		//STATEIDLE = 0x80000000 
	ldi		r17, high(STATEIDLE)
	ldi		r18, byte3(STATEIDLE)
	ldi		r19, byte4(STATEIDLE)
	rjmp	outNewState

; Cargo el nuevo estado "STATEx" a los registros
setNewState:
	ldi		r30, low(msgStateUno << 1)
	ldi		r31, high(msgStateUno << 1)
	call	PutConstTextOnBufTX

	lds		r16, ledLastState
	inc		r16
	sts		ledLastState, r16

	cpi		r16, 1
	breq	SetStateUNO
	cpi		r16, 2
	breq	SetStateDOS
	cpi		r16, 3
	breq	SetStateTRES
	cpi		r16, 4
	breq	SetStateCUATRO
	cpi		r16, 5
	breq	SetStateCINCO
	cpi		r16, 6
	breq	SetStateSEIS
	cpi		r16, 7
	breq	SetStateSIETE
	cpi		r16, 8
	breq	SetStateOCHO
	cpi		r16, 9
	breq	SetStateNUEVE
	cpi		r16, 10
	breq	SetStateDIEZ

SetStateUNO:
	ldi		r16, low(STATEUNO)		
	ldi		r17, high(STATEUNO)
	ldi		r18, byte3(STATEUNO)
	ldi		r19, byte4(STATEUNO)

	rjmp	outNewState

SetStateDOS:
	ldi		r16, low(STATEDOS)		
	ldi		r17, high(STATEDOS)
	ldi		r18, byte3(STATEDOS)
	ldi		r19, byte4(STATEDOS)

	rjmp	outNewState

SetStateTRES:
	ldi		r16, low(STATETRES)		
	ldi		r17, high(STATETRES)
	ldi		r18, byte3(STATETRES)
	ldi		r19, byte4(STATETRES)

	rjmp	outNewState

SetStateCUATRO:
	ldi		r16, low(STATECUATRO)		
	ldi		r17, high(STATECUATRO)
	ldi		r18, byte3(STATECUATRO)
	ldi		r19, byte4(STATECUATRO)

	rjmp	outNewState
	
SetStateCINCO:
	ldi		r16, low(STATECINCO)		
	ldi		r17, high(STATECINCO)
	ldi		r18, byte3(STATECINCO)
	ldi		r19, byte4(STATECINCO)

	rjmp	outNewState

SetStateSEIS:
	ldi		r16, low(STATESEIS)		
	ldi		r17, high(STATESEIS)
	ldi		r18, byte3(STATESEIS)
	ldi		r19, byte4(STATESEIS)

	rjmp	outNewState

SetStateSIETE:
	ldi		r16, low(STATESIETE)		
	ldi		r17, high(STATESIETE)
	ldi		r18, byte3(STATESIETE)
	ldi		r19, byte4(STATESIETE)

	rjmp	outNewState

SetStateOCHO:
	ldi		r16, low(STATEOCHO)		
	ldi		r17, high(STATEOCHO)
	ldi		r18, byte3(STATEOCHO)
	ldi		r19, byte4(STATEOCHO)

	rjmp	outNewState

SetStateNUEVE:
	ldi		r16, low(STATENUEVE)		
	ldi		r17, high(STATENUEVE)
	ldi		r18, byte3(STATENUEVE)
	ldi		r19, byte4(STATENUEVE)

	rjmp	outNewState

SetStateDIEZ:
	ldi		r16, low(STATEDIEZ)		
	ldi		r17, high(STATEDIEZ)
	ldi		r18, byte3(STATEDIEZ)
	ldi		r19, byte4(STATEDIEZ)

	ldi		r25, 0
	sts		ledLastState, r25

	rjmp	outNewState

; Cargo al ledstate 
outNewState:
	sts		ledState+0, r16
	sts		ledState+1, r17
	sts		ledState+2, r18
	sts		ledState+3, r19
	ret

////////////////////////////////////////////////////////////////////////////////////////////////////////

do100ms:
	lds		r16, time100ms
	dec		r16
	sts		time100ms, r16
	breq	PC+2				// "Program Counter + 2", Salto dos direcciones, si no llego a 0, me voy (ret). Si llego a 0 salteo 2 instrucciones y voy al ldi
	ret

	ldi		r16, 10
	sts		time100ms, r16

	lds		r12, maskState+0	
	lds		r13, maskState+1	
	lds		r14, maskState+2
	lds		r15, maskState+3	
	
	lds		r8, ledState+0		//0x00
	lds		r9, ledState+1		//0x00
	lds		r10, ledState+2		//0x00
	lds		r11, ledState+3		//0x80

	and		r8, r12		
	and		r9, r13		
	and		r10, r14	
	and		r11, r15	
	or		r8, r9
	or		r10, r11
	or		r8, r10 

	breq	ledOff				; Si r8 es 0, el LED debe apagarse, de lo contrario, debe encenderse.

	sbi		PORTB, LEDBUILTIN	; led ON
	rjmp	saveMask

ledOff:
	cbi		PORTB, LEDBUILTIN	;led Off

; La máscara se rota en cada ejecución de do100ms, lo que significa que cada 100 ms se desplaza la posición del bit activo en maskState.
saveMask:
								/*	roto cada registro de maskState, desplazando los bits hacia la derecha.
									en el momento 1, pasamos de:		  a:			y tiene que llegar a:
													r12:	00000000	|	00000000	|	00000001
													r13:	00000000	|	00000000	|	00000000
													r14:	00000000	|	00000000	|	00000000	
													r15:	10000000	|	01000000	|	00000000	
									Por lo que Cada 100 ms, la máscara se rota una posición a la derecha. La máscara tiene 4 registros de 8 bits (32 bits en total).
									Se necesitan 32 rotaciones para que el bit 1 desaparezca completamente y se reinicie la máscara.
									32 × 100 ms = 3,2 s antes de que el LED se vuelva a encender.	*/

	clc							//Borra el flag de carry (C) antes de la rotación.

	ror		r15						
	ror		r14					
	ror		r13					
	ror		r12		

	// Si el carry está en 0 (el último bit de r12 era 1 antes de ROR), el código carga 0x80 (10000000) en r15 para reiniciar la rotación cuando la máscara llega a 00000001. Sino, si el carry está en 1 salteo 2 instrucciones
	brcc	PC+3
	ldi		r16, 0x80
	mov		r15, r16

	//almaceno la mascara actualizada
	sts		maskState+0, r12
	sts		maskState+1, r13
	sts		maskState+2, r14
	sts		maskState+3, r15
	ret

;************************************** Like a main in C **************************************
start:
    cli

;Inicializacion de la SRAM
    ldi		r16, low(RAMEND)
    out		SPL, r16
    ldi		r16, HIGH(RAMEND)
    out		SPH, r16

;Llamo las inicializaciones
    call	ini_ports
    call	ini_timer
    call	ini_USART0

;cargo 0 para inicializar variables
	clr		r16
	out		GPIOR0,r16			//Pongo en 0 la bandera

	sts		iwRX, r16
	sts		irRX, r16
	sts		iwTX, r16
	sts		irTX, r16
	
	sts		maskState+0, r16
	sts		maskState+1, r16
	sts		maskState+2, r16

	sts		ledLastState, r16

	ldi		r16, 0x80
	sts		maskState+3, r16

;Activo el estado IDLE desde el arranque
	ldi		r16, low(STATEDIEZ)		// Cargo byte 1. 0x00
	sts		ledstate+0, r16
	ldi		r16, high(STATEDIEZ)	// Cargo byte 2. 0x00
	sts		ledstate+1, r16
	ldi		r16, byte3(STATEDIEZ)	// Cargo byte 3. 0x00
	sts		ledstate+2, r16
	ldi		r16, byte4(STATEDIEZ)	// Cargo byte 4. 0x80
	sts		ledstate+3, r16

	ldi		r16, 5
	sts		time10ms, r16
	ldi		r16,10
	sts		time100ms,r16

	sei

loop:
	sbis	GPIOR0, IS10MS  // This instruction tests a single bit in an I/O Register and skips the next instruction if the bit is set.
	rjmp	testBufRX

;Entra en esta parte cada 10ms
	cbi		GPIOR0, IS10MS
	call	do100ms
	call	doSW0

	sbic	GPIOR0, ISNEWSTATESW0
	call	doNewStateSW0


; ***************************** USART *****************************
; Procesar buffer RX
testBufRX:							; Revisa si hay datos en bufRX 
    lds		r17, iwRX
    lds		r16, irRX
    cp		r16, r17
    breq	testBufTX	
    call	doRX					; Si se recibio algo, lo procesa en doRx 

; Procesar buffer TX
testBufTX:				; Verifica si hay datos en bufTX para enviarlos por USART.
    lds		r17, iwTX
    lds		r16, irTX
    cp		r16, r17
    brne	PC+2
    jmp		loop

;**** Enviar datos por USART ****
doTX:
	lds		r17, UCSR0A
	sbrs	r17, UDRE0  ; Esperar hasta que el buffer esté libre
	jmp		loop
	ldi		r26, low(bufTX)
	ldi		r27, high(bufTX)
	clr		r17
	add		r26, r16
	adc		r27, r17
	inc		r16
	andi	r16, BUFSIZETX-1
	sts		irTX, r16
	ldi		r16, (1 << TXC0) | (1 << U2X0)
	sts		UCSR0A, r16
	ld		r16, X
	sts		UDR0, r16
	jmp		loop	