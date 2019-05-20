/*
 * FT812Q_Drawing.c
 *
 *  Created on: May 16, 2019
 *      Author: selina
 */

#include "FT812Q_Drawing.h"
#include <stdlib.h>

/* DRAW ON SCREEN */
void drawMainScreen(uint16_t ID){
	uint16_t begin[] = {0, 0}; uint16_t end[] = {0, 0};

	// Initial settings
	writeDisplay(RAM_DL + 0x0,  	0x4, 	CLEAR(1, 1, 1));
	writeDisplay(RAM_DL + 0x4,		0x4, 	CLEAR_COLOR_RGB(0, 0, 0));
	writeDisplay(RAM_DL + 0x8,	 	0x4,	VERTEX_FORMAT(0));

	// Create display list
	begin[0] = 3; begin[1] = 5; end[0] = 478; end[1] = 269;
	uint32_t* address = drawRect((uint32_t*)(RAM_DL + 0xC), begin, end, RTTRED, 100, 0); // draw red rectangle
	begin[0] = 5; begin[1] = 7; end[0] = 476; end[1] = 267;
	address = drawRect((uint32_t*)((uint32_t) address), begin, end, BLACK, 100, 0); // draw black rectangle
	address = drawString((uint32_t*)((uint32_t) address), 10, 10, 18, "ID:", RTTRED);
	char id[2]; itoa(ID, id, 10);
	address = drawString((uint32_t*)((uint32_t) address), 65, 10, 18, id, RTTRED);

	// Draw
	writeDisplay((uint32_t)address++, 	0x4, 	DISPLAY);
	writeDisplay(REG_DLSWAP, 			0x1, 	DLSWAP); // Display list swap
}

void drawBasestation(){

	// Initial settings
	writeDisplay(RAM_DL + 0x0,			0x4, 	CLEAR_COLOR_RGB(40, 0, 0));
	writeDisplay(RAM_DL + 0x4,  		0x4, 	CLEAR(1, 1, 1));
	writeDisplay(RAM_DL + 0x8,	 		0x4,	VERTEX_FORMAT(0));

	/* DISPLAY LIST */
	uint32_t* address = drawString((uint32_t*)(RAM_DL + 0xC), 1, 1, 23, "Robot Status", RTTRED);

	// Draw squares + robot ID's
	uint8_t spacingY = 60;
	uint8_t spacingX = 120;
	uint8_t i = 0;
	char id[1];
	for (uint8_t q = 0; q < 4; q++){
		for (uint8_t p = 0; p < 4; p++){
			uint16_t begin[] = {1 + spacingX*p, 31 + spacingY*q};
			uint16_t end[] = {1 + spacingX*(p + 1), 31 + spacingY*(q + 1)};
			if (end[0] >= 480){end[0] = 479;} // this is not a hack
			address = drawRect((uint32_t*)address, begin, end, RTTRED, 100, 1);
			itoa(i, id, 10);
			address = drawString((uint32_t*)address, begin[0] + 5, begin[1] + 2, 23, id, WHITE);
			memcpy(robots[i].beginPoint, begin, 4);
			memcpy(robots[i].endPoint, end, 4);
			i++;
		}
	}

	// USB connection
	address = drawUSBicon((uint32_t*)address, 430, 13, 1);

	// Draw display list
	writeDisplay((uint32_t)address++, 	0x4, 	DISPLAY);
	writeDisplay(REG_DLSWAP, 			0x1, 	DLSWAP); // Display list swap

	// touchscreen stuff
//	while(1){
//
//		uint16_t* touchPoint = readTouch();
//		Putty_printf("touch_x: %X, %hd \n\r", touchPoint[0], touchPoint[0]);
//		Putty_printf("touch_y: %X, %hd \n\r", touchPoint[1], touchPoint[1]);
//
//		if ((touchPoint[0] > 3 && touchPoint[0] < 123) && (touchPoint[1] > 31 && touchPoint[1] < 91)){
//			writeDisplay(RAM_DL + 0x0, 		0x4, 	CLEAR_COLOR_RGB(255, 200, 0));
//			writeDisplay(RAM_DL + 0x4,  	0x4, 	CLEAR(1, 1, 1));
//			writeDisplay(RAM_DL + 0x8, 		0x4, 	DISPLAY);
//			writeDisplay(REG_DLSWAP, 		0x1, 	DLSWAP); // Display list swap
//			return;
//		}
//
//	}

}

