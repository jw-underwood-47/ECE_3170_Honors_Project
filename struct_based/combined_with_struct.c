#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "encoders_with_struct.h"

char CODE_TO_USE = 0;
char ERROR_TYPE = 2;
int NUM_ITERATIONS = 1000;
int BIT_ERROR_RATE = 1000;
int TOTAL_BITS;
uint64_t original;

void error_test(results_t*);
void unsafe(results_t*);
void brute_force_1(results_t*);
void brute_force_2(results_t*);
void brute_force_3(results_t*);
void brute_force_4(results_t*);
void hamming_1(results_t*);
void hamming_2(results_t*);
void hamming_3(results_t*);
void hamming_4(results_t*);

/* prints info after test is run
 * Iterations is how many time the sim function was called;
 * muptiply by 64 to get total bits in original message
 * "bits uncorrupted" is bits of original message; that plus
 * "bits of original message corrupted" should be 64*NUM_ITERATIONS
 * "bit errors during transmission" is how many bits were subjected
 * to the bit error event; for anything but inverting error, those
 * bits were not necessarily actually flipped (a 0 that gets 0'd is still 0)
 */
void print_results(results_t* results){
    printf("%d total iterations:\n\t%"PRIu64" bits uncorrupted\n\t%"PRIu64" bits of original message corrupted\n\t%"PRIu64" bit errors during transmission\n",
           NUM_ITERATIONS, results->right, results->wrong, results->changed);
}


int main(int argc, char*argv[]){
    srandom(time(NULL));
    results_t *results = (results_t*)calloc(1, sizeof(results_t));
    if (argc == 1){
        printf("Error: need argument to specify code to use");
        exit(-1);
    }
    CODE_TO_USE = (char)atoi(argv[1]);
    if (argc > 2){
        ERROR_TYPE = atoi(argv[2]);
        if (argc > 3){
            NUM_ITERATIONS = atoi(argv[3]);
            if (argc > 4){
                BIT_ERROR_RATE = atoi(argv[4]);
            }
        }
    }
    original = (random() << 31) | (random());
    error_test(results);
}

/* print some info about run, then call function to run appropriate sim */
void error_test(results_t* results){
    //int *results = (int*)calloc(2, sizeof(int));
    printf("Using error rate 1 per %d error type # %d\n", BIT_ERROR_RATE, ERROR_TYPE);
    switch(CODE_TO_USE){
        case 0:
            for(int i = 0; i < NUM_ITERATIONS; i++){
                unsafe(results);
            }
            break;
        case BRUTE_FORCE_1:
            for(int i = 0; i < NUM_ITERATIONS; i++){
                brute_force_1(results);
            }
            break;
        case HAMMING_1:
            for(int i = 0; i < NUM_ITERATIONS; i++){
                hamming_1(results);
            }
            break;
        case BRUTE_FORCE_2:
            for(int i = 0; i < NUM_ITERATIONS; i++){
                brute_force_2(results);
            }
            break;
        case HAMMING_2:
            for(int i = 0; i < NUM_ITERATIONS; i++){
                hamming_2(results);
            }
            break;
        case BRUTE_FORCE_3:
            for(int i = 0; i < NUM_ITERATIONS; i++){
                brute_force_3(results);
            }
            break;
        case HAMMING_3:
            for(int i = 0; i < NUM_ITERATIONS; i++){
                hamming_3(results);
            }
            break;
        case BRUTE_FORCE_4:
            for(int i = 0; i < NUM_ITERATIONS; i++){
                brute_force_4(results);
            }
            break;
        case HAMMING_4:
            for(int i = 0; i < NUM_ITERATIONS; i++){
                hamming_4(results);
            }
            break;
    }
    print_results(results);
}

