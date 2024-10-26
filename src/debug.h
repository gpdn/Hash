
#ifndef HASH_DEBUG_H
#define HASH_DEBUG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define COLOR_WHITE "\e[0;37m"
#define COLOR_RED "\x1b[31m"
#define COLOR_GRAY "\x1b[30m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN "\x1b[36m"
#define COLOR_RESET "\x1b[0m"

#define DEBUG_MIN(X, Y) ((X < Y) ? (X) : (Y))

#define DEBUG 1
#define DEBUG_ALL 1

#if DEBUG
#define DEBUG_LOG(...) printf(__VA_ARGS__)
#define DEBUG_COLOR(X, Y) printf("%s%s%s\n", X, Y, COLOR_RESET)
#define DEBUG_COLOR_SET(X) printf("%s", X)
#define DEBUG_COLOR_RESET(X) DEBUG_COLOR_SET(COLOR_RESET)
#define DEBUG_PRINT(X) DEBUG_COLOR(COLOR_WHITE, X)
#define DEBUG_ERROR(...) DEBUG_COLOR_SET(COLOR_RED); DEBUG_LOG(__VA_ARGS__); DEBUG_COLOR_RESET()
#define DEBUG_WARNING(X) DEBUG_COLOR(COLOR_YELLOW, X)
#define DEBUG_INFO(X) DEBUG_COLOR(COLOR_CYAN, X)
#define DEBUG_SUCCESS(X) DEBUG_COLOR(COLOR_GREEN, X)
#define DEBUG_CLEAR() clrscr();
#define DEBUG_TITLE(X) printf("--- %s ---\n", X)
#define DEBUG_PRINT_LINE(X) printf("\n")
#define DEBUG_BINARY_8(X)\
    ((X) & (1 << 7) ? '1' : '0'),\
    ((X) & (1 << 6) ? '1' : '0'),\
    ((X) & (1 << 5) ? '1' : '0'),\
    ((X) & (1 << 4) ? '1' : '0'),\
    ((X) & (1 << 3) ? '1' : '0'),\
    ((X) & (1 << 2) ? '1' : '0'),\
    ((X) & (1 << 1) ? '1' : '0'),\
    ((X) & (1 << 0) ? '1' : '0')
#define DEBUG_LOG_BINARY_8(X) DEBUG_LOG("%c%c%c%c%c%c%c%c\n", DEBUG_BINARY_8(X));
#else
#define DEBUG_LOG(...)
#define DEBUG_COLOR(X, Y)
#define DEBUG_PRINT(X)
#define DEBUG_ERROR(X)
#define DEBUG_WARNING(X)
#define DEBUG_INFO(X)
#define DEBUG_SUCCESS(X)
#define DEBUG_CLEAR()
#define DEBUG_TITLE(X)
#define DEBUG_PRINT_LINE(X)
#define DEBUG_COLOR_SET(X)
#define DEBUG_COLOR_RESET(X)
#endif

#if PROFILE
#include "timer.h"
#define TIMER_START(X) start_timer(X)
#define TIMER_END(X) end_timer(X)
#define TIMER_GET_ELAPSED(X) get_time_elapsed(X)
#else
#define TIMER_START(X)
#define TIMER_END(X)
#define TIMER_GET_ELAPSED(X)
#endif

#if DEBUG
#include "bytecode_store.h"
#include "h_token.h"
#define ECHO(X)        \
    printf("%s\n", X); \
    return X;
void disassemble_bytecode_store(bytecode_store_t *store, const char *name, FILE *file);
size_t disassemble_instruction(bytecode_store_t *store, size_t offset, FILE *file);
void token_print(token_t *token);
size_t token_write_print_string(FILE *file, token_t *token);
const char* resolve_token_type(token_type_t type);
#endif

#if DEBUG_ALL
    #define DEBUG_TRACE_VM_BYTECODE 1
    #define DEBUG_TRACE_VM_STACK 1
    #define DEBUG_TRACE_LEXER_TOKEN 1
    #define DEBUG_TRACE_LEXER_CURRENT_CHAR 1
    #define DEBUG_TRACE_LOG_FILE 1
    #define DEBUG_TRACE_LOG_COMMAND_LINE_ARGS 1
    #define DEBUG_TRACE_LOG_COMMAND_LINE_ARGS_OPTS 1
    #define DEBUG_TRACE_LEXER_PRINT_SKIPPED 1
    #define DEBUG_TRACE_PARSER_AST 1
    #define DEBUG_TRACE_ICG_BYTECODE 1

    #define DEBUG_TRACE_LOCALS_STACK 0
    #define DEBUG_TRACE_TYPES_TABLE 0

    #define DEBUG_FILE_LEXER_TOKEN 1
    #define DEBUG_FILE_BYTECODE 1
    #define DEBUG_FILE_PARSER_AST 1

    #define DEBUG_FOLDER_PATH "debug_files/"
    #define DEBUG_FILE_LEXER_TOKEN_PATH DEBUG_FOLDER_PATH "tokens"
    #define DEBUG_FILE_BYTECODE_PATH DEBUG_FOLDER_PATH "bytecode"
    #define DEBUG_FILE_PARSER_AST_PATH DEBUG_FOLDER_PATH "parser_ast"
#endif

#endif