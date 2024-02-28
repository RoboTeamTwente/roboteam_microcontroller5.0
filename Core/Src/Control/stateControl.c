
#include "stateControl.h"
#include "wheels.h"
#include "main.h"
#include "logging.h"

///////////////////////////////////////////////////// STRUCTS

// The PID values for the wheels
static PIDvariables wheelsK[4];

// The PID values for u, v, w and yaw.
static PIDvariables stateLocalK[4];

///////////////////////////////////////////////////// VARIABLES

// The current status of the system.
static PID_states status_control = off;

// The global x, y, w and yaw velocities to be achieved [m/s]
static float stateGlobalRef[4] = {0.0f}; 

// The wheel velocities to be achieved [rad/s]
static float wheelRef[4] = {0.0f};

// The voltages to be applied to each wheel
static float voltage_list[4] = {0.0f};

// The wheel PWMs sent to each wheel [-6000, 6000]
static int32_t pwm_list[4] = {0};

// The current global u, v, w and yaw velocities.
static float stateLocal[4] = {0.0f};

// The current u, v, w reference velocities.
float stateLocalRef[3] = {0, 0, 0};

// Whether to move to an absolute angle. If true yes, otherwise use angular velocity.
static bool useAbsoluteAngle = true;
static bool previousUseAbsoluteAngle = true;

// The velocity coupling matrix, used to transform local velocities into wheel velocities [4x3]
static float D[12] = {0.0f};

///////////////////////////////////////////////////// PRIVATE FUNCTION DECLARATIONS

/**
 * Translates the velocity from a local perspective to wheel speeds.
 * 
 * @param wheelSpeed The speed to be achieved for each wheel [rad/s]
 * @param stateLocal The velocities to be achieved seen from the body perspective {vel_u, vel_v, vel_w} [m/s]
 */
static void body2Wheels(float wheelSpeed[4], float stateLocal[3]);

/**
 * Translates the global coordinate frame to the local coordinate frame.
 * 
 * @param global 	The global coordinates {vel_x, vel_y, vel_w, yaw}
 * @param local 	The local coordinates {vel_u, vel_v, vel_w, yaw}
 * @param angle 	Yaw
 */
static void global2Local(float global[4], float local[4], float angle);

/**
 * Determines the desired wheel speeds given the desired velocities
 * 
 * @param stateLocal 		The current u, v, w and yaw speeds seen from the body
 * @param stateGlobalRef 			The instructed global x, y, w and yaw speeds
 * @param velocityWheelRef 	The resulting wheel speeds that should be achieved for each wheel
 */
static void velocityControl(float stateLocal[3], float stateGlobalRef[4], float velocityWheelRef[4]);

/**
 * Determine the speed that the wheels should achieve in order to move towards a desired angle.
 * 
 * @param angleRef The angle to be achieved.
 * @param angle    The current angle.
 * @return float   The PID value to achieve the set angle.
 */
static float absoluteAngleControl(float angleRef, float angle);

///////////////////////////////////////////////////// PUBLIC FUNCTION IMPLEMENTATIONS


int stateControl_Init(){
	status_control = on;
	initPID(&stateLocalK[vel_u], default_P_gain_u, default_I_gain_u, default_D_gain_u);
	initPID(&stateLocalK[vel_v], default_P_gain_v, default_I_gain_v, default_D_gain_v);
	initPID(&stateLocalK[vel_w], default_P_gain_w, default_I_gain_w, default_D_gain_w); 
	initPID(&stateLocalK[yaw], default_P_gain_yaw, default_I_gain_yaw, default_D_gain_yaw);

	/* Initialize wheel controllers */
	for (wheel_names wheel = wheels_RF; wheel <= wheels_RB; wheel++){
		initPID(&wheelsK[wheel], default_P_gain_wheels, default_I_gain_wheels, default_D_gain_wheels);
	}

	HAL_TIM_Base_Start_IT(TIM_CONTROL);

	// Initialize the velocity coupling matrix.
	D[0] = -sinFront;
	D[1] = cosFront; 
	D[2] = rad_robot;
	D[3] = -sinFront;
	D[4] = -cosFront;
	D[5] = rad_robot;
	D[6] = sinBack;
	D[7] = -cosBack;
	D[8] = rad_robot;
	D[9] = sinBack;
	D[10] = cosBack;
	D[11] = rad_robot;

	return 0;
}

