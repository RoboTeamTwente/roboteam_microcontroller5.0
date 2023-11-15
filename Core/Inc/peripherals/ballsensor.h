#ifndef BALLSENSOR_H_
#define BALLSENSOR_H_

#include "gpio_util.h"


/* public functions declarations */
void ballSensor_Init(); // initialize
void ballSensor_DeInit(); // deinitialize
void ballSensor_IRQ_Handler(); // irq handler
bool ballSensor_seesBall(); // getter


#endif /* BALLSENSOR_H_ */
