#include "h_preprocessor_conditions_stack.h"

h_preprocessor_conditions_stack_t* h_preprocessor_conditions_stack_init(size_t capacity) {
    h_preprocessor_conditions_stack_t* conditions_stack = (h_preprocessor_conditions_stack_t*)malloc(sizeof(h_preprocessor_conditions_stack_t));
    conditions_stack->size = 0;
    conditions_stack->capacity = capacity;
    conditions_stack->stack = (h_preprocessor_condition_t*)malloc(sizeof(h_preprocessor_condition_t) * capacity);
    conditions_stack->stack_top = conditions_stack->stack;
    return conditions_stack;
}

int h_preprocessor_conditions_stack_push(h_preprocessor_conditions_stack_t* conditions_stack, h_preprocessor_directive_t directive, int output_enabled) {
    if(conditions_stack->size >= conditions_stack->capacity) {
        conditions_stack->capacity *= 2;
        conditions_stack->stack = (h_preprocessor_condition_t*)realloc(conditions_stack->stack, sizeof(h_preprocessor_condition_t) * conditions_stack->capacity);
    }
    *conditions_stack->stack_top++ = (h_preprocessor_condition_t){.directive = directive, .output = output_enabled};
    ++conditions_stack->size;
    return 1;
}

int h_preprocessor_conditions_stack_update(h_preprocessor_conditions_stack_t* conditions_stack, h_preprocessor_directive_t directive, int reverse_output) {
    if(conditions_stack->size == 0) return 0;
    --conditions_stack->stack_top;
    conditions_stack->stack_top->directive = directive;
    conditions_stack->stack_top->output = !conditions_stack->stack_top->output;
    ++conditions_stack->stack_top;
    return 1;
}

h_preprocessor_condition_t h_preprocessor_conditions_stack_pop(h_preprocessor_conditions_stack_t* conditions_stack) {
    if(conditions_stack->size == 0) return (h_preprocessor_condition_t){.directive = H_DIRECTIVE_UNKNOWN, .output = 0};
    --conditions_stack->size;
    return *(--conditions_stack->stack_top);
}

void h_preprocessor_conditions_stack_print(h_preprocessor_conditions_stack_t* conditions_stack) {
    for(h_preprocessor_condition_t* it = conditions_stack->stack; it != conditions_stack->stack_top; ++it) {
        print_preprocessor_directive_no_newline(it->directive);
        DEBUG_LOG("- Output enabled: %d\n", it->output);
    }
}

void h_preprocessor_conditions_stack_free(h_preprocessor_conditions_stack_t* conditions_stack) {
    free(conditions_stack->stack);
    free(conditions_stack);
}