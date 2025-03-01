#ifndef HASH_ARGS_PARSER
#define HASH_ARGS_PARSER

#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "h_error_codes.h"
#include "h_args_flags.h"

typedef enum execution_mode_t {
    MODE_FILE, 
    MODE_REPL
} execution_mode_t;

void parse_args(int argc, char** argv, execution_mode_t* run_mode, const char** source_file_path, const char** repl_save_file_path, uint8_t* args_flags);

#endif