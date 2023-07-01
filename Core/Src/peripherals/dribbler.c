#include "dribbler.h"

static const uint16_t DRIBBLER_MAX_PWM = 10000;
static const uint8_t  delay_size = 4;
static const float minimum_reliable_data = 200.0f;
#define MOVINGAVERAGE_BUFFER_SIZE 5

typedef struct movingAverage {
    float speed_buf[MOVINGAVERAGE_BUFFER_SIZE];   // stores measured speeds for a moving average filter
    int speed_idx;                                // index for the moving average buffer
    float command_buf[MOVINGAVERAGE_BUFFER_SIZE]; // stores commanded speeds to check if the dribbler has been turned off
    int command_idx;                              // index for commanded speed buffer 
    float speedBeforeGotBall;                     // keeps speed of ball while we don't have the ball
} movingAverage;

movingAverage moving_average = {0};

///////////////////////////////////////////////////// VARIABLES
static float speed = 0.0f;                       // Stores most recent measurement of dribbler speed in rad/s
static float speed_filtered = 0.0f;              // Stores filtered measurement of dribbler speed in rad/s
static float speed_filtered_previous = 0.0f;     // Stores the previous filtered measurement of dribbler speed in rad/s
static bool has_ball = false;                    // Stores information if dribbler thinks it has the ball
static uint32_t last_encoder_measurement = 0;    // Stores the last encoder measurement

///////////////////////////////////////////////////// PRIVATE FUNCTION DECLARATIONS

// Reads out the values of the wheel encoders
static int32_t getEncoderData();
// Resets the dribbler encoder
static void resetDribblerEncoders();
// Calculates angular velocity in rad/s for each wheel based on their encoder values
static void computeDribblerSpeed();
// moving average filter on the dribbler speed
float smoothen_dribblerSpeed(float speed);
// Calculates the average of an array (buffer)
float get_buffer_average(float *buffer, int size);

///////////////////////////////////////////////////// PUBLIC FUNCTIONS IMPLEMENTATIONS

void dribbler_Init(){
	start_PWM(PWM_Dribbler);
	/* Start the encoder */
	HAL_TIM_Base_Start(ENC_DRIBBLER);
	dribbler_SetSpeed(0);
}

void dribbler_DeInit(){
	stop_PWM(PWM_Dribbler);
	/* Stop the encoder */
	HAL_TIM_Base_Stop(ENC_DRIBBLER);
}

void dribbler_SetSpeed(float speed){
	if(speed > 1){
		speed = 1;
	}else if(speed < 0){
		speed = 0;
	}

    moving_average.command_buf[moving_average.command_idx] = speed;
    moving_average.command_idx = (moving_average.command_idx+1) % MOVINGAVERAGE_BUFFER_SIZE;	

	set_PWM(PWM_Dribbler, (uint32_t)(speed * DRIBBLER_MAX_PWM));
}


/**
 * @brief Updates the dribbler variables
 */
void dribbler_Update(){
	speed_filtered_previous = speed_filtered;
	computeDribblerSpeed();
	speed_filtered = smoothen_dribblerSpeed(speed);
}

/**
 * @brief Get the last measured dribbler speed in rad/s
 */
float dribbler_GetMeasuredSpeeds() {
	return speed;
}

/**
 * @brief Get the last filtered dribbler speed in rad/s
 */
float dribbler_GetFilteredSpeeds() {
	return speed_filtered;
}

/**
 * @brief Get the dribbler speed it had before getting the ball in rad/s
 */
float dribbler_GetSpeedBeforeGotBall() {
	return moving_average.speedBeforeGotBall;
}

/**
 * @brief Estimate if dribbler has the ball or not
 */
void dribbler_CalculateHasBall(){
	// check if moving average is moving up or down (speed reduces when dribbler gets the ball)
	bool speed_reducing   = ((speed_filtered - speed_filtered_previous + 1) < 0); 
	bool speed_increasing = ((speed_filtered - speed_filtered_previous - 1) > 0);
 
	float speed_command_average = get_buffer_average(moving_average.command_buf, MOVINGAVERAGE_BUFFER_SIZE);

	// update speed of the dribbler until it thinks it has the ball. This is used as the threshold value
	if (!has_ball){
		// Check if data is in the reliable range
		float* buf = moving_average.speed_buf;
		uint8_t idx = moving_average.speed_idx;
		float next = buf[(idx + 1) % MOVINGAVERAGE_BUFFER_SIZE];
		if (next > minimum_reliable_data){
			// Use a delayed value as the threshold (before it loses speed)
			moving_average.speedBeforeGotBall = next;
		}
	}

	// check if all conditions are met, assume we have the ball if so
	if (speed_reducing && (speed_filtered > minimum_reliable_data) && (speed_command_average > 0)){
		has_ball = true;
	}
	
	// Only say we lose the ball if the speed increases above the threshold value or if the dribbler turns off
	if (has_ball){
		if ((speed_increasing && (speed_filtered > (moving_average.speedBeforeGotBall-5))) || speed_command_average < 0.05f){
			has_ball = false;
		}
	}	

}

/**
 * @brief Get the dribbler has ball status
 */
bool dribbler_GetHasBall(){
	return has_ball;
}

/**
 * @brief Get the latest encoder measurement
 */
uint32_t dribbler_GetEncoderMeasurement() {
	return last_encoder_measurement;
}

///////////////////////////////////////////////////// PRIVATE FUNCTION IMPLEMENTATIONS

/**
 * @brief Reads out the counts of the dribbler encoder
 */
static int32_t getEncoderData(){
	uint32_t value = __HAL_TIM_GET_COUNTER(ENC_DRIBBLER);
	last_encoder_measurement = value;
	return value;
}

/**
 * @brief Resets the dribbler encoder
 */
static void resetDribblerEncoders() {
	__HAL_TIM_SET_COUNTER(ENC_DRIBBLER, 0);
}

/**
 * @brief Calculates angular velocity in rad/s for the dribbler based on their encoder values
 * 
 * @todo This function requires to be called every 100 milliseconds, as dictated by the variable TIME_DIFF contained
 * within the variable ENCODERtoOMEGA. This can of course not always be perfectly guaranteed. Therefore, a timer should
 * be used to calculate the time difference between two calculations.
 */
static void computeDribblerSpeed(){
	int32_t encoder_value = getEncoderData();
	resetDribblerEncoders();
	
	// Convert encoder values to rad/s
	speed = DRIBBLER_ENCODER_TO_OMEGA * (float) fabs((float)encoder_value);
}

/**
 * @brief Moving average filter for the dribbler speeds
 */
float smoothen_dribblerSpeed(float speed){
    moving_average.speed_buf[moving_average.speed_idx] = speed;
	moving_average.speed_idx = (moving_average.speed_idx+1) % MOVINGAVERAGE_BUFFER_SIZE;

    return get_buffer_average(moving_average.speed_buf, MOVINGAVERAGE_BUFFER_SIZE);
} 

/**
 * @brief Calculates the average of an array (buffer)
 */
float get_buffer_average(float *buffer, int size){
	if (size == 0) return 0.0; // make sure we don't divide by zero
	
	float sum = 0.0;
	for (int i=0; i<size; i++){
        sum += buffer[i];
    }

	return sum/(float)size;
}