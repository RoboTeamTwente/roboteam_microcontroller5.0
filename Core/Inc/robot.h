#ifndef __ROBOT__H_
#define __ROBOT__H_

#include <unistd.h>
#include "RobotCommand.h"
#include "RobotAssuredAck.h"

// These three are currently a callback hack for the REM implementation
volatile uint8_t robotCommandIsFresh;

uint8_t ROBOT_ID;

void print(char _out[]);

void init(void);
void loop(void);

void robot_setRobotCommandPayload(RobotCommandPayload* rcp);
void robot_receiveRobotAssuredAck(RobotAssuredAckPayload* raap);
uint8_t robot_getID();

#endif /* __ROBOT__H_ */