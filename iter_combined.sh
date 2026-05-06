#!/bin/bash
gcc combined.c -o $2.exe
RUN=./$2.exe
CODES=({0..8}) # all tests
ERROR_RATES=(2 10 50 100 500 1000 10000)
ITERATIONS=1000
ERROR_TYPES=(0 1 2)
STORAGE_DIRECTORY="results$1" # lets me run multiple times and store results to different folder each time
mkdir -p "$STORAGE_DIRECTORY"
for code in "${CODES[@]}"; do
    for bit_error_rate in "${ERROR_RATES[@]}"; do
        for error_type in "${ERROR_TYPES[@]}"; do
            output_file="${STORAGE_DIRECTORY}/code_${code}_error_type_${error_type}_bit_error_rate_${bit_error_rate}.txt"
            $RUN $code $error_type $ITERATIONS $bit_error_rate > "$output_file"
        done
    done
done
echo "done"
