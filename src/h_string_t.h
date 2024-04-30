#ifndef H_STRING_H
#define H_STRING_H

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct h_string_t {
    char* string;
    uint32_t hash;
    size_t length;
    size_t capacity;
} h_string_t;

h_string_t* h_string_init(const char* string, size_t length);
h_string_t* h_string_init_hash(const char* string, size_t length);
uint8_t h_string_compare(h_string_t* string_1, h_string_t* string_2);
h_string_t* h_string_concatenate(h_string_t* string_1, h_string_t* string_2);
char h_string_get(h_string_t* string, size_t index);
void h_string_set(h_string_t* string, char c, size_t index);
h_string_t* h_string_substr(h_string_t* string, size_t start, size_t offset);
void h_string_free(h_string_t* string);

#endif