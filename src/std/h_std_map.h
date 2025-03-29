#ifndef H_STD_MAP
#define H_STD_MAP

#include <stdlib.h>
#include "h_nfi.h"

value_t h_std_map_init(struct value_t* parameters, size_t args_count);
value_t h_std_map_set(struct value_t* parameters, size_t args_count);
value_t h_std_map_get(struct value_t* parameters, size_t args_count);
value_t h_std_map_unset(struct value_t* parameters, size_t args_count);
value_t h_std_map_clear(struct value_t* parameters, size_t args_count);
value_t h_std_map_check(struct value_t* parameters, size_t args_count);
value_t h_std_map_size(struct value_t* parameters, size_t args_count);
value_t h_std_map_print(struct value_t* parameters, size_t args_count);
int h_std_map_import(h_locals_stack_t* map, h_ht_enums_t* enums_table, h_ht_types_t* types_table);

#endif