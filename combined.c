#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <inttypes.h>
#include "encoders.h"

char CODE_TO_USE = 0;
char ERROR_TYPE = 2;
int NUM_ITERATIONS = 1000;
int BIT_ERROR_RATE = 1000;
uint64_t RIGHT = 0;
uint64_t WRONG = 0;
uint64_t FIXED = 0;
uint64_t CHANGED = 0;
int TOTAL_BITS;
uint64_t original;

void error_test();
void unsafe();
void brute_force_1();
void brute_force_2();
void brute_force_3();
void brute_force_4();
void hamming_1();
void hamming_2();
void hamming_3();
void hamming_4();

void print_results(){
    printf("%d total iterations:\n\t%"PRIu64" bits uncorrupted\n\t%"PRIu64" bits of original message corrupted\n\t%"PRIu64" bit errors during transmission\n",
           NUM_ITERATIONS, RIGHT, WRONG, CHANGED);
}


int main(int argc, char*argv[]){
    srandom(time(NULL));
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
    error_test();
}

/* print some info about run, then call function to run appropriate sim */
void error_test(){
    int *results = (int*)calloc(2, sizeof(int));
    printf("Using error rate 1 per %d error type # %d\n", BIT_ERROR_RATE, ERROR_TYPE);
    switch(CODE_TO_USE){
        case 0:
            for(int i = 0; i < NUM_ITERATIONS; i++){
                unsafe();
            }
            break;
        case BRUTE_FORCE_1:
            for(int i = 0; i < NUM_ITERATIONS; i++){
                brute_force_1();
            }
            break;
        case HAMMING_1:
            for(int i = 0; i < NUM_ITERATIONS; i++){
                hamming_1();
            }
            break;
        case BRUTE_FORCE_2:
            for(int i = 0; i < NUM_ITERATIONS; i++){
                brute_force_2();
            }
            break;
        case HAMMING_2:
            for(int i = 0; i < NUM_ITERATIONS; i++){
                hamming_2();
            }
            break;
        case BRUTE_FORCE_3:
            for(int i = 0; i < NUM_ITERATIONS; i++){
                brute_force_3();
            }
            break;
        case HAMMING_3:
            for(int i = 0; i < NUM_ITERATIONS; i++){
                hamming_3();
            }
            break;
        case BRUTE_FORCE_4:
            for(int i = 0; i < NUM_ITERATIONS; i++){
                brute_force_4();
            }
            break;
        case HAMMING_4:
            for(int i = 0; i < NUM_ITERATIONS; i++){
                hamming_4();
            }
            break;
    }
    print_results();
}

