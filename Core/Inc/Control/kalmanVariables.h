
/* Description: Kalman filter variables
 *
 * Instructions:
 * 1) Initializes the necessary arrays and variables for the Kalman filter
 *
 * Extra functions:
 *
 * Notes:
 *
*/


#include "arm_math.h"
#include "stdint.h"

#ifndef KALMAN_KALMANVARIABLES_H_
#define KALMAN_KALMANVARIABLES_H_

///////////////////////////////////////////////////// DEFINITIONS

#define STATE 4				// The dimensions for the current state
#define OBSERVE 4  			// The dimensions for the measured state
#define TIMESTEP 0.01F		// The time step between each iteration (100Hz loop)

// certainties
#define VEL_VAR 0.000025F 	// variance in the velocity measurements
#define ACC_VAR 6.25F 		// variance in the acceleration measurements
#define STATE_VAR 0.5F 		// variance in the predicted state
#define RAND_VAR 0.5F 		// variance in the random force

///////////////////////////////////////////////////// VARIABLES

// The state transition matrix.
float aF[STATE*STATE] = {
		1, TIMESTEP, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, TIMESTEP,
		0, 0, 0, 1};

arm_matrix_instance_f32 arm_aF = {STATE, STATE, aF};

// The observed model	
float aH[OBSERVE*STATE] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1};
arm_matrix_instance_f32 arm_aH = {OBSERVE, STATE, aH};

// The covariance of the observed model	
float aR[OBSERVE*OBSERVE] = {
		VEL_VAR, 0, 0, 0,
		0, ACC_VAR, 0, 0,
		0, 0, VEL_VAR, 0,
		0, 0, 0, ACC_VAR};
arm_matrix_instance_f32 arm_aR = {OBSERVE, OBSERVE, aR};

// The identity matrix
float aI[STATE*STATE] = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1};
arm_matrix_instance_f32 arm_aI = {STATE, STATE, aI};

// The old prediction matrix
float aPold[STATE*STATE] = {
		STATE_VAR,0,0,0,
		0,STATE_VAR,0,0,
		0,0,STATE_VAR,0,
		0,0,0,STATE_VAR};
arm_matrix_instance_f32 arm_aPold = {STATE, STATE, aPold};

// The control input matrix		
float aB[STATE*STATE] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1};
arm_matrix_instance_f32 arm_aB = {STATE, STATE, aB};

// The covariance of the process model	
float aQ[STATE*STATE] = {
		TIMESTEP*TIMESTEP*RAND_VAR, TIMESTEP*RAND_VAR, 0, 0,
		TIMESTEP*RAND_VAR, RAND_VAR, 0, 0,
		0, 0, TIMESTEP*TIMESTEP*RAND_VAR, TIMESTEP*RAND_VAR,
		0, 0, TIMESTEP*RAND_VAR, RAND_VAR};
arm_matrix_instance_f32 arm_aQ = {STATE, STATE, aQ};

