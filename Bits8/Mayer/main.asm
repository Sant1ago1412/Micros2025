#include <m328def.inc>
#define FREQ		1000
#define	DUTYC		100

;igualdades
.equ	softversion=0x01
.equ	build=0x04

.equ	SW0 = 4
.equ	LEDBUILTIN = 5
.equ	num = 3997

;** GPIOR0 como regsitros de banderas **
.equ LASTSTATESW0	= 0
.equ TIME		= 1


;definiciones - nombres simb?licos
.def	w=r16

;segmento de Datos SRAM
.dseg
timeLed:	.BYTE	2
timeLedOn:	.BYTE	2

;segmento de C?digo
.cseg
.org	0x00
	jmp	start
/*;interrupciones	
.org	0x16
	jmp	serv_rx0
.org	0x26
	jmp	serv_cmp0
	*/

;constantes
consts:		.DB 0, 255, 0b01010101, -128, 0xaa, 0
varlist:	.DD 0, 0xfadebabe, -2147483648, 1 << 30

;Servicio de interrupciones
serv_rx0:
	reti

serv_cmp0:
	reti

;** Funciones **
ini_ports:
	ldi	r16, (0 << SW0) | (1 << LEDBUILTIN)
	out	DDRB, r16
	ldi	r16, (1 << SW0)
	out	PORTB, r16 //asigna resistencia pull up
	ret

delay1ms:
	ldi	r24, low(num) ; NUM = 2BYTES 
	ldi	r25, high(num)
delayloop: 
	sbiw	r25:r24, 1
	brne	delayloop
	nop
	nop
	nop
	ret
	
resetTimer:
	cbi	GPIOR0, TIME
	clr	r16
	sts	timeLed+0, r16
	sts	timeLed+1, r16
	ret

/*PRESSEDSW0:	//pinb en 0
	cbi	GPIOR0, LASTSTATESW0 //seteo el gpio en 0
	ret	*/
/*
UNPRESSEDSW0:	//pinb en 1
	sbis	GPIOR0, LASTSTATESW0	//si antes estaba presionado (o sea en 0) 
	rjmp	TOGGLE					//cambio de estado, sinó nada (siga)
	jmp	go
TOGGLE:		// entro a esta funcion solo si laststate=0 y pinb =1
	sbi	GPIOR0, LASTSTATESW0	//seteo como prendido el botón
	sbi	GPIOR0, TIME		//seteo la bandera de "resetear tiempo <<TIME>>"
	call	changeLed		//genero un cambio de estado con reseteo de tiempo por la anterior bandera
*/
changeLed: 
	ldi	r21, (1<<LEDBUILTIN)
	in	r20, PINB
	eor	r20, r21
	out	PORTB, r20

	sbic	GPIOR0, TIME
	call	resetTimer
	jmp	loop
	
;Like a main in C
start:
	cli
	call	ini_ports
	ldi	r16, 0

	sts	timeLed+0, r16 ;sts guarda en la sram (flash) 
	sts	timeLed+1, r16
/*
	ldi	r16, low(DUTYC)
	sts	timeLedOn+0, r16
	ldi	r16, high(DUTYC)
	sts	timeLedOn+1, r16
*/
	cbi	GPIOR0, TIME
;	cbi	GPIOR0, LASTSTATESW0
	sbi	PORTB, LEDBUILTIN

;	sbic	PINB, SW0
;	sbi	GPIOR0, LASTSTATESW0
loop:
	call	delay1ms

	lds	r26, timeLed+0
	lds	r27, timeLed+1
	adiw	r26, 1 ;sumo el tiempo +1
	sts	timeLed+0, r26
	sts	timeLed+1, r27 ;lo guardo en la sram

	//base logica => pinb sw0 = 0 cuando esá pulsado
/*	sbic	PINB, SW0	//si pinb seteado 1
	rjmp	UNPRESSEDSW0	//entro a no presionado
	sbis	PINB, SW0	//si pinb 0
	cbi	GPIOR0, LASTSTATESW0
	/rjmp	PRESSEDSW0/	//entro a presionado
go:
*/
	//este segmento entra en change led si timeled = 1000 || 100 ms
	ldi	r16, low(DUTYC)
	ldi	r17, high(DUTYC)
	cp	r16, r26	
	cpc	r17, r27
	breq	changeLed
	sbi	GPIOR0, TIME
	ldi	r16, low(FREQ)
	ldi	r17, high(FREQ)
	cp	r16, r26	;Si R16 < R26 => C=1 ^ Z=0, Si R16 = R26 => C=0 ^ Z=1, Si R16 > R26 => Z=0 ^ C=0;
	cpc	r17, r27	;Si el anterior Z = 0 entonces Z = 0, Sinó se cumplen las condiciones antes mencionadas
	breq	changeLed	;Si Z!=0 entra, sinó sigue
	cbi	GPIOR0, TIME

	jmp	loop