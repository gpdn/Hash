#ifndef H_VALUES_H
#define H_VALUES_H

#include <stdio.h>
#include <stdlib.h>
#include "h_string_t.h"

typedef enum value_type_t {
    H_VALUE_NULL,
    H_VALUE_NUMBER,
    H_VALUE_CHAR,
    H_VALUE_STRING
} value_type_t;

//typedef double value_t;

typedef struct value_t {
    value_type_t type;
    union {
        double number;
        char character;
        h_string_t* string;
    };
} value_t;

#define NUM_VALUE(value) ((value_t){H_VALUE_NUMBER, {.number = value}})
#define STR_VALUE(value) ((value_t){H_VALUE_STRING, {.string = value}})
#define CHAR_VALUE(value) ((value_t){H_VALUE_CHAR, {.char = value}})
#define NULL_VALUE(value) ((value_t){H_VALUE_NULL, {.number = 0}})

#define IS_NUM(value) ((value).type == H_VALUE_NUMBER);
#define IS_STR(value) ((value).type == H_VALUE_STRING);
#define IS_CHAR(value) ((value).type == H_VALUE_CHAR);
#define IS_NULL(value) ((value).type == H_VALUE_NULL);

void print_value(value_t* value);
void print_value_no_newline(value_t* value);
const char* resolve_type(value_t* value);

#endif