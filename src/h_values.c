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
        default:
            return "Unk";
    }
}