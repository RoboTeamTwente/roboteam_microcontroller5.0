
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

	static int count = 0;
	if (count > 10) {
		Putty_printf("%f %f \n\r", vel[0], vel[1]);
		count = 0;
	}
	count++;

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
	static const float wheels2BodyI = 1 / (2 * sinFront + 2 * sinBack);
	static const float wheels2BodyJ = cosFront / (2 * pow(cosFront, 2) + 2 * pow(cosBack, 2));
	static const float wheels2BodyK = sinBack / (2 * sinFront + 2 * sinBack);

	output[body_y] = wheels2BodyI * (wheelSpeeds[wheels_RF] + wheelSpeeds[wheels_LF] - wheelSpeeds[wheels_LB] - wheelSpeeds[wheels_RB]) * rad_wheel;
	output[body_x] = (wheels2BodyJ * (wheelSpeeds[wheels_RF] - wheelSpeeds[wheels_LF]) + (1 - wheels2BodyJ) * (-wheelSpeeds[wheels_LB] + wheelSpeeds[wheels_RB])) * rad_wheel;
	output[body_w] = (wheels2BodyK * (wheelSpeeds[wheels_RF] + wheelSpeeds[wheels_LF]) + (1 - wheels2BodyK) * (wheelSpeeds[wheels_LB] + wheelSpeeds[wheels_RB])) / rad_robot * rad_wheel;
}
