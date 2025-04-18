#include <m328pdef.inc>

;igualdades
.equ	LEDBUILTIN = PB5
.equ	SW0 = PB4
.equ	BUFSIZERX = 32
.equ	BUFSIZETX = 32
.equ	STATEIDLE = 0x12A4F300
.equ	STATEUNO = 0x34B6D900
.equ	STATEDOS = 0x5678AB00
.equ	STATETRES = 0x7890EF00
.equ	STATECUATRO = 0x9A1B2C00
.equ	STATECINCO = 0xBC4D5E00
.equ	STATESEIS = 0xDE7F8A00
.equ	STATESIETE = 0xF0E1D200
.equ	STATEOCHO = 0x2F4A6B00
.equ	STATENUEVE = 0x4E5D7A00
.equ	STATEDIEZ = 0x602F3B00
;aca ponemos algunas igualdades para acceder mas facil a ciertos valores y editarlos de forma comoda
;suerte al flaco entendiendo esos codigos generados al azar, por suerte los 2 primeros bytes siempre son distintos


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
	jmp	TIM0_COMPA ;es una interrupcion de timer comparativo
.org	0x24
	jmp	USART_RXC ; interrupcion de usart rx
.org	0x34
;inicializamos 

;**** CONSTANTS in FLASH ****
msgStateIdle:    .db "STATE IDLE", 0x0A, 0x00   ; 0x0A es el salto de línea, 0x00 es el fin de cadena
msgStateUno:     .db "STATE UNO", 0x0A, 0x00
msgStateDos:     .db "STATE DOS", 0x0A, 0x00
msgStateTres:    .db "STATE TRES", 0x0A, 0x00
msgStateCuatro:  .db "STATE CUATRO", 0x0A, 0x00
msgStateCinco:   .db "STATE CINCO", 0x0A, 0x00
msgStateSeis:    .db "STATE SEIS", 0x0A, 0x00
msgStateSiete:   .db "STATE SIETE", 0x0A, 0x00
msgStateOcho:    .db "STATE OCHO", 0x0A, 0x00
msgStateNueve:   .db "STATE NUEVE", 0x0A, 0x00
msgStateDiez:    .db "STATE DIEZ", 0x0A, 0x00

;**** INTERRUPTS ****
TIM0_COMPA: ; se labura un poco la interrupcion del timer
	in	r2, SREG
	push	r2
	push	r16;guardamos las cosas en el stack
	lds	r16, time10ms
	dec	r16 ;llamamos, decrementamos, guardamos!
	sts	time10ms, r16
	brne	OUT_TIM0_COMPA
	sbi	GPIOR0, IS10MS ;aca no la cazo
	ldi	r16, 5
	sts	time10ms, r16

OUT_TIM0_COMPA:
	pop	r16
	pop	r2 ;recuperamos
	out	SREG, r2
	reti	

USART_RXC: ;se labura un poco la interrupcion de transmision
	in	r2, SREG
	push	r2
	push	r16
	push	r17
	push	r26
	push	r27 ;guardamos
	ldi	r26, byte1(bufRX)
	ldi	r27, byte2(bufRX)
	lds	r16, iwRX

	clr	r17
	add	r26, r16 ;400 iq moment
	adc	r27, r17

	lds	r17, UDR0
	st	X, r17 ;guarda
	inc	r16		
	andi	r16, BUFSIZERX-1 ;una and entre un registro y una constante
	sts	iwRX, r16 ;hay que leer la hoja de datos
OUT_USART_RXC:
	pop	r27
	pop	r26
	pop	r17
	pop	r16
	pop	r2 ;guardamosssssss
	out	SREG, r2
	reti

;**** Funciones ****
ini_ports:
	ldi	r16, (1 << LEDBUILTIN) | (0 << SW0)  ;arrancamos los puertitos
	out	DDRB, r16
	ldi	r16, (1 << SW0)
	out	PORTB, r16
	ret

; fclk/256 - Mode CTC - OCR0A: 125 -> 2ms
ini_TMR0: ;aca se configura en base al datasheet y el objetivo como queremos que labure el timer
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
ini_USART0: ;y aca lo mismo que el timer pero con el usart
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
	push	r17; guardamosss
	push	r26
	push	r27
	lds	r16, iwTX

