
#ifndef H_MEMORY_TYPE
#define H_MEMORY_TYPE

typedef enum h_memory_type_t {
    H_MEMORY_TAG_GENERIC,
    H_MEMORY_TAG_INTERNAL,
    H_MEMORY_TAG_STRING,
    H_MEMORY_TAG_ARRAY,
    H_MEMORY_TAG_LAST
} h_memory_type_t;

const char* h_resolve_memory_type(h_memory_type_t tag);

#endif