/* SUPPORTING FUNCTIONS */
uint32_t* drawString(uint32_t* address, uint16_t x, uint16_t y, uint8_t handle, char* string, uint8_t color[]){
	writeDisplay((uint32_t)address++,  	0x4, 	COLOR_RGB(color[0], color[1], color[2]));
	writeDisplay((uint32_t)address++, 	0x4, 	BITMAPS);

	int i = 0;
	uint32_t formatted[100];
	while(string[i] != '\0'){
		memcpy((uint8_t*)(formatted + i), LETTER_DATA(x, y, handle, string[i]), 4);

		// TODO: dont
		uint8_t* romfont = readDisplay(ROM_FONTROOT, 0x4, getData4);
		uint32_t width = *(uint32_t*)romfont + (148 * (handle - 16));
		uint8_t* spacing = readDisplay(((uint32_t)width + string[i]), 0x1, getData1);
		x+= *spacing;
		i++;
	}
	writeDisplay((uint32_t)address, i*4, (uint8_t*)formatted);

	return address + i;	// return the address at which the next instruction should be
}

uint32_t* drawRect(uint32_t* address, uint16_t begin[], uint16_t end[], uint8_t color[], uint8_t width, uint8_t option){
	if (option == 0){
		writeDisplay((uint32_t)address++,  	0x4, 	COLOR_RGB(color[0], color[1], color[2]));
		writeDisplay((uint32_t)address++,	0x4, 	RECTS);
		writeDisplay((uint32_t)address++,	0x4, 	POINT_DATA(begin[0], begin[1]));
		writeDisplay((uint32_t)address++,	0x4, 	POINT_DATA(end[0], end[1]));
	} else {
		writeDisplay((uint32_t)address++,	0x4, 	LINES);
		writeDisplay((uint32_t)address++,  	0x4, 	COLOR_RGB(color[0], color[1], color[2]));
		writeDisplay((uint32_t)address++,	0x4, 	POINT_DATA(begin[0], begin[1]));
		writeDisplay((uint32_t)address++,	0x4, 	POINT_DATA(end[0], begin[1]));
		writeDisplay((uint32_t)address++,	0x4, 	POINT_DATA(end[0], begin[1]));
		writeDisplay((uint32_t)address++,	0x4, 	POINT_DATA(end[0], end[1]));
		writeDisplay((uint32_t)address++,	0x4, 	POINT_DATA(end[0], end[1]));
		writeDisplay((uint32_t)address++,	0x4, 	POINT_DATA(begin[0], end[1]));
		writeDisplay((uint32_t)address++,	0x4, 	POINT_DATA(begin[0], end[1]));
		writeDisplay((uint32_t)address++,	0x4, 	POINT_DATA(begin[0], begin[1]));
		writeDisplay((uint32_t)address++,	0x4,	LINE_WIDTH(width));

	}
	return address;
}

uint32_t* drawLine(uint32_t* address, uint16_t xb, uint16_t yb, uint16_t xe, uint16_t ye, uint8_t color[], uint16_t width){
	writeDisplay((uint32_t)address++,  	0x4, 	COLOR_RGB(color[0], color[1], color[2]));
	writeDisplay((uint32_t)address++,	0x4,	LINE_WIDTH(width));
	writeDisplay((uint32_t)address++, 	0x4, 	LINES);
	writeDisplay((uint32_t)address++,	0x4, 	POINT_DATA(xb, yb));
	writeDisplay((uint32_t)address++,	0x4,	POINT_DATA(xe, ye));
	return address;
}

uint32_t* drawDot(uint32_t* address, uint16_t x, uint16_t y, uint8_t size, uint8_t color[]){
	writeDisplay((uint32_t)address++,  	0x4, 	COLOR_RGB(color[0], color[1], color[2]));
	writeDisplay((uint32_t)address++, 	0x4, 	POINT_SIZE(size));
	writeDisplay((uint32_t)address++, 	0x4, 	POINTS);
	writeDisplay((uint32_t)address++,	0x4, 	POINT_DATA(x, y));
	return address;
}

uint32_t* drawUSBicon(uint32_t* address, uint16_t x, uint16_t y, bool USBstatus){
	uint8_t color[3];
	if (USBstatus == 1){
		memcpy(color, GREEN, 3);
	} else
		memcpy(color, RED, 3);

	address = drawDot((uint32_t*)address, 	x, 		y, 		80, color); // dot left
	address = drawDot((uint32_t*)address, 	x + 40, y, 		60, color); // dot left
	address = drawLine((uint32_t*)address, 	x, 		y, 		x + 40,		y, color, 2); // line to the right
	address = drawLine((uint32_t*)address, 	x + 10, y, 		x + 15, 	y - 8, color, 2); // line above
	address = drawLine((uint32_t*)address, 	x + 15, y - 8, 	x + 25, 	y - 8, color, 2); // line above
	address = drawDot((uint32_t*)address, 	x + 25, y - 8, 	60, color); // dot above
	address = drawLine((uint32_t*)address, 	x + 15, y, 		x + 20, 	y + 8, color, 2); // line below
	address = drawLine((uint32_t*)address, 	x + 20, y + 8, 	x + 30, 	y + 8, color, 2); // line below
	address = drawDot((uint32_t*)address, 	x + 30, y + 8, 	60, color); // dot below

	return address;
}

/* ROBOT DATA */

