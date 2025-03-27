;
; Actividad1_Baccon.asm
;
; Created: 17/3/2025 11:38:34
; Author : Baccon
;
#include <m328pdef.inc>
;ldi guarda direcciones, lds guarda valores en la direccion
;Igualdades, definiciones
.equ	LEDBUILTIN=PB5
.equ	SW0 = PB4
.equ	ms100=100	;tiempo que el led debe quedar encendido
.equ	ms900=1000	;tiempo inicial que el led debe quedar encendido
.equ	STATEIDLE = 1
.equ	STATEUNO = 100
.equ	STATEDOS = 200
.equ	STATETRES = 300
.equ	STATECUATRO = 400
.equ	STATECINCO = 500
.equ	STATESEIS = 600
.equ	STATESIETE = 700
.equ	STATEOCHO = 800
.equ	STATENUEVE = 900
.equ	STATEDIEZ = 1000
.equ	BUFSIZERX = 32
.equ	BUFSIZETX = 32

;**** GPIOR0 como regsitros de banderas ****
.equ	LastStateSW0=0		;GPIOR0<0>: ultimo estado del pulsador
;GPIOR0<1>: 
;GPIOR0<2>:  
;GPIOR0<3>:  
;GPIOR0<4>:  
.equ	IS10MS = 5			;GPIOR0<5>: 10ms 
.equ	ISNEWSTATESW0 = 7	;GPIOR0<7>: SW0 un nuevo estado
;****	
;reserva datos en la SRAM
.dseg
timeLedOn:	  .BYTE	2	;uso para guardar el valor del tiempo que led debe quedar encendido
time100ms:	  .BYTE	1	;uso para saber si ya pasaron 100ms
timeLedOff:	  .BYTE	2	;para saber el tiempo que el led debe permanecer apagado
timercycle:	  .BYTE	2	;contador para saber el ciclo del timer
dbSW0:		  .BYTE	1
stateCounter: .BYTE	1

bufRX:		.BYTE	BUFSIZERX
iwRX:		.BYTE	1
irRX:		.BYTE	1
bufTX:		.BYTE	BUFSIZETX
iwTX:		.BYTE	1
irTX:		.BYTE	1
;debounceCycle	.BYTE 1	;contador para el estado del debounce

.cseg
.org	0x00
	jmp	start

.org	0x20
	jmp	TIMER0_OVF

.org	0x24
	jmp	USART_RXC

.org	0x34

