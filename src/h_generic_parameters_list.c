#include "h_generic_parameters_list.h"
#include "debug.h"

#define H_DEFAULT_GENERIC_PARAMETERS_LIST_CAPACITY 5

h_generic_parameters_list_t* h_generic_parameters_list_init() {
    h_generic_parameters_list_t* list = (h_generic_parameters_list_t*)malloc(sizeof(h_generic_parameters_list_t));
    list->size = 0;
    list->capacity = H_DEFAULT_GENERIC_PARAMETERS_LIST_CAPACITY;
    list->names = (char*)malloc(sizeof(char) * H_DEFAULT_GENERIC_PARAMETERS_LIST_CAPACITY);
    return list;
}

void h_generic_parameters_list_add(h_generic_parameters_list_t* list, char name) {
    if(list->size >= list->capacity) {
        list->capacity <<= 1;
        list->names = (char*)realloc(list->names, sizeof(char) * list->capacity);
    }
    list->names[list->size++] = name;
}

int h_generic_parameters_list_get_index(h_generic_parameters_list_t* list, char name) {
    for(size_t i = 0; i < list->size; ++i) {
        if(list->names[i] == name) return (int)i;
    }
    return -1;
}

void h_generic_parameters_list_print(h_generic_parameters_list_t* list) {
    for(size_t i = 0; i < list->size; ++i) {
        DEBUG_LOG("%lld - %c\n", i, list->names[i]);
    }
}

void h_generic_parameters_list_free(h_generic_parameters_list_t* list) {
    free(list->names);
    free(list);
}

#undef H_DEFAULT_GENERIC_PARAMETERS_LIST_CAPACITY