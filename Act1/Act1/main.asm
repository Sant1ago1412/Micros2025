;
; Act1.asm
;
; Created: 12/3/2025 17:44:59
; Author : Baccon
;


; Replace with your application code
#include <m328pdef.inc>

;igualdades
.equ	LEDBUILTIN = PB5
.equ	SW0 = PB4

;**** GPIOR0 como regsitros de banderas ****
.equ	LASTSTATESW0 = 0	;GPIOR0<0>: ultimo estado del pulsador
;GPIOR0<1>: 
;GPIOR0<2>:  
;GPIOR0<3>:  
;GPIOR0<4>:  
.equ	IS10MS = 5		;GPIOR0<5>: 10ms 
.equ	TOGGLEOFF = 6		;GPIOR0<6>: toggle del LED desactivado 
.equ	ISNEWSTATESW0 = 7	;GPIOR0<7>: SW0 un nuevo estado
;****	

;segmento de Datos SRAM
.dseg
timeLed:	.BYTE	2
timeLedOn:	.BYTE	2
timeSW03s:	.BYTE	2
dbSW0:		.BYTE	1
time10ms:	.BYTE	1



;segmento de C?digo
.cseg

.org	0x00
	jmp	start

.org	0x20
	jmp	TMR0_OVF
/*.org	0x24
	jmp USART_RX
*/
.org	0x34
;**** INTERRUPTS ****
/*TMR0_OVF:
	in	r2, SREG
	push	r2
	push	r16
	lds	r16, time10ms
	dec	r16
	sts	time10ms, r16
	brne	OUT_TMR0_OVF
	sbi	GPIOR0, IS10MS
	ldi	r16, 10
	sts	time10ms, r16

OUT_TMR0_OVF:
	pop	r16
	pop	r2
	out	SREG, r2
	reti	

;**** Funciones ****
ini_ports:
	ldi	r16, (1 << LEDBUILTIN) | (0 << SW0)
	out	DDRB, r16
	ldi	r16, (1 << SW0)	
	out	PORTB, r16
	ret

; fclk/64 = TOV cada 1.023ms
ini_TMR0:

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

ini_usart:

	ldi r16, low(16)
	ldi r17, high(16)
	sts UBRR0H, r17
	sts UBRR0L, r16

	ldi	r16, 0xFE
	sts	UCSR0A, r16

	ldi	r16, 0b10011000
	sts	UCSR0B, r16

	ldi	r16, 0b00000110
	sts	UCSR0C,r16
		
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

doToggleLed:
	push	r16
	push	r17
	push	r24
	push	r25
	lds	r24, timeLed+0
	lds	r25, timeLed+1
	adiw	r24, 1 
	sts	timeLed+0, r24
	sts	timeLed+1, r25
	ldi	r16, low(1000)
	ldi	r17, high(1000)
	cp	r16, r24
	cpc	r17, r25
	brne	doToggle_LedOff
	sbi	PORTB, LEDBUILTIN
	clr	r16
	sts	timeLed+0, r16
	sts	timeLed+1, r16
	rjmp	outDoToggle
doToggle_LedOff:
	lds	r16, timeLedOn+0
	lds	r17, timeLedOn+1
	cp	r16, r24
	cpc	r17, r25
	brne	outDoToggle
	cbi	PORTB, LEDBUILTIN
outDoToggle:
	pop	r25
	pop	r24
	pop	r17
	pop	r16
	ret*/

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
	sbi	GPIOR0, ISNEWSTATESW0	
outDoSW0:
	sts	dbSW0, r16
	ret

;Like a main in C
start:
	cli
/*
	ldi	r16, low(RAMEND)
	out	SPL, r16
	ldi	r16, high(RAMEND)
	out	SPH, r16

	call	ini_ports
	call	ini_TMR0
	call	ini_usart

	ldi	r16, 0
	sts	timeLed+0, r16
	sts	timeLed+1, r16
	sts	timeSW03s+0, r16
	sts	timeSW03s+1, r16
	ldi	r16, low(100)
	sts	timeLedOn+0, r16
	ldi	r16, high(100)
	sts	timeLedOn+1, r16*/
	ldi	r16, 0
	sts	dbSW0, r16
	ldi	r16, 10
	sts	time10ms, r16
	out	GPIOR0, r16
	sbic	PINB, SW0
	sbi	GPIOR0, LASTSTATESW0
	sbi	PORTB, LEDBUILTIN
	sei
loop:
	;call	delay1ms
	sbis	GPIOR0, IS10MS
	rjmp	loop
	cbi	GPIOR0, IS10MS	
;	sbis	TIFR0, TOV0
;	rjmp	loop
;	sbi	TIFR0, TOV0
	sbis	GPIOR0, TOGGLEOFF
	call	doToggleLed

 	call	doSW0
	sbis	GPIOR0, ISNEWSTATESW0
	rjmp	SW0Pressed1
	cbi	GPIOR0, ISNEWSTATESW0
	sbis	GPIOR0, LASTSTATESW0
	rjmp	SW0Pressed0

	cbi	GPIOR0, TOGGLEOFF
	clr	r16
	lds	r24, timeSW03s+0
	lds	r25, timeSW03s+1
	cp	r24, r16
	cpc	r25, r16
	breq	PC+2
	rjmp	changeTimeToggle
	cbi	PORTB, LEDBUILTIN
/*	sbi	GPIOR0, TOGGLEOFF	
	ldi	r16, 0
	sts	timeLed+0, r16
	sts	timeLed+1, r16*/
	rjmp	loop
SW0Pressed0:
	ldi	r24, low(3000)
	ldi	r25, high(3000)
	sts	timeSW03s+0, r24
	sts	timeSW03s+1, r25
	rjmp	loop
SW0Pressed1:
	sbic	GPIOR0, LASTSTATESW0
	rjmp	loop
	clr	r16
	lds	r24, timeSW03s+0
	lds	r25, timeSW03s+1
	cp	r24, r16
	cpc	r25, r16
	brne	PC+2
	rjmp	loop
	sbiw	r24, 1
	sts	timeSW03s+0, r24
	sts	timeSW03s+1, r25
	rjmp	loop
changeTimeToggle:		
	lds	r24, timeLedOn+0
	lds	r25, timeLedOn+1
	ldi	r18, low(100)
	ldi	r19, high(100)
	cp	r24, r18
	cpc	r25, r19
	brne	setLedOnTime
	ldi	r18, low(900)
	ldi	r19, high(900)
setLedOnTime:
	sts	timeLedOn+0, r18
	sts	timeLedOn+1, r19
	jmp	loop	
