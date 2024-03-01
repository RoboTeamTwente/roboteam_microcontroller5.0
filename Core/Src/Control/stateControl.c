
#include "stateControl.h"
#include "logging.h"
#include "wheels.h"
#include "robot.h"

///////////////////////////////////////////////////// VARIABLES

// The current status of the system.
static PID_states status = off;

// The PID values for u, v, w and yaw.
static PIDvariables stateLocalK[4];

// The global x, y, w and yaw velocities to be achieved [m/s]
static float stateGlobalRef[4] = {0.0f}; 

// The wheel velocities to be achieved [rad/s]
static float wheelRef[4] = {0.0f};

// The current global u, v, w and yaw velocities.
static float stateLocal[4] = {0.0f};

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
	status = on;
	initPID(&stateLocalK[vel_u], default_P_gain_u, default_I_gain_u, default_D_gain_u);
	initPID(&stateLocalK[vel_v], default_P_gain_v, default_I_gain_v, default_D_gain_v);
	initPID(&stateLocalK[vel_w], default_P_gain_w, default_I_gain_w, default_D_gain_w); 
	initPID(&stateLocalK[yaw], default_P_gain_yaw, default_I_gain_yaw, default_D_gain_yaw);
	HAL_TIM_Base_Start_IT(TIM_CONTROL);

	// Initialize the velocity coupling matrix.
	D[0] = -sinFront;
	D[1] = cosFront; 
	D[2] = 1;
	D[3] = -sinFront;
	D[4] = -cosFront;
	D[5] = 1;
	D[6] = sinBack;
	D[7] = -cosBack;
	D[8] = 1;
	D[9] = sinBack;
	D[10] = cosBack;
	D[11] = 1;

	return 0;
}

int stateControl_DeInit(){
	status = off;
	HAL_TIM_Base_Stop_IT(TIM_CONTROL);
	return 0;
}

void stateControl_Update(){
	if (status == on){
		float velocityWheelRef[4] = {0.0f};
		velocityControl(stateLocal, stateGlobalRef, velocityWheelRef);

		float angularRef = useAbsoluteAngle ? absoluteAngleControl(stateGlobalRef[yaw], stateLocal[yaw]) : 0.0f;

		for (wheel_names wheel=wheels_RF; wheel<=wheels_RB; wheel++){
			wheelRef[wheel] = velocityWheelRef[wheel] + angularRef;
		}
	}

	
	// wheels_SetSpeeds( stateControl_GetWheelRef() );
	computeWheelSpeeds();
	// wheels_Update();

	/* Don't run the wheels if these are not initialized */
	/* Not that anything would happen anyway, because the PWM timers wouldn't be running, but still .. */
	if(!wheels_AreInitialized()){
		wheels_Stop();
		return;
	}

	float wheels_measured_speeds_test[4] = {0.0f};
	wheels_GetMeasuredSpeeds(wheels_measured_speeds_test);
	float* wheels_commanded_speeds_test;
	wheels_commanded_speeds_test = stateControl_GetWheelRef();

	int32_t wheel_pwm_list[4] = {0.0f};
	for(wheel_names wheel = wheels_RF; wheel <= wheels_RB; wheel++){
		// Calculate the velocity error
		float angular_velocity_error = wheels_commanded_speeds_test[wheel] - wheels_measured_speeds_test[wheel];

		PIDvariables* wheelsK_test = wheels_GiveWheelsK();
		
		// If the error is very small, ignore it (why is this here?)
		if (fabs(angular_velocity_error) < 0.1) {
			angular_velocity_error = 0.0;
			wheelsK_test[wheel].I = 0;
		}

		float feed_forward[4] = {0.0f};
		float threshold = 0.5;

		if (fabs(wheels_commanded_speeds_test[wheel]) < threshold) {
    		feed_forward[wheel] = 0;
		} 
		else if (wheels_commanded_speeds_test[wheel] > 0) {
			feed_forward[wheel] = wheels_commanded_speeds_test[wheel] + 13;
    	}
		else if (wheels_commanded_speeds_test[wheel] < 0) {
			feed_forward[wheel] = wheels_commanded_speeds_test[wheel] - 13;
    	}

		// Add PID to commanded speed and convert to PWM
		wheel_pwm_list[wheel] = (int32_t) OMEGAtoPWM * (feed_forward[wheel] + PID(angular_velocity_error, &wheelsK_test[wheel])); 
		stateControl_wheels_Update2(wheel,wheels_measured_speeds_test,wheels_commanded_speeds_test,wheel_pwm_list,wheelsK_test,angular_velocity_error);
	}

	wheels_SetPWM(wheel_pwm_list);

	
}

