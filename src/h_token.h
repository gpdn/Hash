#ifndef HASH_TOKEN_H
#define HASH_TOKEN_H

#include <stdlib.h>

typedef enum token_type_t {
    H_TOKEN_EOF,
    H_TOKEN_COMMA,
    H_TOKEN_DOT,
    H_TOKEN_PLUS,
    H_TOKEN_PLUS_PLUS,
    H_TOKEN_MINUS,
    H_TOKEN_MINUS_MINUS,
    H_TOKEN_STAR,
    H_TOKEN_SLASH,
    H_TOKEN_EQUAL,
    H_TOKEN_DOUBLE_EQUAL,
    H_TOKEN_BANG,
    H_TOKEN_LESS,
    H_TOKEN_LESS_EQUAL,
    H_TOKEN_GREATER,
    H_TOKEN_GREATER_EQUAL,
    H_TOKEN_IDENTIFIER,
    H_TOKEN_TRUE,
    H_TOKEN_FALSE,
    H_TOKEN_AND,
    H_TOKEN_OR,
    H_TOKEN_IF,
    H_TOKEN_ELSE,
    H_TOKEN_FOR,
    H_TOKEN_WHILE,
    H_TOKEN_NULL,
    H_TOKEN_RETURN,
    H_TOKEN_PRINT,
    H_TOKEN_VAR,
    H_TOKEN_DO,
    H_TOKEN_LEFT_PAR,
    H_TOKEN_RIGHT_PAR,
    H_TOKEN_LEFT_CURLY, 
    H_TOKEN_RIGHT_CURLY,
    H_TOKEN_SEMICOLON,
    H_TOKEN_COLON,
    H_TOKEN_POW,
    H_TOKEN_ERROR,
    H_TOKEN_WARNING,
    H_TOKEN_MODULO,
    H_TOKEN_STRING_LITERAL,
    H_TOKEN_NUMBER_LITERAL,
    H_TOKEN_CLASS,
    H_TOKEN_SUPER,
    H_TOKEN_FUNCTION
} token_type_t;

typedef struct token_t {
    token_type_t type;
    const char* start;
    size_t length;
    size_t line;
} token_t;

#endif