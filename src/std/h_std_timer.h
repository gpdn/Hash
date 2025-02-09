#ifndef H_STD_TIMER
#define H_STD_TIMER

#include "h_nfi.h"
#include <time.h>

typedef struct h_std_timer_t {
    double* times;
    size_t size;
    size_t capacity;
} h_std_timer_t;

value_t h_std_timer_start(struct value_t* parameters, size_t args_count);
value_t h_std_timer_elapsed(struct value_t* parameters, size_t args_count);
value_t h_std_timer_lap(struct value_t* parameters, size_t args_count);
value_t h_std_timer_reset(struct value_t* parameters, size_t args_count);
value_t h_std_timer_stop(struct value_t* parameters, size_t args_count);
value_t h_std_timer_free(struct value_t* parameters, size_t args_count);
int h_std_timer_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table);

#endif