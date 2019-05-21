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
uint32_t drawString(uint32_t addr, uint16_t x, uint16_t y, uint8_t spacing, char* string, uint8_t color[]);
uint32_t drawRect(uint32_t addr, uint16_t begin[], uint16_t end[], uint8_t color[], uint8_t width, uint8_t option); // 0 = filled, 1 = lines
uint32_t drawLine(uint32_t addr, uint16_t xb, uint16_t yb, uint16_t xe, uint16_t ye, uint8_t color[], uint16_t width);
uint32_t drawDot(uint32_t addr, uint16_t x, uint16_t y, uint8_t size, uint8_t color[]);
uint32_t drawUSBicon(uint32_t addr, uint16_t x, uint16_t y, bool USBstatus);
uint32_t drawReturn(uint32_t addr, uint16_t x, uint16_t y, uint8_t color[]);
uint32_t drawRobotInfo(uint8_t id);

void test();

/* ROBOT DATA */
typedef struct drawingData {
	uint16_t beginPoint[2];
	uint16_t endPoint[2];
	uint8_t robotData; // this is not the right type but then again, this is not basestation
	uint8_t TX_Packets;
	uint8_t RX_Packets;
}drawingData;

drawingData robots[16];

#endif /* FT812Q_FT812Q_DRAWING_H_ */
