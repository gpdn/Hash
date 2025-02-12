#include "h_std_array.h"

value_t h_std_array_len(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE(H_NFI_ARR_LEN(parameters[0]));
}

int h_std_array_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table) {
    define_native(stack, "array_size", h_std_array_len, (value_t[]){H_NFI_VALUE(H_VALUE_ARRAY)}, 1, H_NFI_VALUE(H_VALUE_NUMBER));
    return 1;
}