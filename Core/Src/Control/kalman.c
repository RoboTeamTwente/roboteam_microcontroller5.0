
#include "kalman.h"
#include "kalmanVariables.h"
#include "matrix_operations.h"

#define Acc_BUFFER_SIZE 10

///////////////////////////////////////////////////// PRIVATE FUNCTION IMPLEMENTATIONS

/**
 * Smoothens out the IMU accelerometer
 * While this does decrease the response time slightly, it allows for smoother accelerations
 * 
 * @param xsensAcc The current x and y acceleration as measured by the IMU accelerometer [m/s^2].
 * @param smoothed_Acc The smoothed acceleration in x and y direction -> watch out IMU axis are different than robot axis!
 */
static void smoothen_xsensAcc(float xsensAcc[2], float smoothed_Acc[2]);

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
	float correct_acc[2] = {0.0f, 0.0f}; // to not overwrite raw acceleration (acc[2])
	correct_acc[0] = -1 * acc[0]; // xsensAcc1 is in opposite direction of robot velocity v
	correct_acc[1] = acc[1];  

	float smoothed_Acc[2] = {0.0f, 0.0f};
	smoothen_xsensAcc(correct_acc, smoothed_Acc);
	az[0] = vel[0];
	az[1] = smoothed_Acc[1]; // xsensAcc2 = same axis as robot velocity u
	az[2] = vel[1];
	az[3] = smoothed_Acc[0]; // xsensAcc1 = same axis as robot velocity v

	// Computes the formula:
	// Xk = Fk * X(k-1) + Bk * Uk
	multiplyMatrix(aF, aXold, aFX, STATE, 1, STATE);
	multiplyMatrix(aB, aU, aBU, STATE, 1, STATE);
	addMatrix(aFX, aBU, aXcurrent, STATE / 2, STATE / 2);

	// Process data
	// Yk = Zk - Hk * Xk
	multiplyMatrix(aH, aXcurrent, aHX, OBSERVE, 1, STATE);
	subMatrix(az, aHX, ayold, OBSERVE / 2, OBSERVE / 2);

	// Update
	// X(k+1) = Kk * Yk + Xk
	multiplyMatrix(aK, ayold, aKy, STATE, 1, OBSERVE);
	addMatrix(aXcurrent, aKy, aXnew, STATE / 2, STATE / 2);

	for (int i=0; i<STATE; i++){
		aXold[i] = aXnew[i];
	}

}

void kalman_CalculateK(){

	static float count = 0;

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

void kalman_GetState(float state[STATE]) {
	for (int i=0; i<STATE; i++) {
		state[i] = aXold[i];
	}
}

void kalman_GetK(float gain[STATE][OBSERVE]) {
	for (int j=0; j<OBSERVE; j++) {
		for (int i=0; i<STATE; i++) {
			gain[i][j] = aK[i+j*STATE];
		}
	}

}

void kalman_GetP(float P[STATE*STATE]) {
	for (int i = 0; i < STATE*STATE; i++) {
		P[i] = aPold[i];
	}
}

///////////////////////////////////////////////////// PRIVATE FUNCTION IMPLEMENTATIONS

static void smoothen_xsensAcc(float xsensAcc[2], float smoothed_Acc[2]){
    static float buffer_Acc1[Acc_BUFFER_SIZE] = {0.0f}; // circular buffer
	static float buffer_Acc2[Acc_BUFFER_SIZE] = {0.0f}; // circular buffer

    static int idx_Acc1 = 0; // holds current index of buffer
	static int idx_Acc2 = 0; // holds current index of buffer

    buffer_Acc1[idx_Acc1] = xsensAcc[0];
	buffer_Acc2[idx_Acc2] = xsensAcc[1];

    //idx = idx >= RoT_BUFFER_SIZE-1 ? 0 : idx + 1;
	idx_Acc1 = (idx_Acc1+1) % Acc_BUFFER_SIZE ;
	idx_Acc2 = (idx_Acc2+1) % Acc_BUFFER_SIZE ;

    float avg_Acc1 = 0.0f;  // average of buffer, which is the smoothed acceleration
	float avg_Acc2 = 0.0f;  // average of buffer, which is the smoothed acceleration

    for (int i=0; i<Acc_BUFFER_SIZE ; i++){
        avg_Acc1 += buffer_Acc1[i];
    }

	for (int i=0; i<Acc_BUFFER_SIZE ; i++){
        avg_Acc2 += buffer_Acc2[i];
    }

	smoothed_Acc[0] = avg_Acc1 / Acc_BUFFER_SIZE; // smoothed Acc1
	smoothed_Acc[1] = avg_Acc2 / Acc_BUFFER_SIZE; // smoothed Acc2
}