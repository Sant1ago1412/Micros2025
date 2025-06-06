/*
 * display.h
 *
 *  Created on: May 19, 2025
 *      Author: agust
 */

#ifndef INC_I2C_OLED_DISPLAY_H_
#define INC_I2C_OLED_DISPLAY_H_

#include "utilities.h"
#include "fonts.h"
#include "stdlib.h"
#include "string.h"
#include <stdarg.h>

/* I2C address */
#ifndef SSD1306_I2C_ADDR
#define SSD1306_I2C_ADDR         	0x78
#endif

/* SSD1306 settings */
/* SSD1306 width in pixels */
#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH            	128
#endif
/* SSD1306 LCD height in pixels */
#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT           	64
#endif


#ifndef Display_COM_LR_REMAP
#define Display_COM_LR_REMAP    0
#endif // Display_COM_LR_REMAP

#ifndef Display_COM_ALTERNATIVE_PIN_CONFIG
#define Display_COM_ALTERNATIVE_PIN_CONFIG    1
#endif // Display_COM_ALTERNATIVE_PIN_CONFIG

#define OFFSET_DATA				6
#define OLED_DMA_BUFFER_SIZE 	1024


typedef enum {
	SSD1306_COLOR_BLACK = 0x00, /*!< Black color, no pixel */
	SSD1306_COLOR_WHITE = 0x01  /*!< Pixel is set. Color depends on LCD */
} SSD1306_COLOR_t;


void Display_Set_I2C_Master_Transmit(
		e_system (*Master_Transmit)(uint16_t DevAddress, uint8_t *pData, uint16_t Size),
		e_system (*Master_Transmit_Blocking)(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout));

e_system Display_Init();

void Display_I2C_DMA_Ready(uint8_t val);

void Display_I2C_Refresh_Ready(uint8_t val);

e_system ssd1306_I2C_Write(uint8_t address, uint8_t reg, uint8_t data);

e_system ssd1306_I2C_WriteMulti(uint8_t address, uint8_t reg, uint8_t* data, uint16_t count);

void Display_UpdateScreen(void);

void SSD1306_ON(void);

void SSD1306_OFF(void);

void Display_Fill(SSD1306_COLOR_t color);

void Display_Clear (void);

void Display_DrawPixel(uint16_t x, uint16_t y, SSD1306_COLOR_t color);

void Display_DrawBitmap(int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint16_t color);

void Display_ShowBitmap(const unsigned char bitmap[]);


#endif /* INC_I2C_OLED_DISPLAY_H_ */
