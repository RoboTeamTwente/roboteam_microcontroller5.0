/*
 * Geneva.c
 *
 *  Created on: Nov 9, 2018
 *      Author: kjhertenberg
 */

#include "Geneva.h"
#include "tim.h"

///////////////////////////////////////////////////// DEFINITIONS
#define TIME_DIFF 0.01F // time difference due to 100Hz
#define GENEVA_CAL_EDGE_CNT 1980	// the amount of counts from an edge to the center
#define GENEVA_POSITION_DIF_CNT 810	// amount of counts between each of the five geneva positions
#define GENEVA_MAX_ALLOWED_OFFSET 0.2*GENEVA_POSITION_DIF_CNT	// maximum range where the geneva drive is considered in positon
#define SWITCH_OFF_TRESHOLD 200
#define MAX_DUTY_CYCLE_INVERSE_FRACTION 4

///////////////////////////////////////////////////// PRIVATE FUNCTION DECLARATIONS

//Is called when we find the edge and calibrates the encoders values based on the position
static void geneva_EdgeCallback(int edge_cnt);

//checks if we found the edge
static inline void CheckIfStuck(int8_t dir);

//sets the ref value based on the geneva state
static int geneva_SetRef(geneva_positions position);

//PID function
static float singlePID(float ref, float state, struct PIDconstants K);

static void setoutput(float PWM);

static int32_t ClipInt(int32_t input, int32_t min, int32_t max);//keeps the input between min and max values

///////////////////////////////////////////////////// PUBLIC FUNCTION IMPLEMENTATIONS

void geneva_Init(){
	geneva_state = geneva_setup;	// go to setup
	HAL_TIM_Base_Start(&htim2);		// start the encoder
	geneva_cnt  = HAL_GetTick();	// store the start time
	//PID constants
	genevaK.kP = 50.0F;//kp
	genevaK.kI = 4.0F;//ki
	genevaK.kD = 0.7F;//kd
	//Pin/time variables
	actuator = &htim10;
	actuator_channel = TIM_CHANNEL_1;
	dir[0] = Geneva_dir_B_Pin;
	dir[1] = Geneva_dir_A_Pin;
	dir_Port[0] = Geneva_dir_B_GPIO_Port;
	dir_Port[1] = Geneva_dir_A_GPIO_Port;
}

void geneva_Deinit(){
	HAL_TIM_Base_Start(&htim2);		// stop encoder
	geneva_state = geneva_idle;		// go to idle state
}

void geneva_Callback(int genevaStateRef){
	static int genevaRef = 0;
	static int currentStateRef = 3; //impossible state to kick-start

	switch(geneva_state){
		case geneva_idle:
			return;
		case geneva_setup:								// While in setup, slowly move towards the sensor/edge
			genevaRef = (HAL_GetTick() - geneva_cnt)*1;	// if sensor is not seen yet, move to the right at 1 count per millisecond
			CheckIfStuck(1);
			break;
		case geneva_running:					// wait for external sources to set a new ref
			if (genevaStateRef != currentStateRef){
				genevaRef = geneva_SetRef(genevaStateRef);
				currentStateRef = genevaStateRef;
			}
			break;
	}

	int state = (int32_t)__HAL_TIM_GetCounter(&htim2);
	float controlValue = singlePID(genevaRef, state, genevaK);
	setoutput(controlValue);
}

///////////////////////////////////////////////////// PRIVATE FUNCTION IMPLEMENTATIONS

static void geneva_EdgeCallback(int edge_cnt){
	__HAL_TIM_SET_COUNTER(&htim2, edge_cnt);
	geneva_state = geneva_running;
}

static inline void CheckIfStuck(int8_t dir){
	static uint tick = 0xFFFF;
	static int enc;
	int geneva_Encodervalue = (int32_t)__HAL_TIM_GetCounter(&htim2);
	if(geneva_Encodervalue != enc){
		enc = geneva_Encodervalue;
		tick = HAL_GetTick();
	}else if(tick + 70 < HAL_GetTick()){
		geneva_EdgeCallback(dir*GENEVA_CAL_EDGE_CNT);
	}
}

static int geneva_SetRef(geneva_positions position){
	switch(position){
	case geneva_rightright:
		return 2 * GENEVA_POSITION_DIF_CNT;
	case geneva_right:
		return 1 * GENEVA_POSITION_DIF_CNT;
	case geneva_middle:
		return 0 * GENEVA_POSITION_DIF_CNT;
	case geneva_left:
		return -1 * GENEVA_POSITION_DIF_CNT;
	case geneva_leftleft:
		return -2 * GENEVA_POSITION_DIF_CNT;
	case geneva_none:
		break;
	}
	return 0;
}

static float singlePID(float ref, float state, struct PIDconstants K){
	static float prev_e = 0;
	static float I = 0;
	float err = ref - state;
	float P = K.kP*err;
	if(abs(err)>(GENEVA_MAX_ALLOWED_OFFSET*0.5)){
		I += K.kI*err*TIME_DIFF;
	}
	float D = (K.kD*(err-prev_e))/TIME_DIFF;
	prev_e = err;
	float PIDvalue = P + I + D;
	return PIDvalue;
 }

static void setoutput(float pwm){
	if(pwm < -SWITCH_OFF_TRESHOLD){
		HAL_GPIO_WritePin(dir_Port[0], dir[0], 1);
		HAL_GPIO_WritePin(dir_Port[1], dir[1], 0);
	}else if(pwm > SWITCH_OFF_TRESHOLD){
		HAL_GPIO_WritePin(dir_Port[0], dir[0], 0);
		HAL_GPIO_WritePin(dir_Port[1], dir[1], 1);
	}else{
		HAL_GPIO_WritePin(dir_Port[0], dir[0], 0);
		HAL_GPIO_WritePin(dir_Port[1], dir[1], 0);
	}
	pwm = abs(pwm);
	pwm = ClipInt(pwm, 0, actuator->Init.Period/MAX_DUTY_CYCLE_INVERSE_FRACTION);// Power limited by having maximum duty cycle
	__HAL_TIM_SET_COMPARE(actuator, actuator_channel, pwm);
}

static int32_t ClipInt(int32_t input, int32_t min, int32_t max){
	return (input > max) ? max : (input < min) ? min : input;
}

