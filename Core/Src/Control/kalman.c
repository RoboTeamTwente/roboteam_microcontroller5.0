
#include "kalman.h"
#include "kalmanVariables.h"
#include "matrix_operations.h"

///////////////////////////////////////////////////// PUBLIC FUNCTION IMPLEMENTATIONS

void kalman_Init(){
	transMatrix(aH, aHt, OBSERVE, STATE);
	transMatrix(aF, aFt, STATE, STATE);
}

void kalman_DeInit(){
}

void kalman_Update(float acc[2], float vel[2]){

	// Predict
	//	for (int i = 0; i < STATE; i++) {
	//		aU[i] = controlInput[i];
	//	}
		az[0] = vel[0];
		az[1] = acc[0];
		az[2] = vel[1];
		az[3] = acc[1];

		// Computes the formula:
		// Xk = Fk * X(k-1) + Bk * Uk
		multiplyMatrix(aF, aXold, aFX, STATE, 1, STATE);
		multiplyMatrix(aB, aU, aBU, STATE, 1, STATE);
		addMatrix(aFX, aBU, aXcurrent, 1, STATE); 			//1,4 and not 4,1 because this is how the matrix library works

		// Process data
		// Yk = Zk - Hk * Xk
		multiplyMatrix(aH, aXcurrent, aHX, OBSERVE, 1, STATE);
		subMatrix(az, aHX, ayold, 1, OBSERVE);				//1,4 and not 4,1 because this is how the matrix library works

		// Update
		// X(k+1) = Kk * Yk + Xk
		multiplyMatrix(aK, ayold, aKy, STATE, 1, OBSERVE);
		addMatrix(aXcurrent, aKy, aXnew, 1, STATE);			//1,4 and not 4,1 because this is how the matrix library works

		for (int i=0; i<STATE; i++){
			aXold[i] = aXnew[i];
		}

}

static float oldK[STATE*OBSERVE] = {0};
static arm_matrix_instance_f32 arm_oldk = {STATE, OBSERVE, oldK};

void kalman_CalculateK_arm(){
	// Calculates the predicted estimate covariance
	// Pk = Fk * P(k-1) * Ftk + Qk
	arm_mat_mult_f32(&arm_aF, &arm_aPold, &arm_aFP);
	arm_mat_mult_f32(&arm_aFP, &arm_aFt, &arm_aFPFt);
	arm_mat_add_f32(&arm_aFPFt, &arm_aQ, &arm_aPcurrent);

	// Calculates the innovation covariance
	// Sk = Pk * Htk * Hk + Rk
	arm_mat_mult_f32(&arm_aPcurrent, &arm_aHt, &arm_aPHt);
	arm_mat_mult_f32(&arm_aH, &arm_aPHt, &arm_aHPHt);
	arm_mat_add_f32(&arm_aR, &arm_aHPHt, &arm_aS);

	// Compute Kalman Gain
	// Kk = Pk * Htk * Sik
	arm_mat_inverse_f32(&arm_aS, &arm_aSi);
	arm_mat_mult_f32(&arm_aPHt, &arm_aSi, &arm_aK);

	// Calculate the posteriori estimate covariance matrix
	// Pk = (I - Kk * Hk) * P(k - 1) * (I - Kk * Hk)t + Kk * Rk * Ktk

	// KRKt = Kk * Rk * Ktk
	arm_mat_trans_f32(&arm_aK, &arm_aKt);
	arm_mat_mult_f32(&arm_aK, &arm_aR, &arm_aKR);
	arm_mat_mult_f32(&arm_aKR, &arm_aKt, &arm_aKRKt);

	// I-KH = I - Kk * Hh
	arm_mat_mult_f32(&arm_aK, &arm_aH, &arm_aKH);
	arm_mat_sub_f32(&arm_aI, &arm_aKH, &arm_aI_KH);

	// (I-KH)t
	arm_mat_trans_f32(&arm_aI_KH, &arm_aI_KHt);
	
	arm_mat_mult_f32(&arm_aI_KH, &aPcurrent, &aI_KHP);
	arm_mat_mult_f32(&aI_KHP, &arm_aI_KHt, &arm_aI_KHPI_KHt);
	arm_mat_add_f32(&arm_aI_KHPI_KHt, &arm_aKRKt, &aPnew);

	for (int i=0; i<STATE*STATE; i++){
		aPold[i] = aPnew[i];
	}
}

void kalman_GetState(float state[STATE]) {
	for (int i=0; i<STATE; i++) {
		state[i] = aXold[i];
	}
}
