#ifndef SDCARD_H_
#define SDCARD_H_

#include "stdint.h"
#include "stdbool.h"

///////////////////////////////////////////////////// PUBLIC FUNCTION DECLARATIONS

bool SDCard_Init();

bool SDCard_Write(uint8_t* data, uint32_t length, bool no_sync);

bool SDCard_Present();

bool SDCard_Initialized();

uint32_t SDCard_GetTotalBytesWritten();

#endif /* SDCARD_H_ */
