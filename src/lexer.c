#include "lexer.h"

static token_t token_create(lexer_t* lexer, token_type_t type);
static token_t token_create_error(lexer_t* lexer, token_type_t type, const char* message);
static void token_add(lexer_t* lexer, token_t type);
static token_t lexer_check_next(lexer_t* lexer, char c, token_type_t token_match, token_type_t token_mismatch);
static int lexer_skip(lexer_t* lexer);

static token_t token_create(lexer_t* lexer, token_type_t type) {
    token_t token;
    token.type = type;
    token.start = lexer->start;
    token.length = (size_t)(lexer->current - lexer->start);
    token.line = lexer->line;
    return token;
}

static token_t token_create_error(lexer_t* lexer, token_type_t type, const char* message) {
    token_t token;
    token.type = type;
    token.start = message;
    token.length = strlen(message);
    token.line = lexer->line;
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
    }

    if(c != '\0') {
        DEBUG_ERROR("Unrecognised Token. Last recognised token: ");
        token_print(&lexer->tokens_array[lexer->tokens_array_size - 1]);
        return token_create_error(lexer, H_TOKEN_ERROR, "Unrecognised Token");
    }

    --lexer->current;
    return token_create(lexer, H_TOKEN_EOF);

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

    //token_add(lexer, token_create(lexer, H_TOKEN_EOF));

    return lexer->tokens_array;
}
