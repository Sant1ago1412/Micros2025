#include <m328pdef.inc>

;igualdades
.equ	LEDBUILTIN = PB5
.equ	SW0 = PB4

;**** GPIOR0 como regsitros de banderas ****
.equ	LASTSTATESW0 = 0 
;GPIOR0<0>: ultimo estado del pulsador
;GPIOR0<1>: 
;GPIOR0<2>:  
;GPIOR0<3>:  
;GPIOR0<4>:  
;GPIOR0<5>:  
;GPIOR0<6>:  
;GPIOR0<7>:  
;****	

;segmento de Datos SRAM
.dseg
timeLed:	.BYTE	2
timeLedOn:	.BYTE	2
dbSW0:		.BYTE	1

;segmento de C?digo

.cseg
.org	0x00
	jmp	start

.org	0x34

;**** Funciones ****

ini_ports:

	ldi	r16, (1 << LEDBUILTIN) | (0 << SW0)//pongo un 1 en el bit 5 para inicializar el puerto
	out	DDRB, r16//cargo ese valor en DDRB 
	ldi	r16, (1 << SW0)//pongo un 1 en el bit 4 para cargarlo en portb
	out	PORTB, r16
	ret


delay1ms:
	ldi	r24, low(3997)
	ldi	r25, high(3997)
delay1ms_loop:
	sbiw	r25:r24, 1
	brne	delay1ms_loop
	nop
	nop
	nop
	ret

;Like a main in C
start:
	cli
	call	ini_ports
	ldi	r16, 0
	sts	timeLed+0, r16
	sts	timeLed+1, r16
	ldi	r16, low(100)
	sts	timeLedOn+0, r16
	ldi	r16, high(100)
	sts	timeLedOn+1, r16
	ldi	r16, 0
	sts	dbSW0, r16
	cbi	GPIOR0, LASTSTATESW0 //hacemos 0 el registro gpio para la inicializacion del programa
	sbic	PINB, SW0	//skipea si el bit sw0 es 0 en pin b
	sbi	GPIOR0, LASTSTATESW0
	sbi	PORTB, LEDBUILTIN
loop:
	call	delay1ms
	lds	r26, timeLed+0
	lds	r27, timeLed+1
	adiw	r26, 1 
	sts	timeLed+0, r26
	sts	timeLed+1, r27
	ldi	r16, low(1000)
	ldi	r17, high(1000)
	cp	r16, r26
	cpc	r17, r27
	brne	doLed
	sbi	PORTB, LEDBUILTIN
	clr	r16
	sts	timeLed+0, r16
	sts	timeLed+1, r16
	rjmp	doSW0
doLed:
	lds	r16, timeLedOn+0
	lds	r17, timeLedOn+1
	cp	r16, r26
	cpc	r17, r27
	brne	doSW0
	cbi	PORTB, LEDBUILTIN
doSW0:
	lds	r16, dbSW0
	sbic	GPIOR0, LASTSTATESW0
	rjmp	testLowSW0
	sbis	PINB, SW0
	rjmp	doSW0db1
	sbi	GPIOR0, LASTSTATESW0
	rjmp	doSW0db0
testLowSW0:
	sbic	PINB, SW0
	rjmp	doSW0db1
	cbi	GPIOR0, LASTSTATESW0
doSW0db0:
	ldi	r16, 50
doSW0db1:
	tst	r16
	breq	outDoSW0
	dec	r16
	brne	outDoSW0	
	sbic	GPIOR0, LASTSTATESW0
	rjmp	outDoSW0
	lds	r26, timeLedOn+0
	lds	r27, timeLedOn+1
	ldi	r18, low(100)
	ldi	r19, high(100)
	cp	r26, r18
	cpc	r27, r19
	brne	setLedOnTime
	ldi	r18, low(900)
	ldi	r19, high(900)
setLedOnTime:
	sts	timeLedOn+0, r18
	sts	timeLedOn+1, r19
outDoSW0:
	sts	dbSW0, r16