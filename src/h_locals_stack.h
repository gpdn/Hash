#ifndef H_LOCALS_STACK_H
#define H_LOCALS_STACK_H

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
void h_local_stack_push(h_locals_stack_t* locals_stack, h_local_t local);
value_t h_local_stack_pop(h_locals_stack_t* locals_stack);
value_t h_local_stack_peek(h_locals_stack_t* locals_stack);
void h_locals_stack_free(h_locals_stack_t* locals_stack);

#endif