TIMER0_OVF:;interrupcion a la que va a entrar 1 de 256 clocks (prescaler)

	in		r2,SREG		 ;cargo en r2, todos los valores de SREG que me guarda todos los estados de las banderas del micro (registro de estado)
	push	r2			 ;guardo el estado de SREG en la pila para no tener alteraciones por la interrupcion 
	push	r16			 ;como es una interrupcion guardo el valor de r16 por si tenia algo guardado
	
	lds		r16,time100ms
	dec		r16
	sts		time100ms,r16 ;3 lineas para restar 1 a time100ms :(

	brne	OUT_TIMER0_OVF ;entra a esta funcion siempre que time10ms no haya llegado a 0

	;si dio 0 hago lo siguiente ,osea que llego a los 10ms

	sbi		GPIOR0, IS10MS	;seteo en 1 el bit 5 de gpior0 (indica si pasaron los 10ms)
	ldi		r16,50
	sts		time100ms,r16 ;reincio el contador en 50 (cantidad de veces que necesito para que cuente 100ms)

OUT_TIMER0_OVF:

	pop		r16
	pop		r2
	out		SREG,r2 ;devuelve el estado origianl de SREG (porque usamos la bandera z en la interrupcion)
	reti

USART_RXC:
	in	r2, SREG
	push	r2
	push	r16
	push	r17
	push	r26
	push	r27
	ldi	r26, low(bufRX)
	ldi	r27, high(bufRX)
	lds	r16, iwRX
	clr	r17
	add	r26, r16
	adc	r27, r17
	lds	r17, UDR0
	st	X, r17
	inc	r16
	andi	r16, BUFSIZERX-1
	sts	iwRX, r16
OUT_USART_RXC:
	pop	r27
	pop	r26
	pop	r17
	pop	r16
	pop	r2
	out	SREG, r2
	reti
ini_timer0:

	ldi	r16, (1<<WGM01)	//0b00000010	timer en modo CTC
	out	TCCR0A, r16

	clr	r16
	out	TCNT0, r16
	out	TIFR0, r16

	ldi	r16, (1 << OCIE0A)	//0b00000010
	sts	TIMSK0, r16

	ldi	r16, (1<<CS02)	//0b00000100	cargo el prescaler en 256
	out	TCCR0B, r16

	ldi	r16, 124	//cargo el total de tiempos del clock para la interrupcion 
	out	OCR0A ,r16

	ret

ini_ports:
	
	ldi	r16, (1 << LEDBUILTIN) | (0 << SW0)
	out	DDRB, r16
	ldi	r16, (1 << SW0)	;activamos pull up interno 
	out	PORTB, r16
	ret

ini_USART0:
	ldi	r16, 0xFE
	sts	UCSR0A, r16
	ldi	r16, (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0)
	sts	UCSR0B, r16
	ldi	r16, 0x06
	sts	UCSR0C, r16
	ldi	r16, 16
	sts	UBRR0L, r16
	ldi	r16, 0
	sts	UBRR0H, r16
	ret

start:
    cli

	;le doy el valor del inicio de la ram al puntero de pila para no perder espacio en ram 
	ldi		r16,low(RAMEND)
	out		SPL,r16
	ldi		r16,high(RAMEND)
	out		SPH,r16

	;inicio puertos y timers
	call	ini_ports
	call	ini_timer0
	call	ini_USART0

	;cargo 0 para inicializar variables
	clr		r16
	out		GPIOR0,r16

	ldi		r16,50
	sts		time100ms,r16
	;cargo el tiempo que el led debe quedar encendido
	ldi		r16, low(ms100)
	sts		timeLedOn+0, r16
	ldi		r16, high(ms100)
	sts		timeLedOn+1, r16

	ldi		r16, low(ms900)
	sts		timeLedOff+0, r16
	ldi		r17, high(ms900)
	sts		timeLedOff+1, r17

	clr		r16
	sts		timercycle+0, r16
	sts		timercycle+1, r16
	sts		stateCounter, r16
	;enciendo el led inicialmente
	sbi		PORTB, LEDBUILTIN	
	sbic	PINB, SW0
	sbi		GPIOR0, LastStateSW0
	;Vuelvo a habilitar las interrupciones
    sei	

loop:

	;para saber si hubo una interrupcion se levanta una bandera en gpior0 en ea posicion is10ms
/*	in		r16,GPIOR0
	ldi		r17,(1<<IS10MS)
	and		r16,r17;hago esto para modificar las demas banderas y me queda solo en 1 is10ms
	cp		r16,r17
	breq	OnOff	*/

	call	doSW0
/*	sbis	GPIOR0, ISNEWSTATESW0
	rjmp	SW0Pressed1*/
	cbi		GPIOR0, ISNEWSTATESW0
	sbis	GPIOR0, LastStateSW0
	rjmp	SW0Pressed0
;	sbi		PORTB, LEDBUILTIN
	sbic	GPIOR0, ISNEWSTATESW0
	call	doNewStateSW0
	;si es igual significa que hubo una interrupcion, va a scock para sumar 1 hata llegar a 5
	//aca se puede usar sbic para saltear el t100ms
	call	switchState

	jmp loop

doLed:

	in		r16,PORTB
	ldi		r17,(1<<LEDBUILTIN)
	eor		r16, r17	
	out		PORTB,r16

	jmp		loop

OnOff:

	cbi		GPIOR0,IS10MS

	lds		r24,timercycle+0
	lds		r25,timercycle+1

	;tengo que usar un registro auxiliar, porque adiw no me deja sumar 100 
	ldi		r16,100

	clr		r0

	add		r24,r16
	adc		r25,r0
	sts		timercycle+0,r24
	sts		timercycle+1,r25

	lds		r16,timeLedOn+0
	lds		r17,timeLedOn+1

	cp		r24,r16
	cpc		r25,r17

	breq	doLed

	lds		r16,timeLedOff+0
	lds		r17,timeLedOff+1

	cp		r24,r16
	cpc		r25,r17
	
	;tengo que saber si la comparacion anterior dio 0 para resetear el timercicle
	breq	CycleReset

	jmp		loop
	
CycleReset:
	
	clr		r16
	sts		timercycle+0,r16
	sts		timercycle+1,r16
	rjmp	doled

doSW0:
	lds		r16, dbSW0
	sbic	GPIOR0, LastStateSW0
	rjmp	testLowSW0
	//en caso de que el estado anterior 
	sbis	PINB, SW0
	rjmp	doSW0db1
	sbi		GPIOR0, LastStateSW0
	rjmp	doSW0db0

testLowSW0:
	sbic	PINB, SW0
	rjmp	doSW0db1
	cbi	GPIOR0, LastStateSW0

doSW0db0:
	ldi	r16, 6

doSW0db1:
	tst	r16
	breq	outDoSW0
	dec	r16
	brne	outDoSW0
	sbi	GPIOR0, ISNEWSTATESW0	

outDoSW0:
	sts	dbSW0, r16
	ret

SW0Pressed0:
	
	sbis	GPIOR0, LastStateSW0
	cbi		PORTB, LEDBUILTIN
	sbic	GPIOR0, LastStateSW0
	sbi		PORTB, LEDBUILTIN

PutConstTextOnBufTX:
	push	r16
	push	r17
	push	r26
	push	r27
	lds	r16, iwTX

nextByteText:
	ldi	r27, high(bufTX)
	ldi	r26, low(bufTX)
	add	r26, r16
	brcc	PC+2
	subi	r27, -1
	inc	r16
	andi	r16, BUFSIZETX-1
	lpm	r17, Z+
	cpi	r17, '\0'
	breq	outPutConstText		
	st	X, r17
	brne	nextByteText

outPutConstText:
	dec	r16
	andi	r16, BUFSIZETX-1
	sts	iwTX, r16
	pop	r27
	pop	r26
	pop	r17
	pop	r16
	ret

doNewStateSW0:

	cbi		GPIOR0, ISNEWSTATESW0
	sbic	GPIOR0, LASTSTATESW0
	ret

	
	lds		r16,stateCounter
	inc		r16
	sts		stateCounter,r16


	ret

switchState:
	
	lds		r17,stateCounter

	cpi		r17,0
	breq	State0

	cpi		r17,1
	breq	State1

	cpi		r17,2
	breq	State2

	cpi		r17,3
	breq	State3

	cpi		r17,4
	breq	State4

	cpi		r17,5
	breq	State5

	cpi		r17,6
	breq	State6

	cpi		r17,7
	breq	State7

	cpi		r17,8
	breq	State8

	cpi		r17,9
	breq	State9

	cpi		r17,10
	breq	State10

State0:
	
	ldi		r16,low(STATEUNO)
	ldi		r17,high(STATEUNO)

	sts		timeledOff+0,r16
	sts		timeledOff+1,r17
	sts		timeledOn+0,r16
	sts		timeledOn+1,r17

	jmp loop

State1:
	
	ldi		r16,low(STATEUNO)
	ldi		r17,high(STATEUNO)

	sts		timeledOff+0,r16
	sts		timeledOff+1,r17
	sts		timeledOn+0,r16
	sts		timeledOn+1,r17

	jmp loop

State2:
	
	ldi		r16,low(STATEDOS)
	ldi		r17,high(STATEDOS)

	sts		timeledOff+0,r16
	sts		timeledOff+1,r17
	sts		timeledOn+0,r16
	sts		timeledOn+1,r17

	jmp loop

State3:
	
	ldi		r16,low(STATETRES)
	ldi		r17,high(STATETRES)

	sts		timeledOff+0,r16
	sts		timeledOff+1,r17
	sts		timeledOn+0,r16
	sts		timeledOn+1,r17

	jmp loop

State4:
	
	ldi		r16,low(STATECUATRO)
	ldi		r17,high(STATECUATRO)

	sts		timeledOff+0,r16
	sts		timeledOff+1,r17
	sts		timeledOn+0,r16
	sts		timeledOn+1,r17

	jmp loop

State5:
	
	ldi		r16,low(STATECINCO)
	ldi		r17,high(STATECINCO)

	sts		timeledOff+0,r16
	sts		timeledOff+1,r17
	sts		timeledOn+0,r16
	sts		timeledOn+1,r17

	jmp loop

State6:
	
	ldi		r16,low(STATESEIS)
	ldi		r17,high(STATESEIS)

	sts		timeledOff+0,r16
	sts		timeledOff+1,r17
	sts		timeledOn+0,r16
	sts		timeledOn+1,r17

	jmp loop

State7:
	
	ldi		r16,low(STATESIETE)
	ldi		r17,high(STATESIETE)

	sts		timeledOff+0,r16
	sts		timeledOff+1,r17
	sts		timeledOn+0,r16
	sts		timeledOn+1,r17

	jmp loop

State8:
	
	ldi		r16,low(STATEOCHO)
	ldi		r17,high(STATEOCHO)

	sts		timeledOff+0,r16
	sts		timeledOff+1,r17
	sts		timeledOn+0,r16
	sts		timeledOn+1,r17

	jmp loop

State9:
	
	ldi		r16,low(STATENUEVE)
	ldi		r17,high(STATENUEVE)

	sts		timeledOff+0,r16
	sts		timeledOff+1,r17
	sts		timeledOn+0,r16
	sts		timeledOn+1,r17

	jmp loop

State10:
	
	ldi		r16,low(STATEDIEZ)
	ldi		r17,high(STATEDIEZ)

	sts		timeledOff+0,r16
	sts		timeledOff+1,r17
	sts		timeledOn+0,r16
	sts		timeledOn+1,r17

	jmp loop