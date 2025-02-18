#include "lexer.h"

static token_t token_create(lexer_t* lexer, token_type_t type);
static token_t token_create_line(lexer_t* lexer, token_type_t type, size_t line);
static token_t token_create_error(lexer_t* lexer, token_type_t type, const char* message);
static void token_add(lexer_t* lexer, token_t type);
static token_t lexer_check_next(lexer_t* lexer, char c, token_type_t token_match, token_type_t token_mismatch);
static token_t lexer_check_next_multiple(lexer_t* lexer, char c1, token_type_t first_token_match, char c2, token_type_t second_token_match, token_type_t token_mismatch);
static int lexer_skip(lexer_t* lexer);
static token_t lexer_string(lexer_t* lexer);
static token_t lexer_number(lexer_t* lexer);
static token_t lexer_identifier(lexer_t* lexer);
static inline token_t check_keyword(lexer_t* lexer, size_t start, size_t length, const char* string_to_match, token_type_t type);
static inline token_t create_keyword(lexer_t* lexer, token_type_t type, size_t length);
static void lexer_report_error(lexer_t* lexer, token_t* token, token_t* prev_token);
static void lexer_report_warning(lexer_t* lexer, token_t* token, token_t* prev_token);

static token_t token_create(lexer_t* lexer, token_type_t type) {
    token_t token;
    token.type = type;
    token.start = lexer->start;
    token.length = (size_t)(lexer->current - lexer->start);
    token.line = lexer->line;
    return token;
}

static token_t token_create_line(lexer_t* lexer, token_type_t type, size_t line) {
    token_t token;
    token.type = type;
    token.start = lexer->start;
    token.length = (size_t)(lexer->current - lexer->start);
    token.line = line;
    return token;
}


static token_t token_create_error(lexer_t* lexer, token_type_t type, const char* message) {
    token_t token;
    token.type = type;
    token.start = message;
    token.length = strlen(message);
    token.line = lexer->line;
    ++lexer->errors_count;
    return token;
}

static token_t token_create_warning(lexer_t* lexer, token_type_t type, const char* message) {
    token_t token;
    token.type = type;
    token.start = message;
    token.length = strlen(message);
    token.line = lexer->line;
    ++lexer->warnings_count;
    return token;
}

static void token_add(lexer_t* lexer, token_t token) {
    if(lexer->tokens_array_size >= lexer->tokens_array_capacity) {
        lexer->tokens_array_capacity *= 2;
        lexer->tokens_array = (token_t*)realloc(lexer->tokens_array, sizeof(token_t) * lexer->tokens_array_capacity);
    }

    lexer->tokens_array[lexer->tokens_array_size++] = token;
}

static int lexer_skip(lexer_t* lexer) {
    char c = *lexer->current;
    
    if(c == '\0') return 0;

    switch(c) {
        case ' ':
        case '\r':
        case '\t':
            #if DEBUG_TRACE_LEXER_PRINT_SKIPPED
                DEBUG_LOG("Skipped whitespace\n");
            #endif
            ++lexer->current;
            return 1;
            break;
        case '\n':
            #if DEBUG_TRACE_LEXER_PRINT_SKIPPED
                DEBUG_LOG("Skipped newline\n");
            #endif
            ++lexer->current;
            ++lexer->line;
            return 1;
            break;
        case '/':
            if(*(lexer->current+1) != '\0' && *(lexer->current+1) == '/') {
                ++lexer->current;
                ++lexer->current;
                while(*lexer->current != '\0' && *lexer->current != '\n') {
                    ++lexer->current;
                }

                #if DEBUG_TRACE_LEXER_PRINT_SKIPPED
                    DEBUG_LOG("Skipped comment\n");
                #endif

                return 1;
            }
            if(*(lexer->current+1) != '\0' && *(lexer->current+1) == '^') {
                ++lexer->current;
                ++lexer->current;
                while(*lexer->current != '\0' && *lexer->current != '/' &&  *(lexer->current + 1) != '\0' && *(lexer->current + 1) != '^') {
                    ++lexer->current;
                }

                #if DEBUG_TRACE_LEXER_PRINT_SKIPPED
                    DEBUG_LOG("Skipped comment\n");
                #endif

                return 1;
            }
            return 0;
            break;
    }

    return 0;
}

