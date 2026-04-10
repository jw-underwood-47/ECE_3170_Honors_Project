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


char CODE_TO_USE = 0;
int NUM_ITERATIONS = 1000;
int BIT_ERROR_RATE = 1000;
uint64_t RIGHT = 0;
uint64_t WRONG = 0;
uint64_t FIXED = 0;
uint64_t DETECTED = 0;
int TOTAL_BITS;
uint64_t original;

void set_error_spots(uint64_t* target);
void unsafe();
void hammond_1();
void hammond_2();
void brute_force_1();
void brute_force_2();
void error_test();

uint8_t diff_bits(uint64_t x, uint64_t y){
    return __builtin_popcountll(x^y);
}

void print_results(){
    printf("%d total iterations:\n\t%"PRIu64" bits uncorrupted\n\t%"PRIu64" errors passed\n\t%"PRIu64" errors detected\n\t%"PRIu64" errors fixed\n",
           NUM_ITERATIONS, RIGHT, WRONG, DETECTED, FIXED);
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
    original = (rand() << 31) | (rand());
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
                hammond_1();
            }
            break;
        case BRUTE_FORCE_2:
            for(int i = 0; i < NUM_ITERATIONS; i++){
                brute_force_2();
            }
            break;
        case HAMMOND_2:
            for(int i = 0; i < NUM_ITERATIONS; i++){
                hammond_2();
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
    WRONG += d; RIGHT += 64-d;
}
void hammond_1(){

}
void hammond_2(){

}
void brute_force_1(){

}
void brute_force_2(){

}

void set_error_spots(uint64_t *target){
    for (int i = 0; i < TOTAL_BITS; i++){
        if((random()%BIT_ERROR_RATE) == 0) *target ^= (uint64_t)1<<i;
    }
}
