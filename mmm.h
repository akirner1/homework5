#ifndef MMM_H_
#define MMM_H_

// shared globals
extern unsigned int mode;
extern unsigned int size, num_threads;
extern double **A, **B, **SEQ_MATRIX, **PAR_MATRIX;

void mmm_init();
void mmm_reset(double **);
void mmm_freeup();
void mmm_seq();
void* mmm_par(void *);
double mmm_verify();

typedef struct mmmInputs {
	int startIndex;
	int endIndex;
}mmmInputs;

void printMatrix(double**);
void* threadTest(void*);
#endif /* MMM_H_ */
