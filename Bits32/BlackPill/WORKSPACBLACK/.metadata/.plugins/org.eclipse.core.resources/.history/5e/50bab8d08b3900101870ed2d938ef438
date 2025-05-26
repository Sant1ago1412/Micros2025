/*
 * OLED.c
 *
 *  Created on: May 29, 2024
 *      Author: Gerónimo Spiazzi
 */

#include "OLED.h"

static uint8_t displayBuffer[DISPLAY_BUFFER_SIZE];

static _sSSD1306Data displayData;	// Screen object

/**
 *	BLOCKING METHOD TO WRITE INTO THE DISPLAY
 */
static uint8_t OLED_WriteCommand(I2C_HandleTypeDef *hi2c, uint8_t command){
    return HAL_I2C_Mem_Write(hi2c, OLED_I2C_ADDR, 0x00, 1, &command, 1, 10);
}

/**
 * NON-BLOCKING METHOD TO WRITE INTO THE DISPLAY
 */
static uint8_t OLED_WriteCommand_DMA(I2C_HandleTypeDef *hi2c, uint8_t command){
    return HAL_I2C_Mem_Write_DMA(hi2c, OLED_I2C_ADDR, 0x00, 1, &command, 1);
}

/**
 * INITIALIZE THE DISPLAY
 */
uint8_t OLED_Init(I2C_HandleTypeDef *hi2c){
    HAL_Delay(100);	// Wait for the screen to boot
    int status = 0;

    status += OLED_WriteCommand(hi2c, OLED_OFF);   // Display off
    status += OLED_WriteCommand(hi2c, MEMORY_MODE);   // Set Memory Addressing Mode
    status += OLED_WriteCommand(hi2c, 0x10);   // 00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    status += OLED_WriteCommand(hi2c, 0xB0);   // Set Page Start Address for Page Addressing Mode,0-7
    status += OLED_WriteCommand(hi2c, 0xC8);   // Set COM Output Scan Direction
    status += OLED_WriteCommand(hi2c, SET_LOW_COLUMN);   // Set low column address
    status += OLED_WriteCommand(hi2c, SET_HIGH_COLUMN);   // Set high column address
    status += OLED_WriteCommand(hi2c, SET_START_LINE);   // Set start line address
    status += OLED_WriteCommand(hi2c, 0x81);   // set contrast control register
    status += OLED_WriteCommand(hi2c, 0xFF);
    status += OLED_WriteCommand(hi2c, SET_SEGMENT_REMAP);   // Set segment re-map 0 to 127
    status += OLED_WriteCommand(hi2c, NORMAL_DISPLAY);   // Set normal display

    status += OLED_WriteCommand(hi2c, SET_MULTIPLEX);   // Set multiplex ratio(1 to 64)
    status += OLED_WriteCommand(hi2c, OLED_HEIGHT - 1);

    status += OLED_WriteCommand(hi2c, DISPLAY_ALL_ON_RESUME);   // 0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    status += OLED_WriteCommand(hi2c, SET_OLED_OFFSET);   // Set display offset
    status += OLED_WriteCommand(hi2c, 0x00);   // No offset
    status += OLED_WriteCommand(hi2c, SET_OLED_CLOCK_DIV);   // Set display clock divide ratio/oscillator frequency
    status += OLED_WriteCommand(hi2c, 0xF0);   // Set divide ratio
    status += OLED_WriteCommand(hi2c, SET_PRECHARGE);   // Set pre-charge period
    status += OLED_WriteCommand(hi2c, 0x22);

    status += OLED_WriteCommand(hi2c, SET_COM_PINS);   // Set com pins hardware configuration
    status += OLED_WriteCommand(hi2c, OLED_COM_LR_REMAP << 5 | OLED_COM_ALTERNATIVE_PIN_CONFIG << 4 | 0x02);

    status += OLED_WriteCommand(hi2c, SET_VCOM_DETECT);   // Set vcomh
    status += OLED_WriteCommand(hi2c, 0x20);   // 0x20,0.77xVcc
    status += OLED_WriteCommand(hi2c, CHARGE_PUMP);   // Set DC-DC enable
    status += OLED_WriteCommand(hi2c, 0x14);   //
    status += OLED_WriteCommand(hi2c, OLED_ON);   // Turn on Display panel

    if (status != 0)
        return 1;

    OLED_Fill(Black);		// Clear screen
    displayData.currentX = 0;		// Set default values for screen object
    displayData.currentY = 0;
    displayData.initialized = 1;	// Checked initialization of the display

    return 0;
}

