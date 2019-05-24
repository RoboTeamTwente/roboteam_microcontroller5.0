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
#include "FT812Q_Drawing.h"
#include "PuTTY.h"

extern QSPI_HandleTypeDef hqspi;

/* BASIC */
void 		display_Init		();
void		display_Deinit		();
uint32_t 	writeDispBuf		(uint32_t base, uint32_t size, uint8_t* data);
void 		writeDisplay		(uint32_t address, uint32_t size, uint8_t* data);
uint8_t*	readDisplay			(uint32_t address, uint32_t size, uint8_t* data);

/* TOUCH */
uint16_t* 	readTouch			();
uint8_t		isInArea			(uint16_t* point);

/* FUNCTIONS FOR DRAWING */
uint8_t* 	CLEAR				(uint8_t c, uint8_t s, uint8_t t); // Clears screen
uint8_t* 	CLEAR_COLOR_RGB		(uint8_t red, uint8_t green, uint8_t blue); // Set background color
uint8_t* 	COLOR_RGB			(uint8_t red, uint8_t green, uint8_t blue); // Sets drawing color
uint8_t* 	POINT_SIZE			(uint16_t size); // Set the point size
uint8_t* 	LINE_WIDTH			(uint16_t width); // Set the line width
uint8_t* 	VERTEX_FORMAT		(uint8_t size);

uint8_t* 	POINT_DATA			(uint16_t x, uint16_t y); // Construct data for a point at (x,y)
uint8_t* 	LETTER_DATA			(uint16_t x, uint16_t y, uint8_t font, uint8_t letter); // Construct data for a letter at (x,y)

/* BUFFER */
extern uint8_t* 	DispBuf;			// pointer to display buffer
#define 			DISP_BUF_LENGTH 	0x1FFF

/* STATE MACHINE */
enum states {
	CONTROL, // robot
	CONTROL_MENU, // robot
    INIT, 
    MAIN, // basestation
    MAIN_MENU, // robot
    MENU, // basestation
    READ_TOUCH_CONTROL, // robot
	READ_TOUCH_ID, // basestation
	READ_TOUCH_RETURN, // basestation
	READ_TOUCH_MAIN_MENU, // robot
	READ_TOUCH_MENU, // basestation
    ROBOT, // basestation
} state;

#define ROBOT_ID_MAX	15
#define CHIP_PLUS		100
#define CHIP_MINUS		101
#define CHIP_VALUE		102
#define CONTROL_VALUE   103
#define DRIBBLE_PLUS	104
#define DRIBBLE_MINUS	105
#define DRIBBLE_VALUE	106
#define KICK_PLUS		107
#define KICK_MINUS		108
#define KICK_VALUE		109
#define MENU_VALUE		110
#define POWERDOWN_VALUE 111
#define RETURN_VALUE	112
#define	NO_TOUCH		200

typedef struct testStruct {
	uint8_t robotID;
}testStruct;
testStruct test2[1];

#endif /* FT812Q_FT812Q_H_ */
