
#include "../Inc/stateEstimation.h"

///////////////////////////////////////////////////// VARIABLES

static float state[3] = {0.0f};

///////////////////////////////////////////////////// PRIVATE FUNCTION DECLARATIONS

//Transforms wheel speed to body velocity
static void wheels2Body(float wheelSpeeds[4], float output[3]);
void movingAverage(float acc[2], float meanAcc[2]);
///////////////////////////////////////////////////// PUBLIC FUNCTION IMPLEMENTATIONS

int stateEstimation_Init(){
	kalman_Init();
	return 0;
}

int stateEstimation_DeInit(){
	kalman_DeInit();
	return 0;
}

void stateEstimation_Update(StateInfo* input) {
	float vel[2] = {0.0f};
	wheels2Body(input->wheelSpeeds, vel);

	kalman_CalculateK();

	// Function that applies the moving average to the Xsens data before it goes into the Kalman Filter
	float meanAcc[2] = {0.0f};
	movingAverage(input->xsensAcc, meanAcc);

	//kalman_Update(input->xsensAcc, vel); 	// var = 6.25 Use for testing the kalman filter without MA.
	kalman_Update(meanAcc, vel);			// var = 0.25 Use for testing the kalman filter with MA.

	float kalman_State[4] = {0.0f};
	kalman_GetState(kalman_State);

	yaw_Calibrate(input->xsensYaw, input->visionYaw, input->visionAvailable, input->rateOfTurn);
	float calibratedYaw = yaw_GetCalibratedYaw();

	state[body_x] = kalman_State[0]; // Velocity of x
	state[body_y] = kalman_State[2]; // Velocity of y
	state[body_w] = calibratedYaw;
}

float* stateEstimation_GetState() {
	return state;
}

///////////////////////////////////////////////////// PRIVATE FUNCTION IMPLEMENTATIONS

static void wheels2Body(float wheelSpeeds[4], float output[3]){
	//Applying transpose(M_inv) matrix to go from wheel angular velocity to body velocity (assuming no slip)
	output[body_x] = (wheelSpeeds[wheels_RF] + wheelSpeeds[wheels_RB] - wheelSpeeds[wheels_LB] - wheelSpeeds[wheels_LF])/sin60 * rad_wheel/4;
	output[body_y] = (wheelSpeeds[wheels_RF] - wheelSpeeds[wheels_RB] - wheelSpeeds[wheels_LB] + wheelSpeeds[wheels_LF])/cos60 * rad_wheel/4;
	output[body_w] = (wheelSpeeds[wheels_RF] + wheelSpeeds[wheels_RB] + wheelSpeeds[wheels_LB] + wheelSpeeds[wheels_LF])/rad_robot * rad_wheel/4;
}


void movingAverage(float acc[2], float meanAcc[2]){
	// Define sumX and sumY, which are the sum of all (10) stored values to calculate the moving average
	static float sumX = 0;
	static float sumY = 0;
	static float storedValuesX[10] = {0.0f};
	static float storedValuesY[10] = {0.0f};

	// Update the sum of all stored values by subtracting the last value
	sumX -= storedValuesX[9];
	sumY -= storedValuesY[9];

	// Shift all stored values one index upward
	for (int i = 8; i>=0; i--){
		storedValuesX[i+1] = storedValuesX[i];
		storedValuesY[i+1] = storedValuesY[i];
	}

	// Include the current acceleration in the array
	storedValuesX[0] = acc[0];
	storedValuesY[0] = acc[1];

	// Update the sum by adding the acceleration values of the current timestep
	sumX += acc[0];
	sumY += acc[1];

	// Calculate the mean
	meanAcc[body_x] = sumX/10;
	meanAcc[body_y] = sumY/10;
}

