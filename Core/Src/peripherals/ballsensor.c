#include "ballsensor.h"
#include "limits.h"
#include "gpio_util.h"
#include "logging.h"
#include "string.h"
#include "peripheral_util.h"
#include "main.h"

// Static to keep everything local to this file
static uint32_t error; // for i2c errors
static volatile bool seesBall = false; // ball sensor sees ball



// ====================== PUBLIC FUNCTIONS ====================== //

void ballSensor_Init() {
	seesBall = false;
}

void ballSensor_DeInit() {
	seesBall = false;
}

void ballSensor_IRQ_Handler() {
	read_gpio();
}

bool ballSensor_seesBall() {
	return seesBall;
}



// ====================== PRIVATE FUNCTIONS ====================== //
void read_gpio() {
	//HAL_GPIO_ReadPin returns GPIO_PIN_SET if value is high, and GPIO_PIN_RESET if value is low
	seesBall = HAL_GPIO_ReadPin(BS_IRQ_GPIO_Port, BS_IRQ_Pin) != GPIO_PIN_SET;
}

void bs_I2C_error(uint8_t error){
	seesBall = false;
	LOG_printf("[bs_I2C_error] %d\n", error);
}


