#ifndef H_ARENA_MANAGER_T
#define H_ARENA_MANAGER_T

#include "h_arena_t.h"

typedef struct h_arena_type_list_t {
    h_arena_t* internal_arena;
    h_arena_t** arenas;
    size_t size;
    size_t capacity;
} h_arena_type_list_t;

typedef struct h_arena_manager_t {
    h_arena_t* internal_arena;
    h_arena_type_list_t arenas[H_MEMORY_TAG_LAST];
    unsigned long long int size[H_MEMORY_TAG_LAST];
} h_arena_manager_t;

h_arena_manager_t* h_arena_manager_init();
h_arena_t* h_am_make_arena(h_arena_manager_t* manager, size_t capacity, h_memory_type_t tag);
int h_am_register_arena(h_arena_manager_t* manager, h_arena_t* arena);
int h_am_report_memory_tag(h_arena_manager_t* manager, h_memory_type_t tag);
void h_am_report_memory(h_arena_manager_t* manager);
void* h_am_get_memory(h_arena_manager_t* manager, h_memory_type_t tag, size_t capacity);
void* h_am_reallocate_memory(h_arena_manager_t* manager, h_memory_type_t tag, void* block, size_t prev_capacity, size_t new_capacity);
void h_arena_manager_free(h_arena_manager_t* manager);

#endif