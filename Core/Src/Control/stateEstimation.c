#include "stateEstimation.h"
#include "logging.h"

#define RoT_BUFFER_SIZE 5

///////////////////////////////////////////////////// VARIABLES

/**
 * @brief The local current state of the robot.
 * 
 * The state consists out of u [m/s], v [m/s], w [rad/s] and yaw [rad].
 */
static float stateLocal[4] = {0.0f};

// The pseudoinverse of the velocity coupling matrix, used to transform wheel speeds into local u [m/s], v [m/s] and w [rad/s] speeds.
static float Dinv[12] = {0.0f};

///////////////////////////////////////////////////// PRIVATE FUNCTION DECLARATIONS

/**
 * Translates the velocities from the wheels to the body velocities.
 * 
 * @param wheelSpeeds The speed achieved by each wheel [rad/s].
 * @param output 	  The u [m/s], v [m/s] and w [rad/s] speeds from a body perspective.
 */
static void wheels2Body(float wheelSpeeds[4], float output[3]);

/**
 * Smoothens out the IMU rate of turn data.
 * While this does decrease the response time slightly, it allows for smoother rotations.
 * 
 * @param rateOfTurn The current rate of turn as measured by the IMU [rad/s].
 * @return float     The smoothed out rate of turn [rad/s].
 */
float smoothen_rateOfTurn(float rateOfTurn);


///////////////////////////////////////////////////// PUBLIC FUNCTION IMPLEMENTATIONS

int stateEstimation_Init(){
	kalman_Init();

	// Initialize the pseudoinverse of the velocity coupling matrix.
	float divisor_a = sinBack + sinFront;
	Dinv[0] = -0.5 / divisor_a;
	Dinv[1] = Dinv[0];
	Dinv[2] = -Dinv[0];
	Dinv[3] = -Dinv[0];

	float divisor_b = cosBack * cosBack + cosFront * cosFront;
	Dinv[4] = 0.5 * cosFront / divisor_b;
	Dinv[5] = -Dinv[4];
	Dinv[6] = -0.5 * cosBack / divisor_b;
	Dinv[7] = -Dinv[6];

	Dinv[8] = 0.5 * sinBack / divisor_a;
	Dinv[9] = Dinv[8];
	Dinv[10] = 0.5 * sinFront / divisor_a;
	Dinv[11] = Dinv[10];

	return 0;
}

int stateEstimation_DeInit(){
	kalman_DeInit();
	return 0;
}

void stateEstimation_Update(StateInfo* input) {
	// TODO: when angular velocity control is implemented, you could improve state estimation
    //  by extending the Kalman filter to include angular velocity. You can combine the Xsens
    //  rateOfTurn with the angular velocity from wheel encoder data (already computed by wheels2Body)
    //  in this Kalman filter to improve the estimate.

	float vel[3] = {0.0f};
	wheels2Body(input->wheelSpeeds, vel);

	kalman_CalculateK();
	kalman_Update(input->xsensAcc, vel);

	float kalman_State[4] = {0.0f};
	kalman_GetState(kalman_State);

	// TODO: check if input->visionYaw is scaled properly with the new REM messages
	yaw_Calibrate(input->xsensYaw, input->visionYaw, input->visionAvailable, input->rateOfTurn);
	float calibratedYaw = yaw_GetCalibratedYaw();
	
	stateLocal[vel_u] = kalman_State[0];
	stateLocal[vel_v] = kalman_State[2];
	stateLocal[vel_w] = smoothen_rateOfTurn(input->rateOfTurn);
	stateLocal[yaw] = calibratedYaw;

	// Compensate for constant slippage by multiplying with empirically determined values.
	//  Explanation: https://wiki.roboteamtwente.nl/technical/control/slippage
	stateLocal[vel_u] = SLIPPAGE_FACTOR_U * stateLocal[vel_u];
	stateLocal[vel_v] = SLIPPAGE_FACTOR_V * stateLocal[vel_v];
	stateLocal[vel_w] = SLIPPAGE_FACTOR_W * stateLocal[vel_w];
}

void stateEstimation_GetState(float _stateLocal[4]) {
	_stateLocal[vel_u] = stateLocal[vel_u];
	_stateLocal[vel_v] = stateLocal[vel_v];
	_stateLocal[vel_w] = stateLocal[vel_w];
	_stateLocal[yaw] = stateLocal[yaw];
}

float stateEstimation_GetFilteredRoT() {
    return stateLocal[vel_w];
}


///////////////////////////////////////////////////// PRIVATE FUNCTION IMPLEMENTATIONS

static void wheels2Body(float wheelSpeeds[4], float output[3]){
	float wheelSpeedsLinear[4]; // The linear speed for each wheel [m/s]
	
	// Translate angular wheel velocities [rad/s] into regular velocities [m/s] on the outside of each wheel.
	for (wheel_names wheel=wheels_RF; wheel <= wheels_RB; wheel++) {
		wheelSpeedsLinear[wheel] = wheelSpeeds[wheel] * rad_wheel;
	}

	// Translate the wheel speeds into local u, v and r * w speeds.
	output[vel_u] = wheelSpeedsLinear[0] * Dinv[0] + wheelSpeedsLinear[1] * Dinv[1] + wheelSpeedsLinear[2] * Dinv[2] + wheelSpeedsLinear[3] * Dinv[3];
	output[vel_v] = wheelSpeedsLinear[0] * Dinv[4] + wheelSpeedsLinear[1] * Dinv[5] + wheelSpeedsLinear[2] * Dinv[6] + wheelSpeedsLinear[3] * Dinv[7];
	output[vel_w] = wheelSpeedsLinear[0] * Dinv[8] + wheelSpeedsLinear[1] * Dinv[9] + wheelSpeedsLinear[2] * Dinv[10] + wheelSpeedsLinear[3] * Dinv[11];

	// Translate the rotational velocity of the robot back to rad/s.
	output[vel_w] = output[vel_w] / rad_robot;
}

float smoothen_rateOfTurn(float rateOfTurn){
    static float buffer[RoT_BUFFER_SIZE] = {0.0f}; // circular buffer
    static int idx = 0; // holds current index of buffer

    buffer[idx] = rateOfTurn;
    //idx = idx >= RoT_BUFFER_SIZE-1 ? 0 : idx + 1;
	idx = (idx+1) % RoT_BUFFER_SIZE;

    float avg = 0.0f;  // average of buffer, which is the smoothed rate of turn
    for (int i=0; i<RoT_BUFFER_SIZE; i++){
        avg += buffer[i];
    }
    return avg / RoT_BUFFER_SIZE;
} 