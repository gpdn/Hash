#ifndef H_ARENA_T
#define H_ARENA_T

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "h_memory_type.h"

typedef struct h_arena_t {
    void* memory;
    char* start;
    char* current;
    char* end;
    struct h_arena_t* next;
    h_memory_type_t tag;
} h_arena_t;

h_arena_t* h_arena_alloc(size_t capacity, h_memory_type_t tag);
h_arena_t* h_arena_calloc(size_t capacity, h_memory_type_t tag);
void* h_arena_realloc(h_arena_t* arena, void* block, size_t prev_capacity, size_t new_capacity, int* new_size);
void h_arena_reset(h_arena_t* arena);
void* h_arena_get_memory(h_arena_t* arena, size_t capacity, int* new_size);
void h_arena_free(h_arena_t* arena);

#endif