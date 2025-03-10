#include "h_struct_t.h"

#define H_DEFAULT_STRUCT_FIELDS_COUNT 5

h_struct_t* h_struct_init(h_string_t* name) {
    h_struct_t* new_struct = (h_struct_t*)malloc(sizeof(h_struct_t));
    new_struct->name = name;
    new_struct->field_names = (h_string_t*)calloc(H_DEFAULT_STRUCT_FIELDS_COUNT, sizeof(h_string_t));
    new_struct->field_values = (value_t*)calloc(H_DEFAULT_STRUCT_FIELDS_COUNT, sizeof(value_t));
    new_struct->size = 0;
    new_struct->capacity = H_DEFAULT_STRUCT_FIELDS_COUNT;
    new_struct->generics = NULL;
    return new_struct;
}

void h_struct_field_add(h_struct_t* data, h_string_t* name, value_t value) {
    if(data->size >= data->capacity) {
        data->capacity *= 2;
        data->field_names = (h_string_t*)realloc(data->field_names, sizeof(h_string_t) * data->capacity);
        data->field_values = (value_t*)realloc(data->field_values, sizeof(value_t) * data->capacity);
    }
    data->field_names[data->size] = *name;
    data->field_values[data->size++] = value;
}

int h_struct_field_get_index(h_struct_t* data, h_string_t* name) {
    if(data->size == 0) return -1;
    for(size_t i = 0; i < data->size; ++i) {
        if(data->field_names[i].hash == name->hash && data->field_names[i].length == name->length && strcmp(data->field_names[i].string, name->string) == 0) return i;
    }
    return -1;
} 

void h_struct_print(h_struct_t* data) {
    if(!data) return;
    printf("data: %s", data->name->string);
    for(size_t i = 0; i < data->size; ++i) {
        printf("\t%s - ", data->field_names[i].string);
        print_value_no_newline(data->field_values + i);
    }
    printf("\n");
}

void h_struct_free(h_struct_t* data) {
    free(data->field_names);
    free(data->field_values);
    free(data);
}

#undef H_DEFAULT_STRUCT_FIELDS_COUNT