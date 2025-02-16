#include "h_preprocessor.h"

#define PREPROCESSOR_FILES_LIST_CAPACITY 5
#define PREPROCESSOR_ENVIRONMENT_CAPACITY 100
#define PREPROCESSOR_CONDITIONS_STACK_CAPACITY 10

#define PREPROCESSOR_PRINT_ERROR(...) fprintf(stderr, "%s%s. File: %s at line %lld%s\n", COLOR_RED, __VA_ARGS__, preprocessor->files_list[preprocessor->files_list_size - 1], preprocessor->line, COLOR_RESET)
#define PREPROCESSOR_CHECK_ERROR(X, ...) \
        (X == 1) ? 1 : ((PREPROCESSOR_PRINT_ERROR(__VA_ARGS__)) && 0)

static int preprocessor_resolve(h_preprocessor_t* preprocessor, const char* file);
static int preprocessor_resolve_directive(h_preprocessor_t* preprocessor);
static inline int preprocessor_file_push(h_preprocessor_t* preprocessor, const char* file);
static inline void preprocessor_files_list_push(h_preprocessor_t* preprocessor, const char* file);
static inline int preprocessor_check_directive(h_preprocessor_t* preprocessor, size_t start, size_t length, const char* string_to_match);
static void preprocessor_files_list_print(h_preprocessor_t* preprocessor);
static inline int resolve_directive_if(h_preprocessor_t* preprocessor);
static inline int resolve_directive_import(h_preprocessor_t* preprocessor);
static inline int resolve_directive_std(h_preprocessor_t* preprocessor);
static int preprocessor_resolve_directive_std(h_preprocessor_t* preprocessor);
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
static int preprocessor_process_file(h_preprocessor_t* preprocessor);
static const char* preprocessor_filter_path(h_preprocessor_t* preprocessor, const char* path);

static const char* preprocessor_filter_path(h_preprocessor_t* preprocessor, const char* path) {
    int start = 0;
    int end = 0;
    int last_dot = 0;

    if(path[0] == '.') ++start;
    if(path[start] == '/') ++start;

    end = strlen(path);

    for(const char* c = path + end; c != path; --c) {
        if(*c == '.') last_dot = (path + end) - c;
    }

    int length = end - start - last_dot;

    char* filtered_path = (char*)malloc(sizeof(char) * (length + 6));
    
    strncpy(filtered_path, path + start, length);
    filtered_path[length] = '\0';
    strncat(filtered_path, ".hasha", 5);

    return filtered_path;
}

static inline void preprocessor_read_string_inline(h_preprocessor_t* preprocessor) {    
    while(*preprocessor->current == ' ') ++preprocessor->current;
    preprocessor->start = preprocessor->current;
    while(isalnum(*preprocessor->current) || *preprocessor->current == '_' || *preprocessor->current == '.') ++preprocessor->current;
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
       preprocessor->start = preprocessor->current;
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

    const char* filtered_path = preprocessor_filter_path(preprocessor, file);

    DEBUG_LOG("%s\n", filtered_path);

    if(h_files_set_push(preprocessor->files_set, filtered_path) == 0) return 0;

    preprocessor_files_list_push(preprocessor, filtered_path);

    #if DEBUG_TRACE_PREPROCESSOR_FILES_SET
        preprocessor_files_list_print(preprocessor);
    #endif

    return 1;
}

static inline void preprocessor_advance_new_line(h_preprocessor_t* preprocessor) {
    while(*preprocessor->current == ' ' || *preprocessor->current == '\t' || *preprocessor->current == '\r' || (*preprocessor->current == '\n' && ++preprocessor->line)) ++preprocessor->current;
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
    if(preprocessor->output_enabled == 0) {
        preprocessor->ignore_input = 1;
        h_preprocessor_conditions_stack_push(preprocessor->conditions_stack, H_DIRECTIVE_IF, 1, 0);
        return 1;
    }
    DEBUG_LOG("Resolving if\n");
    h_preprocessor_value_t value = preprocessor_read_value_inline(preprocessor);
    int result = preprocessor_evaluate_value(preprocessor, value);
    preprocessor->output_enabled = result;
    h_preprocessor_conditions_stack_push(preprocessor->conditions_stack, H_DIRECTIVE_IF, 0, result);
    h_preprocessor_conditions_stack_print(preprocessor->conditions_stack);
    DEBUG_LOG("If evaluated: %d\n", result);
    return 1;
}

static inline int resolve_directive_import(h_preprocessor_t* preprocessor) {
    DEBUG_LOG("Resolving import\n");
    preprocessor_read_string_inline(preprocessor);
    h_string_t* name = h_string_init_hash(preprocessor->start, (size_t)(preprocessor->current - preprocessor->start));
    DEBUG_LOG("File name: %s\n", name->string);
    if(preprocessor_file_push(preprocessor, name->string) == 0) {
        PREPROCESSOR_PRINT_ERROR("File", name->string, "already imported");
        return 0;
    }
    preprocessor->line = 0;
    return preprocessor_process_file(preprocessor);
}

