#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "mmm.h"

double randVal(){
	int min = 0;
	int max = 100;
	return rand() % (max-min) + min;
}

double** initMatrix(int doRandom){
	double** matrix = (double **) malloc(size * sizeof(double*));
	for(int i = 0; i < size; i++){
		matrix[i] = (double*) malloc(size * sizeof(double));
		for(int j = 0; j < size; j++){
			if(doRandom){
				matrix[i][j] = randVal();
			}else{
				matrix[i][j] = 0;
			}
		}
	}
	return matrix;
}
/**
 * Allocate and initialize the matrices on the heap. Populate
 * the input matrices with random numbers from 0 to 99
 */
void mmm_init() {
	srand((unsigned)time(NULL));
	A = initMatrix(1);
	B = initMatrix(1);
	SEQ_MATRIX = initMatrix(0);
	PAR_MATRIX = initMatrix(0);
}

/**
 * Reset a given matrix to zeroes (their size is in the global var)
 * @param matrix pointer to a 2D array
 */
void mmm_reset(double **matrix) {
	// TODO
	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j++){
			matrix[i][j] = 0;
		}
	}
}

void freeMatrix(double** matrix){
	for(int i = 0; i < size; i++){
		free(matrix[i]);
	}
	free(matrix);
}
/**
 * Free up memory allocated to all matrices
 * (their size is in the global var)
 */
void mmm_freeup() {
	// TODO
	freeMatrix(A);
	freeMatrix(B);
	freeMatrix(SEQ_MATRIX);
	freeMatrix(PAR_MATRIX);
	
}
void mmCalc(double** matrix, int start, int finish){
	for(int row = start; row < finish; row++){
		for(int col = 0; col < size; col++){
			for(int i = 0; i < size; i++){
				double val = A[row][i] * B[i][col];
				matrix[row][col] += val;
			}
			//printf("%d, %d", row, col);
		}
	}
}

/**
 * Sequential MMM (size is in the global var)
 * 
 */
void mmm_seq() {
	// TODO - code to perform sequential MMM
	mmCalc(SEQ_MATRIX, 0, size);
}

/**
 * Parallel MMM
 */
void *mmm_par(void *args) {
	//printf("thread entering!\n");
	// TODO - code to perform parallel MMM
	struct mmmInputs* inputs = (mmmInputs*) args;
	mmCalc(PAR_MATRIX, inputs -> startIndex, inputs -> endIndex);
	return NULL;
}



/**
 * Verifies the correctness between the matrices generated by
 * the sequential run and the parallel run.
 *
 * @return the largest error between two corresponding elements
 * in the result matrices
 */
double mmm_verify() {
	// TODO
	// You may find the math.h function fabs() to be useful here (absolute value)
	double maxDiff = 0;
	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j++){
			double diff = fabs(SEQ_MATRIX[i][j] - PAR_MATRIX[i][j]);
			if(diff > maxDiff){
				maxDiff = diff;
			}
		}
	}
	return maxDiff;
}

void printMatrix(double** matrix){
	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j++){
			printf("%f ", matrix[i][j]);
		}
		printf("\n");
	}
}

void* threadTest(void* args){
	printf("in thread!");
	return NULL;
}