nextByteText:
	ldi	r27, byte2(bufTX)
	ldi	r26, byte1(bufTX) ;voy a tener que indagar mas aca, lo hizo german
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
	pop	r17; sacamos
	pop	r16
	ret

doNewStateSW0:
    cbi     GPIOR0, ISNEWSTATESW0       ; Limpia el flag de nuevo estado
    sbic    GPIOR0, LASTSTATESW0        ; Si el último estado del botón es 1, retorna
    ret
    lds     r16, ledState+3             ; Carga el valor de ledState+3 en r16
    cpi     r16, 0x12                  ; Compara r16 con 0x12
    breq    setNewStateUno              ; Si es igual, salta a setNewStateUno
    cpi     r16, 0x34                  
    breq    setNewStateDos              ; Si es igual, salta a setNewStateDos
	cpi     r16, 0x56                  
    breq    setNewStateTres             ;la chanchada definitiva... falta
	cpi     r16, 0x78                  
    breq    setNewStateCuatro             
	cpi     r16, 0x9A                  
    breq    setNewStateCinco
	cpi     r16, 0xBC                  
    breq    setNewStateSeisPit
	cpi     r16, 0xDE                  
    breq    setNewStateSietePit ;hacemos una parada de pits porque breq tiene 64 instrucciones de rango y programo muy mal
	cpi     r16, 0xF0                  
    breq    setNewStateOchoPit
	cpi     r16, 0x2F                 
    breq    setNewStateNuevePit
	cpi     r16, 0x4E                  
    breq    setNewStateDiezPit            
    ldi     r30, byte1(msgStateIdle << 1);si no fue ninguna de todas las otras entonces esta en idle
    ldi     r31, byte2(msgStateIdle << 1)
    call    PutConstTextOnBufTX         ; Muestra el mensaje de estado IDLE
    ldi     r16, byte1(STATEIDLE)
    ldi     r17, byte2(STATEIDLE)
    ldi     r18, byte3(STATEIDLE)
    ldi     r19, byte4(STATEIDLE)
    rjmp    outNewState                 ; Salta a outNewState

setNewStateUno:
    ldi     r30, byte1(msgStateUno << 1)
    ldi     r31, byte2(msgStateUno << 1)
    call    PutConstTextOnBufTX         ; Muestra el mensaje de estado UNO
    ldi     r16, byte1(STATEUNO)
    ldi     r17, byte2(STATEUNO)
    ldi     r18, byte3(STATEUNO)
    ldi     r19, byte4(STATEUNO)
    rjmp    outNewState                 ; Salta a outNewState

setNewStateDos:
    ldi     r30, byte1(msgStateDos << 1)
    ldi     r31, byte2(msgStateDos << 1)
    call    PutConstTextOnBufTX         ; Muestra el mensaje de estado DOS
    ldi     r16, byte1(STATEDOS)
    ldi     r17, byte2(STATEDOS)
    ldi     r18, byte3(STATEDOS)
    ldi     r19, byte4(STATEDOS)
	rjmp    outNewState     

setNewStateTres:
    ldi     r30, byte1(msgStateTres << 1)
    ldi     r31, byte2(msgStateTres << 1)
    call    PutConstTextOnBufTX         
    ldi     r16, byte1(STATETRES)
    ldi     r17, byte2(STATETRES)
    ldi     r18, byte3(STATETRES)
    ldi     r19, byte4(STATETRES)
	rjmp    outNewState     

setNewStateCuatro:
    ldi     r30, byte1(msgStateCuatro << 1)
    ldi     r31, byte2(msgStateCuatro << 1)
    call    PutConstTextOnBufTX         
    ldi     r16, byte1(STATECUATRO)
    ldi     r17, byte2(STATECUATRO)
    ldi     r18, byte3(STATECUATRO)
    ldi     r19, byte4(STATECUATRO)
	rjmp    outNewState     

setNewStateCinco:
    ldi     r30, byte1(msgStateCinco << 1)
    ldi     r31, byte2(msgStateCinco << 1)
    call    PutConstTextOnBufTX        
    ldi     r16, byte1(STATECINCO)
    ldi     r17, byte2(STATECINCO)
    ldi     r18, byte3(STATECINCO)
    ldi     r19, byte4(STATECINCO)

	rjmp    outNewState     
