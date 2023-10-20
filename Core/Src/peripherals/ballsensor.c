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

/*
*	read from GPIO:
		If high:
			set seesball to false --> we lost the ball
		else low --> ball is present:
			set seesball to true
		write to LOG about current ball state
		set LED to value of seesBall
*/
void ballSensor_IRQ_Handler() {
	read_gpio();
	if (!seesBall){
		LOG("[BALLSENSOR]:: We lost the ball");
	} else{
		LOG("[BALLSENSOR]:: We have the ball");
	}
	set_Pin(LED4_pin, seesBall);
}

bool ballSensor_seesBall() {
	return seesBall;
}



// ====================== PRIVATE FUNCTIONS ====================== //
void read_gpio() {
	/*
		Call readPin, will return GPIO_PIN_SET --> that value is high
					  otherwise will return GPIO_PIN_RESET
		If GPIO_PIN_SET then set return to false, otherwise true
	*/
	GPIO_PinState portState = HAL_GPIO_ReadPin(BS_IRQ_GPIO_Port, BS_IRQ_Pin);
	if (portState == GPIO_PIN_RESET){
		seesBall = false;
		return;
	}
	seesBall = true;
}

void bs_I2C_error(uint8_t error){
	seesBall = false;
	LOG_printf("[bs_I2C_error] %d\n", error);
}


