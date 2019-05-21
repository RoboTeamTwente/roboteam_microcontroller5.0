/*
 * FT812Q_Drawing.c
 *
 *  Created on: May 16, 2019
 *      Author: selina
 */

#include "FT812Q_Drawing.h"
#include <stdlib.h>

uint32_t nxt;

/* DRAW ON SCREEN */
void drawBasestation(bool USBstatus){

	// Initial settings
	nxt = writeDispBuf(0,		0x4, 	CLEAR_COLOR_RGB(40, 0, 0));
	nxt = writeDispBuf(nxt, 	0x4,	CLEAR(1, 1, 1));
	nxt = writeDispBuf(nxt, 	0x4,	VERTEX_FORMAT(0));

	/* DISPLAY LIST */
	nxt = drawString(nxt, 1, 1, 23, "Robot Status", RTTRED);

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
			nxt = drawRect(nxt, begin, end, RTTRED, 100, 1);
			itoa(i, id, 10);
			nxt = drawString(nxt, begin[0] + 5, begin[1] + 2, 23, id, WHITE);
			memcpy(robots[i].beginPoint, begin, 4);
			memcpy(robots[i].endPoint, end, 4);
			i++;
		}
	}

	// USB connection
	nxt = drawUSBicon(nxt, 430, 13, USBstatus);

	/* DRAW */
	nxt = writeDispBuf(nxt,		0x4, 	DISPLAY);
	writeDisplay(RAM_DL,		nxt, 	DispBuf);
	writeDisplay(REG_DLSWAP, 	0x1, 	DLSWAP); // Display list swap
}

uint32_t drawRobotInfo(uint8_t id){

	// Initial settings
	nxt = writeDispBuf(0,		0x4, 	CLEAR_COLOR_RGB(40, 0, 0));
	nxt = writeDispBuf(nxt,  	0x4, 	CLEAR(1, 1, 1));
	nxt = writeDispBuf(nxt,	 	0x4,	VERTEX_FORMAT(0));

	/* DISPLAY LIST */
	nxt = drawString(nxt, 40, 0, 29, "Robot:", RTTRED);
	char robotID[1]; itoa(id, robotID, 10);

	// TODO: check spacing
	nxt = drawString(nxt, 120, 1, 29, robotID, WHITE);
	nxt = drawString(nxt, 100, 100, 23, "yada yada", RTTRED);
	nxt = drawReturn(nxt, 0, 0, RTTRED);

	/* DRAW */
	nxt = writeDispBuf(nxt, 	0x4, 	DISPLAY);
	writeDisplay(RAM_DL, 		nxt, 	DispBuf);
	writeDisplay(REG_DLSWAP, 	0x1, 	DLSWAP); // Display list swap
	return nxt;
}

/* SUPPORTING FUNCTIONS */
uint32_t drawString(uint32_t addr, uint16_t x, uint16_t y, uint8_t handle, char* string, uint8_t color[]){
	addr = writeDispBuf(addr,  	0x4, 	COLOR_RGB(color[0], color[1], color[2]));
	addr = writeDispBuf(addr, 	0x4, 	BITMAPS);

	int i = 0;
	uint8_t* romfont;
	uint32_t width;
	uint8_t* spacing;
	while(string[i] != '\0'){
		addr = writeDispBuf(addr, 0x4, LETTER_DATA(x, y, handle, string[i]));
		// TODO: dont
		romfont = readDisplay(ROM_FONTROOT, 0x4, getData4);
		width = *(uint32_t*)romfont + (148 * (handle - 16));
		spacing = readDisplay(((uint32_t)width + string[i]), 0x1, getData1);
		x+= *spacing;
		i++;
	}
	return addr;	// return the address at which the next instruction should be
}

