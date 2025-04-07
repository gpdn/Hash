#include "h_memory.h"

h_arena_manager_t* h_memory_create_manager() {
    return h_arena_manager_init();
}

h_arena_t* h_memory_create_arena(size_t capacity, h_memory_type_t tag) {
    return h_arena_alloc(capacity, tag);
}

h_arena_t* h_memory_register_arena(h_arena_manager_t* manager, size_t capacity, h_memory_type_t tag) {
    return h_am_make_arena(manager, capacity, tag);
}

h_arena_t* h_memory_get_arena(h_arena_manager_t* manager, h_memory_type_t tag) {
    return manager->arenas[tag].arenas[0];
}

inline void h_memory_report(h_arena_manager_t* manager) {
    h_am_report_memory(manager);
}

inline int h_memory_report_tag(h_arena_manager_t* manager, h_memory_type_t tag) {
    return h_am_report_memory_tag(manager, tag);
}

inline void* h_memory_get_block(h_arena_manager_t* manager, h_memory_type_t tag, size_t capacity) {
    return h_am_get_memory(manager, tag, capacity);
}

inline void* h_memory_reallocate_block(h_arena_manager_t* manager, h_memory_type_t tag, void* block, size_t prev_capacity, size_t new_capacity) {
    return h_am_reallocate_memory(manager, tag, block, prev_capacity, new_capacity);
}