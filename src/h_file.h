#ifndef HASH_FILE_H
#define HASH_FILE_H

#include <stdio.h>
#include <stdlib.h>
#include "h_error_codes.h"
#include "debug.h"

const char* read_file(const char* source);
size_t write_file(const char* path, const char* content, size_t element_size, size_t elements_count);
FILE* create_file(const char* path, const char* mode);
int close_file(FILE* file);
size_t append_file(FILE* path, const char* content, size_t element_size, size_t elements_count);
size_t append_file_str(const char* path, const char* content, size_t element_size, size_t elements_count);

#endif