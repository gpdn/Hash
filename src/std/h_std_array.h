#ifndef H_STD_ARRAY
#define H_STD_ARRAY

#include "h_nfi.h"

value_t h_std_array_len(struct value_t* parameters, size_t args_count);
value_t h_std_array_push(struct value_t* parameters, size_t args_count);
int h_std_array_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table);

#endif