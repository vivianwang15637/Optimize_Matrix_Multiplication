#include "opti_matrix_multi.h"

struct buffer b;

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
    b.in = (b.in + 1) % BUFFER_SIZE;

    // Signal not empty (if needed) and release lock.
    if (b.count == 1){
        pthread_cond_broadcast(&b.not_empty);
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
    b.out = (b.out + 1) % BUFFER_SIZE;

    // Signal not full (if needed) and release lock.
    if (b.count == (BUFFER_SIZE - 1)){
        pthread_cond_broadcast(&b.not_full);
    }

    pthread_mutex_unlock(&b.buf_m);

    return b_out;

}

/* Free all memory allocated for inputPair. */
void freeInputPair(struct inputPair **p){
    if (*p != NULL){
        if ((*p)->m1 != NULL){
            free((*p)->m1);
            (*p)->m1 = NULL;
        }

        if ((*p)->m2 != NULL){
            free((*p)->m2);
            (*p)->m2 = NULL;
        }

        if ((*p)->res != NULL){
            free((*p)->res);
            (*p)->res = NULL;
        }

        free(*p);
        *p = NULL;
    }
}

/* Print resulting matrix and corresponding inputs */
void printToFile (struct inputPair *p){
    pthread_mutex_lock(&b.print_m);

    // Create file to put output
    char outFile[MAX_LINE_SIZE];
    char str[10];
    strncpy(outFile, b.fileIn, MAX_LINE_SIZE -11);
    snprintf(str, sizeof(str), "%d", b.nCons);
    strncat(outFile, str, 10);
    strncat(outFile, "_out.txt", 8);


    FILE *fp = fopen(outFile, "a");
    if (fp == NULL) {
        printf("Error: Could not open file.\n");
        exit(1);
    }

    // Print inputs and output
    fprintf(fp,"\nInput Matrix 1:\n");
    for (int i = 0; i < p->r1; ++i) {
        for (int j = 0; j < p->c1; ++j){
            fprintf(fp,"%d  ", p->m1[i * p->c1 + j]);       /* Row major ordering */
        }
        fprintf(fp,"\n");
    }

    fprintf(fp,"Input Matrix 2:\n");
    for (int i = 0; i < p->r2; ++i) {
        for (int j = 0; j < p->c2; ++j){
            fprintf(fp,"%d  ", p->m2[j * p->r2 + i]);       /* Column major ordering */
        }
        fprintf(fp,"\n");
    }

    fprintf(fp,"Resultant Matrix:\n");
    for (int i = 0; i < p->r1; ++i) {
        for (int j = 0; j < p->c2; ++j){
            fprintf(fp,"%d  ", p->res[i * p->c2 + j]);      /* Row major ordering */
        }
        fprintf(fp,"\n");
    }

    fclose(fp);
    pthread_mutex_unlock(&b.print_m);
}


/* Parse through input files and add inputPairs into the buffer until invalid inputPair 
 * detected or EOF.
 */
