#ifndef H_STD_MATH
#define H_STD_MATH

#include "h_nfi.h"
#include <stdlib.h>
#include <math.h>

value_t h_std_math_abs(struct value_t* parameters, size_t args_count);
value_t h_std_math_sqrt(struct value_t* parameters, size_t args_count);
value_t h_std_math_sin(struct value_t* parameters, size_t args_count);
value_t h_std_math_cos(struct value_t* parameters, size_t args_count);
value_t h_std_math_tan(struct value_t* parameters, size_t args_count);
value_t h_std_math_asin(struct value_t* parameters, size_t args_count);
value_t h_std_math_acos(struct value_t* parameters, size_t args_count);
value_t h_std_math_atan(struct value_t* parameters, size_t args_count);
value_t h_std_math_sinh(struct value_t* parameters, size_t args_count);
value_t h_std_math_cosh(struct value_t* parameters, size_t args_count);
value_t h_std_math_tanh(struct value_t* parameters, size_t args_count);
value_t h_std_math_ceil(struct value_t* parameters, size_t args_count);
value_t h_std_math_floor(struct value_t* parameters, size_t args_count);
value_t h_std_math_round(struct value_t* parameters, size_t args_count);
value_t h_std_math_pow(struct value_t* parameters, size_t args_count);
value_t h_std_math_log(struct value_t* parameters, size_t args_count);
value_t h_std_math_log10(struct value_t* parameters, size_t args_count);
value_t h_std_math_exp(struct value_t* parameters, size_t args_count);
value_t h_std_math_clamp(struct value_t* parameters, size_t args_count);
value_t h_std_math_min(struct value_t* parameters, size_t args_count);
value_t h_std_math_max(struct value_t* parameters, size_t args_count);
value_t h_std_math_rand(struct value_t* parameters, size_t args_count);
value_t h_std_math_srand(struct value_t* parameters, size_t args_count);
int h_std_math_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table);

#endif