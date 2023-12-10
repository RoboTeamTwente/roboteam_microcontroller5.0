
/* Description: Estimates the state
 *
 * Instructions:
 * 1) Takes all the data received/gathered by the robot
 * 2) Uses the kalman filter and yawcalibration to filter the data
 * 3) creates an up to date state estimation
 *
 * Extra functions:
 *
 * Notes:
 *
*/

#ifndef DO_STATEESTIMATION_H_
#define DO_STATEESTIMATION_H_

#include "yawCalibration.h"
#include "kalman.h"
#include "control_util.h"

///////////////////////////////////////////////////// CONSTANTS

/* Factors to align vision velocities with robot velocities (see https://wiki.roboteamtwente.nl/technical/control/slippage) */
#define SLIPPAGE_FACTOR_U 0.87	// Correction constant for slippage u direction
#define SLIPPAGE_FACTOR_V 0.91  // Correction constant for slippage v direction
#define SLIPPAGE_FACTOR_W 1.00  // Correction constant for slippage w direction (angular velocity)

///////////////////////////////////////////////////// STRUCTS

typedef struct StateInfo {
	float visionYaw;					// The yaw for this robot as indicated by vision [rad]
	bool visionAvailable;				// Wether vision data can be used at this point
	float xsensAcc[2];					// The acceleration as measured by the IMU in the X and Y directions [m/(s^2)]
	float xsensYaw;						// They yaw for this robot as indicated by the IMU [rad]
	float rateOfTurn;					// The angular velocity of the robot [rad/s]
	float wheelSpeeds[4];				// The speed for each wheel [rad/s]
	float dribblerSpeed;				// The measured speed of the dribbler [rad/s]
	float dribblerFilteredSpeed;		// The filtered speed of the dribbler [rad/s]
	float dribbleSpeedBeforeGotBall;	// The speed of the dribbler before it had a ball [rad/s]
} StateInfo;

///////////////////////////////////////////////////// PUBLIC FUNCTION DECLARATIONS

/**
 * Initializes the state estimation, which for now just sets up the kalman filter.
 */
int stateEstimation_Init();

/**
 * De-initializes the kalman filter.
 */
int stateEstimation_DeInit();

/**
 * Updates the current state based on the measured wheel speeds, vision yaw and acceleration
 * 
 * @param input The vision, IMU and encoder data used for state estimation
 */
void stateEstimation_Update(StateInfo* input);

/**
 * Get the current estimated local state
 * 
 * @param _stateLocal The curent state for u [m/s], v [m/s], w [rad/s] and yaw [rad]
 */
void stateEstimation_GetState(float _stateLocal[4]);

/**
 * Identical to stateEstimation_GetState(), but only returns w [rad/s].
 * 
 * @return float Rate of return
 */
float stateEstimation_GetFilteredRoT();

void stateControl_ResetPID();

#endif /* DO_STATEESTIMATION_H_ */
