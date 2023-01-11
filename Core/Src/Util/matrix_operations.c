#include "matrix_operations.h"

///////////////////////////// PUBLIC FUNCTION IMPLEMENTATIONS

void inverse(float A[], float B[], int m) {
    if (m == 2) {
        float a = A[0];
        float b = A[1];
        float c = A[2];
        float d = A[3];
        float determinant = a * d - b * c;
        B[0] = d / determinant;
        B[1] = -b / determinant;
        B[2] = -c / determinant;
        B[3] = a / determinant;
    } else if (m == 4) {
        float a = A[0];
        float b = A[1];
        float c = A[4];
        float d = A[5];
        float determinant = a * d - b * c;
        B[0] = d / determinant;
        B[1] = -b / determinant;
        B[4] = -c / determinant;
        B[5] = a / determinant;

        a = A[10];
        b = A[11];
        c = A[14];
        d = A[15];
        determinant = a * d - b * c;
        B[10] = d / determinant;
        B[11] = -b / determinant;
        B[14] = -c / determinant;
        B[15] = a / determinant;
    }
}

void multiplyMatrix(float A[], float B[], float C[], int m, int n, int c){
	for (int i=0; i<m*n; i++){
		C[i] = 0;
	}
	for (int i=0; i<m; i++){
		for (int j=0; j<n; j++){
			for (int k=0; k<c; k++){
				C[i*n+j] += A[i*m+k] * B[k*n+j];
			}
		}
	}
}

void addMatrix(float A[], float B[], float C[], int m, int n){
    for (int i=0; i<m; i++) {
        for (int j=0; j<n; j++) {
            C[i*n+j] = A[i*n+j] + B[i*n+j];
        }
    }
}

void subMatrix(float A[], float B[], float C[], int m, int n){
	for (int i=0; i<m; i++) {
        for (int j=0; j<n; j++) {
            C[i*n+j] = A[i*n+j] - B[i*n+j];
        }
    }
}

void transMatrix(float A[], float B[], int m, int n){
	for (int i=0; i<n; i++){
		for (int j=0; j<m; j++){
			B[i*m+j] = A[j*n+i];
		}
	}
}