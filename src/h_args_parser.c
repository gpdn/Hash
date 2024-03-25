#include "h_args_parser.h"

static int match_arg(size_t start, size_t count, const char* arg, const char* string_to_math);
static void consume_arg(const char* arg, size_t index, size_t argc, char** argv, const char** value_to_set);

static int match_arg(size_t start, size_t count, const char* arg, const char* string_to_match) {
    return (strlen(arg) - start == count && memcmp(arg + start, string_to_match, count) == 0);
}

static void consume_arg(const char* arg, size_t index, size_t argc, char** argv, const char** value_to_set) {
    if(index > argc) {
        fprintf(stderr, "Invalid value for command line argument %s\n", arg);
        exit(HASH_COMMAND_LINE_INVALID_ARG);
    }

    *value_to_set = argv[index];
}

void parse_args(int argc, char** argv, execution_mode_t* run_mode, const char** source_file_path) {
    #if DEBUG_TRACE_LOG_COMMAND_LINE_ARGS
        DEBUG_PRINT_LINE();
        DEBUG_TITLE("Command line args");
        DEBUG_COLOR_SET(COLOR_CYAN);
        for(int i = 0; i < argc; ++i) {
            printf("%d - %s\n", i, argv[i]);
        }
        DEBUG_COLOR_RESET();
        DEBUG_PRINT_LINE();
    #endif

        #if DEBUG_TRACE_LOG_COMMAND_LINE_ARGS_OPTS
        DEBUG_TITLE("Command line options");
        DEBUG_COLOR_SET(COLOR_CYAN);
    #endif

    for(int i = 1; i < argc; ++i) {
        if(strncmp(argv[i], "--", 2) == 0) {
            #if DEBUG_TRACE_LOG_COMMAND_LINE_ARGS_OPTS
                DEBUG_LOG("Matching option: %s\n", argv[i]);
            #endif
            switch(argv[i][2]) {
                case 'r': if(match_arg(3, 3, argv[i], "epl")) {*run_mode = MODE_REPL;} break;
                case 'f': if(match_arg(3, 3, argv[i], "ile")) consume_arg("file", i+1, argc, argv, source_file_path); break;
            }
        }
    }

    #if DEBUG_TRACE_LOG_COMMAND_LINE_ARGS_OPTS
        DEBUG_COLOR_RESET(COLOR_CYAN);
    #endif

    if(argc < 2) {
        fprintf(stderr, "No path specified. Using test.hash\n");
        //return HASH_NOT_SOURCE_FILE_SPECIFIED;
    }
    
}