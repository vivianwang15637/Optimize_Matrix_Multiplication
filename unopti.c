#include "opti_matrix_multi.h"

struct buffer b;

/* TODO:Stencil kernal to multiply two matrices of any compatible size */
void multiM(struct inputPair *p) {
    // Placeholder 3-loop matrix multiplication logic
    for (int i = 0; i < p->r1; ++i) {
        for (int j = 0; j < p->c2; ++j) {
            for (int k = 0; k < p->c1; ++k) {
                //p->res[i * p->c2 + j] += p->m1[i * p->c1 + k] * p->m2[k * p->c2 + j];
            }
        }
    }

}

/* Initilize buffer status variables, mutex lock, and condition variables. */
void bufferInit(void){
    b.in = 0;
    b.out = 0;
    b.count = 0;
    pthread_mutex_init(&b.buf_m,NULL);
    pthread_mutex_init(&b.print_m,NULL);
    pthread_cond_init(&b.not_full,NULL);
    pthread_cond_init(&b.not_empty,NULL);
}

/* Add new inputPair pointer to shared buffer if possible.*/
void bufferAdd(struct inputPair *p){
    // Lock for access to buffer
    pthread_mutex_lock(&b.buf_m);

    // Check & wait while buffer is full
    while(b.count == BUFFER_SIZE) pthread_cond_wait(&b.not_full, &b.buf_m);

    // Add new pointer to shared buffer.
    b.buf[b.in] = p;
    b.count++;

    // Update insertion index.
    b.in = ++b.in % BUFFER_SIZE;

    // Signal not empty (if needed) and release lock.
	if (b.count == 1){
		pthread_cond_signal(&b.not_empty);
	}
	pthread_mutex_unlock(&b.buf_m);
}

/* Removes inputPair from buffer if possible. Returns pointer to pair for processing. */
struct inputPair* bufferTake(void){
    // Lock for access to buffer
    pthread_mutex_lock(&b.buf_m);

    // Check & wait while buffer is empty
    while(b.count == 0) pthread_cond_wait(&b.not_empty, &b.buf_m);

    // Create new pointer to allocated inputPair and remove from buffer.
    struct inputPair *b_out = b.buf[b.out];
    b.buf[b.out] = NULL;
    b.count--;

    // Update removal index.
    b.out = ++b.out % BUFFER_SIZE;

    // Signal not full (if needed) and release lock.
	if (b.count == (BUFFER_SIZE - 1)){
		pthread_cond_signal(&b.not_full);
	}
	pthread_mutex_unlock(&b.buf_m);

    return b_out;

}

/* Free all memory allocated for inputPair. */
void freeInputPair(struct inputPair *p){
    if (p->m1 != NULL){
        free(p->m1);
        p->m1 = NULL;
    }

    if (p->m2 != NULL){
        free(p->m2);
        p->m2 = NULL;
    }

    if (p->res != NULL){
        free(p->res);
        p->res = NULL;
    }

    if (p != NULL){
        free(p);
        p = NULL;
    }
}

/* Print resulting matrix and corresponding inputs */
void printToFile (struct inputPair *p){
    pthread_mutex_lock(&b.print_m);
    
    printf("\nInput Matrix 1:\n");
    for (int i = 0; i < p->r1; ++i) {
        for (int j = 0; j < p->c1; ++j){
            printf("%d  ", p->m1[i * p->c1 + j]);       /* Row major ordering */
        }
        printf("\n");
    }

    printf("Input Matrix 2:\n");
    for (int i = 0; i < p->r2; ++i) {
        for (int j = 0; j < p->c2; ++j){
            printf("%d  ", p->m2[j * p->r2 + i]);       /* Column major ordering */
        }
        printf("\n");
    }

    printf("Resultant Matrix:\n");
    for (int i = 0; i < p->r1; ++i) {
        for (int j = 0; j < p->c2; ++j){
            printf("%d  ", p->res[i * p->c2 + j]);      /* Row major ordering */
        }
        printf("\n");
    }

    pthread_mutex_unlock(&b.print_m);
}


/* Parse through input files and add inputPairs into the buffer until invalid inputPair 
 * detected or EOF.
 */