static token_t lexer_string(lexer_t* lexer) {
    lexer->start = lexer->current;
    size_t start_line = lexer->line;
    while(*lexer->current != '"' && *lexer->current != '\0') {
        if(*lexer->current == '\n') {++lexer->line;}
        ++lexer->current;
    }

    if(*lexer->current == '\0') return token_create_error(lexer, H_TOKEN_ERROR, "Unterminated string");

    token_t token = token_create_line(lexer, H_TOKEN_STRING_LITERAL, start_line);

    ++lexer->current;

    return token;
}

static token_t lexer_number(lexer_t* lexer) {
    while(isdigit(*lexer->current)) ++lexer->current;
    if(*lexer->current == '.') {
        ++lexer->current;
        if(!isdigit(*lexer->current)) return token_create_warning(lexer, H_TOKEN_WARNING, "No decimal part provided for number after '.'.");
        while(isdigit(*lexer->current)) ++lexer->current;
    }
    return token_create(lexer, H_TOKEN_NUMBER_LITERAL);
}

static token_t lexer_identifier(lexer_t* lexer) {
    while(isalnum(*lexer->current) || *lexer->current == '_') ++lexer->current;

    switch(lexer->start[0]) {
        case 'a': 
            if(lexer->current - lexer->start > 1) {
                switch(lexer->start[1]) {
                    case 'n': return check_keyword(lexer, 2, 1, "d", H_TOKEN_AND);
                    case 'r': return check_keyword(lexer, 2, 1, "r", H_TOKEN_ARR);
                }
            }
            break;
        case 'b': return check_keyword(lexer, 1, 4, "reak", H_TOKEN_BREAK);
        case 'c': return check_keyword(lexer, 1, 4, "onst", H_TOKEN_CONST);
        case 'd': 
            if(lexer->current - lexer->start > 1) {
                switch(lexer->start[1]) {
                    case 'o': return check_keyword(lexer, 2, 0, "o", H_TOKEN_DO);
                    case 'a': return check_keyword(lexer, 2, 2, "ta", H_TOKEN_DATA);
                }
            }
            break;
        case 'e': 
            if(lexer->current - lexer->start > 1) {
                switch(lexer->start[1]) {
                    case 'l': return check_keyword(lexer, 2, 2, "se", H_TOKEN_ELSE);
                    case 'n': return check_keyword(lexer, 2, 2, "um", H_TOKEN_ENUM);
                }
            }
            break;
        case 'f': 
            if(lexer->current - lexer->start > 1) {
                switch(lexer->start[1]) {
                    case 'a': return check_keyword(lexer, 2, 3, "lse", H_TOKEN_FALSE);
                    case 'n': return check_keyword(lexer, 2, 0, "", H_TOKEN_FUNCTION);
                    case 'o': return check_keyword(lexer, 2, 1, "r", H_TOKEN_FOR);
                }
            }
            break;
        case 'g':
            if(lexer->current - lexer->start > 1) {
                switch(lexer->start[1]) {
                    case 'l': return check_keyword(lexer, 2, 2, "ob", H_TOKEN_GLOB);
                    case 'o': return check_keyword(lexer, 2, 2, "to", H_TOKEN_GOTO);
                }
            }
            break;
        case 'i': return check_keyword(lexer, 1, 1, "f", H_TOKEN_IF);
        case 'l':
            if(lexer->current - lexer->start > 1) {
                switch(lexer->start[1]) {
                    case 'a': return check_keyword(lexer, 2, 3, "bel", H_TOKEN_LABEL);
                    case 'o': return check_keyword(lexer, 2, 2, "op", H_TOKEN_LOOP);
                }
            }
            break;
        case 'n': 
            if(lexer->current - lexer->start > 2 && lexer->start[1] == 'u') {
                switch(lexer->start[2]) {
                    //case 'm': return token_create(lexer, H_TOKEN_NUM);
                    case 'm': return check_keyword(lexer, 3, 0, "", H_TOKEN_NUM);
                    case 'l': return check_keyword(lexer, 3, 1, "l", H_TOKEN_NULL);
                }
            }
            break;
        case 'o': return check_keyword(lexer, 1, 1, "r", H_TOKEN_OR);
        case 'p': return check_keyword(lexer, 1, 4, "rint", H_TOKEN_PRINT);
        case 'r':
            if(lexer->current - lexer->start > 2 && lexer->start[1] == 'e') {
                switch(lexer->start[2]) {
                    case 'p': return check_keyword(lexer, 3, 3, "eat", H_TOKEN_REPEAT);
                    case 't': return create_keyword(lexer, H_TOKEN_RETURN, 3);
                }
            }
            break;
        case 's': 
            if(lexer->current - lexer->start > 1) {
                switch(lexer->start[1]) {
                    case 'k': return check_keyword(lexer, 2, 2, "ip", H_TOKEN_SKIP);
                    case 't': 
                        if(lexer->current - lexer->start > 2) {
                            switch(lexer->start[2]) {
                                case 'o': return check_keyword(lexer, 3, 1, "p", H_TOKEN_STOP);
                                case 'r': return check_keyword(lexer, 3, 0, "", H_TOKEN_STR);
                            }
                        }
                }
            }
            break;
        case 't': return check_keyword(lexer, 1, 1, "o", H_TOKEN_TO);
        case 'v': return check_keyword(lexer, 1, 2, "ar", H_TOKEN_VAR);
        case 'w': return check_keyword(lexer, 1, 4, "hile", H_TOKEN_WHILE);
    }

    return token_create(lexer, H_TOKEN_IDENTIFIER);
}

