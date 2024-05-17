#ifndef H_VALUE_TYPES_H
#define H_VALUE_TYPES_H

typedef enum value_type_t {
    H_VALUE_UNDEFINED,
    H_VALUE_NULL,
    H_VALUE_NUMBER,
    H_VALUE_CHAR,
    H_VALUE_STRING,
    H_VALUE_ARRAY
} value_type_t;

#endif