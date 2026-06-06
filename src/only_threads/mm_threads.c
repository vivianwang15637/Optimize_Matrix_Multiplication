#include "../src/mm_shared_funcs.c"

/* Cache/memory access optimized matrix multiplication.*/
static void multiM(struct inputPair *p) {
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
static void *consumeInputPair(void *cons_params){
    struct consumerParams *param = (struct consumerParams *)cons_params;
    struct buffer *b = param->b;
    int cons_num = param->id;

    while (1){

        // Remove an inputPair from shared buffer.
        struct inputPair *p;
        p = bufferTake(b);

        // Check that an operation can be done.
        if(p->r1 == -1){
            freeInputPair(b,&p);
            //printf("Consumer %d exited", cons_num);
            return (NULL);
        }

        // Perform multiplication.
        multiM(p);

        // Display inputs and result.
        printToFile(b,p);

        // Free memory
        freeInputPair(b,&p);
    }
        return(NULL);
}

/* Call this function with command lines inputs of input file and number of consumers/threads
 * wanted to preform multiplication.
 */
void startMultiM(int nCon, char *fileName){
    struct buffer b;
    bufferInit(&b);
    b.nCons = nCon;

    pthread_t *cons = (pthread_t *)alloca(nCon * sizeof(pthread_t));
    struct consumerParams *consInfo = alloca(nCon * sizeof(struct consumerParams));
    // Spawn consumer threads.
    for (int i = 0; i < nCon; i++){
        consInfo[i].b  = &b;
        consInfo[i].id = i;
        int t = pthread_create(&cons[i], NULL, consumeInputPair, (void *)&consInfo[i]);
        printf("thread[%d] created",i);
        assert(t == 0);
    }

    // Start producer thread (runs on main thread).
    produceInputPair(&b, fileName);

    // Clean up.
    for (int i = 0; i<nCon; i++){
        int t = pthread_join(cons[i], NULL);
	printf("thread[%d] joined",i);
        assert(t == 0);
    }
}
