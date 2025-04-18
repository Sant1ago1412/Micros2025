;
; USART.asm
;
; Created: 20/3/2025 11:31:09
; Author : Baccon
;


; Replace with your application code
#include <m328pdef.inc>

;igualdades
.equ	LEDBUILTIN = PB5
.equ	SW0 = PB4

.equ	BUFSIZERX = 32
.equ	BUFSIZETX = 32

.equ	STATEIDLE = 0x80000000
.equ	STATEUNO = 0xF100AA00

;**** GPIOR0 como regsitros de banderas ****
.equ	LASTSTATESW0 = 0	;GPIOR0<0>: ultimo estado del pulsador
;GPIOR0<1>: 
;GPIOR0<2>:  
;GPIOR0<3>:  
;GPIOR0<4>:  
.equ	IS10MS = 5		;GPIOR0<5>: is 10ms 
;GPIOR0<6>:
.equ	ISNEWSTATESW0 = 7	;GPIOR0<7>: SW0 un nuevo estado
;****	

;segmento de Datos SRAM
.dseg
bufRX:		.BYTE	BUFSIZERX
iwRX:		.BYTE	1
irRX:		.BYTE	1
bufTX:		.BYTE	BUFSIZETX
iwTX:		.BYTE	1
irTX:		.BYTE	1
time10ms:	.BYTE	1
time100ms:	.BYTE	1
dbSW0:		.BYTE	1
maskState:	.BYTE	4
ledState:	.BYTE	4



;segmento de C?digo
.cseg
.org	0x00
	jmp	start
.org	0x1C
	jmp	TIM0_COMPA
.org	0x24
	jmp	USART_RXC

.org	0x34
;**** CONSTANTS in FLASH ****
version:	.db "20250314_01b01",'\n','\0'
msgTest:	.db "TEST",'\n','\0'
msgStateIdle:	.db "STATE IDLE",'\n','\0'
msgStateUno:	.db "STATE UNO",'\n','\0','\0'

;**** INTERRUPTS ****
TIM0_COMPA:
	in	r2, SREG
	push	r2
	push	r16
	lds	r16, time10ms
	dec	r16
	sts	time10ms, r16
	brne	OUT_TIM0_COMPA
	sbi	GPIOR0, IS10MS
	ldi	r16, 5
	sts	time10ms, r16
OUT_TIM0_COMPA:
	pop	r16
	pop	r2
	out	SREG, r2
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

;**** Funciones ****
ini_ports:
	ldi	r16, (1 << LEDBUILTIN) | (0 << SW0)
	out	DDRB, r16
	ldi	r16, (1 << SW0)
	out	PORTB, r16
	ret

; fclk/256 - Mode CTC - OCR0A: 125 -> 2ms
ini_TMR0:
	ldi	r16, 0
	out	TCNT0, r16
	ldi	r16, 0x02
	out	TCCR0A, r16
	ldi	r16, 0x07
	out	TIFR0, r16
	ldi	r16, 0x02
	sts	TIMSK0, r16
	ldi	r16, 125
	out	OCR0A, r16
	ldi	r16, 0b00000100
	out	TCCR0B, r16
	ret

;115200,8,1,none,none
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

;INPUT R30, R31 the address start of Constant text in flash
;The string must terminate with a null character
;The null character is not sent
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
	cbi	GPIOR0, ISNEWSTATESW0
	sbic	GPIOR0, LASTSTATESW0
	ret
	lds	r16, ledState+3
	cpi	r16, 0x80
	breq	setNewState
	ldi	r30, low(msgStateIdle << 1)
	ldi	r31, high(msgStateIdle << 1)
	call	PutConstTextOnBufTX
	ldi	r16, low(STATEIDLE)
	ldi	r17, high(STATEIDLE)
	ldi	r18, byte3(STATEIDLE)
	ldi	r19, byte4(STATEIDLE)
	rjmp	outNewState
setNewState:
	ldi	r30, low(msgStateUno << 1)
	ldi	r31, high(msgStateUno << 1)
	call	PutConstTextOnBufTX
	ldi	r16, low(STATEUNO)
	ldi	r17, high(STATEUNO)
	ldi	r18, byte3(STATEUNO)
	ldi	r19, byte4(STATEUNO)
outNewState:
	sts	ledState+0, r16
	sts	ledState+1, r17
	sts	ledState+2, r18
	sts	ledState+3, r19
	ret

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

doRX:
	lds	r16, irRX
	ldi	r26, low(bufRX)
	ldi	r27, high(bufRX)
	clr	r17
	add	r26, r16
	adc	r27, r17
	inc	r16
	andi	r16, BUFSIZERX-1
	sts	irRX, r16
	ld	r16, X
doRX_V:	;if 'V' is received, the firmware version is answered  
	cpi	r16, 'V'
	breq	PC+2
	rjmp	doRX_T
	ldi	r30, low(version << 1)
	ldi	r31, high(version << 1)
	call	PutConstTextOnBufTX
	ret
