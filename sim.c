#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <inttypes.h>
#define MAX_RAND 1000000000000000
#define BRUTE_FORCE_1 1
#define HAMMOND_1 2
#define BRUTE_FORCE_2 3
#define HAMMOND_2 4
#define MAX_BURST_SIZE 15


char CODE_TO_USE = 0;
int NUM_ITERATIONS = 1000;
int BIT_ERROR_RATE = 1000;
uint64_t RIGHT = 0;
uint64_t WRONG = 0;
uint64_t FIXED = 0;
uint64_t CHANGED = 0;
int TOTAL_BITS;
uint64_t original;

/* Lookup table for Hamming(7, 4) codes
 * can just index by four-bit sequence
 */
uint8_t hamming_74_lookup[16] = {
    0x00, // 0000
    0x69, // 0001
    0x2A, // 0010
    0x43, // 0011
    0x4C, // 0100
    0x25, // 0101
    0x66, // 0110
    0x0F, // 0111
    0x70, // 1000
    0x19, // 1001
    0x5A, // 1010
    0x33, // 1011
    0x3C, // 1100
    0x55, // 1101
    0x16, // 1110
    0x7F  // 1111
};

void set_error_spots(uint64_t* target);
void do_burst_error(uint64_t *target);
void unsafe();
void hamming_1();
void hamming_2();
void brute_force_1();
void brute_force_2();
void error_test();
void do_clustered_error(uint64_t *target);



uint8_t diff_bits(uint64_t x, uint64_t y){
    return __builtin_popcountll(x^y);
}

void print_results(){
    printf("%d total iterations:\n\t%"PRIu64" bits uncorrupted\n\t%"PRIu64" bits of original message corrupted\n\t%"PRIu64" bit errors during transmission\n",
           NUM_ITERATIONS, RIGHT, WRONG, CHANGED);
}
/*
 * hamming(7, 4) encoding of global uint64_t original;
 * assumes given target of two uint64_ts
 */
void hamming_encode_74(uint64_t *target){
    for (int i = 0; i < 16; i++){
        //printf("hamming code: %x\n", hamming_74_lookup[(original>>(4*i))&0xF]);
        target[i/8] |= (uint64_t)((hamming_74_lookup[(original>>(4*i))&0xF])&0x7F)<<(8*(i%8));
    }
}
/* decode hamming(7, 4) assuming valid input and output
 * of uint64_t array and pointer, respectively and preemtively
 * set output to 0 */
void hamming_decode_74(uint64_t* input, uint64_t* output){
    *output = 0;
    for (int i = 0; i < 16; i++){
        uint8_t current_byte = (input[i/8]>>(8*(i%8)))&0x7F;
        /* byte to decode */
        //printf("Decoding byte: %x\n", current_byte);
        for (int index = 0; index < 16; index++){
            if (__builtin_popcount(current_byte^hamming_74_lookup[index]) <= 1){
                *output |= (((uint64_t)index)<<(4*i));
                //printf("Got a byte: %x -> %"PRIx64"\n", (uint64_t)index, *output);
                break;
                /* we found the right index */
            }
        }
    }
}

int main(int argc, char*argv[]){
    srandom(time(NULL));
    if (argc == 1){
        printf("Error: need argument to specify code to use");
        exit(-1);
    }
    CODE_TO_USE = (char)atoi(argv[1]);
    if (argc > 2){
        NUM_ITERATIONS = atoi(argv[2]);
        if (argc > 3){
            BIT_ERROR_RATE = atoi(argv[3]);
        }
    }
    original = (random() << 31) | (random());
    error_test();
}