setNewStateSeisPit:
	rjmp setNewStateSeis
setNewStateSietePit:
	rjmp setNewStateSiete
setNewStateOchoPit:
	rjmp setNewStateOcho
setNewStateNuevePit:		;esto lo tuve que hacer por el rango de breq
	rjmp setNewStateNueve
setNewStateDiezPit:
	rjmp setNewStateDiez

setNewStateSeis:
    ldi     r30, byte1(msgStateSeis << 1)
    ldi     r31, byte2(msgStateSeis << 1)
    call    PutConstTextOnBufTX         
    ldi     r16, byte1(STATESEIS)
    ldi     r17, byte2(STATESEIS)
    ldi     r18, byte3(STATESEIS)
    ldi     r19, byte4(STATESEIS)
	rjmp    outNewState     

setNewStateSiete:
    ldi     r30, byte1(msgStateSiete << 1)
    ldi     r31, byte2(msgStateSiete << 1)
    call    PutConstTextOnBufTX         
    ldi     r16, byte1(STATESIETE)
    ldi     r17, byte2(STATESIETE)
    ldi     r18, byte3(STATESIETE)
    ldi     r19, byte4(STATESIETE)
	rjmp    outNewState     

setNewStateOcho:
    ldi     r30, byte1(msgStateOcho << 1)
    ldi     r31, byte2(msgStateOcho << 1)
    call    PutConstTextOnBufTX         
    ldi     r16, byte1(STATEOCHO)
    ldi     r17, byte2(STATEOCHO)
    ldi     r18, byte3(STATEOCHO)
    ldi     r19, byte4(STATEOCHO)
	rjmp    outNewState     

setNewStateNueve:
    ldi     r30, byte1(msgStateNueve << 1)
    ldi     r31, byte2(msgStateNueve << 1)
    call    PutConstTextOnBufTX         
    ldi     r16, byte1(STATENUEVE)
    ldi     r17, byte2(STATENUEVE)
    ldi     r18, byte3(STATENUEVE)
    ldi     r19, byte4(STATENUEVE)
	rjmp    outNewState     

setNewStateDiez:
    ldi     r30, byte1(msgStateDiez << 1)
    ldi     r31, byte2(msgStateDiez << 1)
    call    PutConstTextOnBufTX         
    ldi     r16, byte1(STATEDIEZ)
    ldi     r17, byte2(STATEDIEZ)
    ldi     r18, byte3(STATEDIEZ)
    ldi     r19, byte4(STATEDIEZ)    
outNewState:
	sts	ledState+0, r16
	sts	ledState+1, r17
	sts	ledState+2, r18		;registramos todo
	sts	ledState+3, r19
	ret

doSW0:		;aca voy a tener que revisar tambien, es el debounce de german
	lds	r16, dbSW0
	sbic	GPIOR0, LASTSTATESW0
	rjmp	testbyte1SW0
	sbis	PINB, SW0
	rjmp	doSW0db1
	sbi	GPIOR0, LASTSTATESW0
	rjmp	doSW0db0
testbyte1SW0:
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
	ldi	r26, byte1(bufRX)
	ldi	r27, byte2(bufRX)
	clr	r17
	add	r26, r16
	adc	r27, r17
	inc	r16
	andi	r16, BUFSIZERX-1
	sts	irRX, r16
	ld	r16, X
  ; aca es para controlar que se escribe todo bien
    cpi r20, 0       ; cuando r20 vale 0 estamos esperando una C
    breq buscarC
    cpi r20, 1       ; cuando r20 vale 1 estamos esperando un numero, mas adelanto lo filtramos
    breq buscarPrimernumero
    cpi r20, 2       ; cuando r20 vale 2 estamos esperando otro numero, mas adelanto lo filtramos
    breq buscarSegundoNum
    cpi r20, 3       ; cuando r20 vale 3 estamos esperando una S
	breq BuscamosS
    jmp reiniciarEstadoPit ; si no aparece ese patron se resetea la secuencia