static inline int resolve_directive_std(h_preprocessor_t* preprocessor) {
    DEBUG_LOG("Resolving std\n");
    preprocessor_read_string_inline(preprocessor);
    return preprocessor_resolve_directive_std(preprocessor);
}

static inline int resolve_directive_elif(h_preprocessor_t* preprocessor) {
    DEBUG_LOG("Resolving elif\n");
    return 1;
}

static inline int resolve_directive_else(h_preprocessor_t* preprocessor) {
    int result = h_preprocessor_conditions_stack_update(preprocessor->conditions_stack, H_DIRECTIVE_ELSE);
    if(result == -1) return PREPROCESSOR_CHECK_ERROR(0, "No matching #if for #else");
    preprocessor->ignore_input = result >> 1 & 1;
    if(preprocessor->ignore_input == 1) return 1;
    preprocessor->output_enabled = result & 1;
    return 1;
}

static inline int resolve_directive_endif(h_preprocessor_t* preprocessor) {
    //if(preprocessor->ignore_input == 1) return 1;
    DEBUG_LOG("Resolving endif\n");
    h_preprocessor_condition_t condition = h_preprocessor_conditions_stack_pop(preprocessor->conditions_stack);
    preprocessor->output_enabled = !condition.output;
    preprocessor->ignore_input = condition.input;
    return PREPROCESSOR_CHECK_ERROR(condition.directive != H_DIRECTIVE_UNKNOWN, "Unnecessary #endif. No conditional statements opened");
}

static inline int resolve_directive_define(h_preprocessor_t* preprocessor) {
    DEBUG_LOG("Resolving define\n");
    preprocessor_read_string_inline(preprocessor);
    h_string_t* name = h_string_init_hash(preprocessor->start, (size_t)(preprocessor->current - preprocessor->start));
    h_preprocessor_value_t value = preprocessor_read_value_inline(preprocessor);
    return PREPROCESSOR_CHECK_ERROR(h_preprocessor_env_define(preprocessor->environment, name, value), "Variable", name->string, "already defined");
}

static inline int resolve_directive_set(h_preprocessor_t* preprocessor) {
    DEBUG_LOG("Resolving set\n");
    preprocessor_read_string_inline(preprocessor);
    h_string_t* name = h_string_init_hash(preprocessor->start, (size_t)(preprocessor->current - preprocessor->start));
    h_preprocessor_value_t value = preprocessor_read_value_inline(preprocessor);
    return PREPROCESSOR_CHECK_ERROR(h_preprocessor_env_set(preprocessor->environment, name, value), "Unable to set undefined variable", name->string);
}

static inline int resolve_directive_unset(h_preprocessor_t* preprocessor) {
    DEBUG_LOG("Resolving unset\n");
    preprocessor_read_string_inline(preprocessor);
    h_string_t* name = h_string_init_hash(preprocessor->start, (size_t)(preprocessor->current - preprocessor->start));
    return PREPROCESSOR_CHECK_ERROR(h_preprocessor_env_unset(preprocessor->environment, name), "Unable to unset undefined variable", name->string);
}

static inline int resolve_directive_print(h_preprocessor_t* preprocessor) {
    DEBUG_LOG("Resolving print\n");
    preprocessor_read_string_inline(preprocessor);
    h_string_t* name = h_string_init_hash(preprocessor->start, (size_t)(preprocessor->current - preprocessor->start));
    preprocessor_print_value(h_preprocessor_env_get(preprocessor->environment, name));
    return 1;
}

