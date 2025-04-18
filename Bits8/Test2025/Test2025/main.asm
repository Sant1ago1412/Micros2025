;
; Test2025.asm
;
; Created: 5/3/2025 18:13:53
; Author : Baccon
;
; Replace with your application code
#include <m328pdef.inc>
// puerto serie 115200 , 8 bits de datos sin paridad, 1 bit de estado
;igualdades
.equ	SW0=4
.equ	LEDBUILTIN=5
.equ	IS10MS=6

;definiciones - nombres simb?licos
.def	w=r16
.def	w1=r17
.def	saux=r18
.def	flag1=r19
.def	newButton=r21

/*
;Segmento de EEPROM
.eseg
econfig:	.BYTE	1

;constantes
const2:		.DB 1, 2, 3

*/

;segmento de Datos SRAM
.dseg

timeLed:	.BYTE	2
timeLedOn:	.BYTE	2
timeLedff:	.BYTE	2
timer10ms:	.BYTE	1

;segmento de C?digo
.cseg
.org	0x00
	jmp	start
.org	0x20
	jmp	TIMER0_OVF

.org 0x34	//indica donde finalizan los vector4es de interrupciones
;constantes
;consts:		.DB 0, 255, 0b01010101, -128, 0xaa, 0
;varlist:	.DD 0, 0xfadebabe, -2147483648, 1 << 30
TIMER0_OVF:

	in		r2,STATUS
	push	r2
	push	r16
	lds		r16,time10ms
	dec		r16
	sts		time10ms,r16
	brne	OUT_TMR0_OVF
	sbi		GPIOR0,IS10MS
	ldi		r16,10
	sts		time10ms,r16

OUT_TIMER_OVF:
	pop		r16
	pop		r2
	out		SREG,r2
	reti
;**** Funciones ****
ini_timer0:
	ldi	r16,0
	out	TCCR0A,	r16
	out	TCNT0,	r16
	ldi	r16,0x07
	out TIFR0,r16
	ldi	r16,0x01
	sts	TIMSK0, r16
	ldi	r16,0b00000011
	out	TCCR0B,	r16
	ret

ini_ports:
;ldi solo funciona con los registros del 16 a 31
	ldi	r16,(0 << SW0)|(1 << LEDBUILTIN)
	out DDRB,r16
	ldi r16,(1<<SW0)
	out PORTB,r16
	ret

;ddrb 1 entrada

delay1ms:

	ldi r24,low(3997)
	ldi r25,high(3997)

delayloop:
	
	//vuelvo a unir 
	sbiw	r25:r24,1
	brne	delayloop //brne vuelve a entrar a delay loop si la bandera z esta en 0 
	//nop es una fucnion que no hace nada y ocupa 1 ciclo 
	nop
	nop
	nop
	ret

Timer900:
	//como es un word y los registros son de 8 bits, los separo en 2
	ldi	r30,low(900)
	ldi r31,high(900)
	ret

Timer100:
	ldi r28,low(100)
	ldi r29,high(100)
	ret

;Like a main in C
start:
	cli
	//aca guardar en los registros puntero de la pila en sph y spl para saber en donde comienza la ram y no perder la parte del boooteo del programa

	ldi		r16,low(RAMEND)
	out		SPL,r16
	ldi		r16,high(RAMEND)
	out		SPH,r16

	call	ini_ports

	call	ini_timer0

	ldi		r16, 0
	sts		timeLed+0, r16
	sts		timeLed+1, r16	//cargo 0 en la sram 

	rcall	Timer100
	sts		timeLedOn+0, r28 //cargo la 100 en timer led on 
	sts		timeLedOn+1, r29

	rcall Timer900
/*	sts		timeLedOff+0, r30 //cargo la 900 en timer led off
	sts		timeLedOff+1, r31*/

	sbi	PORTB, LEDBUILTIN	//pongo un 1 en portb en la posicion 5

;	call	ini_serie0
loop:
	
;predner 100 y apagar 900 segundos el led
;presionar una tecla e invertir los tiempos
;si dejo la tecla presionada por mas de 3 segundos se apaga y no enciende hasta volver a presionar
	/*sbis	TIFR0,TOV0
	rjmp	loop
	sbi		TIFR0,TOV0*/
	
	sbis	GPIOR0, IS10MS
	rjmp	loop
	cbi		GPIOR0, IS10MS
		
	//call	delay1ms
	lds		r26, timeLed+0
	lds		r27, timeLed+1 //cargo desde la sram el valor de time led (tiempo encendido) a los registros
	adiw	r26, 1		//sumo 1 al word de r26 y r27
	sts		timeLed+0, r26	//cargo a la sram, el valor de los registros
	sts		timeLed+1, r27
	ldi	r16, low(1000)	//cargo en los registros el valor bajo de 1000
	ldi	r17, high(1000)	//alto
	cp	r16, r26		//comparo el valor actual de tiempo del led con el registro de 1000
	cpc	r17, r27		//comparo la parte alta del byte

	brne	doLed		//si la bandera Z = 0 entra a do led (caso de que la comparacion anterior ambos sean desiguales)

	clr	r16				//limpio el r16
	sts	timeLed+0, r16	//vuelvo a cargar a la sram
	sts	timeLed+1, r16
	jmp	loop

doLed:

	lds	r16, timeLedOn+0 //guardo el tiempo que deberia tner
	lds	r17, timeLedOn+1

	cp	r16, r26 //comparo con el tiempo que lleva 
	cpc	r17, r27

	cbi	PORTB, LEDBUILTIN

	