static inline token_t check_keyword(lexer_t* lexer, size_t start, size_t length, const char* string_to_match, token_type_t type) {
    if((size_t)(lexer->current - (lexer->start + start)) == length && memcmp(lexer->start + start, string_to_match, length) == 0 && ((size_t)(lexer->current - lexer->start)) == (start + length)) {
        return token_create(lexer, type);
    }
    return token_create(lexer, H_TOKEN_IDENTIFIER);
}

static inline token_t create_keyword(lexer_t* lexer, token_type_t type, size_t length) {
    if((lexer->current - lexer->start) == (long long int)length) return token_create(lexer, type);
    return token_create(lexer, H_TOKEN_IDENTIFIER);
}

static void lexer_report_error(lexer_t* lexer, token_t* token, token_t* prev_token) {
    DEBUG_COLOR_SET(COLOR_RED);
    fprintf(stderr, "\n\n[LEXER ERROR | Line: %lld] %.*s\n", token->line, (int)token->length, token->start);
    
    const char* token_start = (prev_token) ? prev_token->start + prev_token->length : lexer->start;

    size_t lines_count_before = token->line - lexer->tokens_array[0].line;
    size_t lines_count_after = lexer->tokens_array[lexer->tokens_array_size - 1].line - token->line;

    token_t* temp = lexer->tokens_array;
    token_t* temp_next = lexer->tokens_array;

    while(temp->line != token->line - DEBUG_MIN(lines_count_before, 5)) ++temp;

    temp_next = temp;
    while(temp_next->line != token->line) ++temp_next;

    DEBUG_COLOR_SET(COLOR_GRAY);
    fprintf(stderr, "%.*s", (int)(temp_next->start - temp->start), temp->start);
    
    temp = temp_next;
    DEBUG_COLOR_SET(COLOR_YELLOW);
    fprintf(stderr, "%.*s", (int)(token_start - temp->start), temp->start);

    while(temp_next != token + 1)++temp_next;
    DEBUG_COLOR_SET(COLOR_RED);
    fprintf(stderr, "%.*s", (int)(temp_next->start - token_start), token_start);

    temp = temp_next;
    while(temp_next->line == token->line) ++temp_next;
    DEBUG_COLOR_SET(COLOR_YELLOW);
    fprintf(stderr, "%.*s", (int)(temp_next->start - temp->start), temp->start);

    temp = temp_next;
    while(temp_next->line != token->line + DEBUG_MIN(lines_count_after, 5)) ++temp_next;
    DEBUG_COLOR_SET(COLOR_GRAY);
    fprintf(stderr, "%.*s", (int)(temp_next->start - temp->start), temp->start);

    DEBUG_COLOR_RESET();
    
}

