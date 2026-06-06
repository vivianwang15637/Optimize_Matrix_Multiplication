#include "opti_matrix_multi.h"

/* Cache/memory access optimized matrix multiplication.*/
void multiM(struct inputPair *p) {
    // Placeholder 3-loop matrix multiplication logic
    for (int i = 0; i < p->r1; ++i) {
        for (int j = 0; j < p->c2; ++j) {
            int sum = 0;
            for (int k = 0; k < p->c1; ++k) {
                sum += p->m1[i * p->c1 + k] * p->m2[j * p->c2 + k];
            }
             p->res[i * p->c2 + j] = sum;
        }
    }

}

/* Consume inputPairs from the shared buffer until the termination signal is recieved. */
void *consumeInputPair(void *cons_num){
    while (1){
        // Remove an inputPair from shared buffer.
        struct inputPair *p;
        p = bufferTake();

        // Check that an operation can be done.
        if(p->r1 == -1){
            freeInputPair(&p);
            //printf("Consumer %d exited", (int)(intptr_t)cons_num);
            return (NULL);
        }

        // Perform multiplication.
        multiM(p);

        // Display inputs and result.
        printToFile(p);

        // Free memory
        freeInputPair(&p);
    }
        return(NULL);
}
