#include "h_data_t.h"

h_data_t* h_data_init(size_t capacity) {
    h_data_t* data = (h_data_t*)malloc(sizeof(h_data_t));
    data->size = 0;
    data->capacity = capacity;
    data->data = (value_t*)calloc(capacity, sizeof(value_t));
    return data;
}

void h_data_push(h_data_t* data, value_t value) {
    if(data->size >= data->capacity) {
        data->capacity *= 2;
        data->data = (value_t*)realloc(data->data, sizeof(value_t) * data->capacity);
    }
    data->data[data->size++] = value;
}

value_t h_data_get(h_data_t* data, size_t index) {
    return data->data[index];
}

void h_data_set(h_data_t* data, size_t index, value_t value) {
    data->data[index] = value;
}

void h_data_print(h_data_t* data) {
    if(data->size == 0) return;
    for(size_t i = 0; i < data->size; ++i) print_value(data->data + i);
}

void h_data_print_no_newline(h_data_t* data) {
    if(data->size == 0) return;
    for(size_t i = 0; i < data->size - 1; ++i) { 
        print_value_no_newline(data->data + i);
        printf(", ");
    }
    print_value_no_newline(data->data + data->size - 1);
}

void h_data_free(h_data_t* data) {
    free(data->data);
}