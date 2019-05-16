
#include "yawCalibration.h"
#include "PuTTY.h"

///////////////////////////////////////////////////// DEFINITIONS

#define BUFFER_SIZE 5 			// assume 50 ms (5 time steps) delay between vision and XSens
#define CALIBRATION_TIME 0.2f 	// number of seconds to do for averaging TODO: test this
#define MAX_RATE_OF_TURN (M_PI/2.0f) / 4.0f 	// highest rate of turn (rad/s) allowed to do calibration

///////////////////////////////////////////////////// VARIABLES

static float calibratedYaw = 0.0f;
static bool hasCalibratedOnce = false;

///////////////////////////////////////////////////// PRIVATE FUNCTION DECLARATIONS

// If the vision yaw and xsens yaw deviate too much for several time steps, set calibration needed to true
static bool isCalibrationNeeded(float visionYaw, float xsensYaw, float yawOffset);

// Check if robot has been rotating sufficiently slow for several time steps
static bool isRotatingSlow(float visionYaw);

// Get the oldest Xsens yaw from the buffer (as old as the assumed delay)
static float getOldXsensYaw(float newXsensYaw);

///////////////////////////////////////////////////// PUBLIC FUNCTION IMPLEMENTATIONS

void yaw_Calibrate(float newXsensYaw, float rateOfTurn, float visionYaw, bool visionAvailable) {
	static float yawOffset = 0.0f;
	static int restCounter = 0;
	static float sumXsensVec[2] = {0.0f};
	static float sumVisionVec[2] = {0.0f};
	static float prevVisionYaw = 0.0f;

	if (visionYaw == prevVisionYaw) {
		visionYaw += rateOfTurn * TIME_DIFF;
	}

	float oldXsensYaw = getOldXsensYaw(newXsensYaw);

	bool calibratedThisTick = false;
	if (isCalibrationNeeded(visionYaw, oldXsensYaw, yawOffset) && isRotatingSlow(visionYaw) && visionAvailable) {
		if (restCounter > CALIBRATION_TIME * TIME_DIFF) {
			// calculate offset
			float avgVisionYaw = atan2f(sumVisionVec[1], sumVisionVec[0]);
			float avgXsensYaw = atan2f(sumXsensVec[1], sumXsensVec[0]);
			yawOffset = constrainAngle(avgVisionYaw - avgXsensYaw);
			hasCalibratedOnce = true;
			calibratedThisTick = true;
			restCounter = 0;
		} else {
			// Sum the unit vectors with these angles and then take the angle of the resulting vector.
			sumXsensVec[0] += cosf(oldXsensYaw);
			sumXsensVec[1] += sinf(oldXsensYaw);
			sumVisionVec[0] += cosf(visionYaw);
			sumVisionVec[1] += sinf(visionYaw);
			restCounter++;
		}
	} else {
		restCounter = 0;
		sumXsensVec[0] = 0.0f;
		sumXsensVec[1] = 0.0f;
		sumVisionVec[0] = 0.0f;
		sumVisionVec[1] = 0.0f;
	}

	prevVisionYaw = visionYaw;
	calibratedYaw = constrainAngle(oldXsensYaw + yawOffset);

	uint printtime = 0;
	if (HAL_GetTick() - printtime > 100) {
		char msg[200];
		int n = 0;
		n += sprintf(msg + n, "vision: %f\n\r", visionYaw);
		n += sprintf(msg + n, "new xsens: %f\n\r", newXsensYaw);
		n += sprintf(msg + n, "old xsens: %f\n\r", oldXsensYaw);
		n += sprintf(msg + n, "calibrated: %f\n\r", calibratedYaw);
		n += sprintf(msg + n, "offset: %f\n\r", yawOffset);
		n += sprintf(msg + n, "bool: %u\n\r", calibratedThisTick);
		Putty_printf(msg);
		printtime = HAL_GetTick();
	}
}

float yaw_GetCalibratedYaw(){
	return calibratedYaw;
}

bool yaw_hasCalibratedOnce(){
	return hasCalibratedOnce;
};

///////////////////////////////////////////////////// PRIVATE FUNCTION IMPLEMENTATIONS

static bool isCalibrationNeeded(float visionYaw, float xsensYaw, float yawOffset) {

	static bool calibrationNeeded = false;
	static int checkCounter = 0;
	if (fabs(constrainAngle(visionYaw - (xsensYaw + yawOffset))) > M_PI/180) { // require 1 degree accuracy
		checkCounter++;
	} else {
		checkCounter = 0;
		calibrationNeeded = false;
	}
	if (checkCounter > 10) {
		checkCounter = 0;
		calibrationNeeded = true;
	}
	return calibrationNeeded;
}

static bool isRotatingSlow(float visionYaw) {
	static bool rotatingSlow = false;
	static int rotateCounter = 0;
	static float startYaw = 0;
	if (fabs(constrainAngle(startYaw - visionYaw)) < (MAX_RATE_OF_TURN * TIME_DIFF)) {
		rotateCounter++;
	} else {
		rotateCounter = 0;
		startYaw = visionYaw;
		rotatingSlow = false;
	}
	if (rotateCounter > 10) {
		rotateCounter = 0;
		startYaw = visionYaw;
		rotatingSlow = true;
	}
	return rotatingSlow;
}

static float getOldXsensYaw(float newXsensYaw) {
	static float buffer[BUFFER_SIZE] = {0.0f};
	static int index = 0;

	float oldXsensYaw = buffer[index];
	buffer[index] = newXsensYaw;
	index++;
	index = (index >= BUFFER_SIZE) ? 0 : index;
	return oldXsensYaw;
}

