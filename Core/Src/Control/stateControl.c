
#include "stateControl.h"
#include "logging.h"
#include "wheels.h"
#include "main.h"

///////////////////////////////////////////////////// STRUCTS

// The PID values for the wheels
static PIDvariables wheelsK[4];

///////////////////////////////////////////////////// VARIABLES

// The current status of the system.
static PID_states status = off;

// The PID values for u, v, w and yaw.
static PIDvariables stateLocalK[4];

// The global x, y, w and yaw velocities to be achieved [m/s]
static float stateGlobalRef[4] = {0.0f}; 

// The wheel velocities to be achieved [rad/s]
static float wheelRef[4] = {0.0f};

// The voltages to be sent to the wheels
static float voltage_list[4] = {0.0f};

// The pwm values sent to the wheels
static int32_t wheel_pwm_list[4] = {0.0f};

// The current global u, v, w and yaw velocities.
static float stateLocal[4] = {0.0f};

// The reference u, v, w reference velocities.
static float stateLocalRef[4] = {0.0f};

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
static void velocityControl(float stateLocal[4], float stateGlobalRef[4], float velocityWheelRef[4]);

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
	status = on;
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
	status = off;
	HAL_TIM_Base_Stop_IT(TIM_CONTROL);
	return 0;
}

void stateControl_Update(){
	if (status == on){
		stateControl_Update_Body();
		computeWheelSpeeds();
		stateControl_Update_Wheels();
		stateControl_voltage2PWM();
		wheels_SetPWM(wheel_pwm_list);
	}	
}

void stateControl_Update_Body() {

	// velocityControl(stateLocal, stateGlobalRef, velocityWheelRef);
	velocityControl(stateLocal, stateGlobalRef, wheelRef);

}

float sineEval(float x,float a,float b,float c) {
	float y = a*sinf(b*x+c);
	return y;
}

float constEval(float x,float b,float c, float d) {
	float y_sign = sineEval(x,d,b,c);
	float y = 0.0f;
	if (y_sign>0) {
		y = d;
	} else {
		y = -d;
	}
	return y;
}

float constsineEval(float x,float a,float b,float c, float d) {
	float y = sineEval(x,a,b,c) + constEval(x,b,c,d);
	return y;
}

float feedforwardFriction(float wheelRef, float rho, float theta, float omega, wheel_names wheel) {
	// Parameters to tune
	float vw_max_round_to_rotational_scaling = 1;
	float rotation_feedforward_value[4] = {0.8f,0.8f,0.8f,0.8f};
	
	float a_list[4] = {0.8264f,0.8264f,0.8264f,0.8264f};
	float b_list[4] = {(2*M_PI)/360, (2*M_PI)/360, (2*M_PI)/360, (2*M_PI)/360};
	float c_list[4] = {60*(M_PI/180), -60*(M_PI/180), -150*(M_PI/180), 150*(M_PI/180)};
	float d_list[4] = {0.4132f,0.4132f,0.4132f,0.4132f};

	// Calculations
	float vw_max_round_to_rotational = vw_max_round_to_rotational_scaling*(rho/rad_robot);
	float z_rotational = rotation_feedforward_value[wheel];
	float z_translation = constsineEval(theta,a_list[wheel],b_list[wheel],c_list[wheel],d_list[wheel]);

	int wheel_velocity_larger_than_zero;
	if(wheelRef > 0) {
		wheel_velocity_larger_than_zero = 1;
	}
	else {
		wheel_velocity_larger_than_zero = 0;
	}

	float z_rotational_fixed;
	float z_translation_fixed = z_translation;
	if(wheel_velocity_larger_than_zero == 1) {
		z_rotational_fixed = z_rotational;
		if(z_translation < 0) {
			z_translation_fixed = d_list[wheel];
		}
	}
	else {
		z_rotational_fixed = -1*z_rotational;
		if(z_translation > 0) {
			z_translation_fixed = -d_list[wheel];
		}
	}

	float gamma = 0.5-0.5*cos(M_PI * fabs(omega/vw_max_round_to_rotational));
	if (fabs(omega) > vw_max_round_to_rotational) {
		gamma = 1;
	}
	if (gamma > 1) {
		gamma = 1;
	}
	float feedforwardFrictionValue = ((1-gamma)*z_translation_fixed + gamma*z_rotational_fixed);
	return feedforwardFrictionValue;
}

