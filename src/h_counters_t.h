#ifndef H_COUNTERS
#define H_COUNTERS

#include "debug.h"
#include <stdint.h>
#include <memory.h>

typedef struct h_counters_t {
    uint32_t globals_count;
    uint32_t locals_count;
    uint32_t labels_count;
    uint32_t enums_count;
    uint32_t types_count;
    uint32_t switch_count;
    uint32_t fwd_count;
} h_counters_t;

h_counters_t* h_counters_init();
void h_counters_resolve(h_counters_t* counters);
void h_counters_print(h_counters_t* counters);
void h_counters_free(h_counters_t* counters);

#endif