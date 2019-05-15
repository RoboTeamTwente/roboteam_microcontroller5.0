/*
 * FT812Q.h
 *
 *  Created on: May 15, 2019
 *      Author: selina
 */

#ifndef FT812Q_FT812Q_H_
#define FT812Q_FT812Q_H_

#include "main.h"
#include "stm32f7xx_hal_def.h"
#include "stm32f7xx_hal_qspi.h"
#include "FT812Q_Constants.h"
#include "PuTTY.h"

extern QSPI_HandleTypeDef hqspi;

/* BASIC */
HAL_StatusTypeDef 	writeDisplay			(uint32_t address, uint32_t size, uint8_t* data);
HAL_StatusTypeDef 	readDisplay				(uint32_t address, uint32_t size, uint8_t* data);
HAL_StatusTypeDef 	display_Init			();

/* FUNCTIONS FOR DRAWING */
uint8_t* 	CLEAR				(uint8_t c, uint8_t s, uint8_t t); // Clears screen
uint8_t* 	CLEAR_COLOR_RGB		(uint8_t red, uint8_t green, uint8_t blue); // Clears colour of screen
uint8_t* 	COLOR_RGB			(uint8_t red, uint8_t green, uint8_t blue); // Sets drawing colour
uint8_t* 	POINT_SIZE			(uint16_t size);

uint8_t* 	drawPoint			(uint16_t x, uint16_t y);

void drawDot();

//8FFFFFC1

#endif /* FT812Q_FT812Q_H_ */
