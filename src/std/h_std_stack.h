#ifndef H_STD_STACK
#define H_STD_STACK

#include <stdlib.h>
#include "h_nfi.h"

value_t h_std_stack_init(struct value_t* parameters, size_t args_count);
value_t h_std_stack_push(struct value_t* parameters, size_t args_count);
value_t h_std_stack_pop(struct value_t* parameters, size_t args_count);
value_t h_std_stack_clear(struct value_t* parameters, size_t args_count);
value_t h_std_stack_size(struct value_t* parameters, size_t args_count);
value_t h_std_stack_print(struct value_t* parameters, size_t args_count);
int h_std_stack_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table);

#endif