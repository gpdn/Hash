#include "h_std_type.h"

value_t h_std_type_is_str(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE(parameters[0].type == H_VALUE_STRING ? 1 : 0);
}

value_t h_std_type_is_num(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE(parameters[0].type == H_VALUE_NUMBER ? 1 : 0);
}

value_t h_std_type_is_array(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE(parameters[0].type == H_VALUE_ARRAY ? 1 : 0);
}

value_t h_std_type_is_null(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE(parameters[0].type == H_VALUE_NULL ? 1 : 0);
}

value_t h_std_type_is_function(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE((parameters[0].type == H_VALUE_FUNCTION || parameters[0].type == H_VALUE_NATIVE) ? 1 : 0);
}

value_t h_std_type_is_data(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE((parameters[0].type == H_VALUE_DATA) ? 1 : 0);
}

int h_std_type_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table) {
    //define_native(stack, "type_is_num", h_std_type_is_num, (value_t[]){(value_t){.type = H_VALUE_STRING}}, 1, (value_t){.type = H_VALUE_STRING});
    //define_native(stack, "type_is_array", h_std_type_is_array, (value_t[]){(value_t){.type = H_VALUE_STRING}}, 1, (value_t){.type = H_VALUE_STRING});
    //define_native(stack, "type_is_str", h_std_type_is_str, (value_t[]){(value_t){.type = H_VALUE_STRING}}, 1, (value_t){.type = H_VALUE_STRING});
    //define_native(stack, "type_is_null", h_std_type_is_null, (value_t[]){(value_t){.type = H_VALUE_NUMBER}}, 1, (value_t){.type = H_VALUE_NULL});
    //define_native(stack, "type_is_function", h_std_type_is_function, (value_t[]){(value_t){.type = H_VALUE_NUMBER}}, 1, (value_t){.type = H_VALUE_NULL});
    //define_native(stack, "type_is_data", h_std_type_is_data, (value_t[]){(value_t){.type = H_VALUE_NUMBER}}, 1, (value_t){.type = H_VALUE_NULL});
    return 1;
}