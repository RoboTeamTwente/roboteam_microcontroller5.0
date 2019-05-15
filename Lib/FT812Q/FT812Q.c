/*
 * FT812Q.c
 *
 *  Created on: May 15, 2019
 *      Author: selina
 */

#include "FT812Q.h"

/* DATA */
uint8_t displaySetPowerdown[3] = {0x50, 0x0, 0x0};
uint8_t displaySetSleep[3] = {0x41, 0x0, 0x0};
uint8_t displaySetStandby[3] = {0x41, 0x0, 0x0};
uint8_t displaySetActive[3] = {0x0, 0x0, 0x0};
uint8_t HCYCLE[2] = {0x24, 0x2};
uint8_t HOFFSET[2] = {0x2B, 0x0};
uint8_t HSYNC0[2] = {0x0, 0x0};
uint8_t HSYNC1[2] = {0x29, 0x0};
uint8_t VCYCLE[2] = {0x24, 0x1};
uint8_t V0FFSET[2] = {0xC, 0x0};
uint8_t VSYNC0[2] = {0x0, 0x0};
uint8_t VSYNC1[2] = {0xA, 0x0};
uint8_t SWIZZLE[1] = {0x0};
uint8_t PCLK_POL[1] = {0x1};
uint8_t CSPREAD[1] = {0x1};
uint8_t HSIZE[2] = {0xE0, 0x1};
uint8_t VSIZE[2] = {0x10, 0x1};

uint8_t CLEARRGB[4] = {0x0, 0x0, 0xCC, 0x02};
uint8_t CLEAR[4] = {0x7, 0x0, 0x0, 0x26};
uint8_t DISPLAY[4] = {0x0, 0x0, 0x0, 0x0};
uint8_t DLSWAP[1] = {0x02};
uint8_t GPIO_DIR[1] = {0x80};
uint8_t GPIO[1] = {0x80};
uint8_t PCLK[1] = {0x5};

// get data
uint8_t getData1[1] = {0x0};
uint8_t getData2[2] = {0x0, 0x0};
uint8_t getData3[3] = {0x0, 0x0, 0x0};
uint8_t getData4[4] = {0x0, 0x0, 0x0, 0x0};

