#!/bin/bash
RUN=./sim_zero.exe
CODES=({0..8}) # all tests
ERROR_RATES=(2 10 50 100 500 1000 10000)
ITERATIONS=100000
STORAGE_DIRECTORY="results_zero$1" # lets me run multiple times and store results to different folder each time
mkdir -p "$STORAGE_DIRECTORY"
for code in "${CODES[@]}"; do
    for bit_error_rate in "${ERROR_RATES[@]}"; do
        output_file="${STORAGE_DIRECTORY}/code_${code}_bit_error_rate_${bit_error_rate}.txt"
        $RUN $code $ITERATIONS $bit_error_rate > "$output_file"
    done
done
echo "done"
