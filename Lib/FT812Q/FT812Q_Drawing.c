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

	/* INITIAL SETTINGS */
	nxt = writeDispBuf(0,		0x4, 	CLEAR_COLOR_RGB(40, 0, 0));
	nxt = writeDispBuf(nxt, 	0x4,	CLEAR(1, 1, 1));
	nxt = writeDispBuf(nxt, 	0x4,	VERTEX_FORMAT(0));

	/* DISPLAY LIST */
	nxt = drawMenuButton(nxt, 429, 0, RTTRED);
	nxt = drawString(nxt, 97, 4, 23, "RoboTeam Twente Status Board", WHITE);
	uint16_t beginRect[] = {1, 31}; uint16_t endRect[] = {XRES, YRES};
	nxt = drawRect(nxt, beginRect, endRect, RTTRED, 2, 0);

	// Draw squares + robot ID's
	uint8_t spacingY = 60;
	uint8_t spacingX = 120;
	uint8_t i = 0;
	bool fakeStatus[16] = {false, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false};
	uint8_t fakeTX[16] = {60, 55, 90, 65, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60};
	uint8_t fakeRX[16] = {60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60};

	char id[1];
	for (uint8_t q = 0; q < 4; q++){
		for (uint8_t p = 0; p < 4; p++){
			
			// set rectangle per robot id
			uint16_t begin[] = {1 + spacingX*p + 2, 31 + spacingY*q + 2};
			uint16_t end[] = {1 + spacingX*(p + 1) - 1, 31 + spacingY*(q + 1) - 2};
			if (end[0] >= 480){end[0] = 478;} // this is not a hack
			memcpy(robots[i].beginPoint, begin, 4);
			memcpy(robots[i].endPoint, end, 4);
			itoa(i, id, 10);
			nxt = drawRect(nxt, begin, end, DARKRED, 2, 0); // background rectangle
			nxt = drawString(nxt, begin[0] + 5, begin[1] + 2, 23, id, WHITE);

			// Set online/offline
			if (fakeStatus[i] == true){
				robots[i].robotStatus = true;
				nxt = drawString(nxt, begin[0] + 40, begin[1] + 3, 22, "Online", GREEN);
			} else {
				robots[i].robotStatus = false;
				nxt = drawString(nxt, begin[0] + 40, begin[1] + 3, 22, "Offline", WHITE);
			}

			// Set RX
			uint16_t RXBegin[] = {begin[0] + 5, begin[1] + 30};
			uint16_t RXEnd[] = {begin[0] + 54, end[1] - 10};
			if (fakeStatus[i] == false){
				nxt = drawRect(nxt, RXBegin, RXEnd, GREY, 2, 0);
				nxt = drawString(nxt, begin[0] + 24, begin[1] + 31, 20, "RX", WHITE);
			} else if ((fakeRX[i] < 55) || (fakeRX[i] > 65)){ // not receiving enough packages
				char RX_char[5]; sprintf(RX_char, "%i", fakeRX[i]);
				nxt = drawRect(nxt, RXBegin, RXEnd, ORANGE, 2, 0);
				nxt = drawString(nxt, begin[0] + 24, begin[1] + 31, 20, RX_char, WHITE);
			} else {
				nxt = drawRect(nxt, RXBegin, RXEnd, GREEN, 2, 0);
				nxt = drawString(nxt, begin[0] + 24, begin[1] + 31, 20, "RX", WHITE);
			}
			
			// Set TX
			uint16_t TXBegin[] = {RXEnd[0] + 6, RXBegin[1]};
			uint16_t TXEnd[] = {end[0] - 6, end[1] - 10};
			if (fakeStatus[i] == false){
				nxt = drawRect(nxt, TXBegin, TXEnd, GREY, 2, 0);
				nxt = drawString(nxt, begin[0] + 80, begin[1] + 31, 20, "TX", WHITE);
			} else if ((fakeTX[i] < 55) || (fakeTX[i] > 65)){ // not sending enough packages
				char TX_char[5]; sprintf(TX_char, "%i", fakeTX[i]);
				nxt = drawRect(nxt, TXBegin, TXEnd, ORANGE, 2, 0);
				nxt = drawString(nxt, begin[0] + 80, begin[1] + 31, 20, TX_char, WHITE);
			} else {
				nxt = drawRect(nxt, TXBegin, TXEnd, GREEN, 2, 0);
				nxt = drawString(nxt, begin[0] + 80, begin[1] + 31, 20, "TX", WHITE);
			}

			i++;
		}
	}

	// USB connection
	nxt = drawUSBicon(nxt, 5, 13, USBstatus);

	/* DRAW */
	nxt = writeDispBuf(nxt,		0x4, 	DISPLAY);
	writeDisplay(RAM_DL,		nxt, 	DispBuf);
	writeDisplay(REG_DLSWAP, 	0x1, 	DLSWAP); // Display list swap
}

