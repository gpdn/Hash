#ifndef H_GENERIC_PARAMETERS_LIST
#define H_GENERIC_PARAMETERS_LIST

#include <stdlib.h>

typedef struct h_generic_parameters_list_t {
    char* names;
    size_t size;
    size_t capacity;
} h_generic_parameters_list_t;

h_generic_parameters_list_t* h_generic_parameters_list_init();
void h_generic_parameters_list_add(h_generic_parameters_list_t* list, char name);
int h_generic_parameters_list_get_index(h_generic_parameters_list_t* list, char name);
void h_generic_parameters_list_print(h_generic_parameters_list_t* list);
void h_generic_parameters_list_free(h_generic_parameters_list_t* list);

#endif