int stateControl_DeInit(){
	status_control = off;
	HAL_TIM_Base_Stop_IT(TIM_CONTROL);
	return 0;
}

void stateControl_Full(){
	// Temp hotfix please remove:
	status_control = on;
	
	if (status_control == on){
		stateControl_Update_Body();
		stateControl_Update_Wheels();

		stateControl_voltage2PWM();
		wheels_SetPWM(pwm_list);
	}

	/* 
	 - implement the feedforward such that it outputs voltage (use file I made already on google drive)
	 	- implement simple feedforward assuming pure translational
			- do make sure you use the rho, theta_local and w as inputs here for generalisability, so make a vu,vx,vw to rho,theta_local,w conversion function
		- then extend it to include rotational
	 - make the feedback controllers output voltage
	 - voltage2PWM
	 - also implement the wheel ref adjustment again based on the slippage factors (also consider if applying it on the absolute angle controller output might make sense)
	 - make code such that it is possible to put a reference velocity to individual wheels, work out if feedforward is still possible in that case (or a simplified version)
	*/
	
}

void stateControl_Update_Body(){
	
	velocityControl(stateLocal, stateGlobalRef, wheelRef);

}

void stateControl_Update_Wheels(){

	// Below is what formerly used to be in wheel.c, but is now moved to control instead of the wheel peripherals:

	/* Don't run the wheels if these are not initialized */
	/* Not that anything would happen anyway, because the PWM timers wouldn't be running, but still .. */
	if(!wheels_Initialized()){
		wheels_Stop();
		return;
	}

	body2Wheels(wheelRef, stateLocalRef); //translate velocity to wheel speed
	float vu = stateLocalRef[vel_u];
	float vv = stateLocalRef[vel_v];
	float rho = sqrt(vu*vu + vv*vv);
	float theta_local = atan2(vu, vv);
	float omega = stateLocalRef[vel_w];

	/* Calculate the speeds of each wheel by looking at the encoders */
	float wheels_measured_speeds[4] = {0.0f};
	computeWheelSpeeds();
	wheels_GetMeasuredSpeeds(wheels_measured_speeds);

	float feed_forward[4] = {0.0f};
	
	for(wheel_names wheel = wheels_RF; wheel <= wheels_RB; wheel++){
		// Feedforward
		float identified_friction = 1.15f;
		float identified_damping = 0.0888f;
		if (wheelRef[wheel] > 0) {
			feed_forward[wheel] = identified_damping*wheelRef[wheel] + identified_friction;
		}
		else if (wheelRef[wheel] < 0) {
			feed_forward[wheel] = identified_damping*wheelRef[wheel] - identified_friction;
		}

		// Feedback
		// Calculate the velocity error
		float angular_velocity_error = wheelRef[wheel] - wheels_measured_speeds[wheel];

		// If the error is very small, ignore it (why is this here?)
		if (fabs(angular_velocity_error) < 0.1) {
			angular_velocity_error = 0.0f;
			wheelsK[wheel].I = 0;
		}

		// Add PID to commanded speed and convert to voltage 
		float PIDvoltageoutputfactor = 0.004f; // (24V /6000pwm) // Get rid of this factor and the OMEGAtoPWM factor by simply removing them and multiplying the P,IandD gains by (OMEGAtoPWM*PIDvoltageoutputfactor)
		voltage_list[wheel] = feed_forward[wheel] + OMEGAtoPWM * PIDvoltageoutputfactor * PID(angular_velocity_error, &wheelsK[wheel]); 
	}
	// for(wheel_names wheel = wheels_RF; wheel <= wheels_RB; wheel++){
	// 	voltage_list[wheel] = 1.5f;
	// }

	
}

