#include "h_generic_arguments_list.h"

#define H_DEFAULT_GENERIC_ARGUMENTS_LIST_CAPACITY 5

h_generic_arguments_list_t* h_generic_arguments_list_init() {
    h_generic_arguments_list_t* list = (h_generic_arguments_list_t*)malloc(sizeof(h_generic_arguments_list_t));
    list->size = 0;
    list->capacity = H_DEFAULT_GENERIC_ARGUMENTS_LIST_CAPACITY;
    list->arguments = (value_t*)malloc(sizeof(value_t) * H_DEFAULT_GENERIC_ARGUMENTS_LIST_CAPACITY);
    return list;
}

void h_generic_arguments_list_add(h_generic_arguments_list_t* list, value_t value) {
    if(list->size >= list->capacity) {
        list->capacity <<= 1;
        list->arguments = (value_t*)realloc(list->arguments, sizeof(value_t) * list->capacity);
    }
    list->arguments[list->size++] = value;
}

value_t h_generic_arguments_list_get(h_generic_arguments_list_t* list, size_t index) {
    return list->arguments[index];
}

void h_generic_arguments_list_print(h_generic_arguments_list_t* list) {
    for(size_t i = 0; i < list->size; ++i) {
        DEBUG_LOG("%lld - ", i);
        //print_value_no_newline(&list->arguments[i]);
    }
}

void h_generic_arguments_list_free(h_generic_arguments_list_t* list) {
    free(list->arguments);
    free(list);
}

#undef H_DEFAULT_GENERIC_ARGUMENTS_LIST_CAPACITY