doRX_T: ;if 'T' is received, the test message is answered
	cpi	r16, 'T'
	breq	PC+2
	rjmp	doRX_S
	ldi	r30, low(msgTest << 1)
	ldi	r31, high(msgTest << 1)
	call	PutConstTextOnBufTX
	ret
doRX_S: ;if 'S' is received, change ledState
	cpi	r16, 'S'
	breq	PC+2
	ret
	lds	r16, ledState+3
	cpi	r16, 0x80
	breq	setStateUno
	ldi	r30, low(msgStateIdle << 1)
	ldi	r31, high(msgStateIdle << 1)
	call	PutConstTextOnBufTX
	ldi	r16, low(STATEIDLE)
	ldi	r17, high(STATEIDLE)
	ldi	r18, byte3(STATEIDLE)
	ldi	r19, byte4(STATEIDLE)
	rjmp	outDoRX_S
setStateUno:
	ldi	r30, low(msgStateUno << 1)
	ldi	r31, high(msgStateUno << 1)
	call	PutConstTextOnBufTX
	ldi	r16, low(STATEUNO)
	ldi	r17, high(STATEUNO)
	ldi	r18, byte3(STATEUNO)
	ldi	r19, byte4(STATEUNO)
outDoRX_S:
	sts	ledState+0, r16
	sts	ledState+1, r17
	sts	ledState+2, r18
	sts	ledState+3, r19
	ret

do100ms:
	lds	r16, time100ms
	dec	r16
	sts	time100ms, r16
	breq	PC+2
	ret
	ldi	r16, 10
	sts	time100ms, r16
	lds	r12, maskState+0
	lds	r13, maskState+1
	lds	r14, maskState+2
	lds	r15, maskState+3
	lds	r8, ledState+0
	lds	r9, ledState+1
	lds	r10, ledState+2
	lds	r11, ledState+3
	and	r8, r12
	and	r9, r13
	and	r10, r14
	and	r11, r15
	or	r8, r9
	or	r10, r11
	or	r8, r10
	breq	ledOff
	sbi	PORTB, LEDBUILTIN	;led ON
	rjmp	saveMask
ledOff:
	cbi	PORTB, LEDBUILTIN
saveMask:
	clc
	ror	r15
	ror	r14
	ror	r13
	ror	r12
	brcc	PC+3
	ldi	r16, 0x80
	mov	r15, r16
	sts	maskState+0, r12
	sts	maskState+1, r13
	sts	maskState+2, r14
	sts	maskState+3, r15
	ret

;Funcionamiento:
;Si se envia el caracter T: devuelve TEST\n
;Si se envia el caracter V: devuelve la version
;Si se envia el caracter S: cambia el estado del led y devuelve el nombre del nuevo estado
;Cada vez que se presiona el boton se cambia de estado y se envia por el puerto serie el nombre del estado

;Like a main in C
start:
	cli
	ldi	r16, low(RAMEND)
	out	SPL, r16
	ldi	r16, HIGH(RAMEND)
	out	SPH, r16
	call	ini_ports
	call	ini_TMR0
	call	ini_USART0
	ldi	r16, 0
	sts	dbSW0, r16
	sts	iwRX, r16
	sts	irRX, r16
	sts	iwTX, r16
	sts	irTX, r16
	sts	maskState+0, r16 
	sts	maskState+1, r16 
	sts	maskState+2, r16
	ldi	r16, 0x80 
	sts	maskState+3, r16
	ldi	r16, low(STATEIDLE)
	sts	ledState+0, r16
	ldi	r16, high(STATEIDLE)
	sts	ledState+1, r16
	ldi	r16, byte3(STATEIDLE)
	sts	ledState+2, r16
	ldi	r16, byte4(STATEIDLE)
	sts	ledState+3, r16
	ldi	r16, 5
	sts	time10ms, r16
	ldi	r16, 10
	sts	time100ms, r16
	out	GPIOR0, r16
	sbic	PINB, SW0
	sbi	GPIOR0, LASTSTATESW0
	sei
loop:
	sbis	GPIOR0, IS10MS
	rjmp	testBufRX
	cbi	GPIOR0, IS10MS	
 	call	do100ms
	call	doSW0
	sbic	GPIOR0, ISNEWSTATESW0
	call	doNewStateSW0
testBufRX:
	lds	r17, iwRX
	lds	r16, irRX
	cp	r16, r17
	breq	testBufTX
	call	doRX
testBufTX:
	lds	r17, iwTX
	lds	r16, irTX
	cp	r16, r17
	brne	PC+2		
	jmp	loop
doTX:
	lds	r17, UCSR0A
	sbrs	r17, UDRE0
	jmp	loop
	ldi	r26, low(bufTX)
	ldi	r27, high(bufTX)
	clr	r17
	add	r26, r16
	adc	r27, r17
	inc	r16
	andi	r16, BUFSIZETX-1
	sts	irTX, r16
	ldi	r16, (1 << TXC0) | (1 << U2X0)
	sts	UCSR0A, r16
	ld	r16, X
	sts	UDR0, r16
	jmp	loop	
	
