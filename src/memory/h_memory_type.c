#include "h_memory_type.h"

const char* h_resolve_memory_type(h_memory_type_t tag) {
    switch(tag) {
        case H_MEMORY_TAG_GENERIC: 
            return "H_MEMORY_TAG_GENERIC";
        case H_MEMORY_TAG_INTERNAL: 
            return "H_MEMORY_TAG_INTERNAL";
        case H_MEMORY_TAG_STRING: 
            return "H_MEMORY_TAG_STRING";
        case H_MEMORY_TAG_ARRAY: 
            return "H_MEMORY_TAG_ARRAY";
        case H_MEMORY_TAG_LAST: 
        default:
            return "Undefined memory tag";
    }
}