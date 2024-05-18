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

void h_locals_stack_push(h_locals_stack_t* locals_stack, h_string_t* name, value_t value, size_t scope) {
    if(locals_stack->size >= locals_stack->capacity) {
        locals_stack->capacity *= 2;
        locals_stack->locals_array = (h_local_t*)realloc(locals_stack->locals_array, sizeof(h_local_t) * locals_stack->capacity);
    }
    ++locals_stack->size;
    *locals_stack->locals_stack_top = (h_local_t){name, .value = value, scope};
    ++locals_stack->locals_stack_top;
}

value_t h_locals_stack_pop(h_locals_stack_t* locals_stack) {
    return (--locals_stack->locals_stack_top)->value;
}

value_t h_locals_stack_peek(h_locals_stack_t* locals_stack) {
    return (locals_stack->locals_stack_top - 1)->value;
}

value_t h_locals_stack_get(h_locals_stack_t* locals_stack, h_string_t* name, size_t scope) {
    h_local_t* it = locals_stack->locals_stack_top - 1;
    for(; it->name->hash != name->hash && it->scope <= scope && it != locals_stack->locals_array; --it);
    return it->value;
}

size_t h_locals_stack_get_index(h_locals_stack_t* locals_stack, h_string_t* name) {
    h_local_t* it = locals_stack->locals_stack_top - 1;
    for(; it->name->hash != name->hash && it != locals_stack->locals_array; --it);
    return it - locals_stack->locals_array;
}

value_t h_locals_array_get(h_locals_stack_t* locals_stack, size_t index) {
    return locals_stack->locals_array[index].value;
}

value_t h_locals_array_get_index(h_locals_stack_t* locals_stack, size_t index, size_t element_index) {
    return h_array_get(locals_stack->locals_array[index].value.array, element_index);
}

void h_locals_array_set_index(h_locals_stack_t* locals_stack, size_t index, size_t element_index, value_t value) {
    h_array_set(locals_stack->locals_array[index].value.array, element_index, value);
}

void h_locals_array_set(h_locals_stack_t* locals_stack, size_t index, value_t value) {
    locals_stack->locals_array[index].value = value; 
}

void h_print_local(h_local_t* local) {
    printf("Name: %s - Value: ", local->name->string);
    print_value_no_newline(&local->value);
    printf(" - Scope: %lld\n", local->scope);
}

value_t h_locals_array_increase_get(h_locals_stack_t* locals_stack, size_t index) {
    ++locals_stack->locals_array[index].value.number;
    return locals_stack->locals_array[index].value;
}

value_t h_locals_array_decrease_get(h_locals_stack_t* locals_stack, size_t index) {
    --locals_stack->locals_array[index].value.number;
    return locals_stack->locals_array[index].value;
}

value_t h_locals_array_post_increase_get(h_locals_stack_t* locals_stack, size_t index) {
    value_t value = locals_stack->locals_array[index].value;
    ++locals_stack->locals_array[index].value.number;
    return value;
}

value_t h_locals_array_post_decrease_get(h_locals_stack_t* locals_stack, size_t index) {
    value_t value = locals_stack->locals_array[index].value;
    --locals_stack->locals_array[index].value.number;
    return value;
}

void h_locals_stack_print(h_locals_stack_t* locals_stack) {
    printf("Size: %lld\n", locals_stack->size);
    printf("0 - Name: - Value: NULL(0) - Scope: 0\n");
    for(size_t i = 1; i < locals_stack->size; ++i) {
        printf("%lld - ", i);
        h_print_local(locals_stack->locals_array + i);
    }
}

void h_locals_stack_free(h_locals_stack_t* locals_stack) {
    free(locals_stack->locals_array);
    free(locals_stack);
}