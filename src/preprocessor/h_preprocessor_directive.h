
#ifndef H_PREPROCESSOR_DIRECTIVES
#define H_PREPROCESSOR_DIRECTIVES

#include "../debug.h"

typedef enum h_preprocessor_directive_t {
    H_DIRECTIVE_UNKNOWN,
    H_DIRECTIVE_IF,
    H_DIRECTIVE_ELIF,
    H_DIRECTIVE_ELSE,
    H_DIRECTIVE_ENDIF,
    H_DIRECTIVE_IMPORT,
    H_DIRECTIVE_DEFINE,
    H_DIRECTIVE_SET,
    H_DIRECTIVE_UNSET,
    H_DIRECTIVE_PRINT,
    H_DIRECTIVE_FN,
    H_DIRECTIVE_CALL
} h_preprocessor_directive_t;

void print_preprocessor_directive(h_preprocessor_directive_t directive);
void print_preprocessor_directive_no_newline(h_preprocessor_directive_t directive);

#endif