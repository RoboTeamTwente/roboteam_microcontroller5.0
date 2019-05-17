/*
 * FT812Q_Drawing.c
 *
 *  Created on: May 16, 2019
 *      Author: selina
 */

#include "FT812Q_Drawing.h"
#include <stdlib.h>

/* DRAW ON SCREEN */
uint32_t* drawString(uint32_t* address, uint16_t x, uint16_t y, uint8_t handle, char* string, uint8_t color[]){
	writeDisplay((uint32_t)address++,  	0x4, 	COLOR_RGB(color[0], color[1], color[2]));
	writeDisplay((uint32_t)address++, 	0x4, 	BITMAPS);

	int i = 0;
	uint32_t formatted[100];
	while(string[i] != '\0'){
		memcpy((uint8_t*)(formatted + i),LETTER_DATA(x, y, handle, string[i]), 4);
		x+= 10;
		i++;
	}
	writeDisplay((uint32_t)address, i*4, (uint8_t*)formatted);

	return address + i;	// return the address at which the next instruction should be
}

uint32_t* drawRect(uint32_t* address, uint16_t begin[], uint16_t end[], uint8_t color[]){
	writeDisplay((uint32_t)address++,  	0x4, 	COLOR_RGB(color[0], color[1], color[2]));
	writeDisplay((uint32_t)address++,	0X4, 	RECTS);
	writeDisplay((uint32_t)address++,	0x4, 	POINT_DATA(begin[0], begin[1]));
	writeDisplay((uint32_t)address++,	0x4, 	POINT_DATA(end[0], end[1]));
	return address;
}

uint32_t* drawLine(uint32_t* address, uint16_t xb, uint16_t yb, uint16_t xe, uint16_t ye, uint8_t color[], uint8_t width){
	writeDisplay((uint32_t)address++, 	0x4, 	LINES);
	writeDisplay((uint32_t)address++,  	0x4, 	COLOR_RGB(color[0], color[1], color[2]));
	writeDisplay((uint32_t)address++,	0x4, 	POINT_DATA(xb, yb));
	writeDisplay((uint32_t)address++,	0x4,	POINT_DATA(xe, ye));
	writeDisplay((uint32_t)address++,	0x4,	LINE_WIDTH(width));
	return address;
}

void drawMainScreen(uint16_t ID){
	uint16_t begin[] = {0, 0}; uint16_t end[] = {0, 0}; uint8_t color[] = {0, 0, 0};

	// Initial settings
	writeDisplay(RAM_DL + 0x0,  	0x4, 	CLEAR(1, 1, 1));
	writeDisplay(RAM_DL + 0x4,		0x4, 	CLEAR_COLOR_RGB(0, 0, 0));
	writeDisplay(RAM_DL + 0x8,	 	0x4,	VERTEX_FORMAT(0));

	// Create display list
	begin[0] = 3; begin[1] = 5; end[0] = 478; end[1] = 269; color[0] = 204;
	uint32_t* address = drawRect((uint32_t*)(RAM_DL + 0xC), begin, end, color); // draw red rectangle
	begin[0] = 5; begin[1] = 7; end[0] = 476; end[1] = 267; color[0] = 0;
	address = drawRect((uint32_t*)((uint32_t) address), begin, end, color); // draw black rectangle
	color[0] = 204;
	address = drawString((uint32_t*)((uint32_t) address), 10, 10, 18, "ID:", color);
	char id[2]; itoa(ID, id, 10);
	address = drawString((uint32_t*)((uint32_t) address), 65, 10, 18, id, color);

	// Draw
	writeDisplay((uint32_t)address++, 	0x4, 	DISPLAY);
	writeDisplay(REG_DLSWAP, 	0x1, 	DLSWAP); // Display list swap
}

void drawBasestation(){

	// Initial settings
	writeDisplay(RAM_DL + 0x0,  	0x4, 	CLEAR(1, 1, 1));
	writeDisplay(RAM_DL + 0x4,		0x4, 	CLEAR_COLOR_RGB(0, 0, 0));
	writeDisplay(RAM_DL + 0x8,	 	0x4,	VERTEX_FORMAT(0));

	// Create display list
	uint8_t color[] = {204, 0, 0};
	uint32_t* address = drawString((uint32_t*)(RAM_DL + 0xC), 5, 5, 18, "Robot Status", color);

	uint8_t spacingY = 60;
	uint8_t spacingX = 120;

	for (int i = 0; i < 5; i++){ // draw horizontal lines
		address = drawLine((uint32_t*)address, 0, 30 + i*spacingY, 479, 30 + i*spacingY, RTTRED, 100);
	}

	for (int i = 0; i < 5; i++){ // draw vertical lines
		if (0 + i == 480){i = i - 1;} // line is only partly visible if on 480th pixel
		address = drawLine((uint32_t*)address, 0 + i*spacingX, 30, 0 + i*spacingX, 270, RTTRED, 100);
	}

	uint16_t start_y = 31; uint16_t x;
	uint16_t start_x = 3; uint16_t y = start_y;

	int k = 0; int p = 0;
	for (int i = 0; i < 16; i++){
		if ((i !=0) && (i % 4 == 0)){
			p++;
			y = start_y + p*spacingY;
			k = 0;
		}
		x = start_x + k*spacingX;
		char id[4]; itoa(i, id, 10);
		address = drawString((uint32_t*)address, x, y, 23, id, RTTRED);
		k++;
	}

	// Draw
	writeDisplay((uint32_t)address++, 	0x4, 	DISPLAY);
	writeDisplay(REG_DLSWAP, 	0x1, 	DLSWAP); // Display list swap

	// try to get touchscreen working
	// display settings
	writeDisplay(REG_CTOUCH_EXTENDED, 0x1, EXTENDED_MODE);
	writeDisplay(REG_TOUCH_TRANSFORM_A, 0x4, TRANSFORM_A);
	writeDisplay(REG_TOUCH_TRANSFORM_E, 0x4, TRANSFORM_E);
	writeDisplay(REG_TOUCH_TRANSFORM_F, 0x4, TRANSFORM_F);

	while(1){
		uint8_t* data = readDisplay(REG_TOUCH_TAG1_XY, 0x4, getData4);
		Putty_printf("touch data: %X %X %X %X \n\r", data[0], data[1], data[2], data[3]);
		int16_t* touch_x = (uint16_t*)data;
		int16_t* touch_y = (uint16_t*)(data+2);
		Putty_printf("touch_x: %X, %hd \n\r", *touch_x, *touch_x);
		Putty_printf("touch_y: %X, %hd \n\r", *touch_y, *touch_y);
		uint8_t* dataA = readDisplay(REG_TOUCH_TRANSFORM_A, 0x4, getData4);
		Putty_printf("transform A: %X %X %X %X \n\r", dataA[0], dataA[1], dataA[2], dataA[3]);
		uint8_t* dataE = readDisplay(REG_TOUCH_TRANSFORM_E, 0x4, getData4);
		Putty_printf("transform E: %X %X %X %X \n\r", dataE[0], dataE[1], dataE[2], dataE[3]);
		uint8_t* dataF = readDisplay(REG_TOUCH_TRANSFORM_F, 0x4, getData4);
		Putty_printf("transform F: %X %X %X %X \n\r", dataF[0], dataF[1], dataF[2], dataF[3]);
		HAL_Delay(700);

	}

}

