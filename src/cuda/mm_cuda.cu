#include "opti_matrix_multi.h"

/* .*/
__global__ void multiM(struct inputPair *p) {


}

/* CUDA specific version of consume inputPairs. Consumes pairs from the shared buffer until the termination signal is recieved. */
void *consumeInputPair(void *cons_num){
    while (1){
        // Remove an inputPair from shared buffer.
        struct inputPair *p;
        p = bufferTake();

        // Check that an operation can be done.
        if(p->r1 == -1) return(NULL);

        // Allocate reasources.

        // Launch kernel.

        // Display inputs and result.
        printToFile(p);

        // Free memory
        freeInputPair(p);
    }
        return(NULL);
}
