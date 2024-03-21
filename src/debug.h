
#ifndef HASH_DEBUG_H
#define HASH_DEBUG_H

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#define COLOR_WHITE   "\e[0;37m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"

#if DEBUG
    #define DEBUG_LOG(...) printf(__VA_ARGS__)
    #define DEBUG_COLOR(X, Y) printf("%s%s%s\n", X, Y, COLOR_RESET)
    #define DEBUG_COLOR_SET(X) printf("%s\n", X)
    #define DEBUG_COLOR_RESET(X) DEBUG_COLOR_SET(COLOR_RESET)
    #define DEBUG_PRINT(X) DEBUG_COLOR(COLOR_WHITE, X)
    #define DEBUG_ERROR(X) DEBUG_COLOR(COLOR_RED, X)
    #define DEBUG_WARNING(X) DEBUG_COLOR(COLOR_YELLOW, X)
    #define DEBUG_INFO(X) DEBUG_COLOR(COLOR_CYAN, X)
    #define DEBUG_SUCCESS(X) DEBUG_COLOR(COLOR_GREEN, X)
    #define DEBUG_CLEAR() clrscr();
    #define DEBUG_TITLE(X) printf("--- %s ---\n", X)
#else
    #define DEBUG_LOG(X)
    #define DEBUG_COLOR(X, Y)
    #define DEBUG_PRINT(X)
    #define DEBUG_ERROR(X)
    #define DEBUG_WARNING(X)
    #define DEBUG_INFO(X)
    #define DEBUG_SUCCESS(X)
    #define DEBUG_CLEAR()
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
    void disassemble_bytecode_store(bytecode_store_t* store, const char* name);
    size_t disassemble_instruction(bytecode_store_t* store, size_t offset);
#endif

#endif