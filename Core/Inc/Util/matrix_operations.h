/**
 * @brief Matrix operation support 
 * @file matrix_operations.h
 * 
 * @author Tom Meulenkamp (supertom01)
 * @version Jan 11, 2023
 */

#ifndef UTILS_MATRIX_OPERATIONS_H_
#define UTILS_MATRIX_OPERATIONS_H_
/**
 * @brief Calculate the inverse of a given square matrix.
 * @attention Only sizes of 2 and 4 are supported
 * 
 * @param A The matrix to determine the inverse of
 * @param B The inverse of the input matrix
 * @param m The size of the matrix (m X m)
 */
void inverse(float A[], float B[], int m);

/**
 * @brief Calculates the result from multiplying two matrices.
 * 
 * @param A The left hand side matrix with dimensions mXc
 * @param B The right hand side matrix with dimensions cXn
 * @param C The resulting matrix with dimensions mXn
 * @param m Dimension m
 * @param n Dimension n
 * @param c Dimension c
 */
void multiplyMatrix(float A[], float B[], float C[], int m, int n, int c);

/**
 * @brief Adds two matrices together.
 * 
 * @param A The first matrix                [mXn]
 * @param B The second matrix               [mXn]
 * @param C The result after adding         [mXn]
 * @param m The width of each matrix
 * @param n The height of each matrix
 */
void addMatrix(float A[], float B[], float C[], int m, int n);

/**
 * @brief Subtracts two matrices from each other.
 * 
 * @param A The matrix on the left hand side  [mXn]
 * @param B The matrix on the right hand side [mXn]
 * @param C The resulting matrix              [mXn]
 * @param m The width of each matrix
 * @param n The height of each matrix
 */
void subMatrix(float A[], float B[], float C[], int m, int n);

/**
 * @brief Determines the transpose of a given matrix.
 * 
 * @param A The input matrix    [mXn]
 * @param B The output matrix   [nXm]
 * @param m Dimension m
 * @param n Dimension n
 */
void transMatrix(float A[], float B[], int m, int n);

#endif /* UTILS_MATRIX_OPERATIONS_H_ */