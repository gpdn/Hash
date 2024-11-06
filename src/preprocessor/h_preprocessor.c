#include "h_preprocessor.h"

#define PREPROCESSOR_FILES_LIST_CAPACITY 5
#define PREPROCESSOR_ENVIRONMENT_CAPACITY 100
#define PREPROCESSOR_CONDITIONS_STACK_CAPACITY 10

static int preprocessor_resolve(h_preprocessor_t* preprocessor, const char* file);
static int preprocessor_resolve_directive(h_preprocessor_t* preprocessor);
static inline int preprocessor_file_push(h_preprocessor_t* preprocessor, const char* file);
static inline void preprocessor_files_list_push(h_preprocessor_t* preprocessor, const char* file);
static inline int preprocessor_check_directive(h_preprocessor_t* preprocessor, size_t start, size_t length, const char* string_to_match);
static void preprocessor_files_list_print(h_preprocessor_t* preprocessor);
static inline int resolve_directive_if(h_preprocessor_t* preprocessor);
static inline int resolve_directive_import(h_preprocessor_t* preprocessor);
static inline int resolve_directive_elif(h_preprocessor_t* preprocessor);
static inline int resolve_directive_else(h_preprocessor_t* preprocessor);
static inline int resolve_directive_endif(h_preprocessor_t* preprocessor);
static inline int resolve_directive_define(h_preprocessor_t* preprocessor);
static inline int resolve_directive_set(h_preprocessor_t* preprocessor);
static inline int resolve_directive_unset(h_preprocessor_t* preprocessor);
static inline void preprocessor_read_string_inline(h_preprocessor_t* preprocessor);
static inline h_preprocessor_value_t preprocessor_read_value_inline(h_preprocessor_t* preprocessor);
static int preprocessor_evaluate_value(h_preprocessor_t* preprocessor, h_preprocessor_value_t value);
static inline void preprocessor_advance_new_line(h_preprocessor_t* preprocessor);

static inline void preprocessor_read_string_inline(h_preprocessor_t* preprocessor) {    
    while(*preprocessor->current == ' ') ++preprocessor->current;
    preprocessor->start = preprocessor->current;
    while(isalnum(*preprocessor->current) || *preprocessor->current == '_') ++preprocessor->current;
}

static inline h_preprocessor_value_t preprocessor_read_value_inline(h_preprocessor_t* preprocessor) {
    while(*preprocessor->current == ' ') ++preprocessor->current;

    preprocessor->start = preprocessor->current;

    if(isdigit(*preprocessor->current)) {
        while(isdigit(*preprocessor->current)) ++preprocessor->current;
        return PREPROCESSOR_VALUE_NUM(strtod(preprocessor->start, NULL));
    }

    if(*preprocessor->current == '"') {
       ++preprocessor->current;
       while(*preprocessor->current != '\0' && *preprocessor->current != '"') ++preprocessor->current;
       h_string_t* name = h_string_init_hash(preprocessor->start, (size_t)(preprocessor->current - preprocessor->start));
       return PREPROCESSOR_VALUE_STRING(name); 
    }
    
    if(isalnum(*preprocessor->current) || *preprocessor->current == '_') {
       while(isalnum(*preprocessor->current) || *preprocessor->current == '_') ++preprocessor->current;
       h_string_t* name = h_string_init_hash(preprocessor->start, (size_t)(preprocessor->current - preprocessor->start));
       return PREPROCESSOR_VALUE_IDENTIFIER(name);
    }

    return PREPROCESSOR_VALUE_UNDEFINED();
}

static int preprocessor_evaluate_value(h_preprocessor_t* preprocessor, h_preprocessor_value_t value) {
    switch(value.type) {
        case H_PREPROCESSOR_VALUE_INT:
            return value.number != 0;
        case H_PREPROCESSOR_VALUE_STRING:
            return 1;
        case H_PREPROCESSOR_VALUE_IDENTIFIER:
            return preprocessor_evaluate_value(preprocessor, h_preprocessor_env_get(preprocessor->environment, value.string));
        default:
            return 0;
    }
}

static int preprocessor_resolve(h_preprocessor_t* preprocessor, const char* file) {
    return 1;
}

static inline void preprocessor_files_list_push(h_preprocessor_t* preprocessor, const char* file) {
    if(preprocessor->files_list_size >= preprocessor->files_list_capacity) {
        preprocessor->files_list_capacity *= 2;
        preprocessor->files_list = (const char**)realloc(preprocessor->files_list, sizeof(const char*) * preprocessor->files_list_capacity);
    }
    preprocessor->files_list[preprocessor->files_list_size++] = file;
}

