
#include "stateControl.h"
#include "logging.h"

///////////////////////////////////////////////////// VARIABLES

// The current status of the system.
static PID_states status = off;

// The PID values for u, v, w and yaw.
static PIDvariables stateK[4];

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
 * @param localState The velocities to be achieved seen from the body perspective {vel_u, vel_v, vel_w} [m/s]
 */
static void body2Wheels(float wheelSpeed[4], float localState[3]);

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
 * @param localState 		The current u, v, w and yaw speeds seen from the body
 * @param stateGlobalRef 			The instructed global x, y, w and yaw speeds
 * @param velocityWheelRef 	The resulting wheel speeds that should be achieved for each wheel
 */
static void velocityControl(float localState[3], float stateGlobalRef[4], float velocityWheelRef[4]);

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
	initPID(&stateK[vel_u], default_P_gain_u, default_I_gain_u, default_D_gain_u);
	initPID(&stateK[vel_v], default_P_gain_v, default_I_gain_v, default_D_gain_v);
	initPID(&stateK[vel_w], default_P_gain_w, default_I_gain_w, default_D_gain_w); 
	initPID(&stateK[yaw], default_P_gain_yaw, default_I_gain_yaw, default_D_gain_yaw);
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
}

void stateControl_SetRef(float input[4]){
	stateGlobalRef[vel_x] = input[vel_x];
	stateGlobalRef[vel_y] = input[vel_y];
	stateGlobalRef[vel_w] = input[vel_w];
	stateGlobalRef[yaw] = input[yaw];
}

float* stateControl_GetWheelRef() {
	return wheelRef;
}

void stateControl_SetState(float input[4]){
	stateLocal[vel_u] = input[vel_u];
	stateLocal[vel_v] = input[vel_v];
	stateLocal[vel_w] = input[vel_w];
	stateLocal[yaw] = input[yaw];
}

void stateControl_GetPIDGains(PIDvariables gains[4]){
	gains[vel_u].kP = stateK[vel_u].kP;
	gains[vel_u].kI = stateK[vel_u].kI;
	gains[vel_u].kD = stateK[vel_u].kD;

	gains[vel_v].kP = stateK[vel_v].kP;
	gains[vel_v].kI = stateK[vel_v].kI;
	gains[vel_v].kD = stateK[vel_v].kD;

	gains[vel_w].kP = stateK[vel_w].kP;
	gains[vel_w].kI = stateK[vel_w].kI;
	gains[vel_w].kD = stateK[vel_w].kD;

	gains[yaw].kP = stateK[yaw].kP;
	gains[yaw].kI = stateK[yaw].kI;
	gains[yaw].kD = stateK[yaw].kD;
}

float stateControl_GetIntegral(robot_axes direction) {
	return stateK[direction].I;
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
    stateK[vel_u].kP = PIDGains->PbodyX;
    stateK[vel_u].kI = PIDGains->IbodyX;
    stateK[vel_u].kD = PIDGains->DbodyX;

    stateK[vel_v].kP = PIDGains->PbodyY;
    stateK[vel_v].kI = PIDGains->IbodyY;
    stateK[vel_v].kD = PIDGains->DbodyY;

    stateK[vel_w].kP = PIDGains->PbodyW;
    stateK[vel_w].kI = PIDGains->IbodyW;
    stateK[vel_w].kD = PIDGains->DbodyW;

    stateK[yaw].kP = PIDGains->PbodyYaw;
    stateK[yaw].kI = PIDGains->IbodyYaw;
    stateK[yaw].kD = PIDGains->DbodyYaw;
}

void stateControl_ResetAngleI(){
	stateK[yaw].I = 0;
	stateK[vel_w].I = 0;
}

void stateControl_ResetPID(){
	stateK[yaw].prev_e = 0;
	stateK[yaw].prev_PID = 0;
	stateK[vel_w].prev_e = 0;
	stateK[vel_w].prev_PID = 0;
}

///////////////////////////////////////////////////// PRIVATE FUNCTION IMPLEMENTATIONS

static void body2Wheels(float wheelSpeed[4], float vel[3]){

	// Translate the local u, v, and omega velocities into wheel velocities.
	wheelSpeed[wheels_RF] = vel[vel_u] * D[0] + vel[vel_v] * D[1];
	wheelSpeed[wheels_LF] = vel[vel_u] * D[3] + vel[vel_v] * D[4];
	wheelSpeed[wheels_LB] = vel[vel_u] * D[6] + vel[vel_v] * D[7];
	wheelSpeed[wheels_RB] = vel[vel_u] * D[9] + vel[vel_v] * D[10];

	// Translate wheel velocities into angular velocities
	for (wheel_names wheel=wheels_RF; wheel <= wheels_RB; wheel++) {
		wheelSpeed[wheel] = wheelSpeed[wheel] / rad_wheel;
	}

	// If we use angular velocities, take those into account too.
	if (!useAbsoluteAngle) {
        for (wheel_names wheel=wheels_RF; wheel<=wheels_RB; wheel++){
            wheelSpeed[wheel] += vel[vel_w] * rad_robot / rad_wheel;
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

	// Manually adjusting velocity command
	//     Explanation: see Velocity Difference file on drive (https://docs.google.com/document/d/1pGKysiwpu19DKLpAZ4GpluMV7UBhBQZ65YMTtI7bd_8/)
	stateLocalRef[vel_u] = 1.12 * stateLocalRef[vel_u];
	stateLocalRef[vel_v] = 1.1 * stateLocalRef[vel_v];

	// Local control
	float veluErr = (stateLocalRef[vel_u] - stateLocal[vel_u]);
	float velvErr = (stateLocalRef[vel_v] - stateLocal[vel_v]);
	float velwErr = (stateLocalRef[vel_w] - stateLocal[vel_w]);

	stateLocalRef[vel_u] += PID(veluErr, &stateK[vel_u]);
	stateLocalRef[vel_v] += PID(velvErr, &stateK[vel_v]);
	stateLocalRef[vel_w] += PID(velwErr, &stateK[vel_w]);

	body2Wheels(velocityWheelRef, stateLocalRef); //translate velocity to wheel speed
}

static float absoluteAngleControl(float angleRef, float angle){
	static float prevangleErr = 0;
	float angleErr = constrainAngle(angleRef - angle);//constrain it to one circle turn
	if (angleErr == 0){
		angleErr = 0.000001*prevangleErr;
	}
	if (fabs(angleErr) < YAW_MARGIN || prevangleErr/angleErr < 0) {
		stateK[yaw].I = 0;
	}
	prevangleErr = angleErr;
	return PID(angleErr, &stateK[yaw]);// PID control from control_util.h
}