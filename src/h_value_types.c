#include "h_value_types.h"

const char* resolve_value_type(value_type_t type) {
    switch(type) {
        case H_VALUE_NUMBER:
            return "Num";
        case H_VALUE_STRING:
            return "Str";
        case H_VALUE_ARRAY:
            return "Arr";
        case H_VALUE_FUNCTION:
            return "Fn";
        case H_VALUE_DATA:
            return "Data";
        case H_VALUE_TYPE:
            return "Custom";
        case H_VALUE_NATIVE:
            return "Native";
        default:
            return "Unk";
    }
}