static inline int preprocessor_file_push(h_preprocessor_t* preprocessor, const char* file) {
    #if DEBUG_TRACE_PREPROCESSOR_FILES_SET
        DEBUG_LOG("%s - %s\n", "Preprocessing file", file);
    #endif

    if(h_files_set_push(preprocessor->files_set, file) == 0) return 0;

    preprocessor_files_list_push(preprocessor, file);
    preprocessor_files_list_print(preprocessor);

    return 1;
}

static inline void preprocessor_advance_new_line(h_preprocessor_t* preprocessor) {
    ++preprocessor->current;
    while(*preprocessor->current == ' ' || *preprocessor->current == '\n') ++preprocessor->current;
    ++preprocessor->current;
}

static void preprocessor_files_list_print(h_preprocessor_t* preprocessor) {
    for(size_t i = 0; i < preprocessor->files_list_size; ++i) {
        printf("File %lld - %s\n", i, preprocessor->files_list[i]);
    }
}

static inline int preprocessor_check_directive(h_preprocessor_t* preprocessor, size_t start, size_t length, const char* string_to_match) {
    return ((size_t)(preprocessor->current - (preprocessor->start + start)) == length) 
            && (memcmp(preprocessor->start + start, string_to_match, length) == 0) 
            && ((size_t)(preprocessor->current - preprocessor->start)) == (start + length);
}

static inline int resolve_directive_if(h_preprocessor_t* preprocessor) {
    DEBUG_LOG("Resolving if\n");
    h_preprocessor_value_t value = preprocessor_read_value_inline(preprocessor);
    int result = preprocessor_evaluate_value(preprocessor, value);
    h_preprocessor_conditions_stack_push(preprocessor->conditions_stack, H_DIRECTIVE_IF, result);
    h_preprocessor_conditions_stack_print(preprocessor->conditions_stack);
    DEBUG_LOG("If evaluated: %d\n", result);
    return 1;
}

static inline int resolve_directive_import(h_preprocessor_t* preprocessor) {
    DEBUG_LOG("Resolving import\n");
    return 1;
}

static inline int resolve_directive_elif(h_preprocessor_t* preprocessor) {
    DEBUG_LOG("Resolving elif\n");
    return 1;
}

static inline int resolve_directive_else(h_preprocessor_t* preprocessor) {
    DEBUG_LOG("Resolving else\n");
    h_preprocessor_conditions_stack_update(preprocessor->conditions_stack, H_DIRECTIVE_ELSE, 0);
    return 1;
}

static inline int resolve_directive_endif(h_preprocessor_t* preprocessor) {
    DEBUG_LOG("Resolving endif\n");
    h_preprocessor_condition_t condition = h_preprocessor_conditions_stack_pop(preprocessor->conditions_stack);
    print_preprocessor_directive_no_newline(condition.directive);
    DEBUG_LOG("- Output enabled: %d\n", condition.output);
    return condition.directive != H_DIRECTIVE_UNKNOWN;
}

static inline int resolve_directive_define(h_preprocessor_t* preprocessor) {
    DEBUG_LOG("Resolving define\n");
    preprocessor_read_string_inline(preprocessor);
    h_string_t* name = h_string_init_hash(preprocessor->start, (size_t)(preprocessor->current - preprocessor->start));
    h_preprocessor_value_t value = preprocessor_read_value_inline(preprocessor);
    return h_preprocessor_env_define(preprocessor->environment, name, value);
}

static inline int resolve_directive_set(h_preprocessor_t* preprocessor) {
    DEBUG_LOG("Resolving set\n");
    preprocessor_read_string_inline(preprocessor);
    h_string_t* name = h_string_init_hash(preprocessor->start, (size_t)(preprocessor->current - preprocessor->start));
    h_preprocessor_value_t value = preprocessor_read_value_inline(preprocessor);
    return h_preprocessor_env_set(preprocessor->environment, name, value);
}

static inline int resolve_directive_unset(h_preprocessor_t* preprocessor) {
    DEBUG_LOG("Resolving unset\n");
    preprocessor_read_string_inline(preprocessor);
    h_string_t* name = h_string_init_hash(preprocessor->start, (size_t)(preprocessor->current - preprocessor->start));
    return h_preprocessor_env_unset(preprocessor->environment, name);
}

static inline int resolve_directive_print(h_preprocessor_t* preprocessor) {
    DEBUG_LOG("Resolving print\n");
    preprocessor_read_string_inline(preprocessor);
    h_string_t* name = h_string_init_hash(preprocessor->start, (size_t)(preprocessor->current - preprocessor->start));
    preprocessor_print_value(h_preprocessor_env_get(preprocessor->environment, name));
    return 1;
}

