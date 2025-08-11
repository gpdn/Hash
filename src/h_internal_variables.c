#include "h_internal_variables.h"

#define H_VM_TRACE_INITIAL_CAPACITY 500

static h_internal_variable_t internal_variables[H_INTERNAL_LAST];

static inline h_string_t* trace_init() {
    h_string_t* h_string = (h_string_t*)malloc(sizeof(h_string_t));
    h_string->string = (char*)malloc(sizeof(char) * H_VM_TRACE_INITIAL_CAPACITY);
    h_string->length = 0;
    h_string->capacity = H_VM_TRACE_INITIAL_CAPACITY;
    h_string->hash = 0;
    return h_string;
}

inline void h_internal_variables_init() {
    h_string_t* trace = trace_init();
    internal_variables[H_INTERNAL_TRACE] = (h_internal_variable_t){.tag = H_INTERNAL_TRACE, .name = h_string_init_hash("trace", 5), .value = STR_VALUE(trace)};
    internal_variables[H_INTERNAL_LAST] = (h_internal_variable_t){.tag = H_INTERNAL_LAST, .name = NULL, .value = NULL_VALUE()};
}

h_internal_variable_t h_internal_variables_resolve(h_string_t* string) {
    for (int i = 0; i < H_INTERNAL_LAST; ++i) {
        if (
            internal_variables[i].name->length == string->length && 
            internal_variables[i].name->hash == string->hash && 
            memcmp(internal_variables[i].name->string, string->string, string->length) == 0
        ) {
            return internal_variables[i];
        }
    }
    return internal_variables[H_INTERNAL_LAST];
}

inline h_internal_variable_t h_internal_variables_resolve_tag(h_internal_variable_tag_t tag) {
    return internal_variables[tag];
}

inline value_t h_internal_variables_resolve_get(h_internal_variable_tag_t tag) {
    return internal_variables[tag].value;
}

inline void h_internal_variables_free() {
    //trace will be freed in allocator eventually
    //for now it is being freed in the VM
    //which holds a pointer to the trace string
}

#undef H_VM_TRACE_INITIAL_CAPACITY