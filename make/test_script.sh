#!/bin/bash

make MM_Threads
echo "====================================="

#Run the program with test cases
for input in ../tests/test_1_smalls.txt ../tests/test_few_larges.txt ../tests/test_many_smalls.txt ../tests/test_many_larges.txt
do
        echo "Running MM_Threads with $input"
        echo "====================================="
        #Run the program with different number of consumers
        for consumers in 1 2 4 6
        do
            #Run the program
            echo "Running with $consumers consumer(s):"
	        start_dec=$EPOCHREALTIME
            time ./MM_Threads "$input" "$consumers"
            end_dec=$EPOCHREALTIME
            start_ms=${start_dec/./}
            end_ms=${end_dec/./}
            diff=$((end_ms-start_ms))

            if [ "$diff" != "0" ]; then
                echo "Execution time: $diff"
            fi
            echo "-------------------------------------"
        done
        echo "====================================="
done

mkdir -p ../tests/output
mv ../tests/*_out.txt ../tests/output/



