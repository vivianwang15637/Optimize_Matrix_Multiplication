FLAGS = -std=c99 -pthread -pedantic -Wimplicit-function-declaration -Wreturn-type -Wformat -g -c

all: MM_Threads MM_CUDA

MM_Threads: unopti.c opti_matrix_multi.h
	gcc $(FLAGS) -o unopti.c

clean:
	rm -rf *.o MM_Threads MM_CUDA