/**
 * FILL THE DISPLAY WITH A CERTAIN COLOR
 */
void OLED_Fill(Display_COLOR color){
    uint16_t i;

    for(i = 0; i < sizeof(displayBuffer); i++)	// Fill screenbuffer with a constant value (color)
        displayBuffer[i] = (color == Black) ? 0x00 : 0xFF;
}

/**
 * UPDATE DATA OF THE DISPLAY
 */
uint8_t OLED_UpdateScreen(I2C_HandleTypeDef *hi2c){
	static uint8_t updateScreenState = 0;
	static uint8_t repetition = 0;
	uint8_t IsUpdateScreenTime = 1;

	switch (updateScreenState) {
		case 0:
			OLED_WriteCommand_DMA(hi2c, 0xB0 + repetition);
			break;
		case 1:
			OLED_WriteCommand_DMA(hi2c, 0x00);
			break;
		case 2:
			OLED_WriteCommand_DMA(hi2c, 0x10);
			break;
		case 3:
			HAL_I2C_Mem_Write_DMA(hi2c, OLED_I2C_ADDR, 0x40, 1, &displayBuffer[OLED_WIDTH * repetition], OLED_WIDTH);
			break;
	}

	updateScreenState++;

	if (updateScreenState > 3) {
		updateScreenState = 0;
		repetition++;

		if (repetition > 7) {
			updateScreenState = 0;
			repetition = 0;
			IsUpdateScreenTime = 0;
		}
	}
	return(IsUpdateScreenTime);
}

/**
 * DRAW A PIXEL IN THE DISPLAY
 */
void OLED_DrawPixel(uint8_t x, uint8_t y, Display_COLOR color){
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT)	// Don't write outside the buffer
        return;

    if (displayData.inverted)				// Check if pixel should be inverted
    	color = (Display_COLOR)!color;

    if (color == White)		// Draw in the correct color
        displayBuffer[x + (y / 8) * OLED_WIDTH] |= 1 << (y % 8);
    else
    	displayBuffer[x + (y / 8) * OLED_WIDTH] &= ~(1 << (y % 8));
}


/**
 * WRITE A CHAR INTO THE DISPLAY BUFFER
 */
char OLED_WriteChar(char ch, FontDef Font, Display_COLOR color){
    uint32_t i, b, j;

    // Check remaining space on current line
    if (OLED_WIDTH <= (displayData.currentX + Font.FontWidth) ||
        OLED_HEIGHT <= (displayData.currentY + Font.FontHeight))
    {
        return 0;	// Not enough space on current line
    }

    for (i = 0; i < Font.FontHeight; i++){	// Translate font to screenbuffer
        b = Font.data[(ch - 32) * Font.FontHeight + i];
        for (j = 0; j < Font.FontWidth; j++){
            if ((b << j) & 0x8000)
                OLED_DrawPixel(displayData.currentX + j, (displayData.currentY + i), (Display_COLOR) color);
            else
                OLED_DrawPixel(displayData.currentX + j, (displayData.currentY + i), (Display_COLOR)!color);
        }
    }
    displayData.currentX += Font.FontWidth;	// The current space is now taken

    return ch;	// Return written char for validation
}

/**
 * WRITE STRING INTO THE DISPLAY BUFFER
 */
char OLED_WriteString(const char* str, FontDef Font, Display_COLOR color){
    while (*str){
        if (OLED_WriteChar(*str, Font, color) != *str)	// Write until null-byte
        	return *str;	// Char could not be written

        str++;	// Next char
    }
    return *str;	// Everything ok
}

/**
 * INVERT DISPLAY COLORS
 */
void OLED_InvertColors(void){
	displayData.inverted = !displayData.inverted;
}

