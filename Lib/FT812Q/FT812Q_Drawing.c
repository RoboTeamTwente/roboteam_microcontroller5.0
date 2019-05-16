/*
 * FT812Q_Drawing.c
 *
 *  Created on: May 16, 2019
 *      Author: selina
 */

#include "FT812Q_Drawing.h"

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
	writeDisplay(RAM_DL + 0x0,  	0x4, 	CLEAR(1, 1, 1));
	writeDisplay(RAM_DL + 0x4, 		0x4, 	BITMAPS);
	uint32_t* address = drawString((uint32_t*)(RAM_DL + 0x8), 100, 110, 24, "hello there");
	address = drawString((uint32_t*)((uint32_t) address), 100, 150, 24, "general kenobi");
	writeDisplay((uint32_t)address++, 	0x4, 	END);
	writeDisplay((uint32_t)address++,		0x4, 	CLEAR_COLOR_RGB(160, 22, 22));
	writeDisplay((uint32_t)address++, 	0x4,	VERTEX_FORMAT(0));
	writeDisplay((uint32_t)address++, 	0x4, 	POINT_SIZE(160));
	writeDisplay((uint32_t)address++, 	0x4, 	POINTS);
	writeDisplay((uint32_t)address++,		0x4, 	POINT_DATA(50, 50));
	writeDisplay((uint32_t)address++, 	0x4, 	END);
	writeDisplay((uint32_t)address++, 	0x4, 	DISPLAY);
	writeDisplay(REG_DLSWAP, 		0x1, 	DLSWAP); // Display list swap
}

uint32_t* drawString(uint32_t* address, uint16_t x, uint16_t y, uint8_t spacing, char* string){
	int i = 0;
	uint32_t formatted[100];
	while(string[i] != '\0'){
		memcpy((uint8_t*)(formatted+i),LETTER_DATA(x, y, 16, string[i]),4);
		x+= spacing;
		//address++;
		i++;
	}
	writeDisplay((uint32_t)address, i*4, (uint8_t*)formatted);

	return address+i;	// return the address at which the next instruction should be
}