/* FUNCTIONS */
HAL_StatusTypeDef writeDisplay(uint32_t address, uint32_t size, uint8_t* data){

	if (address != 0x000000) {
		address |= 0x800000;
	}

	// Create write command
	QSPI_CommandTypeDef WRITE_COMMAND = {
		.InstructionMode 	= QSPI_INSTRUCTION_NONE,
		.Instruction 		= 0,
		.AddressMode 		= QSPI_ADDRESS_1_LINE,
		.AddressSize 		= QSPI_ADDRESS_24_BITS,
		.Address 			= address,
		.AlternateByteMode 	= QSPI_ALTERNATE_BYTES_NONE,
		.AlternateBytes 	= 0,
		.AlternateBytesSize = 0,
		.DataMode 			= QSPI_DATA_1_LINE,
		.DummyCycles 		= 0,
		.NbData 			= size,
		.DdrMode 			= QSPI_DDR_MODE_DISABLE,
		.DdrHoldHalfCycle 	= QSPI_DDR_HHC_ANALOG_DELAY,
		.SIOOMode 			= QSPI_SIOO_INST_EVERY_CMD
	};

	// Change AddressMode to QSPI_ADDRESS_NONE only for powerdown
	if (address == DISPLAY_SET_POWERDOWN) {
		WRITE_COMMAND.AddressMode = QSPI_ADDRESS_NONE;
	}

	// Command
	if (HAL_QSPI_Command(&hqspi, &WRITE_COMMAND, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
	    Putty_printf("HAL_ERROR \n\r");
	    return HAL_ERROR;
	}

	// Transmit
	if (HAL_QSPI_Transmit(&hqspi, data, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
	    Putty_printf("HAL_ERROR \n\r");
	    return HAL_ERROR;
	}

	return HAL_OK;
}

HAL_StatusTypeDef readDisplay(uint32_t address, uint32_t size, uint8_t* data){

	// Create write command
	QSPI_CommandTypeDef READ_COMMAND = {
		.InstructionMode 	= QSPI_INSTRUCTION_NONE,
		.Instruction 		= 0,
		.AddressMode 		= QSPI_ADDRESS_1_LINE,
		.AddressSize 		= QSPI_ADDRESS_24_BITS,
		.Address 			= address,
		.AlternateByteMode 	= QSPI_ALTERNATE_BYTES_NONE,
		.AlternateBytes 	= 0,
		.AlternateBytesSize = 0,
		.DataMode 			= QSPI_DATA_1_LINE,
		.DummyCycles 		= 8,
		.NbData 			= size,
		.DdrMode 			= QSPI_DDR_MODE_DISABLE,
		.DdrHoldHalfCycle 	= QSPI_DDR_HHC_ANALOG_DELAY,
		.SIOOMode 			= QSPI_SIOO_INST_EVERY_CMD
	};

	// Command
	if (HAL_QSPI_Command(&hqspi, &READ_COMMAND, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
	    Putty_printf("HAL_ERROR \n\r");
	    return HAL_ERROR;
	}

	// Receive
	if (HAL_QSPI_Receive(&hqspi, data, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
	    Putty_printf("HAL_ERROR \n\r");
		return HAL_ERROR;
	}

	for (int i = 0; i < size; i++) {
		Putty_printf("Data[%d]: %X \n\r", i, data[i]);
	}

	return HAL_OK;
}

HAL_StatusTypeDef display_Init(){

	writeDisplay(DISPLAY_SET_POWERDOWN, 0x3, displaySetPowerdown); // powerdown
	writeDisplay(DISPLAY_SET_ACTIVE, 0x3, displaySetActive); // bring to active mode
	HAL_Delay(300); // needs up to 300ms to start up

	/* Configure display registers */
	writeDisplay(REG_HCYCLE, 0x2, HCYCLE);
	writeDisplay(REG_HOFFSET, 0x2, HOFFSET);
	writeDisplay(REG_HSYNC0, 0x2, HSYNC0);
	writeDisplay(REG_HSYNC1, 0x2, HSYNC1);
	writeDisplay(REG_VCYCLE, 0x2, VCYCLE);
	writeDisplay(REG_VOFFSET, 0x2, V0FFSET);
	writeDisplay(REG_VSYNC0, 0x2, VSYNC0);
	writeDisplay(REG_VSYNC1, 0x2, VSYNC1);
	writeDisplay(REG_SWIZZLE, 0x1, SWIZZLE);
	writeDisplay(REG_PCLK_POL, 0x1, PCLK_POL); // 0 before, now 1
	writeDisplay(REG_CSPREAD, 0x1, CSPREAD);
	writeDisplay(REG_HSIZE, 0x2, HSIZE);
	writeDisplay(REG_VSIZE, 0x2, VSIZE);

	/* Write first display list */
	writeDisplay(RAM_DL + 0x0, 0x4, CLEARRGB); // TODO: make function
	writeDisplay(RAM_DL + 0x4, 0x4, CLEAR); // TODO: make function
	writeDisplay(RAM_DL + 0x8, 0x4, DISPLAY);

	/*  */
	writeDisplay(REG_DLSWAP, 0x1, DLSWAP); // Display list swap
	writeDisplay(REG_GPIO_DIR, 0x1, GPIO_DIR); //
	writeDisplay(REG_GPIO, 0x1, GPIO); // Enable display bit
	writeDisplay(REG_PCLK, 0x1, PCLK); // After this display is visible on the LCD

	return HAL_OK;
}

uint8_t CLEAR_COLOR_RGB(uint8_t red, uint8_t green, uint8_t blue){

	return *CLEARRGB;
}

