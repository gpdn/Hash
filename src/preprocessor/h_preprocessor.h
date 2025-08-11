#ifndef H_PREPROCESSOR
#define H_PREPROCESSOR

#define RELATIVE_PATH ../
#define MAKE_PATH(PATH) RELATIVE_PATH##PATH

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "../debug.h"
#include "../h_file.h"
#include "h_ht_files_set.h"
#include "h_ht_preprocessor_env.h"
#include "h_preprocessor_directive.h"
#include "h_preprocessor_conditions_stack.h"
#include "../std/h_std.h"

#undef RELATIVE_PATH
#undef MAKE_PATH

typedef enum preprocessor_mode_t {
    H_PREPROCESSOR_MODE_DEBUG,
    H_PREPROCESSOR_MODE_RELEASE
} preprocessor_mode_t;

typedef struct h_preprocessor_t {
    const char* start;
    const char* current;
    size_t line;
    const char** files_list;
    size_t files_list_size;
    size_t files_list_capacity;
    h_files_set_t* files_set;
    FILE* out_file;
    const char* file_buffer[1024];
    size_t file_buffer_size;
    h_preprocessor_env_t* environment;
    h_preprocessor_conditions_stack_t* conditions_stack;
    int output_enabled;
    int ignore_input;
    h_std_t* std;
    h_string_t* executable_path;
    const char* out_file_name;
    preprocessor_mode_t mode;
} h_preprocessor_t;

h_preprocessor_t* preprocessor_init(const char* source_path, const char* out_file_name, h_std_t* std);
int preprocessor_run(h_preprocessor_t* preprocessor);
void preprocessor_destroy(h_preprocessor_t* preprocessor);

#endif 