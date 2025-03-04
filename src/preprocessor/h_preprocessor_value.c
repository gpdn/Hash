#include "h_preprocessor_value.h"

void preprocessor_print_value(h_preprocessor_value_t value) {
    switch(value.type) {
        case H_PREPROCESSOR_VALUE_INT:
            printf("%d\n", value.number);
            break;
        case H_PREPROCESSOR_VALUE_STRING:
            printf("%s\n", value.string->string);
            break;
        case H_PREPROCESSOR_VALUE_IDENTIFIER:
            printf("Identifier %s\n", value.string->string);
            break;
        case H_PREPROCESSOR_VALUE_FUNCTION:
            printf("Function %s\n", value.function->name->string);
            break;
        case H_PREPROCESSOR_VALUE_UNDEFINED:
        default:
            printf("Undefined\n");
            break;
    }
}