void produceInputPair(char* inputFileName){
    FILE* file;
    file = fopen(inputFileName,"r");
    if (file == NULL) {
        printf("%s is not a valid input.\n", inputFileName);
        exit(1);
    }
    b.fileIn = inputFileName;

    char line[MAX_LINE_SIZE] = "";

    // Read file by operation inputs until invalid matrix or EOF.
    while(fgets(line, MAX_LINE_SIZE, file) != NULL){
        struct inputPair *p = malloc(sizeof(struct inputPair));

        // Get and set row and column size for input matricies.
        char *tkn = strtok(line, ", ");
        for (int i = 0; i < 4; i++){
            if (tkn == NULL || atoi(tkn) <= 0) {
                printf("Invalid matrix specification on iteration %d. [%s] is not allowed.\n",i,tkn);
                exit(1);
            }

            switch(i) {
                case 0:
                    p->r1 = atoi(tkn);
                    break;
                case 1:
                    p->c1 = atoi(tkn);
                    break;
                case 2:
                    p->r2 = atoi(tkn);
                    break;
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
            memset(line,'\0',sizeof(line));
            // Get a row.
            if (fgets(line, MAX_LINE_SIZE, file) != NULL || line[0]!='\n'){

                // Check if row fits in input buffer.
                if (strchr(line, '\n') == NULL) {
                    printf("Matrix 1 row %d does not fit input buffer. Increase input buffer size or check input dimension.\n", ++i);
                    exit(1);
                }

                // Get columns in row.
                char *token = strtok(line, " ");
                for (int j = 0; j < p->c1; j++){
                    if (token == NULL || token[0] == '\n') {
                        printf("Invalid matrix specifications on row %d column %d [%s].\n", ++i,++j,token);
                        exit(1);
                    }
                    p->m1[i * p->c1 + j] = atoi(token);
                    token = strtok(NULL, " ");
                }

            } else {
                printf("Input matrix 1 does not match the expected dimensions. Row %d [%s].\n",++i,line);
                exit(1);
            }
        }

        // Check if column matches input dimension.
        if (fgets(line, MAX_LINE_SIZE, file) != NULL && line[0] != '\n') {
            printf("Input matrix 1 does not match the expected dimensions\n");
            exit(1);
        }

        // Parse matrix 2.
        p->m2 = (int *)malloc(p->r2 * p->c2 * sizeof(int));
        for (int i = 0; i < p->r2; i++){
            memset(line,'\0',sizeof(line));
           // Get a row.
            if (fgets(line, MAX_LINE_SIZE, file) != NULL || line[0] != '\n'){
                // Check if row fits in input buffer.
                if (strchr(line, '\n') == NULL) {
                    printf("Matrix 2 row %d does not fit input buffer. Increase input buffer size or check input dimension.\n", ++i);
                    exit(1);
                }

                // Get columns in row.
                char *token = strtok(line, " ");
                for (int j = 0; j < p->c2; j++){
                    if (token == NULL || token[0] == '\n') {
                        printf("Invalid matrix specifications on row %d column %d [%s].\n",++i,++j,token);
                        exit(1);
                    }
                    p->m2[j * p->r2 + i] = atoi(token);
                    token = strtok(NULL, " ");
                }

            } else {
                printf("Input matrix 2 does not match the expected dimensions. Row %d [%s].\n",++i,line);
            	exit(1);
            }
        }

       // Check if column matches input dimension.
        memset(line,'\0',sizeof(line));
        if (fgets(line, MAX_LINE_SIZE, file) != NULL && line[0] != '\n') {
            printf("SHOULD BE EMPTY LINE: [%s] len=%zu\n", line, strlen(line));
            printf("Input matrix 2 does not match the expected dimensions\n");
            exit(1);
        }

        // Make result matrix.
        p->res = (int *)malloc(p->r1 * p->c2 * sizeof(int));

        // Add inputPair to buffer.
        bufferAdd(p);
    }

    fclose(file);

    // Add termination signal for each consumer into buffer.
    for (int i = 0; i < b.nCons; i++){
        struct inputPair *term = malloc(sizeof(struct inputPair));
        term->r1 = -1;
        term->m1 = NULL;
        term->m2=  NULL;
        term->res = NULL;
        bufferAdd(term);
    }

}

/* Call this function with command lines inputs of input file and number of consumers/threads
 * wanted to preform multiplication.
 */
void startMultiM(int nCon, char *fileName){
    bufferInit();
    b.nCons = nCon;
    pthread_t *cons = (pthread_t *)alloca(nCon * sizeof(pthread_t));
    // Spawn consumer threads.
    for (int i = 0; i < nCon; i++){
        int t = pthread_create(&cons[i], NULL, consumeInputPair, (void *)(intptr_t)i);
        printf("thread[%d] created",i);
        assert(t == 0);
    }

    // Start producer thread (runs on main thread).
    produceInputPair(fileName);

    // Clean up.
    for (int i = 0; i<nCon; i++){
        int t = pthread_join(cons[i], NULL);
	printf("thread[%d] joined",i);
        assert(t == 0);
    }
}
