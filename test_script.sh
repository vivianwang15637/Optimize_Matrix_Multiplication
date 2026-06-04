#!/bin/bash

make MM_Threads
echo "====================================="

#Run the program with test cases
for input in test0.txt test1.txt test2.txt test3.txt
do
        echo "Running MM_Threads with $input"
        echo "====================================="
        #Run the program with different number of consumers
        for consumers in 1 2 4 6
        do
            #Run the program
            echo "Running with $consumers consumer(s):"
	    start_dec=$EPOCHREALTIME
            ./MM_Threads "$input" "$consumers"
            end_dec=$EPOCHREALTIME
            start=${start_dec/./}
            end=${end_dec/./}
            diff=$((end-start))
            echo -e "\nExecution time: $diff ms"
            echo "-------------------------------------"
        done
        echo "====================================="
done