void drawMenu(uint8_t input){
	
	if (input == 0){
		/* INITIAL SETTINGS */
		nxt = writeDispBuf(0,		0x4, 	CLEAR_COLOR_RGB(40, 0, 0));
		nxt = writeDispBuf(nxt, 	0x4,	CLEAR(1, 1, 1));
		nxt = writeDispBuf(nxt, 	0x4,	VERTEX_FORMAT(0));

		/* DISPLAY LIST */
		nxt = drawReturn	(nxt, 	0, 		0, 		RTTRED);

		uint16_t begin[] = {140, 100};
		uint16_t end[] = {340, 172};
		nxt = drawRect(nxt, begin, end, RTTRED, 2, 1);
		nxt = drawString(nxt, 150, 115, 25, "Power down", WHITE);

		/* DRAW */
		nxt = writeDispBuf(nxt,		0x4, 	DISPLAY);
		writeDisplay(RAM_DL,		nxt, 	DispBuf);
		writeDisplay(REG_DLSWAP, 	0x1, 	DLSWAP); // Display list swap
	} else {
		nxt = writeDispBuf(0,		0x4, 	CLEAR_COLOR_RGB(40, 0, 0));
		nxt = writeDispBuf(nxt, 	0x4,	CLEAR(1, 1, 1));
		nxt = writeDispBuf(nxt, 	0x4,	VERTEX_FORMAT(0));

		/* DISPLAY LIST */
		nxt = drawReturn	(nxt, 	0, 		0, 		RTTRED);

		uint16_t begin[] = {140, 100};
		uint16_t end[] = {340, 172};
		nxt = drawRect(nxt, begin, end, RTTRED, 2, 1);
		nxt = drawString(nxt, 162, 119, 24, "How about no", WHITE);

		/* DRAW */
		nxt = writeDispBuf(nxt,		0x4, 	DISPLAY);
		writeDisplay(RAM_DL,		nxt, 	DispBuf);
		writeDisplay(REG_DLSWAP, 	0x1, 	DLSWAP); // Display list swap
	}
}

void drawMainMenu(){
	/* INITIAL SETTINGS */
	nxt = writeDispBuf(0,		0x4, 	CLEAR_COLOR_RGB(40, 0, 0));
	nxt = writeDispBuf(nxt, 	0x4,	CLEAR(1, 1, 1));
	nxt = writeDispBuf(nxt, 	0x4,	VERTEX_FORMAT(0));
	writeDisplay(REG_ROTATE, 	0x1,	INV_ROT);
	
	/* DISPLAY LIST */
	uint16_t beginEyes[] = {45, 100}; 
	uint16_t endEyes[] = {195, 172}; 
	// eyes->beginPoint[0] = beginEyes;
	// eyes->endPoint[0] = endEyes;
	nxt = drawRect(nxt, beginEyes, endEyes, RTTRED, 2, 1);
	nxt = drawString(nxt, 83, 115, 25, "Eyes", WHITE);
	uint16_t beginControl[] = {284, 100}; 
	uint16_t endControl[] = {435, 172};	
	// control.beginPoint[0] = beginControl;
	// control.endPoint[0] = endControl;
	nxt = drawRect(nxt, beginControl, endControl, RTTRED, 2, 1);
	nxt = drawString(nxt, 305, 115, 25, "Control", WHITE);

	/* DRAW */
	nxt = writeDispBuf(nxt,		0x4, 	DISPLAY);
	writeDisplay(RAM_DL,		nxt, 	DispBuf);
	writeDisplay(REG_DLSWAP, 	0x1, 	DLSWAP); // Display list swap
}

