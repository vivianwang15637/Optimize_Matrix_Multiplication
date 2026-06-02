FLAGS = -std=c99 -pthread -pedantic -Wimplicit-function-declaration -Wreturn-type -Wformat -g

all: MM_Threads

MM_Threads: mm_threads.c opti_matrix_multi.h
	gcc $(FLAGS) mm_threads.c -o MM_Threads

#MM_CUDA: mm_opti.cu opti_matrix_multi.h
#	nvcc -Xcompiler "$(FLAGS)" mm_opti.cu

clean:
	rm -rf *.o MM_Threads