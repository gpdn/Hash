#ifndef H_STD_TYPE
#define H_STD_TYPE

#include "h_nfi.h"
#include <ctype.h>
#include <stdlib.h>

value_t h_std_type_is_str(struct value_t* parameters, size_t args_count);
value_t h_std_type_is_num(struct value_t* parameters, size_t args_count);
value_t h_std_type_is_array(struct value_t* parameters, size_t args_count);
value_t h_std_type_is_null(struct value_t* parameters, size_t args_count);
value_t h_std_type_is_function(struct value_t* parameters, size_t args_count);
value_t h_std_type_is_data(struct value_t* parameters, size_t args_count);
value_t h_std_type_ntos(struct value_t* parameters, size_t args_count);
value_t h_std_type_ston(struct value_t* parameters, size_t args_count);
int h_std_type_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table);

#endif