static int preprocessor_resolve_directive_std(h_preprocessor_t* preprocessor) {
    switch(*preprocessor->start) {
        case 'a':
            return preprocessor_check_directive(preprocessor, 1, 2, "rr") && h_std_set_flag(preprocessor->std, H_STD_FLAG_ARR);
            break;
        case 'c':
            if(preprocessor->current - preprocessor->start > 1) {
                switch(preprocessor->start[1]) {
                    case 'm':
                        return preprocessor_check_directive(preprocessor, 2, 1, "d") && h_std_set_flag(preprocessor->std, H_STD_FLAG_CMD);
                        break;
                    case 'o':
                        return preprocessor_check_directive(preprocessor, 2, 2, "re") && h_std_set_flags(preprocessor->std, H_STD_FLAG_ARR | H_STD_FLAG_STR | H_STD_FLAG_TYPE);
                        break;
                    default:
                        return 0;
                }
            }
            return 0;
        case 'f':
            return preprocessor_check_directive(preprocessor, 1, 3, "ile") && h_std_set_flag(preprocessor->std, H_STD_FLAG_FILE);
            break;
        case 'i':
            return preprocessor_check_directive(preprocessor, 1, 1, "o") && h_std_set_flags(preprocessor->std, H_STD_FLAG_CMD | H_STD_FLAG_FILE | H_STD_FLAG_SYSTEM);
            break;
        case 's':
            return preprocessor_check_directive(preprocessor, 1, 2, "tr") && h_std_set_flag(preprocessor->std, H_STD_FLAG_STR);
            break;
        case 't':
            if(preprocessor->current - preprocessor->start > 1) {
                switch(preprocessor->start[1]) {
                    case 'i':
                        return preprocessor_check_directive(preprocessor, 2, 2, "me") && h_std_set_flag(preprocessor->std, H_STD_FLAG_TIME);
                        break;
                    case 'y':
                        return preprocessor_check_directive(preprocessor, 2, 2, "pe") && h_std_set_flag(preprocessor->std, H_STD_FLAG_TYPE);
                        break;
                    default:
                        return 0;
                }
            }
            return 0;
        case 'u':
            return preprocessor_check_directive(preprocessor, 1, 4, "tils") && h_std_set_flag(preprocessor->std, H_STD_FLAG_TIMER);
            break;
        default:
            return 0;
    }
    return 0;
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
            if(preprocessor->current - preprocessor->start > 1) {
                switch(preprocessor->start[1]) {
                    case 'e':
                        return preprocessor_check_directive(preprocessor, 2, 1, "t") && resolve_directive_set(preprocessor);
                    case 't':  
                        return preprocessor_check_directive(preprocessor, 2, 1, "d") && resolve_directive_std(preprocessor);
                    default:
                        return 0;
                }
            }
            return 0;
            break;
        case 'u':
            return preprocessor_check_directive(preprocessor, 1, 4, "nset") && resolve_directive_unset(preprocessor);
            break;
        default:
            return 0;
    }

    return 1;
}

h_preprocessor_t* preprocessor_init(const char* source_path, h_std_t* std) {
    h_preprocessor_t* preprocessor = (h_preprocessor_t*)malloc(sizeof(h_preprocessor_t));
    preprocessor->files_set = h_files_set_init(50, 0.75);
    preprocessor->files_list_size = 0;
    preprocessor->files_list_capacity = PREPROCESSOR_FILES_LIST_CAPACITY;
    preprocessor->files_list = (const char**)malloc(sizeof(const char*) * PREPROCESSOR_FILES_LIST_CAPACITY);
    preprocessor->start = NULL;
    preprocessor->current = NULL;
    preprocessor->line = 1;
    preprocessor->environment = h_preprocessor_env_init(PREPROCESSOR_ENVIRONMENT_CAPACITY, 0.75);
    preprocessor->conditions_stack = h_preprocessor_conditions_stack_init(PREPROCESSOR_CONDITIONS_STACK_CAPACITY);
    preprocessor->output_enabled = 1;
    preprocessor->ignore_input = 0;
    preprocessor->std = std;
    preprocessor_file_push(preprocessor, source_path);
    return preprocessor;
}

static int preprocessor_process_file(h_preprocessor_t* preprocessor) {
    const char* file = read_file(preprocessor->files_list[preprocessor->files_list_size - 1]);

    const char* pre_start = preprocessor->start;
    const char* pre_current = preprocessor->current;

    preprocessor->start = file;
    preprocessor->current = file;

    while(*preprocessor->current != '\0' && preprocessor->files_list_size != 0) {
        if(*preprocessor->current == '#') {
            if(preprocessor_resolve_directive(preprocessor) == 0) {
                DEBUG_LOG("Preprocessor failed");
                return 0;
            }
            preprocessor_advance_new_line(preprocessor);
            continue;
        }

        if(preprocessor->output_enabled) append_file(preprocessor->out_file, preprocessor->current, sizeof(char), 1);
        ++preprocessor->current;
    }

    preprocessor->start = pre_start;
    preprocessor->current = pre_current;

    return 1;
}

int preprocessor_run(h_preprocessor_t* preprocessor) {

    preprocessor->out_file = create_file("preprocessed", "wb");

    if(preprocessor->out_file == NULL) {
        DEBUG_LOG("Failed to create preprocessor output file");
        return 0;
    }

    if(preprocessor_process_file(preprocessor) == 0) {
        close_file(preprocessor->out_file);
        return 0;
    }

    if(preprocessor->conditions_stack->size > 0) {
        DEBUG_LOG("%s\n", "Missing #endif directive");
        close_file(preprocessor->out_file);
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
#undef PREPROCESSOR_ENVIRONMENT_CAPACITY
#undef PREPROCESSOR_CONDITIONS_STACK_CAPACITY
#undef PREPROCESSOR_PRINT_ERROR
#undef PREPROCESSOR_CHECK_ERROR