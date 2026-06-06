# Compilation specifics
CC := clang
FLAGS = -std=c99 -pthread -pedantic -Wimplicit-function-declaration -Wreturn-type -Wformat -g -I$(INC_DIR)

# File directories
INC_DIR := ../include/
SRC_DIR := ../src/
SRC_PT_DIR := $(SRC_DIR)/only_threads/
SRC_CU_DIR := $(SRC_DIR)/cuda/
TEST_DIR := ../tests/

all: MM_Threads

MM_Threads: $(SRC_DIR)mm_shared_funcs.o $(SRC_PT_DIR)mm_threads.o $(TEST_DIR)run_mm_threads_implementation.o
	$(CC) -o MM_Threads $(SRC_DIR)mm_shared_funcs.o $(SRC_PT_DIR)mm_threads.o $(TEST_DIR)run_mm_threads_implementation.o

$(SRC_DIR)mm_shared_funcs.o: $(SRC_DIR)mm_shared_funcs.c $(INC_DIR)opti_matrix_multi.h
	$(CC) $(FLAGS) -c $(SRC_DIR)mm_shared_funcs.c -o $(SRC_DIR)mm_shared_funcs.o

$(SRC_PT_DIR)mm_threads.o: $(SRC_PT_DIR)mm_threads.c $(INC_DIR)opti_matrix_multi.h
	$(CC) $(FLAGS) -c $(SRC_PT_DIR)mm_threads.c -o $(SRC_PT_DIR)mm_threads.o

$(TEST_DIR)run_mm_threads_implementation.o: $(TEST_DIR)run_mm_threads_implementation.c $(INC_DIR)opti_matrix_multi.h
	$(CC) $(FLAGS) -c $(TEST_DIR)run_mm_threads_implementation.c -o $(TEST_DIR)run_mm_threads_implementation.o

MM_CUDA: $(SRC_CU_DIR)mm_opti.cu $(INC_DIR)opti_matrix_multi.h
	nvcc -Xcompiler "$(FLAGS)" $(SRC_CU_DIR)mm_opti.cu -o MM_CUDA

clean:
	rm -rf $(SRC_DIR)*.o $(SRC_PT_DIR)*.o $(TEST_DIR)*.o ../tests/cases/output/*_out.txt MM_Threads
