# Compilation specifics
CC := clang
FLAGS = -std=c99 -pthread -pedantic -Wimplicit-function-declaration -Wreturn-type -Wformat -g -I$(INC_DIR)

# File directories
INC_DIR := ../include/
SRC_DIR := ../src/
SRC_PT_DIR := $(SRC_DIR)/only_threads/
SRC_CU_DIR := $(SRC_DIR)/cuda/
TEST_DIR := ../tests/

all: MM_Threads MM_CUDA

MM_Threads:$(SRC_PT_DIR)mm_threads.o $(TEST_DIR)run_mm_threads_implementation.o
	$(CC) -o MM_Threads $(SRC_PT_DIR)mm_threads.o $(TEST_DIR)run_mm_threads_implementation.o

$(SRC_PT_DIR)mm_threads.o: $(SRC_PT_DIR)mm_threads.c $(INC_DIR)opti_matrix_multi.h
	$(CC) $(FLAGS) -c $< -o $@

$(TEST_DIR)run_mm_threads_implementation.o: $(TEST_DIR)run_mm_threads_implementation.c $(INC_DIR)opti_matrix_multi.h
	$(CC) $(FLAGS) -c $< -o $@

MM_CUDA: $(SRC_DIR)mm_shared_funcs.o $(SRC_CU_DIR)mm_cuda.o
	nvcc -o $(SRC_DIR)mm_shared_funcs.o $(SRC_CU_DIR)mm_cuda.o

$(SRC_CU_DIR)mm_cuda.o: $(SRC_CU_DIR)mm_cuda.cu $(INC_DIR)opti_matrix_multi.h
	 nvcc -Xcompiler "$(FLAGS)" -c $< -o $@
clean:
	rm -rf $(SRC_DIR)*.o $(SRC_PT_DIR)*.o $(TEST_DIR)*.o ../tests/cases/output/*_out.txt MM_Threads
