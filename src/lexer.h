#ifndef HASH_LEXER_H
#define HASH_LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "h_token.h"
#include "debug.h"

typedef struct lexer_t {
    const char* start;
    const char* current;
    token_t* tokens_array;
    size_t tokens_array_size;
    size_t tokens_array_capacity;
    size_t line;
    unsigned int errors_count;
    unsigned int warnings_count;
} lexer_t;

lexer_t* lexer_init(const char* source);
void lexer_free(lexer_t* lexer);
token_t lexer_get_token(lexer_t* lexer);
token_t* lexer_tokenise(lexer_t* lexer);
size_t lexer_get_tokens_count(lexer_t* lexer);
unsigned int lexer_get_errors_count(lexer_t* lexer);
unsigned int lexer_get_warnings_count(lexer_t* lexer);
int lexer_report_tokenisation_errors(lexer_t* lexer);

#endif