#ifndef OPTI_MM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <alloca.h>
#include <assert.h>
#include <pthread.h>

#define BUFFER_SIZE 10
#define MAX_LINE_SIZE 9000
#ifdef __CUDACC__
    /* # of threads per block.  */
    #define TILE_SIZE 32
#endif

struct inputPair{
    int r1,c1,r2,c2;
    int *m1;
    int *m2;
    int *res;
};

struct buffer{
   char *fileIn;
   int nCons;
   struct inputPair *buf[BUFFER_SIZE];	    // Shared input stream for consumer threads.
   int in;	                				// Index for next insertion.
   int out;					                // Index for next deletion.
   int count;
   pthread_mutex_t print_m;       			// Lock for printing.
   pthread_mutex_t buf_m;       			// Lock and conditions for buffer access.
   pthread_cond_t not_full;
   pthread_cond_t not_empty;
};

struct consumerParams {
    struct buffer *b;
    int id;
};

void startMultiM(int nCon, char *fileName);

#endif
