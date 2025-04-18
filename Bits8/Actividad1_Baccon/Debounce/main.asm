;
; Debounce.asm
;
; Created: 19/3/2025 11:17:15
; Author : Baccon
;
#include <m328pdef.inc>

;Igualdades, definiciones
.equ	LEDBUILTIN=PB5
.equ	SW0 = PB4
.equ	ms100=100	;tiempo que el led debe quedar encendido
.equ	ms900=1000	;tiempo inicial que el led debe quedar encendido

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
timeLedOn:	.BYTE	2	;uso para guardar el valor del tiempo que led debe quedar encendido
time100ms:	.BYTE	1	;uso para saber si ya pasaron 100ms
timeLedOff:	.BYTE	2	;para saber el tiempo que el led debe permanecer apagado
timercycle:	.BYTE	2	;contador para saber el ciclo del timer
dbSW0:		.BYTE	1
;debounceCycle	.BYTE 1	;contador para el estado del debounce

; Replace with your application code
.cseg
.org	0x00
	jmp	start

.org	0x34

ini_ports:
	
	ldi	r16, (1 << LEDBUILTIN) | (0 << SW0)
	out	DDRB, r16
	ldi	r16, (1 << SW0)	;activamos pull up interno 
	out	PORTB, r16
	ret

start:
	call	ini_ports
	sbi		PORTB, LEDBUILTIN	
	sbic	PINB, SW0
	sbi		GPIOR0, LastStateSW0

loop:
	call	doSW0
	cbi		GPIOR0, ISNEWSTATESW0
	sbis	GPIOR0, LastStateSW0
	rjmp	SW0Pressed0

	jmp		loop

doSW0:
	lds		r16, dbSW0
	sbic	GPIOR0, LastStateSW0
	rjmp	testLowSW0
	sbis	PINB, SW0
	rjmp	doSW0db1
	sbi		GPIOR0, LastStateSW0
	rjmp	doSW0db0
testLowSW0:
	sbic	PINB, SW0
	rjmp	doSW0db1
	cbi	GPIOR0, LastStateSW0
doSW0db0:
	ldi	r16, 50
doSW0db1:
	tst	r16
	breq	outDoSW0
	dec	r16
	brne	outDoSW0
	sbi	GPIOR0, LastStateSW0	
outDoSW0:
	sts	dbSW0, r16
	ret

SW0Pressed0:
	
	sbis	GPIOR0, LastStateSW0
	cbi		PORTB, LEDBUILTIN
	sbic	GPIOR0, LastStateSW0
	sbi		PORTB, LEDBUILTIN
