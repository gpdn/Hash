#ifndef HASH_PIPELINE_H
#define HASH_PIPELINE_H

#include "debug.h"
#include "h_file.h"
#include "h_timer.h"
#include "bytecode_store.h"
#include "lexer.h"
#include "h_parser.h"
#include "h_icg.h"
#include "virtual_machine.h"

interpreter_result_t pipeline_start(const char* source_path);

#endif