void produceInputPair(char* inputFileName, int nCons){
    FILE* file;
    file = fopen(inputFileName,"r");
    if (file == NULL) {
        printf("%s is not a valid input.\n", inputFileName);
        exit(1);
    }
    b.fileIn = inputFileName;

    char line[MAX_LINE_SIZE];

    // Read file by operation inputs until invalid matrix or EOF.
    while(fgets(line, MAX_LINE_SIZE, stdin) != NULL){
        struct inputPair *p = malloc(sizeof(struct inputPair));

        // Get and set row and column size for input matricies.
        char *tkn = strtok(line, ", ");
        for (int i = 0; i < 4; i++){
            if (tkn == NULL) {
                printf("Invalid matrix specification.\n");
                exit(1);
            }

            switch(i) {
                case 0:
                    p->r1 = atoi(tkn);
                case 1:
                    p->c1 = atoi(tkn);
                case 2:
                    p->r2 = atoi(tkn);
                case 3:
                    p->c2 = atoi(tkn);
                    if (p->c1 != p->r2) {
                        printf("Error: Invalid input(s) detected. Column of first matrix must equal row of second.\n");
                        exit(1);
                    }
            }

            tkn = strtok(NULL, ", ");
        }

        // Parse matrix 1.
        p->m1 = (int *)malloc(p->r1 * p->c1 * sizeof(int));
        for (int i = 0; i < p->r1; i++){
            // Get a row.
            if (fgets(line, MAX_LINE_SIZE, stdin) != NULL){

                // Check if row fits in input buffer.
                if (strchr(line, '\n') == NULL) {
                    printf("Matrix 1 row %d does not fit input buffer. Increase input buffer size or check input dimension.\n", i + 1);
                    exit(1);
                }

                // Get columns in row.
                char *token = strtok(line, " ");
                for (int j = 0; j < p->c1; j++){
                    if (token == NULL) {
                        printf("Invalid matrix specifications.\n");
                        exit(1);
                    }
                    p->m1[i * p->c1 + j] = atoi(token);
                    token = strtok(NULL, " ");
                }

            }
        }

        // Check if column matches input dimension.
        fgets(line, MAX_LINE_SIZE, stdin);
        if (!strcmp(line,"\n")) {
            printf("Input matrix 1 does not match the expected dimensions\n");
            exit(1);
        }

        // Parse matrix 2.
        p->m2 = (int *)malloc(p->r2 * p->c2 * sizeof(int));
        for (int i = 0; i < p->r2; i++){
            // Get a row.
            if (fgets(line, MAX_LINE_SIZE, stdin) != NULL){

                // Check if row fits in input buffer.
                if (strchr(line, '\n') == NULL) {
                    printf("Matrix 2 row %d does not fit input buffer. Increase input buffer size or check input dimension.\n", i + 1);
                    exit(1);
                }

                // Get columns in row.
                char *token = strtok(line, " ");
                for (int j = 0; j < p->c2; j++){
                    if (token == NULL) {
                        printf("Invalid matrix specifications.\n");
                        exit(1);
                    }
                    p->m2[j * p->r2 + i] = atoi(token);
                    token = strtok(NULL, " ");
                }

            }
        }

        // Check if column matches input dimension.
        fgets(line, MAX_LINE_SIZE, stdin);
        if (!strcmp(line,"\n")) {
            printf("Input matrix 2 does not match the expected dimensions\n");
            exit(1);
        }

        // Make result matrix.
        p->res = (int *)malloc(p->r1 * p->c2 * sizeof(int));

        // Add inputPair to buffer.
        bufferAdd(p);

    }

    // Add termination signal for each consumer into buffer.
    struct inputPair *term = malloc(sizeof(struct inputPair));
    term->r1 = -1;
    for (int i = 0; i < nCons; i++) bufferAdd(term);

}

/* Consume inputPairs from the shared buffer until the termination signal is recieved. */
void *consumeInputPair(void *cons_num){
    while (1){
        // Remove an inputPair from shared buffer.
        struct inputPair *p;
        p = bufferTake();

        // Check that an operation can be done.
        if(p->r1 == -1) return(NULL);

        // Allocate device resources.
        printf("Device resource allocation");
        // Call stencil kernel.
        printf("Call Kernel");


        // Display inputs and result.
        printToFile(p);

        // Free memory
        freeInputPair(p);
    }
        return(NULL);
}

/* Call this function with command lines inputs of input file and number of consumers/threads
 * wanted to preform multiplication.
 */
void startMultiM(int nCon, char *fileName){
    bufferInit();

    pthread_t *cons = (pthread_t *)alloca(nCon * sizeof(pthread_t));
    // Spawn consumer threads.
    for (int i = 0; i < nCon; i++){
        int t = pthread_create(&cons[i], NULL, consumeInputPair, (void *)(intptr_t)i);
        assert(t == 0);
    }

    // Start producer thread (runs on main thread).
    produceInputPair(fileName, nCon);

    // Clean up.
    for (int i = 0; i<nCon; i++){
        int t = pthread_join(cons[i], NULL);
        assert(t == 0);
    }
}

/*
 * This program aims to optimize execution of mutiple matrix mutiplication operations requested via
 * input file. The number of threads launching kernels/multiplying matricies must be specified via
 * command line argument as the 3rd argument (if 0, the default value of 1 will be used). Matrix inputs
 * must be integer matricies.
 */
int main(int argc, char **argv) {
    // Check args.
    if (argc != 3) {
        printf("Invalid number of arguments.");
        exit(1);
    }

    int nCon, nPro = 1;
    if (atoi(argv[2]) < 0) nCon = atoi(argv[2]);

    startMultiM(nCon, argv[1]);
    return 0;
}
