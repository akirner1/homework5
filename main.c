#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <math.h>
#include "rtclock.h"
#include "mmm.h"

// shared  globals
unsigned int mode; //0 for parallel, 1 for sequential
unsigned int size, num_threads;
double **A, **B, **SEQ_MATRIX, **PAR_MATRIX;


void printErrMsg(){
	printf("Please use the format: \"./mmm P <size>\" for parrallel mode, or \"./mmm S <numThreads> <size>\" for sequential mode\n");
}

/**
 * @param str a string to validate if it's a number
 * validates if each character in the string (with exception for '\0') is a digit.
 * @returns the number the string represents if it's valid, and -1 if not.
*/
int validateStrIsNum(char* str){
	int val = 0;
	int strLen = strlen(str);
	for(int i = 0; i < strLen; i++){
		int asciiChar = (int) str[i] - 48;
		if(asciiChar< 0 || asciiChar > 9){
			return -1;
		}
		val += pow(10, strLen - 1 - i) * asciiChar;
	}
	return val;
}



int main(int argc, char *argv[]) {
	// TODO - deal with command line arguments, save the "mode"
	// "size" and "num threads" into globals so threads can see them

	if(argc < 3 || argc > 4){
		printf("Invalid number of arguments\n");
		printErrMsg();
		return 0;
	}
	if(strcmp(argv[1], "P") == 0){
		mode = 0;
	}else if(strcmp(argv[1], "S") == 0){
		mode = 1;
	}else{
		printf("Invalid argument \"%s\" for mode\n", argv[1]);
		printErrMsg();
		return 0;
	}
	size = validateStrIsNum(argv[2]);
	if(size == -1){
		char* firstNumArg;
		if(mode){
			firstNumArg = "size";
		}else{
			firstNumArg = "numCores";
		}
		printf("Invalid argument \"%s\" for %s\n", argv[2], firstNumArg);
		printErrMsg();
		return 0;
	}
	if(!mode){
		num_threads = size;
		size = validateStrIsNum(argv[3]);
		if(size == -1){
			printf("Invalid argument \"%s\" for size\n", argv[3]);
			printErrMsg();
			return 0;
		}
	}else{
		num_threads = 1;
	}
	//all args are now valid and set in place

	// initialize my matrices
	mmm_init();
	char* modeName;
	if(!mode){
		modeName = "Parallel";
	}else{
		modeName = "Sequential";
	}
	printf("========\nmode: %s\n", modeName);
	if(!mode){
		printf("thread count: %d\n", num_threads);
	}
	printf("size: %d\n========\n", size);


	double clockstart, clockend;
	int numRuns = 3;
	clockstart = rtclock();	// start the clock

	// << stuff I want to clock here >>

	for(int i = 0; i < numRuns; i++){
		mmm_seq();
	}

	clockend = rtclock(); // stop the clock
	printf("Sequential Time (avg of %d runs): %.6f sec\n", numRuns, (clockend - clockstart)/numRuns);


	if(!mode){
		clockstart = rtclock();
		for(int i = 0; i < numRuns; i++){
			pthread_t* threads = (pthread_t*) malloc(sizeof(pthread_t) * num_threads);// allocate space for all threads
			for(int i = 0; i < num_threads; i++){
				struct mmmInputs* inputs = (struct mmmInputs*) malloc(sizeof(mmmInputs));
				int taskSize = size / num_threads;
				inputs->startIndex = i * taskSize;
				inputs->endIndex = (i+1) * taskSize;
				if(i == num_threads - 1){
					inputs->endIndex = size;
					//while this is techinically suboptimal, the final thread will do the extra work caused by any rounding errors
				}
				//printf("creating %d\n", i);
				pthread_create(&threads[i], NULL, mmm_par, inputs);
			}
			for(int i = 0; i < num_threads; i++){
				//printf("joining %d\n", i);
				pthread_join(threads[i], NULL);
				//printf("join done %d\n", i);
			}
			free(threads);
		}
		clockend = rtclock();
		printf("Parallel Time (avg of %d runs): %.6f sec\nVerifying...", numRuns, (clockend-clockstart)/numRuns);
		printf("largest error between parallel and sequential matrix: %f\n", mmm_verify());
		//printMatrix(SEQ_MATRIX);
		//printMatrix(PAR_MATRIX);
	}



	// free some stuff up

	return 0;
}
