#include "h_locals_stack.h"

h_locals_stack_t* h_locals_stack_init(size_t capacity) {
    h_locals_stack_t* stack = (h_locals_stack_t*)malloc(sizeof(h_locals_stack_t));
    stack->capacity = capacity;
    stack->size = 0;
    stack->locals_array = (h_local_t*)calloc(capacity, sizeof(h_local_t));
    stack->locals_stack_top = stack->locals_array;
    stack->locals_stack_it = stack->locals_array;
    return stack;
}

void h_local_stack_push(h_locals_stack_t* locals_stack, h_local_t local) {
    *locals_stack->locals_stack_top = local;
    ++locals_stack->locals_stack_top;
}

value_t h_local_stack_pop(h_locals_stack_t* locals_stack) {
    return (--locals_stack->locals_stack_top)->value;
}

value_t h_local_stack_peek(h_locals_stack_t* locals_stack) {
    return (locals_stack->locals_stack_top - 1)->value;
}

void h_locals_stack_free(h_locals_stack_t* locals_stack) {
    free(locals_stack->locals_array);
    free(locals_stack);
}