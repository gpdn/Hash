#include "h_std_array.h"

value_t h_std_array_len(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE(H_NFI_ARR_LEN(parameters[0]));
}

value_t h_std_array_push(struct value_t* parameters, size_t args_count) {
    h_array_t* array = H_NFI_ARR(parameters[0]);
    h_array_push(array, parameters[1]);
    return NUM_VALUE(H_NFI_ARR_LEN(parameters[0]));
}

value_t h_std_array_pop(struct value_t* parameters, size_t args_count) {
    h_array_t* array = H_NFI_ARR(parameters[0]);
    if(array->size == 0) return NULL_VALUE(); 
    value_t value = h_array_get(array, --array->size);
    return value;
}

value_t h_std_array_find_first(struct value_t* parameters, size_t args_count) {
    h_array_t* array = H_NFI_ARR(parameters[0]);
    value_t value_to_find = parameters[1];
    for(size_t i = 0; i < array->size; ++i) {
        if(compare_value(array->data[i], value_to_find)) return NUM_VALUE(i);
    }
    return NUM_VALUE(-1);
}

value_t h_std_array_find_last(struct value_t* parameters, size_t args_count) {
    h_array_t* array = H_NFI_ARR(parameters[0]);
    value_t value_to_find = parameters[1];
    for(size_t i = array->size - 1; i >= 0; --i) {
        if(compare_value(array->data[i], value_to_find)) return NUM_VALUE(i);
    }
    return NUM_VALUE(-1);
}

value_t h_std_array_find_all(struct value_t* parameters, size_t args_count) {
    h_array_t* array = H_NFI_ARR(parameters[0]);
    value_t value_to_find = parameters[1];
    h_array_t* new_array = h_array_init(H_VALUE_NUMBER, array->capacity);
    for(size_t i = 0; i < array->size; ++i) {
        if(compare_value(array->data[i], value_to_find)) h_array_push(new_array, NUM_VALUE(i));
    }
    return VALUE_ARRAY(new_array);
}

value_t h_std_array_replace_first(struct value_t* parameters, size_t args_count) {
    h_array_t* array = H_NFI_ARR(parameters[0]);
    value_t value_to_find = parameters[1];
    value_t value_to_replace = parameters[2];
    for(size_t i = 0; i < array->size; ++i) {
        if(compare_value(array->data[i], value_to_find)) {
            array->data[i] = value_to_replace;
            return NUM_VALUE(i);
        }
    }
    return NUM_VALUE(-1);
}

value_t h_std_array_replace_last(struct value_t* parameters, size_t args_count) {
    h_array_t* array = H_NFI_ARR(parameters[0]);
    value_t value_to_find = parameters[1];
    value_t value_to_replace = parameters[2];
    for(size_t i = array->size - 1; i >= 0; --i) {
        if(compare_value(array->data[i], value_to_find)) {
            array->data[i] = value_to_replace;
            return NUM_VALUE(i);
        }
    }
    return NUM_VALUE(-1);
}

value_t h_std_array_replace_all(struct value_t* parameters, size_t args_count) {
    h_array_t* array = H_NFI_ARR(parameters[0]);
    value_t value_to_find = parameters[1];
    value_t value_to_replace = parameters[2];
    h_array_t* new_array = h_array_init(H_VALUE_NUMBER, array->capacity);
    for(size_t i = 0; i < array->size; ++i) {
        if(compare_value(array->data[i], value_to_find)) {
            array->data[i] = value_to_replace;
            h_array_push(new_array, NUM_VALUE(i));
        }
    }
    return VALUE_ARRAY(new_array);
}

value_t h_std_array_shift(struct value_t* parameters, size_t args_count) {
    h_array_t* array = H_NFI_ARR(parameters[0]);
    if(array->size == 0) return NULL_VALUE();
    value_t value_to_return = array->data[0];
    memcpy(array->data, array->data + 1, --array->size * sizeof(value_t));
    return value_to_return;
}

value_t h_std_array_unshift(struct value_t* parameters, size_t args_count) {
    h_array_t* array = H_NFI_ARR(parameters[0]);
    value_t value_to_add = parameters[1];
    if(array->size >= array->capacity) {
        array->capacity *= 2;
        array->data = (value_t*)realloc(array->data, sizeof(value_t) * array->capacity);
    }
    if(array->size > 0) {
        memcpy(array->data + 1, array->data, array->size++ * sizeof(value_t));
    }
    array->data[0] = value_to_add;
    return NUM_VALUE(1);
}

value_t h_std_array_combine(struct value_t* parameters, size_t args_count) {
    h_array_t* array = H_NFI_ARR(parameters[0]);
    h_array_t* array_2 = H_NFI_ARR(parameters[1]);
    h_array_t* new_array = h_array_init(array->type, array->capacity + array_2->capacity);
    new_array->size = array->size + array_2->size;
    memcpy(new_array->data, array->data, array->size * sizeof(value_t));
    memcpy(new_array->data + array->size, array_2->data, array_2->size * sizeof(value_t));
    return VALUE_ARRAY(new_array);
}

value_t h_std_array_insert(struct value_t* parameters, size_t args_count) {
    h_array_t* array = H_NFI_ARR(parameters[0]);
    value_t value_to_insert = parameters[1];
    int index = H_NFI_NUM_TO_CINT(parameters[2]);
    if(index > array->size) {
        h_array_push(array, value_to_insert);
        return NUM_VALUE(array->size - 1);
    }
    memcpy(array->data + index + 1, array->data + index, (array->size - index - 1) * sizeof(value_t));
    return NUM_VALUE(index);
}

