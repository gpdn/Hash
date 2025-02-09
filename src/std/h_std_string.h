#ifndef H_STD_STRING
#define H_STD_STRING

#include "h_nfi.h"
#include <string.h>

value_t h_std_str_compare(struct value_t* parameters, size_t args_count);
value_t h_std_str_concat(struct value_t* parameters, size_t args_count);
value_t h_std_str_substr(struct value_t* parameters, size_t args_count);
value_t h_std_str_find(struct value_t* parameters, size_t args_count);
//value_t h_std_str_find_last(struct value_t* parameters, size_t args_count);
value_t h_std_str_reverse(struct value_t* parameters, size_t args_count);
value_t h_std_str_find_all(struct value_t* parameters, size_t args_count);
value_t h_std_str_len(struct value_t* parameters, size_t args_count);
value_t h_std_str_replace(struct value_t* parameters, size_t args_count);
int h_std_string_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table);

#endif