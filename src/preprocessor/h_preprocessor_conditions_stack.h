#ifndef H_PREPROCESSOR_CONDITIONS_STACK
#define H_PREPROCESSOR_CONDITIONS_STACK

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "../debug.h"
#include "h_preprocessor_value.h"
#include "h_preprocessor_directive.h"

typedef struct h_preprocessor_condition_t {
    h_preprocessor_directive_t directive;
    int output;
} h_preprocessor_condition_t;

typedef struct h_preprocessor_conditions_stack_t {
    h_preprocessor_condition_t* stack;
    h_preprocessor_condition_t* stack_top;
    size_t size;
    size_t capacity;
} h_preprocessor_conditions_stack_t;

h_preprocessor_conditions_stack_t* h_preprocessor_conditions_stack_init(size_t capacity);
int h_preprocessor_conditions_stack_push(h_preprocessor_conditions_stack_t* conditions_stack, h_preprocessor_directive_t directive, int output_enabled);
int h_preprocessor_conditions_stack_update(h_preprocessor_conditions_stack_t* conditions_stack, h_preprocessor_directive_t directive, int reverse_output);
h_preprocessor_condition_t h_preprocessor_conditions_stack_pop(h_preprocessor_conditions_stack_t* conditions_stack);
void h_preprocessor_conditions_stack_print(h_preprocessor_conditions_stack_t* stack);
void h_preprocessor_conditions_stack_free(h_preprocessor_conditions_stack_t* stack);

#endif