void error_test(){
    int *results = (int*)calloc(2, sizeof(int));
    printf("Using error rate 1 per %d\n", BIT_ERROR_RATE);
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
        case HAMMOND_1:
            for(int i = 0; i < NUM_ITERATIONS; i++){
                hamming_1();
            }
            break;
        case BRUTE_FORCE_2:
            for(int i = 0; i < NUM_ITERATIONS; i++){
                brute_force_2();
            }
            break;
        case HAMMOND_2:
            for(int i = 0; i < NUM_ITERATIONS; i++){
                hamming_2();
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
    printf("\n\noriginal: %"PRIx64"\n", original);
    printf("encoded after corruption: %"PRIx64" %"PRIx64"\n", corrupted[0], corrupted[1]);
    uint64_t reconstructed = 0; hamming_decode_74((uint64_t*)&corrupted, &reconstructed);
    printf("reconstruction: %"PRIx64"\n", reconstructed);
    uint8_t d = diff_bits(reconstructed, original);
    WRONG += d; RIGHT += 64-d; FIXED += CHANGED-d;
}
void hamming_2(){
    uint64_t corrupted[2] = {0, 0};
    hamming_encode_74((uint64_t*)&corrupted);
    TOTAL_BITS = 2*sizeof(uint64_t)*8; // using 2 full uint64_ts
    do_burst_error((uint64_t*)&corrupted);
    printf("\n\noriginal: %"PRIx64"\n", original);
    printf("encoded after corruption: %"PRIx64" %"PRIx64"\n", corrupted[0], corrupted[1]);
    uint64_t reconstructed = 0; hamming_decode_74((uint64_t*)&corrupted, &reconstructed);
    printf("reconstruction: %"PRIx64"\n", reconstructed);
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

void set_error_spots(uint64_t *target){
    switch(CODE_TO_USE){
        case 0:
        case BRUTE_FORCE_1:
            for (int i = 0; i < TOTAL_BITS; i++){
                if((random()%BIT_ERROR_RATE) == 0){
                    target[i/(sizeof(uint64_t)*8)] ^= (uint64_t)1<<(i%64);
                    CHANGED++;
                }
            }
            break;
        case HAMMOND_1:
            for (int i = 0; i < TOTAL_BITS; i++){
                /* bits 7, 15, etc (from zero index) are not used */
                if (i%8 == 7) continue;
                if((random()%BIT_ERROR_RATE) == 0){
                    target[i/(sizeof(uint64_t)*8)] ^= (uint64_t)1<<(i%64);
                    CHANGED++;
                    //printf("changing bit %d\n", i);
                }
            }
    }
}
void do_burst_error(uint64_t *target){
    int burst_size;
    switch(CODE_TO_USE){
        case 0:
        case BRUTE_FORCE_2:
            for (int i = 0; i < TOTAL_BITS; i++){
                burst_size = (random()%MAX_BURST_SIZE)+1; // burst size of zero would not make sense
                if((random()%(BIT_ERROR_RATE*burst_size)) == 0){
                    for(int j = 0; j < burst_size && i+j < TOTAL_BITS; j++){
                        target[(i+j)/(sizeof(uint64_t)*8)] ^= (uint64_t)1<<((i+j)%64);
                        CHANGED++;
                    }
                    i += burst_size;
                }
            }
            break;
        case HAMMOND_2:
            for (int i = 0; i < TOTAL_BITS; i++){
                burst_size = (random()%MAX_BURST_SIZE)+1;
                /* bits 7, 15, etc (from zero index) are not used */
                if (i%8 == 7) continue;
                if((random()%(BIT_ERROR_RATE*burst_size)) == 0){
                    for(int j = 0; j < burst_size && i+j < TOTAL_BITS; j++){
                        if ((i+j)%8 == 7){
                            j--;
                            i++; // avoid infinite loop
                            continue; // skip unused bit
                        }
                        target[(i+j)/(sizeof(uint64_t)*8)] ^= (uint64_t)1<<((i+j)%64);
                        CHANGED++;
                    }
                    i += burst_size;
                }
            }
    }
}
/* made unintentionally while making do_burst_error */
void do_clustered_error(uint64_t *target){
    int burst_size;
    switch(CODE_TO_USE){
        case 0:
        case BRUTE_FORCE_2:
            for (int i = 0; i < TOTAL_BITS; i++){
                burst_size = (random()%MAX_BURST_SIZE)+1; // burst size of zero would not make sense
                if((random()%(BIT_ERROR_RATE*burst_size)) == 0){
                    for(int j = 0; j < burst_size && i+j < TOTAL_BITS; j++){
                        i++;
                        target[(i+j)/(sizeof(uint64_t)*8)] ^= (uint64_t)1<<((i+j)%64);
                        CHANGED++;
                    }
                }
            }
            break;
        case HAMMOND_2:
            for (int i = 0; i < TOTAL_BITS; i++){
                burst_size = (random()%MAX_BURST_SIZE)+1;
                /* bits 7, 15, etc (from zero index) are not used */
                if (i%8 == 7) continue;
                if((random()%(BIT_ERROR_RATE*burst_size)) == 0){
                    for(int j = 0; j < burst_size && i+j < TOTAL_BITS; j++){
                        i++;
                        if ((i+j)%8 == 7){
                            j--;
                            continue; // skip unused bit
                        }
                        target[(i+j)/(sizeof(uint64_t)*8)] ^= (uint64_t)1<<((i+j)%64);
                        CHANGED++;

                    }
                }
            }
    }
}
