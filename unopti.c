#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <pthreads.h>

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
   struct inputPair buffer[BUFFER_SIZE];	// Shared input stream for consumer threads.
   int in;	                				// Index for next insertion.
   int out;					                // Index for next deletion.
   int count;
   pthread_mutex_t buf_m;       			// Lock and conditions for buffer access.
   pthread_cond_t not_full;
   pthread_cond_t not_empty;
};

struct buffer b;

/* Stencil kernal to multiply two matrices of any compatible size */
void multiM(struct inputPair p) {
    // 3-loop matrix multiplication logic
    for (int i = 0; i < r1; ++i) {
        for (int j = 0; j < c2; ++j) {
            for (int k = 0; k < c1; ++k) {
                result[i][j] += first[i][k] * second[k][j];
            }
        }
    }
}

/* Check if input matrices are valid */
void checkMatrixPair (struct inputPair p){
    if (p.c1 != p.r2) {
        printf("Error: Column of first matrix must equal row of second.\n");
        exit(1);
    }
}

/* Initilize buffer status variables, mutex lock, and condition variables. */
void bufferInit(){
    struct b.in = 0;
    struct b.out = 0;
    struct b.count = 0;
    pthread_mutex_init(&b.buf_m,NULL);
    pthread_cond_init(&b.not_full,NULL);
    pthread_cond_init(&b.not_empty,NULL);
}

/* */
void bufferAdd(struct inputPair p){
}

/* Removes inputPair from buffer if possible. Returns pair for processing. */
struct inputPair bufferTake(){

}

/* Free memory allocated for input, output, */
void freeInputPair(){
}

/* Print resulting matrix and corresponding inputs */
void printToFile (){
}


/* */
void produceInputPair(char* file, int nCons){
    /*
     * Input file format:
     * # rows in 1st matrix, # columns in 1st matrix, # rows in 2nd matrix, # columns in 2nd matrix
     * row 1 of 1st matrix space seperated
     *  ...
     * row n of 1st matrix space seperated
     * \n
     * row 1 of 2nd matrix space seperated
     *  ...
     * row n of 2nd matrix space seperated
     * \n
     * This pattern can be repeated any amount of times within the file, but is limited by the size of
     * MAX_LINE_SIZE.
     */
    FILE* file;
    file = fopen(file,"r");
    b.fileIn = file;

    if (ptr == NULL) {
        printf("%s is not a valid input.\n", argv[1]);
        exit(1);
    }

    char line[MAX_LINE_SIZE];

    // Read file until invalid matrix or EOF.
    while(fgets(line, MAX_LINE_SIZE, stdin) != NULL){
        // Get and set row and column size for input matricies.
        // Make and get all lines and columns of matrix 1.
        // Make and get all lines and columns of martix 2.
        // Make result matrix.
        // Add inputPair to buffer.
    }

    // Send termination signal to consumers.
    for (){
    }
    printf("The end of your ");

}

void *consumeInputPair(void *cons_num){
    while (1){
        struct inputPair p;
        p = bufferTake();

        // Call Kernel

        // Display result
        printf("\nResultant Matrix:\n");
        for (int i = 0; i < r1; ++i) {
            for (int j = 0; j < c2; ++j){
                printf("%d  ", p.result[i][j]);
            }
            printf("\n");
            }
        }

        // Free memory
        freeInputPair();
}

void startMultiM(int arc, char **argv){
    // Check args.
    if (argc != 3) {
        printf("Invalid number of arguments.");
        exit(1);
    }

    int nCon, nPro = 1;
    if (argv < 0) nCon = argv[2];

    bufferInit();


    pthread_t *cons = (pthread_t *)alloca(nCon * sizeof(pthread_t));
    // Spawn consumer threads.
    for (int i = 0; i < nCon; i++){
        int t = pthread_create(&cons[i], NULL, consumeInputPair, (void *)(intptr_t)i);
        assert(t == 0);
    }

    // Start producer thread (runs on main thread).
    produceInputPair(argv[1]);

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

    startMultiM(argc, argv);
    return 0;
}
