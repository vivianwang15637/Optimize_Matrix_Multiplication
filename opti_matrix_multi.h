#ifndef OPTI_MM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloca.h>
#include <pthread.h>

#define BUFFER_SIZE 10
#define MAX_LINE_SIZE 100

struct inputPair{
    int r1,c1,r2,c2;
    int *m1;
    int *m2;
    int *res;
};

struct buffer{
   char *fileIn;
   struct inputPair *buf[BUFFER_SIZE];	    // Shared input stream for consumer threads.
   int in;	                				// Index for next insertion.
   int out;					                // Index for next deletion.
   int count;
   pthread_mutex_t print_m;       			// Lock for printing.
   pthread_mutex_t buf_m;       			// Lock and conditions for buffer access.
   pthread_cond_t not_full;
   pthread_cond_t not_empty;
};

struct buffer b;

/* Buffer functions */
void bufferInit();
void bufferAdd(struct inputPair *p);
struct inputPair* bufferTake();

/* Placeholder function for CUDA kernel */
void multiM(struct inputPair *p);

void produceInputPair(char* file, int nCons);
void *consumeInputPair(void *cons_num);

/* Consumer specific functions */
void freeInputPair(struct inputPair *p);
void printToFile (struct inputPair *p);

void startMultiM(int nCon, char *fileName);

# endif