
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
		multiplyMatrix(aF, aXold, aFX, , 1, STATESTATE);
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

void kalman_CalculateK(){

	static float count = 0;
	if (count < 100){
		static float oldk[STATE*OBSERVE] = {0};

		// Calculates the predicted estimate covariance
		// Pk = Fk * P(k-1) * Ftk + Qk
		multiplyMatrix(aF, aPold, aFP, STATE, STATE, STATE);
		multiplyMatrix(aFP, aFt, aFPFt, STATE, STATE, STATE);
		addMatrix(aFPFt, aQ, aPcurrent, STATE, STATE);

		// Calculates the innovation covariance
		// Sk = Pk * Htk * Hk + Rk
		multiplyMatrix(aPcurrent, aHt, aPHt, STATE, OBSERVE, STATE);
		multiplyMatrix(aH, aPHt, aHPHt, OBSERVE, OBSERVE, STATE);
		addMatrix(aR, aHPHt, aS, OBSERVE, OBSERVE);

		// Compute Kalman Gain
		// Kk = Pk * Htk * Sik
		inverse(aS, aSi, OBSERVE);
		multiplyMatrix(aPHt, aSi, aK, STATE, OBSERVE, OBSERVE);

		// Calculate the posteriori estimate covariance matrix
		// Pk = (I - Kk * Hk) * P(k - 1) * (I - Kk * Hk)t + Kk * Rk * Ktk

		// KRKt = Kk * Rk * Ktk
		transMatrix(aK, aKt, STATE, OBSERVE);
		multiplyMatrix(aK, aR, aKR, STATE, OBSERVE, OBSERVE);
		multiplyMatrix(aKR, aKt, aKRKt, STATE, STATE, OBSERVE);

		// I-KH = I - Kk * Hh
		multiplyMatrix(aK, aH, aKH, STATE, STATE, OBSERVE);
		subMatrix(aI, aKH, aI_KH, STATE, STATE);

		// (I-KH)t
		transMatrix(aI_KH, aI_KHt, STATE, STATE);
		
		multiplyMatrix(aI_KH, aPcurrent, aI_KHP, STATE, STATE, STATE);
		multiplyMatrix(aI_KHP, aI_KHt, aI_KHPI_KHt, STATE, STATE, STATE);
		addMatrix(aI_KHPI_KHt, aKRKt, aPnew, STATE, STATE);


		float same = 0;
		for (int i=0; i<STATE*OBSERVE; i++){
			if (oldk[i] - aK[i] < 0.0000001 && oldk[i] - aK[i] > -0.0000001){
				same +=1;
			}
			oldk[i] = aK[i];
		}

		if (same == STATE*OBSERVE){
			count += 1;
		}

		for (int i=0; i<STATE*STATE; i++){
			aPold[i] = aPnew[i];
		}
	}
}

void kalman_GetState(float state[STATE]) {
	for (int i=0; i<STATE; i++) {
		state[i] = aXold[i];
	}
}
