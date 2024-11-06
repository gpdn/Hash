#include "h_preprocessor_directive.h"

void print_preprocessor_directive(h_preprocessor_directive_t directive) {
    switch (directive)  {
        case H_DIRECTIVE_IF:
            DEBUG_LOG("%s\n", "DIRECTIVE IF");
            break;
        case H_DIRECTIVE_ELIF:
            DEBUG_LOG("%s\n", "DIRECTIVE ELIF");
            break;
        case H_DIRECTIVE_ELSE:
            DEBUG_LOG("%s\n", "H_DIRECTIVE_ELSE");
            break;
        case H_DIRECTIVE_ENDIF:
            DEBUG_LOG("%s\n", "DIRECTIVE ENDIF");
            break;
        case H_DIRECTIVE_IMPORT:
            DEBUG_LOG("%s\n", "DIRECTIVE IMPORT");
            break;
        case H_DIRECTIVE_DEFINE:
            DEBUG_LOG("%s\n", "DIRECTIVE DEFINE");
            break;
        case H_DIRECTIVE_SET:
            DEBUG_LOG("%s\n", "DIRECTIVE SET");
            break;
        case H_DIRECTIVE_UNSET:
            DEBUG_LOG("%s\n", "DIRECTIVE UNSET");
            break;
        case H_DIRECTIVE_PRINT:
            DEBUG_LOG("%s\n", "DIRECTIVE PRINT");
            break;
        case H_DIRECTIVE_UNKNOWN:
            DEBUG_LOG("%s\n", "DIRECTIVE UNKNOWN");
            break;    
    }
}

void print_preprocessor_directive_no_newline(h_preprocessor_directive_t directive) {
    switch (directive)  {
        case H_DIRECTIVE_IF:
            DEBUG_LOG("%s", "DIRECTIVE IF");
            break;
        case H_DIRECTIVE_ELIF:
            DEBUG_LOG("%s", "DIRECTIVE ELIF");
            break;
        case H_DIRECTIVE_ELSE:
            DEBUG_LOG("%s", "H_DIRECTIVE_ELSE");
        break;
        case H_DIRECTIVE_ENDIF:
            DEBUG_LOG("%s", "DIRECTIVE ENDIF");
            break;
        case H_DIRECTIVE_IMPORT:
            DEBUG_LOG("%s", "DIRECTIVE IMPORT");
            break;
        case H_DIRECTIVE_DEFINE:
            DEBUG_LOG("%s", "DIRECTIVE DEFINE");
            break;
        case H_DIRECTIVE_SET:
            DEBUG_LOG("%s", "DIRECTIVE SET");
            break;
        case H_DIRECTIVE_UNSET:
            DEBUG_LOG("%s", "DIRECTIVE UNSET");
            break;
        case H_DIRECTIVE_PRINT:
            DEBUG_LOG("%s", "DIRECTIVE PRINT");
            break;
        case H_DIRECTIVE_UNKNOWN:
            DEBUG_LOG("%s", "DIRECTIVE UNKNOWN");
            break;    
    }
}
