#ifndef BALLSENSOR_H_
#define BALLSENSOR_H_

#include "gpio_util.h"


/* ballsensor functions */
void ballSensor_Init(); // initialize
void ballSensor_DeInit(); // deinitialize
void ballSensor_IRQ_Handler(); // irq handler
bool ballSensor_seesBall(); // getter


/* ballsensor private functions */
void read_gpio();
void bs_I2C_error(uint8_t error);


#endif /* BALLSENSOR_H_ */
