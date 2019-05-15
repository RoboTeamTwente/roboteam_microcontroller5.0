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

HAL_StatusTypeDef writeDisplay(uint32_t address, uint32_t size, uint8_t* data);
HAL_StatusTypeDef readDisplay(uint32_t address, uint32_t size, uint8_t* data);
HAL_StatusTypeDef display_Init();
uint8_t CLEAR_COLOR_RGB(uint8_t red, uint8_t green, uint8_t blue);

#endif /* FT812Q_FT812Q_H_ */
