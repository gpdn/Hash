#include "lexer.h"

static token_t token_create(lexer_t* lexer, token_type_t type);
static token_t token_create_line(lexer_t* lexer, token_type_t type, size_t line);
static token_t token_create_error(lexer_t* lexer, token_type_t type, const char* message);
static void token_add(lexer_t* lexer, token_t type);
static token_t lexer_check_next(lexer_t* lexer, char c, token_type_t token_match, token_type_t token_mismatch);
static int lexer_skip(lexer_t* lexer);
static token_t lexer_string(lexer_t* lexer);
static token_t lexer_number(lexer_t* lexer);
static token_t lexer_identifier(lexer_t* lexer);
static token_t check_keyword(lexer_t* lexer, size_t start, size_t length, const char* string_to_match, token_type_t type);
static void lexer_report_error(lexer_t* lexer, token_t* token, token_t* prev_token);

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
        if(!isdigit(*lexer->current)) return token_create_error(lexer, H_TOKEN_ERROR, "No decimal part provided for number after '.'");
        while(isdigit(*lexer->current)) ++lexer->current;
    }
    return token_create(lexer, H_TOKEN_NUMBER_LITERAL);
}

static token_t lexer_identifier(lexer_t* lexer) {
    while(isalnum(*lexer->current)) ++lexer->current;

    switch(lexer->start[0]) {
        case 'a': return check_keyword(lexer, 1, 2, "nd", H_TOKEN_AND);
        case 'c': return check_keyword(lexer, 1, 4, "lass", H_TOKEN_CLASS);
        case 'd': return check_keyword(lexer, 1, 1, "o", H_TOKEN_DO);
        case 'e': return check_keyword(lexer, 1, 3, "lse", H_TOKEN_ELSE);
        case 'f': 
            if(lexer->current - lexer->start > 1) {
                switch(lexer->start[1]) {
                    case 'a': return check_keyword(lexer, 2, 3, "lse", H_TOKEN_FALSE);
                    case 'n': return check_keyword(lexer, 2, 0, "", H_TOKEN_FUNCTION);
                    case 'o': return check_keyword(lexer, 2, 1, "r", H_TOKEN_FOR);
                }
            }
            break;
        case 'i': return check_keyword(lexer, 1, 1, "f", H_TOKEN_IF);
        case 'n': return check_keyword(lexer, 1, 3, "ull", H_TOKEN_NULL);
        case 'o': return check_keyword(lexer, 1, 1, "r", H_TOKEN_OR);
        case 'p': return check_keyword(lexer, 1, 4, "rint", H_TOKEN_PRINT);
        case 'r': return check_keyword(lexer, 1, 5, "eturn", H_TOKEN_RETURN);
        case 's': return check_keyword(lexer, 1, 4, "uper", H_TOKEN_SUPER);
        case 'v': return check_keyword(lexer, 1, 2, "ar", H_TOKEN_VAR);
        case 'w': return check_keyword(lexer, 1, 4, "hile", H_TOKEN_WHILE);
    }

    return token_create(lexer, H_TOKEN_IDENTIFIER);
}

static token_t check_keyword(lexer_t* lexer, size_t start, size_t length, const char* string_to_match, token_type_t type) {
    if((size_t)(lexer->current - (lexer->start + start)) == length && memcmp(lexer->start + start, string_to_match, length) == 0) {
        return token_create(lexer, type);
    }
    return token_create(lexer, H_TOKEN_IDENTIFIER);
}

