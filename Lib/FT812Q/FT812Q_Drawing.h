/*
 * FT812Q_Drawing.h
 *
 *  Created on: May 16, 2019
 *      Author: selina
 */

#ifndef FT812Q_FT812Q_DRAWING_H_
#define FT812Q_FT812Q_DRAWING_H_

#include "FT812Q.h"
#include "FT812Q_Constants.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/* FUNCTIONS */
void drawBasestation(bool USBstatus);
uint32_t drawRobotInfo(uint8_t id, bool USBstatus);
uint32_t drawString(uint32_t addr, uint16_t x, uint16_t y, uint8_t spacing, char* string, uint8_t color[]);
uint32_t drawRect(uint32_t addr, uint16_t begin[], uint16_t end[], uint8_t color[], uint8_t width, uint8_t option); // 0 = filled, 1 = lines
uint32_t drawLine(uint32_t addr, uint16_t xb, uint16_t yb, uint16_t xe, uint16_t ye, uint8_t color[], uint16_t width);
uint32_t drawDot(uint32_t addr, uint16_t x, uint16_t y, uint8_t size, uint8_t color[]);
uint32_t drawUSBicon(uint32_t addr, uint16_t x, uint16_t y, bool USBstatus);
uint32_t drawReturn(uint32_t addr, uint16_t x, uint16_t y, uint8_t color[]);
uint32_t drawKicker(uint32_t addr, uint16_t x, uint16_t y, bool kickStatus);
uint32_t drawChipper(uint32_t addr, uint16_t x, uint16_t y, bool chipStatus);
uint32_t drawDribbler(uint32_t addr, uint16_t x, uint16_t y, bool dribblerStatus);
uint32_t drawStatusBar(uint32_t addr, uint16_t x, uint16_t y, uint8_t max, float value);

void test();

/* ROBOT DATA */
typedef struct drawingData {
	uint16_t beginPoint[2];
	uint16_t endPoint[2];
	bool robotStatus;
	uint8_t TX_Packets;
	uint8_t RX_Packets;
}drawingData;

drawingData robots[16];

// this is not the right place for this
#define MAX_DRIBBLE_VEL 	7
#define MAX_ANGLE			3
#define MAX_X_VEL			8
#define MAX_Y_VEL			8
#define MAX_BAT				12

#endif /* FT812Q_FT812Q_DRAWING_H_ */
