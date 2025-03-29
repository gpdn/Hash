#ifndef H_STD_QUEUE
#define H_STD_QUEUE

#include <stdlib.h>
#include "h_nfi.h"

value_t h_std_queue_init(struct value_t* parameters, size_t args_count);
value_t h_std_queue_add(struct value_t* parameters, size_t args_count);
value_t h_std_queue_get(struct value_t* parameters, size_t args_count);
value_t h_std_queue_peek(struct value_t* parameters, size_t args_count);
value_t h_std_queue_size(struct value_t* parameters, size_t args_count);
value_t h_std_queue_clear(struct value_t* parameters, size_t args_count);
value_t h_std_queue_print(struct value_t* parameters, size_t args_count);
int h_std_queue_import(h_locals_stack_t* queue, h_ht_enums_t* enums_table, h_ht_types_t* types_table);

#endif