void stateControl_wheels_Update3(){
	float a = 1;
}

void stateControl_wheels_Update1(){
	// /* Don't run the wheels if these are not initialized */
	// /* Not that anything would happen anyway, because the PWM timers wouldn't be running, but still .. */
	// if(!wheels_AreInitialized()){
	// 	wheels_Stop();
	// 	return;
	// }

	// int32_t wheel_pwm_list[4] = {0.0f};

	// float wheels_measured_speeds_test[4] = {0.0f};
	// wheels_GetMeasuredSpeeds(wheels_measured_speeds_test);
	// float* wheels_commanded_speeds_test;
	// wheels_commanded_speeds_test = stateControl_GetWheelRef();

	// for(wheel_names wheel = wheels_RF; wheel <= wheels_RB; wheel++){
	// 	// Calculate the velocity error
	// 	float angular_velocity_error = wheels_commanded_speeds_test[wheel] - wheels_measured_speeds_test[wheel];

	// 	PIDvariables* wheelsK_test = wheels_GiveWheelsK();

	// 	// If the error is very small, ignore it (why is this here?)
	// 	if (fabs(angular_velocity_error) < 0.1) {
	// 		angular_velocity_error = 0.0;
	// 		wheelsK_test[wheel].I = 0;
	// 	}

	// 	float feed_forward[4] = {0.0f};
	// 	float threshold = 0.05;

	// 	if (abs(wheels_commanded_speeds_test[wheel]) < threshold) {
    // 		feed_forward[wheel] = 0;
	// 	} 
	// 	else if (wheels_commanded_speeds_test[wheel] > 0) {
	// 		feed_forward[wheel] = wheels_commanded_speeds_test[wheel] + 13;
    // 	}
	// 	else if (wheels_commanded_speeds_test[wheel] < 0) {
	// 		feed_forward[wheel] = wheels_commanded_speeds_test[wheel] - 13;
    // 	}

	// 	// Add PID to commanded speed and convert to PWM
	// 	wheel_pwm_list[wheel] = (int32_t) OMEGAtoPWM * (feed_forward[wheel] + PID(angular_velocity_error, &wheelsK_test[wheel])); 
	// }
	// wheels_SetPWM(wheel_pwm_list);
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
	wheelSpeed[wheels_RF] = stateLocal[vel_u] * D[0] + stateLocal[vel_v] * D[1];
	wheelSpeed[wheels_LF] = stateLocal[vel_u] * D[3] + stateLocal[vel_v] * D[4];
	wheelSpeed[wheels_LB] = stateLocal[vel_u] * D[6] + stateLocal[vel_v] * D[7];
	wheelSpeed[wheels_RB] = stateLocal[vel_u] * D[9] + stateLocal[vel_v] * D[10];

	// Translate wheel velocities into angular velocities
	for (wheel_names wheel=wheels_RF; wheel <= wheels_RB; wheel++) {
		wheelSpeed[wheel] = wheelSpeed[wheel] / rad_wheel;
	}

	// If we do not use angular velocities (w), remove these.
	if (!useAbsoluteAngle) {
        for (wheel_names wheel=wheels_RF; wheel<=wheels_RB; wheel++){
            wheelSpeed[wheel] += stateLocal[vel_w] * rad_robot / rad_wheel;
        }
	}
}

static void global2Local(float global[4], float local[4], float angle){
	//trigonometry
	local[vel_u] = cosf(angle) * global[vel_x] + sinf(angle) * global[vel_y];
	local[vel_v] = -sinf(angle) * global[vel_x] + cosf(angle) * global[vel_y];
    local[vel_w] = global[vel_w];
	local[yaw] = global[yaw];
}

static void velocityControl(float stateLocal[3], float stateGlobalRef[4], float velocityWheelRef[4]){
	float stateLocalRef[3] = {0, 0, 0};
	global2Local(stateGlobalRef, stateLocalRef, stateLocal[yaw]); //transfer global to local

	// Local control
	float veluErr = (stateLocalRef[vel_u] - stateLocal[vel_u]);
	float velvErr = (stateLocalRef[vel_v] - stateLocal[vel_v]);
	float velwErr = (stateLocalRef[vel_w] - stateLocal[vel_w]);

	stateLocalRef[vel_u] += PID(veluErr, &stateLocalK[vel_u]);
	stateLocalRef[vel_v] += PID(velvErr, &stateLocalK[vel_v]);
	stateLocalRef[vel_w] += PID(velwErr, &stateLocalK[vel_w]);

	body2Wheels(velocityWheelRef, stateLocalRef); //translate velocity to wheel speed
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