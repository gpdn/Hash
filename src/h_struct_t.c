#include "h_struct_t.h"

#define H_DEFAULT_STRUCT_FIELDS_COUNT 5

h_struct_t* h_struct_init(h_string_t* name) {
    h_struct_t* new_struct = (h_struct_t*)malloc(sizeof(h_struct_t));
    new_struct->name = name;
    new_struct->fields = (h_struct_field_t*)calloc(H_DEFAULT_STRUCT_FIELDS_COUNT, sizeof(h_struct_field_t));
    new_struct->size = 0;
    new_struct->capacity = H_DEFAULT_STRUCT_FIELDS_COUNT;
    return new_struct;
}

void h_struct_field_add(h_struct_t* data, h_string_t* name, value_type_t type) {
    if(data->size >= data->capacity) {
        data->capacity *= 2;
        data->fields = (h_struct_field_t*)realloc(data->fields, sizeof(h_struct_field_t) * data->capacity);
    }   
    data->fields[data->size++] = (h_struct_field_t){name, type};
}

void h_struct_print(h_struct_t* data) {
    if(!data) return;
    printf("data: %s", data->name->string);
    for(size_t i = 0; i < data->size; ++i) {
        printf("\t%s %s", data->fields[i].name->string, resolve_value_type(data->fields[i].value));
    }
    printf("\n");
}

void h_struct_free(h_struct_t* data) {
    free(data->fields);
    free(data);
}

#undef H_DEFAULT_STRUCT_FIELDS_COUNT