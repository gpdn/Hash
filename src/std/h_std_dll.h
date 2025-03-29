
#ifndef H_STD_DLL
#define H_STD_DLL

#include <stdlib.h>
#include "h_nfi.h"

value_t h_std_dll_init(struct value_t* parameters, size_t args_count);
value_t h_std_dll_append_head(struct value_t* parameters, size_t args_count);
value_t h_std_dll_append_tail(struct value_t* parameters, size_t args_count);
value_t h_std_dll_remove_first(struct value_t* parameters, size_t args_count);
value_t h_std_dll_remove_last(struct value_t* parameters, size_t args_count);
value_t h_std_dll_remove_all(struct value_t* parameters, size_t args_count);
value_t h_std_dll_replace_first(struct value_t* parameters, size_t args_count);
value_t h_std_dll_replace_last(struct value_t* parameters, size_t args_count);
value_t h_std_dll_replace_all(struct value_t* parameters, size_t args_count);
value_t h_std_dll_find_first(struct value_t* parameters, size_t args_count);
value_t h_std_dll_find_last(struct value_t* parameters, size_t args_count);
value_t h_std_dll_find_all(struct value_t* parameters, size_t args_count);
value_t h_std_dll_combine(struct value_t* parameters, size_t args_count);
value_t h_std_dll_size(struct value_t* parameters, size_t args_count);
value_t h_std_dll_get_first(struct value_t* parameters, size_t args_count);
value_t h_std_dll_get_last(struct value_t* parameters, size_t args_count);
value_t h_std_dll_get_head(struct value_t* parameters, size_t args_count);
value_t h_std_dll_get_tail(struct value_t* parameters, size_t args_count);
value_t h_std_dll_print(struct value_t* parameters, size_t args_count);
int h_std_dll_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table);

#endif