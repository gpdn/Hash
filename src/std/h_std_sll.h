#ifndef H_STD_SLL
#define H_STD_SLL

#include <stdlib.h>
#include "h_nfi.h"

value_t h_std_sll_init(struct value_t* parameters, size_t args_count);
value_t h_std_sll_append_head(struct value_t* parameters, size_t args_count);
value_t h_std_sll_append_tail(struct value_t* parameters, size_t args_count);
value_t h_std_sll_remove_first(struct value_t* parameters, size_t args_count);
value_t h_std_sll_remove_last(struct value_t* parameters, size_t args_count);
value_t h_std_sll_remove_all(struct value_t* parameters, size_t args_count);
value_t h_std_sll_replace_first(struct value_t* parameters, size_t args_count);
value_t h_std_sll_replace_last(struct value_t* parameters, size_t args_count);
value_t h_std_sll_replace_all(struct value_t* parameters, size_t args_count);
value_t h_std_sll_find_first(struct value_t* parameters, size_t args_count);
value_t h_std_sll_find_last(struct value_t* parameters, size_t args_count);
value_t h_std_sll_find_all(struct value_t* parameters, size_t args_count);
value_t h_std_sll_combine(struct value_t* parameters, size_t args_count);
value_t h_std_sll_size(struct value_t* parameters, size_t args_count);
value_t h_std_sll_get_first(struct value_t* parameters, size_t args_count);
value_t h_std_sll_get_last(struct value_t* parameters, size_t args_count);
value_t h_std_sll_get_head(struct value_t* parameters, size_t args_count);
value_t h_std_sll_get_tail(struct value_t* parameters, size_t args_count);
value_t h_std_sll_print(struct value_t* parameters, size_t args_count);
int h_std_sll_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table);

#endif