float aXold[STATE] = {0.0f};			// The old state
arm_matrix_instance_f32 arm_aXold = {STATE, 1, aXold};
float az[OBSERVE] = {0.0f};				// The observed (measured) state
arm_matrix_instance_f32 arm_az = {OBSERVE, 1, az};
float aXcurrent[STATE] = {0.0f};		// The current tate
arm_matrix_instance_f32 arm_aXcurrent = {STATE, 1, aXcurrent};
float aFX[STATE] = {0.0f};				// The X and F matrices multiplied
arm_matrix_instance_f32 arm_aFX = {STATE, 1, aFX};
float aFt[STATE*STATE] = {0.0f};		// The transposed state transition matrix
arm_matrix_instance_f32 arm_aFt = {STATE, STATE, aFt};
float aFP[STATE*STATE] = {0.0f};		// The F and P matrices multiplied
arm_matrix_instance_f32 arm_aFP = {STATE, STATE, aFP};
float aPcurrent[STATE*STATE] = {0.0f};	// The current prediction matrix
arm_matrix_instance_f32 arm_aPcurrent = {STATE, STATE, aPcurrent};
float ayold[OBSERVE] = {0.0f};			// The post-fit residual measurement
arm_matrix_instance_f32 arm_ayold = {OBSERVE, 1, ayold};
float aHX[OBSERVE] = {0.0f};			// The H and X matrices multiplied
arm_matrix_instance_f32 arm_aHX = {OBSERVE, 1, aHX};
float aS[OBSERVE*OBSERVE] = {0.0f};		// The innovation covariance
arm_matrix_instance_f32 arm_aS = {OBSERVE, OBSERVE, aS};
float aHt[STATE*OBSERVE] = {0.0f};		// The transposed observation model matrix
arm_matrix_instance_f32 arm_aHt = {OBSERVE, STATE, aHt};
float aPHt[STATE*OBSERVE] = {0.0f};		// The P and Ht matrices multiplied
arm_matrix_instance_f32 arm_aPHt = {STATE, OBSERVE, aPHt};
float aFPFt[STATE*STATE] = {0.0f};		// The F, P and Ft matrices multiplied
arm_matrix_instance_f32 arm_aFPFt = {STATE, STATE, aFPFt};
float aHPHt[OBSERVE*OBSERVE] = {0.0f};	// The H, P and Ht matrices multiplied
arm_matrix_instance_f32 arm_aHPHt = {OBSERVE, OBSERVE, aHPHt};
float aK[STATE*OBSERVE] = {0.0f};		// Kalman gain
arm_matrix_instance_f32 arm_aK = {STATE, OBSERVE, aK};
float aSi[OBSERVE*OBSERVE] = {0.0f};	// The inverse of innovation covariance
arm_matrix_instance_f32 arm_aSi = {OBSERVE, OBSERVE, aSi};
float aXnew[STATE] = {0.0f};			// The next state
arm_matrix_instance_f32 arm_aXnew = {STATE, 1, aXnew};
float aKy[STATE] = {0.0f};				// The K and yold matrices multiplied
arm_matrix_instance_f32 arm_aKy = {STATE, 1, aKy};
float aPnew[STATE*STATE] = {0.0f};		// The next prediction matrix
arm_matrix_instance_f32 arm_aPnew = {STATE, STATE, aPnew};
float aKt[OBSERVE*STATE] = {0.0f};		// The transpose of the kalman gain
arm_matrix_instance_f32 arm_aKt = {OBSERVE, STATE, aKt};
float aKR[STATE*OBSERVE] = {0.0f};		// The K and R matrices multiplied
arm_matrix_instance_f32 arm_aKR = {STATE, OBSERVE, aKR};
float aKRKt[STATE*STATE] = {0.0f};		// The K, R and Kt matrices multiplied
arm_matrix_instance_f32 arm_aKRKt = {STATE, STATE, aKRKt};
float aKH[STATE*STATE] = {0.0f};		// The K and H matrices multiplied
arm_matrix_instance_f32 arm_aKH = {STATE, STATE, aKH};
float aI_KH[STATE*STATE] = {0.0f};		// The result of subtracting the KH matrix from the I matrix
arm_matrix_instance_f32 arm_aI_KH = {STATE, STATE, aI_KH};
float aI_KHt[STATE*STATE] = {0.0f};		// The transpose of the aI_KH matrix
arm_matrix_instance_f32 arm_aI_KHt = {STATE, STATE, aI_KHt};
float aI_KHP[STATE*STATE] = {0.0f};		// The aI_KH and P matrix multiplied
arm_matrix_instance_f32 arm_aI_KHP = {STATE, STATE, aI_KHP};
float aI_KHPI_KHt[STATE*STATE] = {0.0f};// The aI_KHP and aI_KHt matrices multiplied
arm_matrix_instance_f32 arm_aI_KHPI_KHt = {STATE, STATE, aI_KHPI_KHt};
float aU[STATE] = {0.0f};				// The control vector
arm_matrix_instance_f32 arm_aU = {STATE, 1, aU};
float aBU[STATE] = {0.0f};				// The B and U matrices multiplied
arm_matrix_instance_f32 arm_aBU = {STATE, 1, aBU};

#endif /* KALMAN_KALMANV_H_ */