void unsafe(results_t* results){
    uint64_t corrupted = original;
    TOTAL_BITS = sizeof(uint64_t)*8;
    set_error_spots(&corrupted, results);
    uint8_t d = diff_bits(corrupted, original);
    results->wrong += d; results->right += TOTAL_BITS-d; results->fixed += results->changed-d;
}
/* hamming(7, 4) encoding */
void hamming_1(results_t* results){
    uint64_t corrupted[2] = {0, 0};
    hamming_encode_74((uint64_t*)&corrupted);
    TOTAL_BITS = 2*sizeof(uint64_t)*8; // using 2 full uint64_ts
    set_error_spots((uint64_t*)&corrupted, results);
    //printf("\n\noriginal: %"PRIx64"\n", original);
    //printf("encoded after corruption: %"PRIx64" %"PRIx64"\n", corrupted[0], corrupted[1]);
    uint64_t reconstructed = 0; hamming_decode_74((uint64_t*)&corrupted, &reconstructed);
    //printf("reconstruction: %"PRIx64"\n", reconstructed);
    uint8_t d = diff_bits(reconstructed, original);
    results->wrong += d; results->right += 64-d; results->fixed += (results->changed)-d;
}
void hamming_2(results_t* results){
    uint64_t corrupted[2] = {0, 0};
    hamming_encode_74((uint64_t*)&corrupted);
    TOTAL_BITS = 2*sizeof(uint64_t)*8; // using 2 full uint64_ts
    do_burst_error((uint64_t*)&corrupted, results);
    //printf("\n\noriginal: %"PRIx64"\n", original);
    //printf("encoded after corruption: %"PRIx64" %"PRIx64"\n", corrupted[0], corrupted[1]);
    uint64_t reconstructed = 0; hamming_decode_74((uint64_t*)&corrupted, &reconstructed);
    //printf("reconstruction: %"PRIx64"\n", reconstructed);
    uint8_t d = diff_bits(reconstructed, original);
    results->wrong += d; results->right += 64-d; results->fixed += (results->changed)-d;
}
void hamming_3(results_t* results){
    uint64_t corrupted[2] = {0, 0};
    hamming_encode_74((uint64_t*)&corrupted);
    TOTAL_BITS = 2*sizeof(uint64_t)*8; // using 2 full uint64_ts
    do_clustered_error((uint64_t*)&corrupted, results);
    //printf("\n\noriginal: %"PRIx64"\n", original);
    //printf("encoded after corruption: %"PRIx64" %"PRIx64"\n", corrupted[0], corrupted[1]);
    uint64_t reconstructed = 0; hamming_decode_74((uint64_t*)&corrupted, &reconstructed);
    //printf("reconstruction: %"PRIx64"\n", reconstructed);
    uint8_t d = diff_bits(reconstructed, original);
    results->wrong += d; results->right += 64-d; results->fixed += (results->changed)-d;
}
void hamming_4(results_t* results){
    uint64_t corrupted[2] = {0, 0};
    hamming_encode_74((uint64_t*)&corrupted);
    TOTAL_BITS = 2*sizeof(uint64_t)*8; // using 2 full uint64_ts
    do_more_burst_errors((uint64_t*)&corrupted, results);
    //printf("\n\noriginal: %"PRIx64"\n", original);
    //printf("encoded after corruption: %"PRIx64" %"PRIx64"\n", corrupted[0], corrupted[1]);
    uint64_t reconstructed = 0; hamming_decode_74((uint64_t*)&corrupted, &reconstructed);
    //printf("reconstruction: %"PRIx64"\n", reconstructed);
    uint8_t d = diff_bits(reconstructed, original);
    results->wrong += d; results->right += 64-d; results->fixed += (results->changed)-d;
}
void brute_force_1(results_t *results){
    uint64_t corrupted[3] = {original, original, original};
    TOTAL_BITS = 3*sizeof(uint64_t)*8;
    set_error_spots((uint64_t*)&corrupted, results);
    uint64_t reconstructed = (corrupted[0]&corrupted[1])|(corrupted[0]&corrupted[2])|(corrupted[1]&corrupted[2]);
    uint8_t d = diff_bits(reconstructed, original);
    results->wrong += d; results->right += 64-d; results->fixed += (results->changed)-d;
}
void brute_force_2(results_t* results){
    uint64_t corrupted[3] = {original, original, original};
    TOTAL_BITS = 3*sizeof(uint64_t)*8;
    do_burst_error((uint64_t*)&corrupted, results);
    uint64_t reconstructed = (corrupted[0]&corrupted[1])|(corrupted[0]&corrupted[2])|(corrupted[1]&corrupted[2]);
    uint8_t d = diff_bits(reconstructed, original);
    //int was_wrong = diff_bits(corrupted[0], original) + diff_bits(corrupted[1], original) + diff_bits(corrupted[2], original);
    //total bit errors -- but, technically 3x as many b/c 3x bits sent
    results->wrong += d; results->right += 64-d; results->fixed += (results->changed)-d;
}
void brute_force_3(results_t* results){
    uint64_t corrupted[3] = {original, original, original};
    TOTAL_BITS = 3*sizeof(uint64_t)*8;
    do_clustered_error((uint64_t*)&corrupted, results);
    uint64_t reconstructed = (corrupted[0]&corrupted[1])|(corrupted[0]&corrupted[2])|(corrupted[1]&corrupted[2]);
    uint8_t d = diff_bits(reconstructed, original);
    results->wrong += d; results->right += 64-d; results->fixed += (results->changed)-d;
}
void brute_force_4(results_t* results){
    uint64_t corrupted[3] = {original, original, original};
    TOTAL_BITS = 3*sizeof(uint64_t)*8;
    do_more_burst_errors((uint64_t*)&corrupted, results);
    uint64_t reconstructed = (corrupted[0]&corrupted[1])|(corrupted[0]&corrupted[2])|(corrupted[1]&corrupted[2]);
    uint8_t d = diff_bits(reconstructed, original);
    //int was_wrong = diff_bits(corrupted[0], original) + diff_bits(corrupted[1], original) + diff_bits(corrupted[2], original);
    //total bit errors -- but, technically 3x as many b/c 3x bits sent
    results->wrong += d; results->right += 64-d; results->fixed += (results->changed)-d;
}

