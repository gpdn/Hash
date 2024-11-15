#ifndef H_PREPROCESSOR_FILES_SET
#define H_PREPROCESSOR_FILES_SET

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../debug.h"

typedef struct h_files_set_t {
    size_t elements_count;
    size_t capacity;
    int* array;
    float load_factor;
} h_files_set_t;

h_files_set_t* h_files_set_init(size_t capacity, float load_factor);
int h_files_set_push(h_files_set_t* table, const char* key);
int h_files_set_get(h_files_set_t* table, const char* key);
void h_files_set_free(h_files_set_t* hash_table);

#endif