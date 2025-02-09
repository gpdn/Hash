#include "h_values.h"

void print_value(value_t* value) {
    switch(value->type) {
        case H_VALUE_NUMBER:
            printf("%f\n", value->number);
            break;
        case H_VALUE_STRING:
            printf("%s\n", value->string->string);
            break;
        case H_VALUE_NULL:
            printf("Null\n");
            break;
        case H_VALUE_ARRAY:
            printf("[");
            h_array_print_no_newline(value->array);
            printf("]\n");
            break;
        case H_VALUE_FUNCTION:
            h_function_print(value->function);
            break;
        case H_VALUE_DATA:
            h_struct_print(value->data);
            break;
        case H_VALUE_TYPE:
            h_data_print(value->data_type);
            break;
        case H_VALUE_NATIVE:
            h_native_print(value->native_fn);
            break;
    }
}

void print_value_no_newline(value_t* value) {
    switch(value->type) {
        case H_VALUE_NUMBER:
            printf("%f", value->number);
            break;
        case H_VALUE_STRING:
            printf("%s", value->string->string);
            break;
        case H_VALUE_NULL:
            printf("Null");
            break;
        case H_VALUE_ARRAY:
            printf("Array");
            break;
        case H_VALUE_FUNCTION:
            h_function_print_no_newline(value->function);
            break;
        case H_VALUE_DATA:
            printf("Data");
            break;
        case H_VALUE_TYPE:
            h_data_print_no_newline(value->data_type);
            break;
        case H_VALUE_NATIVE:
            h_native_print_no_newline(value->native_fn);
            break;
    }
}

const char* resolve_type(value_t* value) {
    switch(value->type) {
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