void drawControlMenu(uint8_t kickPower, uint8_t chipPower, int dribbleSpeed){

	/* INITIAL SETTINGS */
	nxt = writeDispBuf(0,		0x4, 	CLEAR_COLOR_RGB(40, 0, 0));
	nxt = writeDispBuf(nxt, 	0x4,	CLEAR(1, 1, 1));
	nxt = writeDispBuf(nxt, 	0x4,	VERTEX_FORMAT(0));
	writeDisplay(REG_ROTATE, 	0x1,	INV_ROT);

	/* DISPLAY LIST */
	// Kicking
	uint16_t beginKick[] 		= {6, 10};
	uint16_t endKick[]			= {190, 50};
	nxt = drawRect				(nxt, beginKick, endKick, RTTRED, 2, 1);
	nxt = drawStatusBar			(nxt, 20, 60, MAX_KICK, (float)kickPower);
	nxt = drawString			(nxt, 72, 20, 23, "KICK", WHITE);
	uint16_t beginKickPlus[] 	= {6, 82};
	uint16_t endKickPlus[] 		= {90, 122};
	nxt = drawRect				(nxt, beginKickPlus, endKickPlus, RTTRED, 2, 1);
	nxt = drawString			(nxt, 45, 89, 23, "+", WHITE);
	uint16_t beginKickMinus[] 	= {106, 82};
	uint16_t endKickMinus[]		= {190, 122};
	nxt = drawRect				(nxt, beginKickMinus, endKickMinus, RTTRED, 2, 1);
	nxt = drawString			(nxt, 147, 89, 23, "-", WHITE);

	// Chipping
	uint16_t beginChip[] 		= {6, 140};
	uint16_t endChip[]			= {190, 180};
	nxt = drawRect				(nxt, beginChip, endChip, RTTRED, 2, 1);
	nxt = drawStatusBar			(nxt, 20, 190, MAX_CHIP, (float)chipPower);
	nxt = drawString			(nxt, 72, 150, 23, "CHIP", WHITE);
	uint16_t beginChipPlus[] 	= {6, 212};
	uint16_t endChipPlus[] 		= {90, 252};
	nxt = drawRect				(nxt, beginChipPlus, endChipPlus, RTTRED, 2, 1);
	nxt = drawString			(nxt, 45, 219, 23, "+", WHITE);
	uint16_t beginChipMinus[] 	= {106, 212};
	uint16_t endChipMinus[]		= {190, 252};
	nxt = drawRect				(nxt, beginChipMinus, endChipMinus, RTTRED, 2, 1);
	nxt = drawString			(nxt, 147, 219, 23, "-", WHITE);

	// Dribbler
	uint16_t beginDribble[] 	= {210, 10};
	uint16_t endDribble[]		= {394, 50};
	nxt = drawRect				(nxt, beginDribble, endDribble, RTTRED, 2, 1);
	nxt = drawString			(nxt, 260, 20, 23, "DRIBBLE", WHITE);
	nxt = drawStatusBar			(nxt, 224, 60, MAX_DRIBBLE, (float)dribbleSpeed);
	uint16_t beginDribblePlus[]	= {210, 82};
	uint16_t endDribblePlus[]	= {294, 122};
	nxt = drawRect				(nxt, beginDribblePlus, endDribblePlus, RTTRED, 2, 1);
	nxt = drawString			(nxt, 249, 89, 23, "+", WHITE);
	uint16_t beginDribbleMinus[]= {310, 82};
	uint16_t endDribbleMinus[]	= {394, 122};
	nxt = drawRect				(nxt, beginDribbleMinus, endDribbleMinus, RTTRED, 2, 1);
	nxt = drawString			(nxt, 351, 89, 23, "-", WHITE);


	/* DRAW */
	nxt = writeDispBuf(nxt,		0x4, 	DISPLAY);
	writeDisplay(RAM_DL,		nxt, 	DispBuf);
	writeDisplay(REG_DLSWAP, 	0x1, 	DLSWAP); // Display list swap
}