static void lexer_report_warning(lexer_t* lexer, token_t* token, token_t* prev_token) {
    DEBUG_COLOR_SET(COLOR_YELLOW);
    fprintf(stderr, "\n\n[LEXER WARNING | Line: %lld] %.*s\n", token->line, (int)token->length, token->start);

    const char* token_start = (prev_token) ? prev_token->start + prev_token->length : lexer->start;

    size_t lines_count_before = token->line - lexer->tokens_array[0].line;
    size_t lines_count_after = lexer->tokens_array[lexer->tokens_array_size - 1].line - token->line;

    token_t* temp = lexer->tokens_array;

    while(temp->line != token->line - DEBUG_MIN(lines_count_before, 3)) ++temp;
    DEBUG_COLOR_SET(COLOR_GRAY);
    fprintf(stderr, "%.*s", (int)(token_start - temp->start), temp->start);

    while(temp != token + 1) ++temp;
    DEBUG_COLOR_SET(COLOR_YELLOW);
    fprintf(stderr, "%.*s", (int)(temp->start - token_start), token_start);
    
    while(temp->line != token->line + DEBUG_MIN(lines_count_after, 3)) ++temp;
    DEBUG_COLOR_SET(COLOR_GRAY);
    fprintf(stderr, "%.*s", (int)(token_start - temp->start), temp->start);
    
    DEBUG_COLOR_RESET();

}

lexer_t* lexer_init(const char* source) {
    #define TOKEN_ARRAY_CAPACITY 100 

    lexer_t* lexer = (lexer_t*)malloc(sizeof(lexer_t));
    lexer->tokens_array = (token_t*)malloc(sizeof(token_t) * TOKEN_ARRAY_CAPACITY);
    lexer->tokens_array_size = 0;
    lexer->tokens_array_capacity = TOKEN_ARRAY_CAPACITY;
    lexer->start = source;
    lexer->current = source;
    lexer->line = 0;
    return lexer;

    #undef TOKEN_ARRAY_CAPACITY
}

void lexer_free(lexer_t* lexer) {
    free(lexer);
}

static token_t lexer_check_next(lexer_t* lexer, char c, token_type_t token_match, token_type_t token_mismatch) {
    if(*lexer->current != '\0' && *lexer->current == c) {
        ++lexer->current;
        return token_create(lexer, token_match);
    }

    return token_create(lexer, token_mismatch);
}

static token_t lexer_check_next_multiple(lexer_t* lexer, char c1, token_type_t first_token_match, char c2, token_type_t second_token_match, token_type_t token_mismatch) {
    
    if(*lexer->current == '\0') return token_create(lexer, token_mismatch);
    
    if(*lexer->current == c1) {++lexer->current; return token_create(lexer, first_token_match);}
    if(*lexer->current == c2) {++lexer->current; return token_create(lexer, second_token_match);}

    return token_create(lexer, token_mismatch);
}

