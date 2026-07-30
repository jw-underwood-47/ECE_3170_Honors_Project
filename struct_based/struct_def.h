#include <inttypes.h>

typedef struct result_struct_t{
    uint64_t right;
    uint64_t wrong;
    uint64_t fixed;
    uint64_t changed;
} results_t;

typedef struct message_struct_t{
    uint64_t original;
    uint64_t *corrupted;
} message_t;
