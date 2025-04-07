#include "h_arena_t.h"

h_arena_t* h_arena_alloc(size_t capacity, h_memory_type_t tag) {
    h_arena_t* arena = (h_arena_t*)malloc(sizeof(h_arena_t));
    arena->memory = malloc(capacity);
    arena->start = arena->memory;
    arena->current = arena->memory;
    arena->end = arena->start + capacity;
    arena->tag = tag;
    arena->next = NULL;
    return arena;
}

h_arena_t* h_arena_calloc(size_t capacity, h_memory_type_t tag) {
    h_arena_t* arena = (h_arena_t*)malloc(sizeof(h_arena_t));
    arena->memory = calloc(capacity, sizeof(char));
    arena->start = arena->memory;
    arena->current = arena->memory;
    arena->end = arena->start + capacity;
    arena->tag = tag;
    arena->next = NULL;
    return arena;
}

void* h_arena_realloc(h_arena_t* arena, void* block, size_t prev_capacity, size_t new_capacity, int* new_size) {
    if(arena->end - arena->current > (int)new_capacity) {
        char* start = arena->current;
        arena->current += new_capacity;
        memcpy(start, block, prev_capacity);
        return start;
    }

    arena->next = h_arena_alloc(new_capacity, arena->tag);
    arena->start = arena->next->start;
    arena->current = arena->start;
    arena->end = arena->next->end;
    if(new_size) {
        printf("New size\n");
        *new_size = new_capacity;
    }
    return arena->start;
}

void h_arena_reset(h_arena_t* arena) {
    arena->start = arena->memory;
    arena->current = arena->memory;
}

void* h_arena_get_memory(h_arena_t* arena, size_t capacity, int* new_size) {
    if(arena->end - arena->start < (int)capacity) {
        arena->next = h_arena_alloc(capacity, arena->tag);
        arena->start = arena->next->start;
        arena->current = arena->start;
        arena->end = arena->next->end;
        if(new_size) {
            *new_size = capacity;
        }
        return arena->start;
    }
    
    if(arena->end - arena->current > (int)capacity) {
        char* start = arena->current;
        arena->current += capacity;
        return start;
    }

    if(!arena->next) {
        arena->next = h_arena_alloc(arena->end - arena->start, arena->tag);
        arena->start = arena->next->start;
        arena->current = arena->start;
        arena->end = arena->next->end;
        if(new_size) {
            *new_size = arena->end - arena->start;
        }
    }
    
    return h_arena_get_memory(arena->next, capacity, new_size);
}

void h_arena_free(h_arena_t* arena) {
    free(arena->memory);
    if(arena->next) h_arena_free(arena->next);
}