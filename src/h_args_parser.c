#include "h_args_parser.h"

static int match_arg(size_t start, size_t count, const char* arg, const char* string_to_match);
static void consume_arg(const char* arg, size_t index, size_t argc, char** argv, const char** value_to_set);
static void consume_set_arg_flag(const char* arg, size_t index, size_t argc, char** argv, const char** value_to_set, uint8_t* flags, args_flags_t flag_to_set);
static inline void set_args_flag(uint8_t* args_flags, uint8_t bit_to_set);
static void show_help();

static void show_help() {
    DEBUG_COLOR_SET(COLOR_CYAN);
    DEBUG_LOG("%s -> %s", "--compile", "Compiles the source instead of intepreting it\n");
    DEBUG_LOG("%s -> %s", "--file \"path\"", "Uses the file at the specified path as source\n");
    DEBUG_LOG("%s -> %s", "--repl", "Launches the REPL\n");
    DEBUG_LOG("%s -> %s", "--save \"path\"", "Only used in REPL mode. Saves the input of the REPL in a file at the specified path\n");
    DEBUG_COLOR_RESET();
}

static int match_arg(size_t start, size_t count, const char* arg, const char* string_to_match) {
    return (strlen(arg) - start == count && memcmp(arg + start, string_to_match, count) == 0);
}

static void consume_arg(const char* arg, size_t index, size_t argc, char** argv, const char** value_to_set) {
    if(index + 1 > argc) {
        DEBUG_COLOR_SET(COLOR_RED);
        fprintf(stderr, "Invalid value for command line argument '%s'\n", arg);
        DEBUG_COLOR_RESET();
        exit(HASH_COMMAND_LINE_INVALID_ARG);
    }

    *value_to_set = argv[index];
}

static void consume_set_arg_flag(const char* arg, size_t index, size_t argc, char** argv, const char** value_to_set, uint8_t* flags, args_flags_t flag_to_set) {
    if(index + 1 > argc) {
        DEBUG_COLOR_SET(COLOR_RED);
        fprintf(stderr, "Invalid value for command line argument '%s'\n", arg);
        DEBUG_COLOR_RESET();
        exit(HASH_COMMAND_LINE_INVALID_ARG);
    }

    *value_to_set = argv[index];
    set_args_flag(flags, flag_to_set);
}

static inline void set_args_flag(uint8_t* args_flags, uint8_t bit_to_set) {
    DEBUG_LOG("Setting flag\n");
    *args_flags |= (1 << bit_to_set);
    //DEBUG_LOG("%.8d\n", *args_flags);
    DEBUG_LOG_BINARY_8(*args_flags);
}

void parse_args(int argc, char** argv, execution_mode_t* run_mode, const char** source_file_path, const char** repl_save_file_path, uint8_t* args_flags) {
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
                case 'c': if(match_arg(3, 6, argv[i], "ompile")) set_args_flag(args_flags, H_ARGS_FLAG_COMPILE); break;
                case 's': if(match_arg(3, 3, argv[i], "ave")) consume_set_arg_flag("save", i+1, argc, argv, repl_save_file_path, args_flags, H_ARGS_FLAG_SAVE_FILE); break;
                case 'l': if(match_arg(3, 3, argv[i], "ist")) show_help(); exit(0);
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