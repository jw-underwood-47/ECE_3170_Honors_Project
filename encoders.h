#ifndef MAX_RAND
    #define MAX_RAND 1000000000000000
#endif

/* it is implausible that any other header would have one element of any of
 * these groups defined but not the rest */
#ifndef BRUTE_FORCE_1
    #define BRUTE_FORCE_1 2
    #define BRUTE_FORCE_2 4
    #define BRUTE_FORCE_3 6
    #define BRUTE_FORCE_4 8
#endif
#ifndef HAMMING_1
    #define HAMMING_1 1
    #define HAMMING_2 3
    #define HAMMING_3 5
    #define HAMMING_4 7
#endif

#ifndef MAX_BURST_SIZE
    #define MAX_BURST_SIZE 15
#endif
#ifndef ZEROING
    #define ZEROING 0
#endif
#ifndef RAISING
    #define RAISING 1
#endif
#ifndef INVERTING
    #define INVERTING 2
#endif
void do_clustered_error(uint64_t *target);
void do_more_burst_errors(uint64_t *target);
void set_error_spots(uint64_t* target);
void do_burst_error(uint64_t *target);
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

/* I feel like there might be a better way to handle these globals */
extern char CODE_TO_USE;
extern char ERROR_TYPE;
extern int NUM_ITERATIONS;
extern int BIT_ERROR_RATE;
extern uint64_t RIGHT;
extern uint64_t WRONG;
extern uint64_t FIXED;
extern uint64_t CHANGED;
extern int TOTAL_BITS;
extern uint64_t original;

/* count bits differing between uint64_ts
 * used to figure out how many bits were corrupted */
