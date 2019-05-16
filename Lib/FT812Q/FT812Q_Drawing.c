/*
 * FT812Q_Drawing.c
 *
 *  Created on: May 16, 2019
 *      Author: selina
 */

#include "FT812Q_Drawing.h"
#include <stdlib.h>

/* DRAW ON SCREEN */
void drawPoint(){
	writeDisplay(RAM_DL + 0x0,		0x4, 	CLEAR_COLOR_RGB(0, 0, 0)); // maybe not necessary? test
	writeDisplay(RAM_DL + 0x4,  	0x4, 	CLEAR(1, 1, 1));
	writeDisplay(RAM_DL + 0x8,  	0x4, 	COLOR_RGB(204, 0, 0));
	writeDisplay(RAM_DL + 0xC, 		0x4, 	POINT_SIZE(160));
	writeDisplay(RAM_DL + 0x10, 	0x4, 	POINTS);
	writeDisplay(RAM_DL + 0x14, 	0x4,	VERTEX_FORMAT(0));
	writeDisplay(RAM_DL + 0x18, 	0x4, 	POINT_DATA(100, 100));
	writeDisplay(RAM_DL + 0x1C, 	0x4, 	END);
	writeDisplay(RAM_DL + 0x20, 	0x4, 	DISPLAY);
	writeDisplay(REG_DLSWAP, 		0x1, 	DLSWAP); // Display list swap
}

void drawLetter(){
	writeDisplay(RAM_DL + 0x0,		0x4, 	CLEAR_COLOR_RGB(0, 0, 0));
	writeDisplay(RAM_DL + 0x4,  	0x4, 	CLEAR(1, 1, 1));
	writeDisplay(RAM_DL + 0x8, 		0x4, 	BITMAPS);
	writeDisplay(RAM_DL + 0xC,  	0x4, 	COLOR_RGB(204, 0, 0));
	writeDisplay(RAM_DL + 0x10,		0x4, 	LETTER_DATA(100, 100, 31, 82));
	writeDisplay(RAM_DL + 0x14,		0x4, 	LETTER_DATA(140, 100, 31, 84));
	writeDisplay(RAM_DL + 0x18,		0x4, 	LETTER_DATA(180, 100, 31, 84));
	writeDisplay(RAM_DL + 0x1C, 	0x4, 	END);
	writeDisplay(RAM_DL + 0x20, 	0x4, 	DISPLAY);
	writeDisplay(REG_DLSWAP, 		0x1, 	DLSWAP); // Display list swap
}

void drawLine(){
	writeDisplay(RAM_DL + 0x0,		0x4, 	CLEAR_COLOR_RGB(0, 0, 0));
	writeDisplay(RAM_DL + 0x4,  	0x4, 	CLEAR(1, 1, 1));
	writeDisplay(RAM_DL + 0x8, 		0x4, 	LINES);
	writeDisplay(RAM_DL + 0xC, 		0x4,	VERTEX_FORMAT(0));
	writeDisplay(RAM_DL + 0x10,		0x4, 	POINT_DATA(10, 10));
	writeDisplay(RAM_DL + 0x14,		0x4,	POINT_DATA(470, 10));
	writeDisplay(RAM_DL + 0x18,		0x4,	LINE_WIDTH(100));
	writeDisplay(RAM_DL + 0x1C, 	0x4, 	END);
	writeDisplay(RAM_DL + 0x20, 	0x4, 	DISPLAY);
	writeDisplay(REG_DLSWAP, 		0x1, 	DLSWAP); // Display list swap

}

void drawExample(){
	writeDisplay(RAM_DL + 0x0,  		0x4, 	CLEAR(1, 1, 1));
	uint8_t color[] = {1, 1, 1};
	uint32_t* address = drawString((uint32_t*)(RAM_DL + 0x4), 100, 110, 24, "hello there", color);
	address = drawString((uint32_t*)((uint32_t) address), 100, 150, 24, "general kenobi", color);
	writeDisplay((uint32_t)address++, 	0x4, 	END);
	writeDisplay((uint32_t)address++,	0x4, 	CLEAR_COLOR_RGB(160, 22, 22));
	writeDisplay((uint32_t)address++, 	0x4,	VERTEX_FORMAT(0));
	writeDisplay((uint32_t)address++, 	0x4, 	POINT_SIZE(160));
	writeDisplay((uint32_t)address++, 	0x4, 	POINTS);
	writeDisplay((uint32_t)address++,	0x4, 	POINT_DATA(50, 50));
	writeDisplay((uint32_t)address++, 	0x4, 	END);
	writeDisplay((uint32_t)address++, 	0x4, 	DISPLAY);
	writeDisplay(REG_DLSWAP, 			0x1, 	DLSWAP); // Display list swap
}

uint32_t* drawString(uint32_t* address, uint16_t x, uint16_t y, uint8_t handle, char* string, uint8_t color[]){
	writeDisplay((uint32_t)address++,  	0x4, 	COLOR_RGB(color[0], color[1], color[2]));
	writeDisplay((uint32_t)address++, 	0x4, 	BITMAPS);

	int i = 0;
	uint32_t formatted[100];
	while(string[i] != '\0'){
		memcpy((uint8_t*)(formatted + i),LETTER_DATA(x, y, handle, string[i]), 4);
		x+= handle;
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

void drawMainScreen(uint16_t ID){
	uint16_t begin[] = {0, 0}; uint16_t end[] = {0, 0}; uint8_t color[] = {0, 0, 0};

	writeDisplay(RAM_DL + 0x0,  	0x4, 	CLEAR(1, 1, 1));
	writeDisplay(RAM_DL + 0x4,		0x4, 	CLEAR_COLOR_RGB(0, 0, 0));
	writeDisplay(RAM_DL + 0x8,	 	0x4,	VERTEX_FORMAT(0));

	begin[0] = 3; begin[1] = 5; end[0] = 478; end[1] = 269; color[0] = 204;
	uint32_t* address = drawRect((uint32_t*)(RAM_DL + 0xC), begin, end, color); // draw red rectangle
	begin[0] = 5; begin[1] = 7; end[0] = 476; end[1] = 267; color[0] = 0;
	address = drawRect((uint32_t*)((uint32_t) address), begin, end, color); // draw black rectangle
	color[0] = 204;
	address = drawString((uint32_t*)((uint32_t) address), 10, 10, 18, "ID:", color);
	char id[2]; itoa(ID, id, 10);
	address = drawString((uint32_t*)((uint32_t) address), 65, 10, 18, id, color);

	writeDisplay((uint32_t)address++, 	0x4, 	DISPLAY);
	writeDisplay(REG_DLSWAP, 	0x1, 	DLSWAP); // Display list swap
}