static void lexer_report_error(lexer_t* lexer, token_t* token, token_t* prev_token) {
    DEBUG_COLOR_SET(COLOR_RED);
    fprintf(stderr, "[ERROR | Line: %lld] %.*s\n", token->line, (int)token->length, token->start);
    
    const char* token_start = (prev_token) ? prev_token->start + prev_token->length : lexer->start;

    DEBUG_COLOR_SET(COLOR_GRAY);
    size_t lines_count_before = token->line - lexer->tokens_array[0].line;
    size_t lines_count_after = lexer->tokens_array[lexer->tokens_array_size - 1].line - token->line;

    DEBUG_LOG("Lines Before: %lld\n", lines_count_before);
    DEBUG_LOG("Lines After: %lld\n", lines_count_after);

    token_t* temp = lexer->tokens_array;

    while(temp->line != token->line - DEBUG_MIN(lines_count_before, 5)) {
        ++temp;
    }

    fprintf(stderr, "%.*s", (int)(token_start - temp->start), temp->start);
    while(temp != (token + 1)) {
        ++temp;
    }
    DEBUG_COLOR_SET(COLOR_RED);
    fprintf(stderr, "%.*s", (int)(temp->start - token_start), token_start);
    DEBUG_COLOR_RESET();

    
    fprintf(stderr, "%.*s", (int)(temp->start - token_start), token_start);
    
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

token_t lexer_get_token(lexer_t* lexer) {
    char c = *lexer->current++;

    #if DEBUG_TRACE_LEXER_CURRENT_CHAR
        DEBUG_LOG("Char: %c\n", c);
    #endif

    switch(c) {
        case '(': return token_create(lexer, H_TOKEN_LEFT_PAR);
        case ')': return token_create(lexer, H_TOKEN_RIGHT_PAR);
        case '{': return token_create(lexer, H_TOKEN_LEFT_CURLY);
        case '}': return token_create(lexer, H_TOKEN_RIGHT_CURLY);
        case ';': return token_create(lexer, H_TOKEN_SEMICOLON);
        case ':': return token_create(lexer, H_TOKEN_COLON);
        case ',': return token_create(lexer, H_TOKEN_COMMA);
        case '.': return token_create(lexer, H_TOKEN_DOT);
        case '%': return token_create(lexer, H_TOKEN_MODULO);
        case '/': return token_create(lexer, H_TOKEN_SLASH);
        case '>': return lexer_check_next(lexer, '=', H_TOKEN_GREATER_EQUAL, H_TOKEN_GREATER); 
        case '<': return lexer_check_next(lexer, '=', H_TOKEN_LESS_EQUAL, H_TOKEN_LESS); 
        case '+': return lexer_check_next(lexer, '+', H_TOKEN_PLUS_PLUS, H_TOKEN_PLUS); 
        case '-': return lexer_check_next(lexer, '-', H_TOKEN_MINUS_MINUS, H_TOKEN_MINUS);
        case '*': return lexer_check_next(lexer, '*', H_TOKEN_POW, H_TOKEN_STAR);
        case '=': return lexer_check_next(lexer, '=', H_TOKEN_DOUBLE_EQUAL, H_TOKEN_EQUAL);
        case '"': return lexer_string(lexer);
    }

    if(isdigit(c)) return lexer_number(lexer);

    if(isalpha(c)) return lexer_identifier(lexer);

    if(c != '\0') {
        DEBUG_ERROR("Unrecognised Token. Last recognised token: ");
        token_print(&lexer->tokens_array[lexer->tokens_array_size - 1]);
        return token_create_error(lexer, H_TOKEN_ERROR, "Unrecognised Token");
    }

    --lexer->current;
    return token_create(lexer, H_TOKEN_EOF);

}

size_t lexer_get_tokens_count(lexer_t* lexer) {
    return lexer->tokens_array_size;
}

unsigned int lexer_get_errors_count(lexer_t* lexer) {
    return lexer->errors_count;
}

void lexer_report_tokenisation_errors(lexer_t* lexer) {
    lexer->current = lexer->start;
    token_t* prev = NULL;
    for(token_t* temp = lexer->tokens_array; temp->type != H_TOKEN_EOF; ++temp) {
        if(temp->type == H_TOKEN_ERROR) {
            lexer_report_error(lexer, temp, prev);
        }
        prev = temp;
    }
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
