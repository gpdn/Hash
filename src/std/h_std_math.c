#include "h_std_math.h"

value_t h_std_math_abs(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE((double)abs(H_NFI_NUM_TO_CINT(parameters[0])));
}

value_t h_std_math_sqrt(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE(sqrt(H_NFI_NUM_TO_CDOUBLE(parameters[0])));
}

value_t h_std_math_sin(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE(sin(H_NFI_NUM_TO_CDOUBLE(parameters[0])));
}

value_t h_std_math_cos(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE(cos(H_NFI_NUM_TO_CDOUBLE(parameters[0])));
}

value_t h_std_math_tan(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE(tan(H_NFI_NUM_TO_CDOUBLE(parameters[0])));
}

value_t h_std_math_asin(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE(asin(H_NFI_NUM_TO_CDOUBLE(parameters[0])));
}

value_t h_std_math_acos(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE(acos(H_NFI_NUM_TO_CDOUBLE(parameters[0])));
}

value_t h_std_math_atan(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE(atan(H_NFI_NUM_TO_CDOUBLE(parameters[0])));
}

value_t h_std_math_sinh(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE(sinh(H_NFI_NUM_TO_CDOUBLE(parameters[0])));
}

value_t h_std_math_cosh(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE(cosh(H_NFI_NUM_TO_CDOUBLE(parameters[0])));
}

value_t h_std_math_tanh(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE(tanh(H_NFI_NUM_TO_CDOUBLE(parameters[0])));
}

value_t h_std_math_ceil(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE(ceil(H_NFI_NUM_TO_CDOUBLE(parameters[0])));
}

value_t h_std_math_floor(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE(floor(H_NFI_NUM_TO_CDOUBLE(parameters[0])));
}

value_t h_std_math_round(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE(round(H_NFI_NUM_TO_CDOUBLE(parameters[0])));
}

value_t h_std_math_pow(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE(pow(H_NFI_NUM_TO_CDOUBLE(parameters[0]), H_NFI_NUM_TO_CDOUBLE(parameters[1])));
}

value_t h_std_math_log(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE(log(H_NFI_NUM_TO_CDOUBLE(parameters[0])));
}

value_t h_std_math_log10(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE(log10(H_NFI_NUM_TO_CDOUBLE(parameters[0])));
}

value_t h_std_math_exp(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE(exp(H_NFI_NUM_TO_CDOUBLE(parameters[0])));
}

int h_std_math_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table) {
    define_native(stack, "math_abs", h_std_math_abs, (value_t[]){(value_t){.type = H_VALUE_NUMBER}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "math_sqrt", h_std_math_sqrt, (value_t[]){(value_t){.type = H_VALUE_NUMBER}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "math_sin", h_std_math_sin, (value_t[]){(value_t){.type = H_VALUE_NUMBER}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "math_cos", h_std_math_cos, (value_t[]){(value_t){.type = H_VALUE_NUMBER}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "math_tan", h_std_math_tan, (value_t[]){(value_t){.type = H_VALUE_NUMBER}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "math_asin", h_std_math_asin, (value_t[]){(value_t){.type = H_VALUE_NUMBER}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "math_acos", h_std_math_acos, (value_t[]){(value_t){.type = H_VALUE_NUMBER}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "math_atan", h_std_math_atan, (value_t[]){(value_t){.type = H_VALUE_NUMBER}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "math_sinh", h_std_math_sinh, (value_t[]){(value_t){.type = H_VALUE_NUMBER}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "math_cosh", h_std_math_cosh, (value_t[]){(value_t){.type = H_VALUE_NUMBER}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "math_tanh", h_std_math_tanh, (value_t[]){(value_t){.type = H_VALUE_NUMBER}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "math_ceil", h_std_math_ceil, (value_t[]){(value_t){.type = H_VALUE_NUMBER}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "math_floor", h_std_math_floor, (value_t[]){(value_t){.type = H_VALUE_NUMBER}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "math_round", h_std_math_round, (value_t[]){(value_t){.type = H_VALUE_NUMBER}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "math_pow", h_std_math_pow, (value_t[]){(value_t){.type = H_VALUE_NUMBER}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "math_log", h_std_math_log, (value_t[]){(value_t){.type = H_VALUE_NUMBER}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "math_log10", h_std_math_log10, (value_t[]){(value_t){.type = H_VALUE_NUMBER}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "math_exp", h_std_math_exp, (value_t[]){(value_t){.type = H_VALUE_NUMBER}}, 1, (value_t){.type = H_VALUE_NUMBER});
    return 1;
}