void stateControl_voltage2PWM(){
	// Needs to be:
	// float batteryVoltage = getBatteryVoltage();
	// once the battery voltage is actually recorded somewhere. Then also directly input in the pwm equation instead of uselessly creating an intermediate variable
	// For now just assume:
	float batteryVoltage = 24;

	for(wheel_names wheel = wheels_RF; wheel <= wheels_RB; wheel++){
		pwm_list[wheel] = (int32_t) voltage_list[wheel]*(MAX_PWM/batteryVoltage);
	}
}

void stateControl_SetRef(float _stateGlobalRef[4]){
	stateGlobalRef[vel_x] = _stateGlobalRef[vel_x];
	stateGlobalRef[vel_y] = _stateGlobalRef[vel_y];
	stateGlobalRef[vel_w] = _stateGlobalRef[vel_w];
	stateGlobalRef[yaw] = _stateGlobalRef[yaw];
}

float* stateControl_GetWheelRef() {
	return wheelRef;
}

void stateControl_SetState(float _stateLocal[4]){
	stateLocal[vel_u] = _stateLocal[vel_u];
	stateLocal[vel_v] = _stateLocal[vel_v];
	stateLocal[vel_w] = _stateLocal[vel_w];
	stateLocal[yaw] = _stateLocal[yaw];
}

void stateControl_GetPIDGains(PIDvariables gainsLocal[4]){
	gainsLocal[vel_u].kP = stateLocalK[vel_u].kP;
	gainsLocal[vel_u].kI = stateLocalK[vel_u].kI;
	gainsLocal[vel_u].kD = stateLocalK[vel_u].kD;

	gainsLocal[vel_v].kP = stateLocalK[vel_v].kP;
	gainsLocal[vel_v].kI = stateLocalK[vel_v].kI;
	gainsLocal[vel_v].kD = stateLocalK[vel_v].kD;

	gainsLocal[vel_w].kP = stateLocalK[vel_w].kP;
	gainsLocal[vel_w].kI = stateLocalK[vel_w].kI;
	gainsLocal[vel_w].kD = stateLocalK[vel_w].kD;

	gainsLocal[yaw].kP = stateLocalK[yaw].kP;
	gainsLocal[yaw].kI = stateLocalK[yaw].kI;
	gainsLocal[yaw].kD = stateLocalK[yaw].kD;
}

float stateControl_GetIntegral(robot_axes direction) {
	return stateLocalK[direction].I;
}

void stateControl_useAbsoluteAngle(bool angularControl){
	if (angularControl != previousUseAbsoluteAngle){
		stateControl_ResetAngleI();
		stateControl_ResetPID();
	}
	previousUseAbsoluteAngle = angularControl;
    useAbsoluteAngle = angularControl;
}

void stateControl_SetPIDGains(REM_RobotSetPIDGains* PIDGains){
    stateLocalK[vel_u].kP = PIDGains->PbodyX;
    stateLocalK[vel_u].kI = PIDGains->IbodyX;
    stateLocalK[vel_u].kD = PIDGains->DbodyX;

    stateLocalK[vel_v].kP = PIDGains->PbodyY;
    stateLocalK[vel_v].kI = PIDGains->IbodyY;
    stateLocalK[vel_v].kD = PIDGains->DbodyY;

    stateLocalK[vel_w].kP = PIDGains->PbodyW;
    stateLocalK[vel_w].kI = PIDGains->IbodyW;
    stateLocalK[vel_w].kD = PIDGains->DbodyW;

    stateLocalK[yaw].kP = PIDGains->PbodyYaw;
    stateLocalK[yaw].kI = PIDGains->IbodyYaw;
    stateLocalK[yaw].kD = PIDGains->DbodyYaw;
}

void wheels_SetPIDGains(REM_RobotSetPIDGains* PIDGains){
	for(wheel_names wheel = wheels_RF; wheel <= wheels_RB; wheel++){
		wheelsK[wheel].kP = PIDGains->Pwheels;
		wheelsK[wheel].kI = PIDGains->Iwheels;
    	wheelsK[wheel].kD = PIDGains->Dwheels;
	}
}

