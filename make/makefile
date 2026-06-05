FLAGS := -std=c99 -pthread -pedantic -Wimplicit-function-declaration -Wreturn-type -Wformat -g -I../include
TH_DIR := ../src/
CU_DIR := ../src/cuda/

all: MM_Threads

MM_Threads: $(TH_DIR)mm_threads.o
	clang -o MM_Threads $(TH_DIR)mm_threads.o

$(TH_DIR)mm_threads.o: $(TH_DIR)mm_threads.c ../include/opti_matrix_multi.h
	clang $(FLAGS) -c $(TH_DIR)mm_threads.c -o $(TH_DIR)mm_threads.o

MM_CUDA: $(CU_DIR)mm_opti.cu $(INC_DIR)opti_matrix_multi.h
	nvcc -Xcompiler "$(FLAGS)" $(CU_DIR)mm_opti.cu -o MM_CUDA

clean:
	rm -rf $(TH_DIR)*.o ../tests/output/*_out.txt MM_Threads
