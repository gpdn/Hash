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
        default:
            return "Unk";
    }
}

const char* resolve_value_type(value_type_t type) {
    switch(type) {
        case H_VALUE_NUMBER:
            return "Num";
        case H_VALUE_STRING:
            return "Str";
        default:
            return "Unk";
    }
}