void stateControl_ResetAngleI(){
	stateLocalK[yaw].I = 0;
	stateLocalK[vel_w].I = 0;
}

void stateControl_ResetPID(){
	stateLocalK[yaw].prev_e = 0;
	stateLocalK[yaw].prev_PID = 0;
	stateLocalK[vel_w].prev_e = 0;
	stateLocalK[vel_w].prev_PID = 0;
}

///////////////////////////////////////////////////// PRIVATE FUNCTION IMPLEMENTATIONS

static void body2Wheels(float wheelSpeed[4], float stateLocal[3]){

	// Translate the local u, v, and omega velocities into wheel velocities.
	wheelSpeed[wheels_RF] = stateLocal[vel_u] * D[0] + stateLocal[vel_v] * D[1]  + stateLocal[vel_w] * D[2];
	wheelSpeed[wheels_LF] = stateLocal[vel_u] * D[3] + stateLocal[vel_v] * D[4]  + stateLocal[vel_w] * D[5];
	wheelSpeed[wheels_LB] = stateLocal[vel_u] * D[6] + stateLocal[vel_v] * D[7]  + stateLocal[vel_w] * D[8];
	wheelSpeed[wheels_RB] = stateLocal[vel_u] * D[9] + stateLocal[vel_v] * D[10] + stateLocal[vel_w] * D[11];

	// Translate wheel velocities into angular velocities
	wheelSpeed[wheels_RF] = wheelSpeed[wheels_RF] / rad_wheel;
	wheelSpeed[wheels_LF] = wheelSpeed[wheels_LF] / rad_wheel;
	wheelSpeed[wheels_LB] = wheelSpeed[wheels_LB] / rad_wheel;
	wheelSpeed[wheels_RB] = wheelSpeed[wheels_RB] / rad_wheel;

}

static void global2Local(float global[4], float local[4], float angle){
	//trigonometry
	local[vel_u] = cosf(angle) * global[vel_x] + sinf(angle) * global[vel_y];
	local[vel_v] = -sinf(angle) * global[vel_x] + cosf(angle) * global[vel_y];
    local[vel_w] = global[vel_w];
	local[yaw] = global[yaw];
}

static void velocityControl(float stateLocal[3], float stateGlobalRef[4], float velocityWheelRef[4]){
	global2Local(stateGlobalRef, stateLocalRef, stateLocal[yaw]); //transfer global to local

	// Local control
	float veluErr = (stateLocalRef[vel_u] - stateLocal[vel_u]);
	float velvErr = (stateLocalRef[vel_v] - stateLocal[vel_v]);
	float velwErr = (stateLocalRef[vel_w] - stateLocal[vel_w]);

	stateLocalRef[vel_u] = stateLocalRef[vel_u] + PID(veluErr, &stateLocalK[vel_u]);
	stateLocalRef[vel_v] = stateLocalRef[vel_v] + PID(velvErr, &stateLocalK[vel_v]);
	stateLocalRef[vel_w] = stateLocalRef[vel_w] + PID(velwErr, &stateLocalK[vel_w]);

	// In case absolute angle control is used define the w (omega) differently based on absolute angle controller
	if (useAbsoluteAngle) {
		float angularRef = absoluteAngleControl(stateGlobalRef[yaw], stateLocal[yaw]);
    	stateLocalRef[vel_w] = (rad_wheel/rad_robot)*angularRef;
	}
}

static float absoluteAngleControl(float angleRef, float angle){
	static float prevangleErr = 0;
	float angleErr = constrainAngle(angleRef - angle);//constrain it to one circle turn
	if (angleErr == 0){
		angleErr = 0.000001*prevangleErr;
	}
	if (fabs(angleErr) < YAW_MARGIN || prevangleErr/angleErr < 0) {
		stateLocalK[yaw].I = 0;
	}
	prevangleErr = angleErr;
	return PID(angleErr, &stateLocalK[yaw]);// PID control from control_util.h
}