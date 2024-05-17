#include "h_array_t.h"

h_array_t* h_array_init(value_type_t type, size_t capacity) {
    h_array_t* array = (h_array_t*)malloc(sizeof(h_array_t));
    array->size = 0;
    array->capacity = capacity;
    array->data = (value_t*)calloc(capacity, sizeof(value_t));
    array->type = type;
    return array;
}

void h_array_push(h_array_t* array, value_t value) {
    if(array->size >= array->capacity) {
        array->capacity *= 2;
        array->data = (value_t*)realloc(array->data, sizeof(value_t) * array->capacity);
    }
    array->data[array->size++] = value;
}

value_t h_array_get(h_array_t* array, size_t index) {
    return array->data[index];
}

void h_array_set(h_array_t* array, size_t index, value_t value) {
    array->data[index] = value;
}

void h_array_print(h_array_t* array) {
    for(size_t i = 0; i < array->size; ++i) print_value(array->data + i);
}

void h_array_print_no_newline(h_array_t* array) {
    for(size_t i = 0; i < array->size - 1; ++i) { 
        print_value_no_newline(array->data + i);
        printf(", ");
    }
    print_value_no_newline(array->data + array->size - 1);
}

void h_array_free(h_array_t* array) {
    free(array->data);
}