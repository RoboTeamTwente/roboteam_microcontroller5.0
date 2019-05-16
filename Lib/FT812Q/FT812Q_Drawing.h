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

void drawPoint(); // Draw a point
void drawLetter(); // Draw a letter
void drawLine();
void drawExample();
uint32_t* drawString(uint32_t* address, uint16_t x, uint16_t y, uint8_t spacing, char* string, uint8_t color[]);
uint32_t* drawRect(uint32_t* address, uint16_t begin[], uint16_t end[], uint8_t color[]);
void drawMainScreen(uint16_t ID);

#endif /* FT812Q_FT812Q_DRAWING_H_ */