/**
 * SET CURSOR POSITION
 */
void OLED_SetCursor(uint8_t x, uint8_t y){
    displayData.currentX = x;
    displayData.currentY = y;
}

/**
 * DRAW BITMAP
 */
void OLED_DrawBitmap(uint8_t W, uint8_t H, const uint8_t* pBMP){
	uint16_t byteWidth = (W + 7) / 8; // Calculate the width in bytes
	for (uint16_t y = 0; y < H; y++){
		for (uint16_t x = 0; x < W; x++){
			if (pBMP[y * byteWidth + x / 8] & (128 >> (x & 7)))
				OLED_DrawPixel(x, y, White);
		}
	}
}

void OLED_DrawHorizontalLine(int16_t x, int16_t y, int16_t length, Display_COLOR color){
  if (y < 0 || y >= OLED_HEIGHT)
	  return;

  if (x < 0){
	  length += x;
	  x = 0;
  }

  if((x + length) > OLED_WIDTH)
	  length = (OLED_WIDTH - x);


  if(length <= 0)
	  return;

  uint8_t * bufferPtr = displayBuffer;
  bufferPtr += (y >> 3) * OLED_WIDTH;
  bufferPtr += x;

  uint8_t drawBit = 1 << (y & 7);

  switch (color){
    case White:
    	while (length--)
    		*bufferPtr++ |= drawBit;
    	break;
    case Black:
    	drawBit = ~drawBit;
    	while (length--)
    		*bufferPtr++ &= drawBit;
    	break;
    case Inverse:
    	while (length--)
    		*bufferPtr++ ^= drawBit;
    	 break;
  }
}

void OLED_DrawVerticalLine(int16_t x, int16_t y, int16_t length, Display_COLOR color){
  if(x < 0 || x >= OLED_WIDTH)
	  return;

  if(y < 0){
	  length += y;
	  y = 0;
  }

  if ( (y + length) > OLED_HEIGHT)
	  length = (OLED_HEIGHT - y);

  if (length <= 0)
	  return;

  uint8_t yOffset = y & 7;
  uint8_t drawBit;
  uint8_t *bufferPtr = displayBuffer;

  bufferPtr += (y >> 3) * OLED_WIDTH;
  bufferPtr += x;

  if (yOffset){
    yOffset = 8 - yOffset;
    drawBit = ~(0xFF >> (yOffset));

    if (length < yOffset)
    	drawBit &= (0xFF >> (yOffset - length));

    switch (color){
      case White:   *bufferPtr |=  drawBit; break;
      case Black:   *bufferPtr &= ~drawBit; break;
      case Inverse: *bufferPtr ^=  drawBit; break;
    }

    if (length < yOffset) return;

    length -= yOffset;
    bufferPtr += OLED_WIDTH;
  }

  if (length >= 8) {
    switch (color){
      case White:
      case Black:
        drawBit = (color == White) ? 0xFF : 0x00;
        do {
          *bufferPtr = drawBit;
          bufferPtr += OLED_WIDTH;
          length -= 8;
        } while (length >= 8);
        break;
      case Inverse:
        do {
          *bufferPtr = ~(*bufferPtr);
          bufferPtr += OLED_WIDTH;
          length -= 8;
        } while (length >= 8);
        break;
    }
  }

  if(length > 0){
    drawBit = (1 << (length & 7)) - 1;
    switch (color){
      case White:   *bufferPtr |=  drawBit; break;
      case Black:   *bufferPtr &= ~drawBit; break;
      case Inverse: *bufferPtr ^=  drawBit; break;
    }
  }
}

void OLED_DrawRect(int16_t x, int16_t y, int16_t width, int16_t height, Display_COLOR color){
	OLED_DrawHorizontalLine(x, y, width, color);
	OLED_DrawVerticalLine(x, y, height, color);
	OLED_DrawVerticalLine(x + width - 1, y, height, color);
	OLED_DrawHorizontalLine(x, y + height - 1, width, color);
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c){
	//if(hi2c->Instance == SSD1306_I2C_PORT.Instance){
		//TODO:
	//}
}
