;
; PruebaLedTimer.asm
;
; Created: 13/3/2025 12:09:23
; Author : Baccon
;
#include <m328pdef.inc>

;Igualdades, definiciones
.equ	LEDBUILTIN=PB5
.equ	IS10MS=5	;veces que se debe desbordar el vector de la interrupcion del timer par acontar 100ms
.equ	ms100=100	;tiempo que el led debe quedar encendido
.equ	ms900=900	;tiempo inicial que el led debe quedar encendido
.equ	SW0 = PB4
.equ	Flag1000=1	;indica cuando paso 1 segundo para resetear el timer

;reserva datos en la SRAM
.dseg
timeLedOn:	.BYTE	2	;uso para guardar el valor del tiempo que led debe quedar encendido
time100ms:	.BYTE	1	;uso para saber si ya pasaron 100ms
timeLedOff:	.BYTE	2	;para saber el tiempo que el led debe permanecer apagado
timercycle:	.BYTE	2	;contador para saber el ciclo del timer

.cseg
.org	0x00
	jmp	start

.org	0x20
	jmp	TIMER0_OVF

.org	0x34

TIMER0_OVF:;interrupcion a la que va a entrar 1 de 256 clocks (prescaler)

	in		r2,SREG		 ;cargo en r2, todos los valores de SREG que me guarda todos los estados de las banderas del micro (registro de estado)
	push	r2			 ;guardo el estado de SREG en la pila para no tener alteraciones por la interrupcion 
	push	r16			 ;como es una interrupcion guardo el valor de r16 por si tenia algo guardado
	
	lds		r16,time100ms
	dec		r16
	sts		time100ms,r16 ;3 lineas para restar 1 a time10ms :(

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
	ldi	r16, (1 << SW0)
	out	PORTB, r16
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

	;cargo 0 para inicializar variables
	clr		r16
	out		GPIOR0,r16
	sts		dbSW0, r16

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
	;enciendo el led inicialmente
	sbi		PORTB, LEDBUILTIN	
	cbi		PORTB, SW0

	;Vuelvo a habilitar las interrupciones
    sei	

loop:

	;para saber si hubo una interrupcion se levanta una bandera en gpior0 en ea posicion is10ms
	in		r16,GPIOR0
	ldi		r17,(1<<IS10MS)
	and		r16,r17;hago esto para modificar las demas banderas y me queda solo en 1 is10ms
	cp		r16,r17
	breq	OnOff	

	;si es igual significa que hubo una interrupcion, va a scock para sumar 1 hata llegar a 5
	//aca se puede usar sbic para saltear el t100ms

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