buscarC:
    cpi r16, 'C'
    brne reiniciarEstadoPit
    clr r20          ; limpiamos de vuelta, en esta parte del codigo mande fruta porque me quedaba algo por ahí molestando
    clr r21          ;
	clr r5
	clr r6
    ldi r20, 1       ; vamos a la siguiente parte
    ret

buscarPrimernumero:
    cpi r16, '0'     ; aca solo puede ser 0 o 1 así que nos fijamos eso, es 0
    breq primerNumeroValido0
    cpi r16, '1'     ; o es 1
    breq primerNumeroValido1
    rjmp reiniciarEstado ; si no es ninguno reseteamos
primerNumeroValido0:
    clr r21          
	mov r6, r16	;aca dejamos en claro que fue 0
    ldi r20, 2
    ret

primerNumeroValido1:
    ldi r21, 1       
	mov r6, r16; y aca que fue 1
    ldi r20, 2
    ret
reiniciarEstadoPit:
	rjmp reiniciarEstado	;otra parada de pits
buscarSegundoNum:
	
    tst r21          ; buscamos un 0
    breq buscarSegundoNumGen  ; si fue 0 nos vamos a general donde aceptamos de 0 a 9
    cpi r16, '0'     ; si fue 1 aceptamos solo un 0
    breq segundoNumValid
    rjmp reiniciarEstado ; no es ninguna reseteamos

buscarSegundoNumGen:
    cpi r16, '0'     ; condiciones de borde entre 0 y 9
    brlo reiniciarEstado
    cpi r16, '9' + 1 ; es mayor que 9?
    brsh reiniciarEstado

segundoNumValid:
	mov r5, r16
    ldi r20, 3       ; todo bien y seguimos
    ret

BuscamosS:
    cpi r16, 'S'
    brne reiniciarEstado
    ; Se recibió correctamente "CxxS"
	mov r16, r6
	cpi r16, '0' ; y aca comparamos y derivamos para donde deba ir y hacer la secuencia que deba hacer
	brne setStateDiez
	mov r16, r5
	cpi r16, '1'
	breq setStateUno
	cpi r16, '2'
	breq setStateDos
	cpi r16, '3'
	breq setStateTres
	cpi r16, '4'
	breq setStateCuatroPit
	cpi r16, '5'
	breq setStateCincoPit
	cpi r16, '6'
	breq setStateSeisPit
	cpi r16, '7'
	breq setStateSietePit
	cpi r16, '8'
	breq setStateOchoPit
	cpi r16, '9'
	breq setStateNuevePit
	ldi	r30, byte1(msgStateIdle << 1)
	ldi	r31, byte2(msgStateIdle << 1)
	call	PutConstTextOnBufTX
	ldi	r16, byte1(STATEIDLE)
	ldi	r17, byte2(STATEIDLE)
	ldi	r18, byte3(STATEIDLE)
	ldi	r19, byte4(STATEIDLE)
    rjmp reiniciarEstado ; Reseteamos después de la acción

reiniciarEstado:
    clr r20          ; Resetear estado de secuencia
    clr r21          ; Resetear estado de X1
    jmp	outDoRX_S ;limpiamos, es una pausa intermedia

setStateDiez:
	ldi	r30, byte1(msgStateDiez << 1)
	ldi	r31, byte2(msgStateDiez << 1)
	call	PutConstTextOnBufTX
	ldi	r16, byte1(STATEDIEZ)
	ldi	r17, byte2(STATEDIEZ)
	ldi	r18, byte3(STATEDIEZ)
	ldi	r19, byte4(STATEDIEZ)
	rjmp outDoRX_S
setStateUno:
	ldi	r30, byte1(msgStateUno << 1)
	ldi	r31, byte2(msgStateUno << 1)
	call	PutConstTextOnBufTX
	ldi	r16, byte1(STATEUNO)
	ldi	r17, byte2(STATEUNO)
	ldi	r18, byte3(STATEUNO)
	ldi	r19, byte4(STATEUNO)
	rjmp outDoRX_S
setStateDos:
	ldi	r30, byte1(msgStateDos << 1)
	ldi	r31, byte2(msgStateDos << 1)
	call	PutConstTextOnBufTX
	ldi	r16, byte1(STATEDOS)
	ldi	r17, byte2(STATEDOS)
	ldi	r18, byte3(STATEDOS)
	ldi	r19, byte4(STATEDOS)
	rjmp outDoRX_S
