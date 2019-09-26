
#include "../Inc/stateEstimation.h"

///////////////////////////////////////////////////// VARIABLES

static float state[3] = {0.0f};

///////////////////////////////////////////////////// PRIVATE FUNCTION DECLARATIONS

//Transforms wheel speed to body velocity
static void wheels2Body(float wheelSpeeds[4], float output[3]);

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
	kalman_Update(input->xsensAcc, vel);

	float kalman_State[4] = {0.0f};
	kalman_GetState(kalman_State);

	yaw_Calibrate(input->xsensYaw, input->visionYaw, input->visionAvailable, input->rateOfTurn);
	float calibratedYaw = yaw_GetCalibratedYaw();

	state[body_x] = kalman_State[0];
	state[body_y] = kalman_State[2];
	state[body_w] = calibratedYaw;
}

float* stateEstimation_GetState() {
	return state;
}

///////////////////////////////////////////////////// PRIVATE FUNCTION IMPLEMENTATIONS

static void wheels2Body(float wheelSpeeds[4], float output[3]){
	static const float denominatorA = rad_wheel / (2 * (pow(sinFront, 2) + pow(sinBack, 2)));
	static const float denominatorB = rad_wheel / (2 * (cosFront + cosBack));

	output[body_x] = (sinFront * wheelSpeeds[wheels_RF] + sinBack * wheelSpeeds[wheels_RB] - sinBack * wheelSpeeds[wheels_LB] - sinFront * wheelSpeeds[wheels_LF]) * denominatorA;
	output[body_y] = (wheelSpeeds[wheels_RF] - wheelSpeeds[wheels_RB] - wheelSpeeds[wheels_LB] + wheelSpeeds[wheels_LF]) * denominatorB;
	output[body_w] = (cosBack * wheelSpeeds[wheels_RF] + cosFront * wheelSpeeds[wheels_RB] + cosFront * wheelSpeeds[wheels_LB] + cosBack * wheelSpeeds[wheels_LF]) * denominatorB / rad_wheel;
}
