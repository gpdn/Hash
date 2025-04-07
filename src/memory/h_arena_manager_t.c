#include "h_arena_manager_t.h"

#define H_AM_DEFAULT_LIST_CAPACITY 5
#define H_AM_DEFAULT_ARENA_SIZE 4096

h_arena_manager_t* h_arena_manager_init() {
    h_arena_t* internal_arena = h_arena_alloc(H_AM_DEFAULT_ARENA_SIZE, H_MEMORY_TAG_INTERNAL);
    h_arena_manager_t* manager = (h_arena_manager_t*)h_arena_get_memory(internal_arena, sizeof(h_arena_manager_t), NULL);
    for(h_memory_type_t i = 0; i < H_MEMORY_TAG_LAST; ++i) {
        manager->arenas[i] = (h_arena_type_list_t){
            .arenas = (h_arena_t**)h_arena_get_memory(internal_arena, sizeof(h_arena_type_list_t*) * H_AM_DEFAULT_LIST_CAPACITY, NULL), 
            .size = 0, 
            .capacity = H_AM_DEFAULT_LIST_CAPACITY
        };
    }
    memset(manager->size, 0, sizeof(int) * H_MEMORY_TAG_LAST);
    manager->size[H_MEMORY_TAG_INTERNAL] = internal_arena->end - internal_arena->start;
    manager->internal_arena = internal_arena; 
    return manager;
}

h_arena_t* h_am_make_arena(h_arena_manager_t* manager, size_t capacity, h_memory_type_t tag) {
    h_arena_t* arena = h_arena_alloc(capacity, tag);
    manager->size[tag] += capacity;
    h_am_register_arena(manager, arena);
    return arena;
}

inline int h_am_register_arena(h_arena_manager_t* manager, h_arena_t* arena) {
    h_memory_type_t tag = arena->tag;
    if(manager->arenas[tag].size >= manager->arenas[tag].capacity) {
        manager->arenas[tag].capacity <<= 1;
    }
    manager->arenas[tag].arenas[manager->arenas[tag].size] = arena;
    return manager->arenas[tag].size++;
}

inline int h_am_report_memory_tag(h_arena_manager_t* manager, h_memory_type_t tag) {
    printf("%s - %lld\n", h_resolve_memory_type(tag), manager->size[tag]);
    return manager->size[tag];
}

inline void h_am_report_memory(h_arena_manager_t* manager) {
    for(h_memory_type_t i = 0; i < H_MEMORY_TAG_LAST; ++i) {
        printf("%s - %lld\n", h_resolve_memory_type(i), manager->size[i]);
    }
}

void* h_am_get_memory(h_arena_manager_t* manager, h_memory_type_t tag, size_t capacity) {
    int new_size = 0;
    void* block = h_arena_get_memory(manager->arenas[tag].arenas[0], capacity, &new_size);
    if(new_size) manager->size[tag] += new_size;
    return block;
}

inline void* h_am_reallocate_memory(h_arena_manager_t* manager, h_memory_type_t tag, void* block, size_t prev_capacity, size_t new_capacity) {
    int new_size = 0;
    void* new_block = h_arena_realloc(manager->arenas[tag].arenas[0], block, prev_capacity, new_capacity, &new_size);
    manager->size[tag] += new_size;
    return new_block;
}

void h_arena_manager_free(h_arena_manager_t* manager) {
    h_arena_free(manager->internal_arena);
}

#undef H_AM_DEFAULT_LIST_CAPACITY