value_t h_std_array_remove_first(struct value_t* parameters, size_t args_count) {
    h_array_t* array = H_NFI_ARR(parameters[0]);
    value_t value_to_remove = parameters[1];
    for(size_t i = 0; i < array->size; ++i) {
        if(compare_value(array->data[i], value_to_remove)) {
            memcpy(array->data + i, array->data + i + 1, (--array->size - i) * sizeof(value_t));
            return NUM_VALUE(i);
        }
    }
    return NUM_VALUE(-1);
}

value_t h_std_array_remove_last(struct value_t* parameters, size_t args_count) {
    h_array_t* array = H_NFI_ARR(parameters[0]);
    value_t value_to_remove = parameters[1];
    for(size_t i = array->size - 1; i > 0; --i) {
        if(compare_value(array->data[i], value_to_remove)) {
            memcpy(array->data + i, array->data + i + 1, (--array->size - i) * sizeof(value_t));
            return NUM_VALUE(i);
        }
    }
    return NUM_VALUE(-1);
}

value_t h_std_array_remove_all(struct value_t* parameters, size_t args_count) {
    h_array_t* array = H_NFI_ARR(parameters[0]);
    value_t value_to_remove = parameters[1];
    h_array_t* new_array = h_array_init(H_VALUE_NUMBER, array->capacity);
    for(size_t i = 0; i < array->size; ++i) {
        if(compare_value(array->data[i], value_to_remove)) {
            memcpy(array->data + i, array->data + i + 1, (--array->size - i) * sizeof(value_t));
            h_array_push(new_array, NUM_VALUE(i));
        }
    }
    return VALUE_ARRAY(new_array);
}

value_t h_std_array_compare(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE(compare_value(parameters[0], parameters[1]));
}

int h_std_array_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table) {
    define_native(stack, "array_size", h_std_array_len, (value_t[]){H_NFI_VALUE(H_VALUE_ARRAY)}, 1, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "array_push", h_std_array_push, (value_t[]){H_NFI_VALUE(H_VALUE_ARRAY), H_NFI_VALUE(H_VALUE_GENERIC)}, 2, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "array_pop", h_std_array_pop, (value_t[]){H_NFI_VALUE(H_VALUE_ARRAY)}, 1, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "array_find_first", h_std_array_find_first, (value_t[]){H_NFI_VALUE(H_VALUE_ARRAY), H_NFI_VALUE(H_VALUE_GENERIC)}, 2, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "array_find_last", h_std_array_find_last, (value_t[]){H_NFI_VALUE(H_VALUE_ARRAY), H_NFI_VALUE(H_VALUE_GENERIC)}, 2, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "array_find_all", h_std_array_find_all, (value_t[]){H_NFI_VALUE(H_VALUE_ARRAY), H_NFI_VALUE(H_VALUE_GENERIC)}, 2, H_NFI_VALUE(H_VALUE_ARRAY));
    define_native(stack, "array_replace_first", h_std_array_replace_first, (value_t[]){H_NFI_VALUE(H_VALUE_ARRAY), H_NFI_VALUE(H_VALUE_GENERIC), H_NFI_VALUE(H_VALUE_GENERIC)}, 3, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "array_replace_last", h_std_array_replace_last, (value_t[]){H_NFI_VALUE(H_VALUE_ARRAY), H_NFI_VALUE(H_VALUE_GENERIC), H_NFI_VALUE(H_VALUE_GENERIC)}, 3, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "array_replace_all", h_std_array_replace_all, (value_t[]){H_NFI_VALUE(H_VALUE_ARRAY), H_NFI_VALUE(H_VALUE_GENERIC), H_NFI_VALUE(H_VALUE_GENERIC)}, 3, H_NFI_VALUE(H_VALUE_ARRAY));
    define_native(stack, "array_shift", h_std_array_shift, (value_t[]){H_NFI_VALUE(H_VALUE_ARRAY)}, 1, H_NFI_VALUE(H_VALUE_GENERIC));
    define_native(stack, "array_unshift", h_std_array_unshift, (value_t[]){H_NFI_VALUE(H_VALUE_ARRAY), H_NFI_VALUE(H_VALUE_GENERIC)}, 2, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "array_combine", h_std_array_combine, (value_t[]){H_NFI_VALUE(H_VALUE_ARRAY), H_NFI_VALUE(H_VALUE_ARRAY)}, 2, H_NFI_VALUE(H_VALUE_ARRAY));
    define_native(stack, "array_insert", h_std_array_insert, (value_t[]){H_NFI_VALUE(H_VALUE_ARRAY), H_NFI_VALUE(H_VALUE_GENERIC), H_NFI_VALUE(H_VALUE_NUMBER)}, 3, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "array_remove_first", h_std_array_remove_first, (value_t[]){H_NFI_VALUE(H_VALUE_ARRAY), H_NFI_VALUE(H_VALUE_GENERIC)}, 2, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "array_remove_last", h_std_array_remove_last, (value_t[]){H_NFI_VALUE(H_VALUE_ARRAY), H_NFI_VALUE(H_VALUE_GENERIC)}, 2, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "array_remove_all", h_std_array_remove_all, (value_t[]){H_NFI_VALUE(H_VALUE_ARRAY), H_NFI_VALUE(H_VALUE_GENERIC)}, 2, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "array_compare", h_std_array_compare, (value_t[]){H_NFI_VALUE(H_VALUE_ARRAY), H_NFI_VALUE(H_VALUE_ARRAY)}, 2, H_NFI_VALUE(H_VALUE_NUMBER));
    return 1;
}