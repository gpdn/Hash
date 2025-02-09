#ifndef H_STD_TIME
#define H_STD_TIME

#include "h_nfi.h"
#include <time.h>

value_t h_std_time_get_current(struct value_t* parameters, size_t args_count);
int h_std_time_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table);

#endif