token_t lexer_get_token(lexer_t* lexer) {

    #define LEXER_CHECK_MULTIPLE(C1, FIRST_TYPE, C2, SECOND_TYPE, TYPE_MISMATCH)\
        if(*lexer->current == '\0') return token_create(lexer, TYPE_MISMATCH);\
        switch(*lexer->current) {\
            case C1:\
                ++lexer->current;\
                return token_create(lexer, FIRST_TYPE);\
            case C2:\
                ++lexer->current;\
                return token_create(lexer, SECOND_TYPE);\
            default:\
                return token_create(lexer, TYPE_MISMATCH);\
        }\
    
    #define LEXER_CHECK_MANY(C1, FIRST_TYPE, C2, SECOND_TYPE, C3, THIRD_TYPE, TYPE_MISMATCH)\
        if(*lexer->current == '\0') return token_create(lexer, TYPE_MISMATCH);\
        switch(*lexer->current) {\
            case C1:\
                ++lexer->current;\
                return token_create(lexer, FIRST_TYPE);\
            case C2:\
                ++lexer->current;\
                return token_create(lexer, SECOND_TYPE);\
            case C3:\
                ++lexer->current;\
                return token_create(lexer, THIRD_TYPE);\
            default:\
                return token_create(lexer, TYPE_MISMATCH);\
        }\

    char c = *lexer->current++;

    #if DEBUG_TRACE_LEXER_CURRENT_CHAR
        DEBUG_LOG("Char: %c\n", c);
    #endif

    switch(c) {
        case '(': return token_create(lexer, H_TOKEN_LEFT_PAR);
        case ')': return token_create(lexer, H_TOKEN_RIGHT_PAR);
        case '[': return token_create(lexer, H_TOKEN_LEFT_SQUARE);
        case ']': return token_create(lexer, H_TOKEN_RIGHT_SQUARE);
        case '{': return token_create(lexer, H_TOKEN_LEFT_CURLY);
        case '}': return token_create(lexer, H_TOKEN_RIGHT_CURLY);
        case ';': return token_create(lexer, H_TOKEN_SEMICOLON);
        case ',': return token_create(lexer, H_TOKEN_COMMA);
        case '.': return token_create(lexer, H_TOKEN_DOT);
        case '%': return token_create(lexer, H_TOKEN_MODULO);
        case '&': return token_create(lexer, H_TOKEN_BITWISE_AND);
        case '|': return token_create(lexer, H_TOKEN_BITWISE_OR);
        case '^': return token_create(lexer, H_TOKEN_BITWISE_XOR);
        case '~': return token_create(lexer, H_TOKEN_BITWISE_NOT);
        case '>': LEXER_CHECK_MULTIPLE('=', H_TOKEN_GREATER_EQUAL, '>', H_TOKEN_BITWISE_SHIFT_RIGHT, H_TOKEN_GREATER);
        case '<': LEXER_CHECK_MULTIPLE('=', H_TOKEN_LESS_EQUAL, '<', H_TOKEN_BITWISE_SHIFT_LEFT, H_TOKEN_LESS);
        case '+': LEXER_CHECK_MULTIPLE('+', H_TOKEN_PLUS_PLUS, '=', H_TOKEN_PLUS_EQUAL, H_TOKEN_PLUS);
        case '-': LEXER_CHECK_MANY('-', H_TOKEN_MINUS_MINUS, '=', H_TOKEN_MINUS_EQUAL, '>', H_TOKEN_ARROW, H_TOKEN_MINUS);
        case '*': LEXER_CHECK_MULTIPLE('*', H_TOKEN_POW, '=', H_TOKEN_STAR_EQUAL, H_TOKEN_STAR);
        case '!': LEXER_CHECK_MULTIPLE('!', H_TOKEN_DOUBLE_BANG, '=', H_TOKEN_BANG_EQUAL, H_TOKEN_BANG);
        case ':': return lexer_check_next(lexer, ':', H_TOKEN_DOUBLE_COLON, H_TOKEN_COLON);
        case '/': return lexer_check_next(lexer, '=', H_TOKEN_SLASH_EQUAL, H_TOKEN_SLASH);
        case '=': return lexer_check_next(lexer, '=', H_TOKEN_DOUBLE_EQUAL, H_TOKEN_EQUAL);
        case '?': return lexer_check_next(lexer, '?', H_TOKEN_DOUBLE_QUESTION_MARK, H_TOKEN_QUESTION_MARK);
        case '"': return lexer_string(lexer);
    }

    if(isdigit(c)) return lexer_number(lexer);

    if(isalpha(c) || c == '_') return lexer_identifier(lexer);

    if(c != '\0') {
        DEBUG_ERROR("Unrecognised Token. Last recognised token: ");
        token_print(&lexer->tokens_array[lexer->tokens_array_size - 1]);
        return token_create_error(lexer, H_TOKEN_ERROR, "Unrecognised Token");
    }

    --lexer->current;
    return token_create(lexer, H_TOKEN_EOF);

    #undef LEXER_CHECK_MULTIPLE
    #undef LEXER_CHECK_MANY
}

size_t lexer_get_tokens_count(lexer_t* lexer) {
    return lexer->tokens_array_size;
}

unsigned int lexer_get_errors_count(lexer_t* lexer) {
    return lexer->errors_count;
}

unsigned int lexer_get_warnings_count(lexer_t* lexer) {
    return lexer->warnings_count;
}

int lexer_report_tokenisation_errors(lexer_t* lexer) {
    lexer->current = lexer->start;
    token_t* prev = NULL;
    for(token_t* temp = lexer->tokens_array; temp->type != H_TOKEN_EOF; ++temp) {
        if(temp->type == H_TOKEN_ERROR) {
            lexer_report_error(lexer, temp, prev);
            return 0;
        }
        if(temp->type == H_TOKEN_WARNING) {
            lexer_report_warning(lexer, temp, prev);
            return 0;
        }
        prev = temp;
    }

    return 1;
}

token_t* lexer_tokenise(lexer_t* lexer) {

    lexer->current = lexer->start;

    while(*lexer->current != '\0') {
        while (lexer_skip(lexer) == 1);
        lexer->start = lexer->current;
        token_t token = lexer_get_token(lexer);

        #if DEBUG_TRACE_LEXER_TOKEN
            DEBUG_COLOR_SET(COLOR_CYAN);
            token_print(&token);
            DEBUG_COLOR_RESET();
        #endif

        token_add(lexer, token);
    }

    token_add(lexer, token_create(lexer, H_TOKEN_EOF));

    return lexer->tokens_array;
}
