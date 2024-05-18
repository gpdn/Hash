#ifndef H_LOCALS_STACK_H
#define H_LOCALS_STACK_H

#include <stdio.h>
#include "h_values.h"
#include "h_string_t.h"

typedef struct h_local_t {
    h_string_t* name;
    value_t value;
    size_t scope;
} h_local_t;

typedef struct h_locals_stack_t {
    h_local_t* locals_array;
    h_local_t* locals_stack_top;
    h_local_t* locals_stack_it;
    size_t size;
    size_t capacity;
} h_locals_stack_t;

h_locals_stack_t* h_locals_stack_init(size_t capacity);
void h_locals_stack_push(h_locals_stack_t* locals_stack, h_string_t* name, value_t value, size_t scope);
value_t h_locals_stack_pop(h_locals_stack_t* locals_stack);
value_t h_locals_stack_peek(h_locals_stack_t* locals_stack);
value_t h_locals_stack_get(h_locals_stack_t* locals_stack, h_string_t* name, size_t scope);
size_t h_locals_stack_get_index(h_locals_stack_t* locals_stack, h_string_t* name);
value_t h_locals_array_get(h_locals_stack_t* locals_stack, size_t index);
value_t h_locals_array_get_index(h_locals_stack_t* locals_stack, size_t index, size_t element_index);
void h_locals_array_set_index(h_locals_stack_t* locals_stack, size_t index, size_t element_index, value_t value);
value_t h_locals_array_increase_get(h_locals_stack_t* locals_stack, size_t index);
value_t h_locals_array_decrease_get(h_locals_stack_t* locals_stack, size_t index);
value_t h_locals_array_post_increase_get(h_locals_stack_t* locals_stack, size_t index);
value_t h_locals_array_post_decrease_get(h_locals_stack_t* locals_stack, size_t index);
void h_locals_array_set(h_locals_stack_t* locals_stack, size_t index, value_t value);
void h_locals_stack_print(h_locals_stack_t* locals_stack);
void h_print_local(h_local_t* local);
void h_locals_stack_free(h_locals_stack_t* locals_stack);

#endif