uint32_t drawRobotInfo(uint8_t id, bool USBstatus){

	/* CALCULATE DATA */
	float angle = -2.35;
	char angle_char[6]; sprintf(angle_char, "%.2f", angle);
	float x_vel = 2.1;
	float y_vel = 4.2;
	bool kickStatus = true;
	bool chipStatus = true;
	bool dribblerStatus = true;

	/* INITIAL SETTINGS */
	nxt = writeDispBuf	(0,		0x4, 	CLEAR_COLOR_RGB(40, 0, 0));
	nxt = writeDispBuf	(nxt,  	0x4, 	CLEAR(1, 1, 1));
	nxt = writeDispBuf	(nxt, 	0x4,	VERTEX_FORMAT(0));

	/* DISPLAY LIST */
	nxt = drawString	(nxt, 	40, 	0, 		29, 		"Robot:", 		WHITE);
	
	char id_value[1]; itoa(id, id_value, 10);
	nxt = drawString	(nxt, 	120, 	1, 		29, 		id_value, 		WHITE);
	nxt = drawReturn	(nxt, 	0, 		0, 		RTTRED);
	nxt = drawUSBicon	(nxt, 	430, 	13, 	USBstatus); // USB connection

	nxt = drawString	(nxt, 	15, 	50, 	18, 		"X_VEL", 		WHITE);
	nxt = drawString	(nxt, 	15, 	80, 	18, 		"Y_VEL", 		WHITE);
	nxt = drawString	(nxt, 	15, 	110, 	18, 		"ANGLE", 		WHITE);

	nxt = drawKicker	(nxt, 	15, 	225, 	kickStatus); // draw kicker status
	nxt = drawChipper	(nxt, 	70,		225,	chipStatus); // draw chipper status
	nxt = drawDribbler	(nxt, 	135, 	225, 	dribblerStatus); // draw dribbler status
	
	nxt = drawStatusBar	(nxt, 	120, 	50, 	MAX_X_VEL, 	x_vel);
	nxt = drawStatusBar	(nxt, 	120, 	80, 	MAX_Y_VEL, 	y_vel);
	nxt = drawString	(nxt, 	120,	110,	18,			angle_char, 	WHITE);

	/* DRAW */
	nxt = writeDispBuf	(nxt, 	0x4, 	DISPLAY);
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

uint32_t drawKicker(uint32_t addr, uint16_t x, uint16_t y, bool kickStatus){
	uint8_t color[3];
	if (kickStatus == 1){
		memcpy(color, GREEN, 3);
	} else
		memcpy(color, RED, 3);

	uint8_t barWidth = 10;
	uint8_t barLength = 31;
	uint16_t begin[] = {x, y};
	uint16_t end[] = {x + barLength, y + barWidth};
	addr = drawRect(addr, begin, end, color, 2, 0); // draw kicker bar
	begin[0] = begin[0] + barWidth; begin[1] = end[1];
	end[0] = end[0] - barWidth; end[1] = begin[1] + barLength - 9;
	addr = drawRect(addr, begin, end, color, 2, 0); // draw bar that does the kicking
	return addr;
}

uint32_t drawChipper(uint32_t addr, uint16_t x, uint16_t y, bool chipStatus){
	uint8_t color[3];
	if (chipStatus == 1){
		memcpy(color, GREEN, 3);
	} else
		memcpy(color, RED, 3);

	uint8_t barWidth = 10;
	uint8_t barLength = 40;
	uint8_t armWidth = 3;
	uint8_t armLength = 22;

	// Draw arms
	uint16_t beginArm[] = {x, y};
	uint16_t endArm[] = {x + armWidth, y + armLength};
	addr = drawRect(addr, beginArm, endArm, WHITE, 2, 0);
	beginArm[0] = x + barLength - armWidth; endArm[0] = endArm[0] + barLength - armWidth;
	addr = drawRect(addr, beginArm, endArm, WHITE, 2, 0);

	// Draw plate
	uint16_t begin[] = {x, y + 22};
	uint16_t end[] = {x + barLength, begin[1] + barWidth};
	addr = drawRect(addr, begin, end, color, 2, 0);
	return addr;
}

uint32_t drawDribbler(uint32_t addr, uint16_t x, uint16_t y, bool dribblerStatus){
	uint8_t color[3];
	if (dribblerStatus == 1){
		memcpy(color, GREEN, 3);
	} else
		memcpy(color, RED, 3);

	uint8_t barLength = 40;
	uint8_t barWidth = 10;
	uint16_t begin[] = {x, y};
	uint16_t end[] = {x + barLength, y + barWidth};

	addr = drawRect(addr, begin, end, color, 2, 0);
	addr = drawDot(addr, (end[0] - x)/2 + x, end[1] + 12, 170, ORANGE);
	return addr;
}

uint32_t drawStatusBar(uint32_t addr, uint16_t x, uint16_t y, uint8_t max, float value){
	uint8_t barLength = 100;
	uint8_t barWidth = 10;
	uint16_t begin[] = {x, y + 1};
	uint16_t end[] = {x + barLength, y + barWidth + 1};
	addr = drawRect(addr, begin, end, WHITE, 2, 1);

	char max_value[1]; itoa((int)max, max_value, 10); // convert max to string to draw
	addr = drawString(addr, x - 15, y, 18, "0", RTTRED);
	addr = drawString(addr, end[0] + 8, y, 18, max_value, RTTRED);
	end[0] = ((value/(float)max)*barLength) + x;
	addr = drawRect(addr, begin, end, WHITE, 2, 0);

	char value_value[4]; sprintf(value_value, "%.1f", value);
	addr = drawString(addr, x + 140, y, 18, value_value, WHITE);
	return addr;
}

uint32_t drawMenuButton(uint32_t addr, uint16_t x, uint16_t y, uint8_t color[]){
	uint16_t begin[] = {x, y};
	uint16_t end[] = {x + 50,y + 31};
	addr = drawRect(addr, begin, end, color, 2, 1);
	addr = drawString(addr, x + 2, y + 4, 23, "Menu", color);
	return addr;
}
