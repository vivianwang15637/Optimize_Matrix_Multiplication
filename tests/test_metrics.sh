#!/bin/bash

cd ../make
make MM_Threads
echo "====================================="

#Run the program with test cases
for input in ../tests/cases/test_1_smalls.txt ../tests/cases/test_few_larges.txt ../tests/cases/test_many_smalls.txt ../tests/cases/test_many_larges.txt
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

mkdir -p ../tests/cases/output
mv ../tests/cases/*_out.txt ../tests/cases/output/