uint8_t diff_bits(uint64_t x, uint64_t y){
    return __builtin_popcountll(x^y);
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
void set_error_spots(uint64_t *target){
    if(CODE_TO_USE % 2){
        for (int i = 0; i < TOTAL_BITS; i++){
            /* bits 7, 15, etc (from zero index) are not used */
            if (i%8 == 7) continue;
            if((random()%BIT_ERROR_RATE) == 0){
                switch (ERROR_TYPE){
                    case INVERTING:
                        target[i/(sizeof(uint64_t)*8)] ^= (uint64_t)1<<(i%64);
                        break;
                    case ZEROING:
                        target[i/(sizeof(uint64_t)*8)] &= ~((uint64_t)1<<(i%64));
                        break;
                    case RAISING:
                        target[i/(sizeof(uint64_t)*8)] |= ((uint64_t)1<<(i%64));
                        break;
                }
                CHANGED++;
                //printf("changing bit %d\n", i);
            }
        }
    }
    else{
        for (int i = 0; i < TOTAL_BITS; i++){
            if((random()%BIT_ERROR_RATE) == 0){
                switch (ERROR_TYPE){
                    case INVERTING:
                        target[i/(sizeof(uint64_t)*8)] ^= (uint64_t)1<<(i%64);
                        break;
                    case ZEROING:
                        target[i/(sizeof(uint64_t)*8)] &= ~((uint64_t)1<<(i%64));
                        break;
                    case RAISING:
                        target[i/(sizeof(uint64_t)*8)] |= ((uint64_t)1<<(i%64));
                        break;
                }
                CHANGED++;
            }
        }
    }
}
/* create burst errors, such that total errors should average out to around
 * expected rate from BIT_ERROR_RATE */
void do_burst_error(uint64_t *target){
    int burst_size;
    if(CODE_TO_USE % 2){
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
                    switch (ERROR_TYPE){
                        case INVERTING:
                            target[(i+j)/(sizeof(uint64_t)*8)] ^= (uint64_t)1<<((i+j)%64);
                            break;
                        case ZEROING:
                            target[(i+j)/(sizeof(uint64_t)*8)] &= ~((uint64_t)1<<((i+j)%64));
                            break;
                        case RAISING:
                            target[(i+j)/(sizeof(uint64_t)*8)] |= ((uint64_t)1<<((i+j)%64));
                            break;
                    }
                    CHANGED++;
                }
                i += burst_size;
            }
        }
    }
    else{
        for (int i = 0; i < TOTAL_BITS; i++){
            burst_size = (random()%MAX_BURST_SIZE)+1; // burst size of zero would not make sense
            if((random()%(BIT_ERROR_RATE*burst_size)) == 0){
                switch (ERROR_TYPE){
                    case INVERTING:
                        target[i/(sizeof(uint64_t)*8)] ^= (uint64_t)1<<(i%64);
                        break;
                    case ZEROING:
                        target[i/(sizeof(uint64_t)*8)] &= ~((uint64_t)1<<(i%64));
                        break;
                    case RAISING:
                        target[i/(sizeof(uint64_t)*8)] |= ((uint64_t)1<<(i%64));
                        break;
                }
                i += burst_size;
            }
        }
    }
}
/* made unintentionally while making do_burst_error */
void do_clustered_error(uint64_t *target){
    int burst_size;
    if(CODE_TO_USE % 2){
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
                    switch (ERROR_TYPE){
                        case INVERTING:
                            target[(i+j)/(sizeof(uint64_t)*8)] ^= (uint64_t)1<<((i+j)%64);
                            break;
                        case ZEROING:
                            target[(i+j)/(sizeof(uint64_t)*8)] &= ~((uint64_t)1<<((i+j)%64));
                            break;
                        case RAISING:
                            target[(i+j)/(sizeof(uint64_t)*8)] |= ((uint64_t)1<<((i+j)%64));
                            break;
                    }
                    CHANGED++;

                }
            }
        }
    }
    else{
        for (int i = 0; i < TOTAL_BITS; i++){
            burst_size = (random()%MAX_BURST_SIZE)+1; // burst size of zero would not make sense
            if((random()%(BIT_ERROR_RATE*burst_size)) == 0){
                for(int j = 0; j < burst_size && i+j < TOTAL_BITS; j++){
                    i++;
                    switch (ERROR_TYPE){
                        case INVERTING:
                            target[(i+j)/(sizeof(uint64_t)*8)] ^= (uint64_t)1<<((i+j)%64);
                            break;
                        case ZEROING:
                            target[(i+j)/(sizeof(uint64_t)*8)] &= ~((uint64_t)1<<((i+j)%64));
                            break;
                        case RAISING:
                            target[(i+j)/(sizeof(uint64_t)*8)] |= ((uint64_t)1<<((i+j)%64));
                            break;
                    }
                    CHANGED++;
                }
            }
        }
    }
}
/* makes bursts with frequency BIT_ERROR_RATE */
void do_more_burst_errors(uint64_t *target){
    int burst_size = (random()%MAX_BURST_SIZE)+1;
    if(CODE_TO_USE % 2){
        for (int i = 0; i < TOTAL_BITS; i++){
            /* bits 7, 15, etc (from zero index) are not used */
            if (i%8 == 7) continue;
            if((random()%(BIT_ERROR_RATE*burst_size)) == 0){
                burst_size = (random()%MAX_BURST_SIZE)+1;
                for(int j = 0; j < burst_size && i+j < TOTAL_BITS; j++){
                    if ((i+j)%8 == 7){
                        j--;
                        i++; // avoid infinite loop
                        continue; // skip unused bit
                    }
                    switch (ERROR_TYPE){
                        case INVERTING:
                            target[(i+j)/(sizeof(uint64_t)*8)] ^= (uint64_t)1<<((i+j)%64);
                            break;
                        case ZEROING:
                            target[(i+j)/(sizeof(uint64_t)*8)] &= ~((uint64_t)1<<((i+j)%64));
                            break;
                        case RAISING:
                            target[(i+j)/(sizeof(uint64_t)*8)] |= ((uint64_t)1<<((i+j)%64));
                            break;
                    }
                    CHANGED++;
                }
                i += burst_size;
            }
        }
    }
    else{
        for (int i = 0; i < TOTAL_BITS; i++){
            if((random()%(BIT_ERROR_RATE*burst_size)) == 0){
                burst_size = (random()%MAX_BURST_SIZE)+1;
                for(int j = 0; j < burst_size && i+j < TOTAL_BITS; j++){
                    switch (ERROR_TYPE){
                        case INVERTING:
                            target[(i+j)/(sizeof(uint64_t)*8)] ^= (uint64_t)1<<((i+j)%64);
                            break;
                        case ZEROING:
                            target[(i+j)/(sizeof(uint64_t)*8)] &= ~((uint64_t)1<<((i+j)%64));
                            break;
                        case RAISING:
                            target[(i+j)/(sizeof(uint64_t)*8)] |= ((uint64_t)1<<((i+j)%64));
                            break;
                    }
                    CHANGED++;
                }
                i += burst_size;
            }
        }
    }
}