uint32_t drawRect(uint32_t addr, uint16_t begin[], uint16_t end[], uint8_t color[], uint8_t width, uint8_t option){
	if (option == 0){
		addr = writeDispBuf(addr,  	0x4, 	COLOR_RGB(color[0], color[1], color[2]));
		addr = writeDispBuf(addr,	0x4, 	RECTS);
		addr = writeDispBuf(addr,	0x4, 	POINT_DATA(begin[0], begin[1]));
		addr = writeDispBuf(addr,	0x4, 	POINT_DATA(end[0], end[1]));
	} else {
		addr = writeDispBuf(addr,	0x4, 	LINES);
		addr = writeDispBuf(addr,  	0x4, 	COLOR_RGB(color[0], color[1], color[2]));
		addr = writeDispBuf(addr,	0x4, 	POINT_DATA(begin[0], begin[1]));
		addr = writeDispBuf(addr,	0x4, 	POINT_DATA(end[0], begin[1]));
		addr = writeDispBuf(addr,	0x4, 	POINT_DATA(end[0], begin[1]));
		addr = writeDispBuf(addr,	0x4, 	POINT_DATA(end[0], end[1]));
		addr = writeDispBuf(addr,	0x4, 	POINT_DATA(end[0], end[1]));
		addr = writeDispBuf(addr,	0x4, 	POINT_DATA(begin[0], end[1]));
		addr = writeDispBuf(addr,	0x4, 	POINT_DATA(begin[0], end[1]));
		addr = writeDispBuf(addr,	0x4, 	POINT_DATA(begin[0], begin[1]));
		addr = writeDispBuf(addr,	0x4,	LINE_WIDTH(width));
	}
	return addr;
}

uint32_t drawLine(uint32_t addr, uint16_t xb, uint16_t yb, uint16_t xe, uint16_t ye, uint8_t color[], uint16_t width){
	addr = writeDispBuf(addr,  0x4, 	COLOR_RGB(color[0], color[1], color[2]));
	addr = writeDispBuf(addr,	0x4,	LINE_WIDTH(width));
	addr = writeDispBuf(addr, 	0x4, 	LINES);
	addr = writeDispBuf(addr,	0x4, 	POINT_DATA(xb, yb));
	addr = writeDispBuf(addr,	0x4,	POINT_DATA(xe, ye));
	return addr;
}

uint32_t drawDot(uint32_t addr, uint16_t x, uint16_t y, uint8_t size, uint8_t color[]){
	addr = writeDispBuf(addr, 0x4, COLOR_RGB(color[0], color[1], color[2]));
	addr = writeDispBuf(addr, 0x4, POINT_SIZE(size));
	addr = writeDispBuf(addr, 0x4, POINTS);
	addr = writeDispBuf(addr, 0x4, POINT_DATA(x, y));
	return addr;
}

uint32_t drawUSBicon(uint32_t addr, uint16_t x, uint16_t y, bool USBstatus){
	uint8_t color[3];
	if (USBstatus == 1){
		memcpy(color, GREEN, 3);
	} else
		memcpy(color, RED, 3);

	addr = drawDot(addr, 	x, 		y, 		80, color); // dot left
	addr = drawDot(addr, 	x + 40, y, 		60, color); // dot left
	addr = drawLine(addr, 	x, 		y, 		x + 40,		y, color, 2); // line to the right
	addr = drawLine(addr, 	x + 10, y, 		x + 15, 	y - 8, color, 2); // line above
	addr = drawLine(addr, 	x + 15, y - 8, 	x + 25, 	y - 8, color, 2); // line above
	addr = drawDot(addr, 	x + 25, y - 8, 	60, color); // dot above
	addr = drawLine(addr, 	x + 15, y, 		x + 20, 	y + 8, color, 2); // line below
	addr = drawLine(addr, 	x + 20, y + 8, 	x + 30, 	y + 8, color, 2); // line below
	addr = drawDot(addr, 	x + 30, y + 8, 	60, color); // dot below

	return addr;
}

uint32_t drawReturn(uint32_t addr, uint16_t x, uint16_t y, uint8_t color[]){
	uint16_t begin[] = {x,y};
	uint16_t end[] = {x + 30,y + 30};
	addr = drawRect(addr, begin, end, color, 2, 1);
	addr = drawLine(addr, x + 5, y + 15, x + 25, y + 15, color, 2);
	addr = drawLine(addr, x + 5, y + 15, x + 15, y + 10, color, 2);
	addr = drawLine(addr, x + 5, y + 15, x + 15, y + 20, color, 2);
	return addr;
}
