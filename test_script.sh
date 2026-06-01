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
            ./MM_Threads "$input" "$consumers"
            echo "-------------------------------------"
        done
        echo "====================================="
done