setStateTres:
	ldi	r30, byte1(msgStateTres << 1)
	ldi	r31, byte2(msgStateTres << 1)
	call	PutConstTextOnBufTX
	ldi	r16, byte1(STATETRES)
	ldi	r17, byte2(STATETRES)
	ldi	r18, byte3(STATETRES)
	ldi	r19, byte4(STATETRES)
	rjmp outDoRX_S
setStateCuatroPit:
	rjmp setStateCuatro
setStateCincoPit:
	rjmp setStateCinco
setStateSeisPit:
	rjmp setStateSeis
setStateSietePit:
	rjmp setStateSiete
setStateOchoPit:
	rjmp setStateOcho
setStateNuevePit:
	rjmp setStateNueve
setStateCuatro:
	ldi	r30, byte1(msgStateCuatro << 1)
	ldi	r31, byte2(msgStateCuatro << 1)
	call	PutConstTextOnBufTX
	ldi	r16, byte1(STATECUATRO)
	ldi	r17, byte2(STATECUATRO)
	ldi	r18, byte3(STATECUATRO)
	ldi	r19, byte4(STATECUATRO)
	rjmp outDoRX_S
setStateCinco:
	ldi	r30, byte1(msgStateCinco << 1)
	ldi	r31, byte2(msgStateCinco << 1)
	call	PutConstTextOnBufTX
	ldi	r16, byte1(STATECINCO)
	ldi	r17, byte2(STATECINCO)
	ldi	r18, byte3(STATECINCO)
	ldi	r19, byte4(STATECINCO)
	rjmp outDoRX_S
setStateSeis:
	ldi	r30, byte1(msgStateSeis << 1)
	ldi	r31, byte2(msgStateSeis << 1)
	call	PutConstTextOnBufTX
	ldi	r16, byte1(STATESEIS)
	ldi	r17, byte2(STATESEIS)
	ldi	r18, byte3(STATESEIS)
	ldi	r19, byte4(STATESEIS)
	rjmp outDoRX_S
setStateSiete:
	ldi	r30, byte1(msgStateSiete << 1)
	ldi	r31, byte2(msgStateSiete << 1)
	call	PutConstTextOnBufTX
	ldi	r16, byte1(STATESIETE)
	ldi	r17, byte2(STATESIETE)
	ldi	r18, byte3(STATESIETE)
	ldi	r19, byte4(STATESIETE)
	rjmp outDoRX_S
setStateOcho:
	ldi	r30, byte1(msgStateOcho << 1)
	ldi	r31, byte2(msgStateOcho << 1)
	call	PutConstTextOnBufTX
	ldi	r16, byte1(STATEOCHO)
	ldi	r17, byte2(STATEOCHO)
	ldi	r18, byte3(STATEOCHO)
	ldi	r19, byte4(STATEOCHO)
	rjmp outDoRX_S
setStateNueve:
	ldi	r30, byte1(msgStateNueve << 1)
	ldi	r31, byte2(msgStateNueve << 1)
	call	PutConstTextOnBufTX
	ldi	r16, byte1(STATENUEVE)
	ldi	r17, byte2(STATENUEVE)
	ldi	r18, byte3(STATENUEVE)
	ldi	r19, byte4(STATENUEVE)
	rjmp outDoRX_S

outDoRX_S:
	sts	ledState+0, r16
	sts	ledState+1, r17
	sts	ledState+2, r18
	sts	ledState+3, r19
	clr r5
	clr r6	;actualizamos todo, limpiamos y nos vamos
	clr r19
	clr r20
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
	ldi	r16, byte1(RAMEND)
	out	SPL, r16
	ldi	r16, byte2(RAMEND)	;limpiamos todo para un inicio correecto
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
	ldi	r16, 0x12 
	sts	maskState+3, r16
	ldi	r16, byte1(STATEIDLE)
	sts	ledState+0, r16
	ldi	r16, byte2(STATEIDLE)
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

testBufRX:		;de aca para abajo revisar que hizo german
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
	ldi	r26, byte1(bufTX)
	ldi	r27, byte2(bufTX)
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