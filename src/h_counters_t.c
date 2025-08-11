#include "h_counters_t.h"

#define H_COUNTERS_MIN_TABLE_SIZE 64

static inline uint32_t calculate_closest_power_2(uint32_t value);

static inline uint32_t calculate_closest_power_2(uint32_t value) {
    if (value < H_COUNTERS_MIN_TABLE_SIZE) return H_COUNTERS_MIN_TABLE_SIZE;
    --value;
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    ++value;
    return value;
}

inline h_counters_t* h_counters_init() {
    h_counters_t* counters = (h_counters_t*)malloc(sizeof(h_counters_t));
    memset(counters, 0, sizeof(h_counters_t));
    return counters;
}

inline void h_counters_print(h_counters_t* counters) {
    #if DEBUG_TRACE_COUNTERS
        DEBUG_LOG("%s - %d\n", "globals_count", counters->globals_count);
        DEBUG_LOG("%s - %d\n", "locals_count", counters->locals_count);
        DEBUG_LOG("%s - %d\n", "labels_count", counters->labels_count);
        DEBUG_LOG("%s - %d\n", "enums_count", counters->enums_count);
        DEBUG_LOG("%s - %d\n", "types_count", counters->types_count);
        DEBUG_LOG("%s - %d\n", "switch_count", counters->switch_count);
        DEBUG_LOG("%s - %d\n", "fwd_count", counters->fwd_count);
    #endif
}

void h_counters_resolve(h_counters_t* counters) {
    counters->globals_count = calculate_closest_power_2(counters->globals_count);
    counters->locals_count = calculate_closest_power_2(counters->locals_count);
    counters->labels_count = calculate_closest_power_2(counters->labels_count);
    counters->types_count = calculate_closest_power_2(counters->types_count + counters->enums_count);
    counters->enums_count = calculate_closest_power_2(counters->enums_count);
    counters->switch_count = calculate_closest_power_2(counters->switch_count);
    counters->fwd_count = calculate_closest_power_2(counters->fwd_count);
}

inline void h_counters_free(h_counters_t* counters) {
    free(counters);
}

#undef H_COUNTERS_MIN_TABLE_SIZE