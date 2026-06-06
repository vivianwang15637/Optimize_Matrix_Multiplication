#include "opti_matrix_multi.h"

/*
 * This testing program aims to optimize execution of mutiple matrix mutiplication operations requested via
 * input file. The number of threads launching kernels/multiplying matricies must be specified via
 * command line argument as the 3rd argument (if 0, the default value of 1 will be used). Matrix inputs
 * must be integer matricies. This implementation optimizes matrix multiplication via multithreading.
 */
int main(int argc, char **argv) {
    // Check args.
    if (argc != 3) {
        printf("Invalid number of arguments.");
        exit(1);
    }

    int nCon, nPro = 1;
   if (atoi(argv[2]) > 0) nCon = atoi(argv[2]);

    startMultiM(nCon, argv[1]);
    return 0;
}