void stateControl_Update_Wheels() {
		/* Don't run the wheels if these are not initialized */
	/* Not that anything would happen anyway, because the PWM timers wouldn't be running, but still .. */
	if(!wheels_AreInitialized()){
		wheels_Stop();
		return;
	}
	
	body2Wheels(wheelRef, stateLocalRef); //translate velocity to wheel speed
	float vu = stateLocalRef[vel_u];
	float vv = stateLocalRef[vel_v];
	float rho = sqrt(vu*vu + vv*vv);
	float theta_local = atan2(vu, vv);
	float omega = stateLocalRef[vel_w];

	float wheels_measured_speeds_test[4] = {0.0f};
	wheels_GetMeasuredSpeeds(wheels_measured_speeds_test);

	float feed_forward[4] = {0.0f};

	for(wheel_names wheel = wheels_RF; wheel <= wheels_RB; wheel++){

		// Feedforward
		double threshold = 0.5;
		float identified_friction = 1.15f;
		float identified_damping = 0.0888f;
		// float identified_friction = 13.0f;
		// float identified_damping = 1.0f;
		
		

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// float friction_this_wheel_this_angle = ;

		// if (fabs(wheelRef[wheel]) < threshold) {
    	// 	feed_forward[wheel] = 0;
		// } 
		// else if (wheelRef[wheel] > 0) {
		// 	feed_forward[wheel] = identified_damping*wheelRef[wheel] + identified_friction;
    	// }
		// else if (wheelRef[wheel] < 0) {
		// 	feed_forward[wheel] = identified_damping*wheelRef[wheel] - identified_friction;
    	// }

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// float a_const = 1.3577f;
		// float d_const = 1.0035f;
		// float a2_const = 0.8264f;
		// float d2_const = 0.4132f;

		// float sine_frequency[4] = {(2*M_PI)/360, (2*M_PI)/360, (2*M_PI)/360, (2*M_PI)/360};
		// float sine_phase[4] = {60*(M_PI/180), -60*(M_PI/180), -150*(M_PI/180), 150*(M_PI/180)};

		// if (fabs(wheelRef[wheel]) < threshold) {
    	// 	feed_forward[wheel] = 0;
		// } 
		// else {
		// 	// feed_forward[wheel] = identified_damping*wheelRef[wheel] + constEval(theta_local[wheel],sine_frequency[wheel],sine_phase[wheel],d_const);
		// 	// feed_forward[wheel] = identified_damping*wheelRef[wheel] + sineEval(theta_local[wheel],a_const,sine_frequency[wheel],sine_phase[wheel]);
		// 	feed_forward[wheel] = identified_damping*wheelRef[wheel] + constsineEval(theta_local,a2_const,sine_frequency[wheel],sine_phase[wheel],d2_const);
    	// }

		////////////////////////////////////////////////////////////////////////////////////////////////////////
		feed_forward[wheel] = identified_damping*wheelRef[wheel] + feedforwardFriction(wheelRef[wheel], rho, theta_local, omega, wheel);

		// Feedback
		// Calculate the velocity error
		float angular_velocity_error = wheelRef[wheel] - wheels_measured_speeds_test[wheel];
		
		// If the error is very small, ignore it (why is this here?)
		if (fabs(angular_velocity_error) < 0.1) {
			angular_velocity_error = 0.0;
			wheelsK[wheel].I = 0;
		}

		// Add PID to commanded speed and convert to PWM
		float PIDvoltageoutputfactor = 0.004f; // (24V /6000pwm) // Get rid of this factor and the OMEGAtoPWM factor by simply removing them and multiplying the P,IandD gains by (OMEGAtoPWM*PIDvoltageoutputfactor)
		// voltage_list[wheel] = PIDvoltageoutputfactor * OMEGAtoPWM * (feed_forward[wheel] + PID(angular_velocity_error, &wheelsK[wheel]));
		voltage_list[wheel] = feed_forward[wheel] + PIDvoltageoutputfactor * OMEGAtoPWM * (PID(angular_velocity_error, &wheelsK[wheel]));
		
	}

}

void stateControl_voltage2PWM(){
	// Needs to be:
	// float batteryVoltage = getBatteryVoltage();
	// once the battery voltage is actually recorded somewhere. Then also directly input in the pwm equation instead of uselessly creating an intermediate variable
	// For now just assume:
	float batteryVoltage = 24;

	for(wheel_names wheel = wheels_RF; wheel <= wheels_RB; wheel++){
		wheel_pwm_list[wheel] = (int32_t) (voltage_list[wheel]*(MAX_PWM/batteryVoltage));
	}
}

void wheels_SetPIDGains(REM_RobotSetPIDGains* PIDGains){
	for(wheel_names wheel = wheels_RF; wheel <= wheels_RB; wheel++){
		wheelsK[wheel].kP = PIDGains->Pwheels;
		wheelsK[wheel].kI = PIDGains->Iwheels;
    	wheelsK[wheel].kD = PIDGains->Dwheels;
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

	// // Translate the local u, v, and omega velocities into wheel velocities.
	// wheelSpeed[wheels_RF] = stateLocal[vel_u] * D[0] + stateLocal[vel_v] * D[1];
	// wheelSpeed[wheels_LF] = stateLocal[vel_u] * D[3] + stateLocal[vel_v] * D[4];
	// wheelSpeed[wheels_LB] = stateLocal[vel_u] * D[6] + stateLocal[vel_v] * D[7];
	// wheelSpeed[wheels_RB] = stateLocal[vel_u] * D[9] + stateLocal[vel_v] * D[10];

	// // Translate wheel velocities into angular velocities
	// for (wheel_names wheel=wheels_RF; wheel <= wheels_RB; wheel++) {
	// 	wheelSpeed[wheel] = wheelSpeed[wheel] / rad_wheel;
	// }

	// // If we do not use angular velocities (w), remove these.
	// if (!useAbsoluteAngle) {
    //     for (wheel_names wheel=wheels_RF; wheel<=wheels_RB; wheel++){
    //         wheelSpeed[wheel] += stateLocal[vel_w] * rad_robot / rad_wheel;
    //     }
	// }
}

static void global2Local(float global[4], float local[4], float angle){
	//trigonometry
	local[vel_u] = cosf(angle) * global[vel_x] + sinf(angle) * global[vel_y];
	local[vel_v] = -sinf(angle) * global[vel_x] + cosf(angle) * global[vel_y];
    local[vel_w] = global[vel_w];
	local[yaw] = global[yaw];
}

static void velocityControl(float stateLocal[4], float stateGlobalRef[4], float velocityWheelRef[4]){
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