void unsafe(){
    uint64_t corrupted = original;
    TOTAL_BITS = sizeof(uint64_t)*8;
    set_error_spots(&corrupted);
    uint8_t d = diff_bits(corrupted, original);
    WRONG += d; RIGHT += TOTAL_BITS-d; FIXED += CHANGED-d;
}
/* hamming(7, 4) encoding */
void hamming_1(){
    uint64_t corrupted[2] = {0, 0};
    hamming_encode_74((uint64_t*)&corrupted);
    TOTAL_BITS = 2*sizeof(uint64_t)*8; // using 2 full uint64_ts
    set_error_spots((uint64_t*)&corrupted);
    //printf("\n\noriginal: %"PRIx64"\n", original);
    //printf("encoded after corruption: %"PRIx64" %"PRIx64"\n", corrupted[0], corrupted[1]);
    uint64_t reconstructed = 0; hamming_decode_74((uint64_t*)&corrupted, &reconstructed);
    //printf("reconstruction: %"PRIx64"\n", reconstructed);
    uint8_t d = diff_bits(reconstructed, original);
    WRONG += d; RIGHT += 64-d; FIXED += CHANGED-d;
}
void hamming_2(){
    uint64_t corrupted[2] = {0, 0};
    hamming_encode_74((uint64_t*)&corrupted);
    TOTAL_BITS = 2*sizeof(uint64_t)*8; // using 2 full uint64_ts
    do_burst_error((uint64_t*)&corrupted);
    //printf("\n\noriginal: %"PRIx64"\n", original);
    //printf("encoded after corruption: %"PRIx64" %"PRIx64"\n", corrupted[0], corrupted[1]);
    uint64_t reconstructed = 0; hamming_decode_74((uint64_t*)&corrupted, &reconstructed);
    //printf("reconstruction: %"PRIx64"\n", reconstructed);
    uint8_t d = diff_bits(reconstructed, original);
    WRONG += d; RIGHT += 64-d; FIXED += CHANGED-d;
}
void hamming_3(){
    uint64_t corrupted[2] = {0, 0};
    hamming_encode_74((uint64_t*)&corrupted);
    TOTAL_BITS = 2*sizeof(uint64_t)*8; // using 2 full uint64_ts
    do_clustered_error((uint64_t*)&corrupted);
    //printf("\n\noriginal: %"PRIx64"\n", original);
    //printf("encoded after corruption: %"PRIx64" %"PRIx64"\n", corrupted[0], corrupted[1]);
    uint64_t reconstructed = 0; hamming_decode_74((uint64_t*)&corrupted, &reconstructed);
    //printf("reconstruction: %"PRIx64"\n", reconstructed);
    uint8_t d = diff_bits(reconstructed, original);
    WRONG += d; RIGHT += 64-d; FIXED += CHANGED-d;
}
void hamming_4(){
    uint64_t corrupted[2] = {0, 0};
    hamming_encode_74((uint64_t*)&corrupted);
    TOTAL_BITS = 2*sizeof(uint64_t)*8; // using 2 full uint64_ts
    do_more_burst_errors((uint64_t*)&corrupted);
    //printf("\n\noriginal: %"PRIx64"\n", original);
    //printf("encoded after corruption: %"PRIx64" %"PRIx64"\n", corrupted[0], corrupted[1]);
    uint64_t reconstructed = 0; hamming_decode_74((uint64_t*)&corrupted, &reconstructed);
    //printf("reconstruction: %"PRIx64"\n", reconstructed);
    uint8_t d = diff_bits(reconstructed, original);
    WRONG += d; RIGHT += 64-d; FIXED += CHANGED-d;
}
void brute_force_1(){
    uint64_t corrupted[3] = {original, original, original};
    TOTAL_BITS = 3*sizeof(uint64_t)*8;
    set_error_spots((uint64_t*)&corrupted);
    uint64_t reconstructed = (corrupted[0]&corrupted[1])|(corrupted[0]&corrupted[2])|(corrupted[1]&corrupted[2]);
    uint8_t d = diff_bits(reconstructed, original);
    WRONG += d; RIGHT += 64-d; FIXED += CHANGED-d;
}
void brute_force_2(){
    uint64_t corrupted[3] = {original, original, original};
    TOTAL_BITS = 3*sizeof(uint64_t)*8;
    do_burst_error((uint64_t*)&corrupted);
    uint64_t reconstructed = (corrupted[0]&corrupted[1])|(corrupted[0]&corrupted[2])|(corrupted[1]&corrupted[2]);
    uint8_t d = diff_bits(reconstructed, original);
    //int was_wrong = diff_bits(corrupted[0], original) + diff_bits(corrupted[1], original) + diff_bits(corrupted[2], original);
    //total bit errors -- but, technically 3x as many b/c 3x bits sent
    WRONG += d; RIGHT += 64-d; FIXED += CHANGED-d;
}
void brute_force_3(){
    uint64_t corrupted[3] = {original, original, original};
    TOTAL_BITS = 3*sizeof(uint64_t)*8;
    do_clustered_error((uint64_t*)&corrupted);
    uint64_t reconstructed = (corrupted[0]&corrupted[1])|(corrupted[0]&corrupted[2])|(corrupted[1]&corrupted[2]);
    uint8_t d = diff_bits(reconstructed, original);
    WRONG += d; RIGHT += 64-d; FIXED += CHANGED-d;
}
void brute_force_4(){
    uint64_t corrupted[3] = {original, original, original};
    TOTAL_BITS = 3*sizeof(uint64_t)*8;
    do_more_burst_errors((uint64_t*)&corrupted);
    uint64_t reconstructed = (corrupted[0]&corrupted[1])|(corrupted[0]&corrupted[2])|(corrupted[1]&corrupted[2]);
    uint8_t d = diff_bits(reconstructed, original);
    //int was_wrong = diff_bits(corrupted[0], original) + diff_bits(corrupted[1], original) + diff_bits(corrupted[2], original);
    //total bit errors -- but, technically 3x as many b/c 3x bits sent
    WRONG += d; RIGHT += 64-d; FIXED += CHANGED-d;
}

