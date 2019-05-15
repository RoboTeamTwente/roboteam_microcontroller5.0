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
uint8_t displayGetActive[1] = {0x0};
uint8_t displayGetHCYCLE[2] = {0x0, 0x0};
uint8_t displaySetHCYCLE[2] = {0x24, 0x2};
uint8_t displaySetHOFFSET[2] = {0x2B, 0x0};
uint8_t displatSetHSYNC0[2] = {0x0, 0x0};
uint8_t displaySetHSYNC1[2] = {0x29, 0x0};
uint8_t displaySetVCYCLE[2] = {0x24, 0x1};
uint8_t displaySetV0FFSET[2] = {0xC, 0x0};
uint8_t displaySetVSYNC0[2] = {0x0, 0x0};
uint8_t displaySetVSYNC1[2] = {0xA, 0x0};
uint8_t displaySetSWIZZLE[1] = {0x0};
uint8_t displaySetPCLK_POL[1] = {0x1};
uint8_t displaySetCSPREAD[1] = {0x1};
uint8_t displaySetHSIZE[2] = {0xE0, 0x1};
uint8_t displaySetVSIZE[2] = {0x10, 0x1};
uint8_t displayGetCLEARRGB[4] = {0x0, 0x0, 0x0, 0x0};
uint8_t displaySetCLEARRGB[4] = {0x2, 0x0, 0x0, 0x0};
uint8_t displaySetCLEAR[4] = {0x26, 0x0, 0x0, 0x07};
uint8_t displaySetDISPLAY[4] = {0x0, 0x0, 0x0, 0x0};
uint8_t displaySetDLSWAP[1] = {0x02};
uint8_t displaySetGPIO_DIR[1] = {0x80};
uint8_t displaySetGPIO[1] = {0x80};
uint8_t displaySetPCLK[1] = {0x5};

/* FUNCTIONS */
HAL_StatusTypeDef writeDisplay(uint32_t address, uint32_t size, uint8_t* data){

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
	writeDisplay(DISPLAY_SET_REG_HCYCLE, 0x2, displaySetHCYCLE);
	writeDisplay(DISPLAY_SET_REG_HOFFSET, 0x2, displaySetHOFFSET);
	writeDisplay(DISPLAY_SET_REG_HSYNC0, 0x2, displatSetHSYNC0);
	writeDisplay(DISPLAY_SET_REG_HSYNC1, 0x2, displaySetHSYNC1);
	writeDisplay(DISPLAY_SET_REG_VCYCLE, 0x2, displaySetVCYCLE);
	writeDisplay(DISPLAY_SET_REG_VOFFSET, 0x2, displaySetV0FFSET);
	writeDisplay(DISPLAY_SET_REG_VSYNC0, 0x2, displaySetVSYNC0);
	writeDisplay(DISPLAY_SET_REG_VSYNC1, 0x2, displaySetVSYNC1);
	writeDisplay(DISPLAY_SET_REG_SWIZZLE, 0x1, displaySetSWIZZLE);
	writeDisplay(DISPLAY_SET_REG_PCLK_POL, 0x1, displaySetPCLK_POL);
	writeDisplay(DISPLAY_SET_REG_CSPREAD, 0x1, displaySetCSPREAD);
	writeDisplay(DISPLAY_SET_REG_HSIZE, 0x2, displaySetHSIZE);
	writeDisplay(DISPLAY_SET_REG_VSIZE, 0x2, displaySetVSIZE);

	/* Write first display list */
	writeDisplay(DISPLAY_SET_CLEAR_COLOR_RGB, 0x4, displaySetCLEARRGB); // TODO: make function
	writeDisplay(DISPLAY_SET_CLEAR, 0x4, displaySetCLEAR); // TODO: make function
	writeDisplay(DISPLAY_SET_DISPLAY, 0x4, displaySetDISPLAY); // TODO: make function
	writeDisplay(DISPLAY_SET_REG_DLSWAP, 0x1, displaySetDLSWAP); // TODO: check if ok
	writeDisplay(DISPLAY_SET_REG_GPIO_DIR, 0x1, displaySetGPIO_DIR);
	writeDisplay(DISPLAY_SET_REG_GPIO, 0x1, displaySetGPIO); // turn display on
//	writeDisplay(DISPLAY_SET_REG_PCLK, 0x1, displaySetPCLK);

	return HAL_OK;
}

uint8_t CLEAR_COLOR_RGB(uint8_t red, uint8_t green, uint8_t blue){

	return displaySetCLEARRGB;
}