static int preprocessor_resolve_directive(h_preprocessor_t* preprocessor) {
    ++preprocessor->current;
    preprocessor->start = preprocessor->current;
    
    //while(*preprocessor->current != ' ' && *preprocessor->current != '\n' && *preprocessor->current != '\0') ++preprocessor->current;
    while(isalnum(*preprocessor->current) || *preprocessor->current == '_') ++preprocessor->current;

    DEBUG_LOG("Directive: %.*s\n", (int)(preprocessor->current - preprocessor->start), preprocessor->start);

    switch(*preprocessor->start) {
        case 'd':
            return preprocessor_check_directive(preprocessor, 1, 5, "efine") && resolve_directive_define(preprocessor);
            break;
        case 'e':
            if(preprocessor->current - preprocessor->start > 1) {
                switch(preprocessor->start[1]) {
                    case 'l':
                        if(preprocessor->current - preprocessor->start > 2) {
                            switch(preprocessor->start[2]) {
                                case 'i':
                                    return preprocessor_check_directive(preprocessor, 3, 1, "f") && resolve_directive_elif(preprocessor);
                                case 's':
                                    return preprocessor_check_directive(preprocessor, 3, 1, "e") && resolve_directive_else(preprocessor);
                                default:
                                    return 0;
                            }
                        }
                        return 0;
                    case 'n':    
                        return preprocessor_check_directive(preprocessor, 2, 3, "dif") && resolve_directive_endif(preprocessor);
                    default:
                        return 0;
                }
            }
            break;
        case 'i':
            if(preprocessor->current - preprocessor->start > 1) {
                switch(preprocessor->start[1]) {
                    case 'f':
                        return preprocessor_check_directive(preprocessor, 2, 0, "") && resolve_directive_if(preprocessor);
                    case 'm':  
                        return preprocessor_check_directive(preprocessor, 2, 4, "port") && resolve_directive_import(preprocessor);
                    default:
                        return 0;
                }
            }
            return 0;
            break;
        case 'p':
            return preprocessor_check_directive(preprocessor, 1, 4, "rint") && resolve_directive_print(preprocessor);
            break;
        case 's':
            return preprocessor_check_directive(preprocessor, 1, 2, "et") && resolve_directive_set(preprocessor);
            break;
        case 'u':
            return preprocessor_check_directive(preprocessor, 1, 4, "nset") && resolve_directive_unset(preprocessor);
            break;
        default:
            return 0;
    }

    return 1;
}

h_preprocessor_t* preprocessor_init(const char* source_path) {
    h_preprocessor_t* preprocessor = (h_preprocessor_t*)malloc(sizeof(h_preprocessor_t));
    preprocessor->files_set = h_files_set_init(50, 0.75);
    preprocessor->files_list_size = 0;
    preprocessor->files_list_capacity = PREPROCESSOR_FILES_LIST_CAPACITY;
    preprocessor->files_list = (const char**)malloc(sizeof(const char*) * PREPROCESSOR_FILES_LIST_CAPACITY);
    preprocessor->start = NULL;
    preprocessor->current = NULL;
    preprocessor->environment = h_preprocessor_env_init(PREPROCESSOR_ENVIRONMENT_CAPACITY, 0.75);
    preprocessor->conditions_stack = h_preprocessor_conditions_stack_init(PREPROCESSOR_CONDITIONS_STACK_CAPACITY);
    preprocessor_file_push(preprocessor, source_path);
    return preprocessor;
}

int preprocessor_run(h_preprocessor_t* preprocessor, const char* file) {
    
    DEBUG_LOG("File: %s\n", file);

    preprocessor->out_file = create_file("preprocessed", "wb");

    if(preprocessor->out_file == NULL) {
        DEBUG_LOG("Failed to create preprocessor output file");
        return 0;
    }

    preprocessor->start = file;
    preprocessor->current = file;

    DEBUG_LOG("HERE");
    DEBUG_LOG("Current %c\n", *preprocessor->current);

    while(*preprocessor->current != '\0' && preprocessor->files_list_size != 0) {
        if(*preprocessor->current == '#') {
            //append_file(preprocessor->out_file, preprocessor->start, sizeof(char), preprocessor->current - preprocessor->start);
            if(preprocessor_resolve_directive(preprocessor) == 0) {
                return 0;
            }
            preprocessor_advance_new_line(preprocessor);
            continue;
        }
        append_file(preprocessor->out_file, preprocessor->current, sizeof(char), 1);
        ++preprocessor->current;
    }

    if(preprocessor->conditions_stack->size > 0) {
        DEBUG_LOG("%s\n", "Missing #endif directive");
        return 0;
    }

    close_file(preprocessor->out_file);
    
    return 1;
}

void preprocessor_destroy(h_preprocessor_t* preprocessor) {
    h_files_set_free(preprocessor->files_set);
    h_preprocessor_env_free(preprocessor->environment);
    h_preprocessor_conditions_stack_free(preprocessor->conditions_stack);
    free(preprocessor);
}

#undef PREPROCESSOR_FILES_LIST_CAPACITY
