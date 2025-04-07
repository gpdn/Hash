#ifndef HASH_MEMORY_H
#define HASH_MEMORY_H

#include "memory/h_arena_manager_t.h"

h_arena_manager_t* h_memory_create_manager();
h_arena_t* h_memory_create_arena(size_t capacity, h_memory_type_t tag);
h_arena_t* h_memory_register_arena(h_arena_manager_t* manager, size_t capacity, h_memory_type_t tag);
h_arena_t* h_memory_get_arena(h_arena_manager_t* manager, h_memory_type_t tag);
void h_memory_report(h_arena_manager_t* manager);
int h_memory_report_tag(h_arena_manager_t* manager, h_memory_type_t tag);
void* h_memory_get_block(h_arena_manager_t* manager, h_memory_type_t tag, size_t capacity);
void* h_memory_reallocate_block(h_arena_manager_t* manager, h_memory_type_t tag, void* block, size_t prev_capacity, size_t new_capacity);

#endif