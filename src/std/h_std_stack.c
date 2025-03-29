
#include "h_std_stack.h"

#define H_STD_DEFAULT_STACK_CAPACITY 64

typedef struct h_stack_t {
    value_t* data;
    value_t* top;
    size_t capacity;
} h_stack_t;

static inline h_data_t* h_stack_data_make();
static void h_stack_init_data_placeholder();

static h_string_t* stack_string_placeholder;
static value_t stack_data_placeholder;

static void h_stack_init_data_placeholder() {
    stack_string_placeholder = h_string_init_hash("Stack", strlen("Stack"));
    stack_data_placeholder = (value_t){.type = H_VALUE_TYPE, .data_type = h_stack_data_make(NULL)};
}

static inline h_data_t* h_stack_data_make(h_stack_t* stack_data) {
    h_data_t* data = h_data_init(0);
    data->type_name = stack_string_placeholder;
    data->other = stack_data;
    data->size = 0;
    return data;
}

value_t h_std_stack_init(struct value_t* parameters, size_t args_count) {
    h_stack_t* stack = (h_stack_t*)malloc(sizeof(h_stack_t));
    stack->data = (value_t*)malloc(sizeof(value_t) * H_STD_DEFAULT_STACK_CAPACITY);
    stack->top = stack->data;
    stack->capacity = H_STD_DEFAULT_STACK_CAPACITY;
    return VALUE_TYPE(h_stack_data_make(stack));
}

value_t h_std_stack_push(struct value_t* parameters, size_t args_count) {
    h_stack_t* stack = (h_stack_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    value_t value = parameters[1];
    if((size_t)(stack->top - stack->data) >= stack->capacity) {
        stack->capacity <<= 2;
        stack->data = (value_t*)realloc(stack->data, sizeof(value_t) * stack->capacity);
    }
    *stack->top++ = value;
    return NUM_VALUE(1);
}

value_t h_std_stack_pop(struct value_t* parameters, size_t args_count) {
    h_stack_t* stack = (h_stack_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    if(stack->top == stack->data) return NULL_VALUE();
    return *(--stack->top);
}

value_t h_std_stack_clear(struct value_t* parameters, size_t args_count) {
    h_stack_t* stack = (h_stack_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    stack->top = stack->data;
    return NULL_VALUE();
}

value_t h_std_stack_size(struct value_t* parameters, size_t args_count) {
    h_stack_t* stack = (h_stack_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    return NUM_VALUE(stack->top - stack->data);
}

value_t h_std_stack_print(struct value_t* parameters, size_t args_count) {
    h_stack_t* stack = (h_stack_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    value_t* it = stack->data;
    for(; it != stack->top; ++it) {
        print_value(it);
    }
    return NULL_VALUE();
}

int h_std_stack_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table) {
    h_stack_init_data_placeholder();
    define_native_data(types_table, "Stack", NULL, 0);
    define_native(stack, "stack_init", h_std_stack_init, NULL, 0, stack_data_placeholder);
    define_native(stack, "stack_push", h_std_stack_push, (value_t[]){stack_data_placeholder, H_NFI_VALUE(H_VALUE_GENERIC)}, 2, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "stack_pop", h_std_stack_pop, (value_t[]){stack_data_placeholder}, 1, H_NFI_VALUE(H_VALUE_GENERIC));
    define_native(stack, "stack_clear", h_std_stack_clear, (value_t[]){stack_data_placeholder}, 1, H_NFI_VALUE(H_VALUE_NULL));
    define_native(stack, "stack_size", h_std_stack_size, (value_t[]){stack_data_placeholder}, 1, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "stack_print", h_std_stack_print, (value_t[]){stack_data_placeholder}, 1, H_NFI_VALUE(H_VALUE_NULL));
    return 1;
}

#undef H_STD_DEFAULT_STACK_CAPACITY