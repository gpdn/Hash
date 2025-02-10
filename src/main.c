
#include "headers.h"
#include "debug.h"
#include "h_args_parser.h"
#include "h_error_codes.h"
#include "h_file.h"
#include "bytecode_store.h"
#include "virtual_machine.h"
#include "h_pipeline.h"
#include "h_hash_table_t.h"
#include "preprocessor/h_preprocessor.h"
#include "std/h_std.h"

#define PATH "./test.hash"

#if DEBUG
    #define PRINT_TITLE() \
    DEBUG_PRINT_LINE();\
    DEBUG_COLOR_SET(COLOR_GREEN);\
    printf("#################################################################\n");\
    printf("#                                                               #\n");\
    printf("#          #           #                                        #\n");\
    printf("#          #           #                                        #\n");\
    printf("#          #           #                                        #\n");\
    printf("#          #           #                                        #\n");\
    printf("#          #           #                                        #\n");\
    printf("#          #############                                        #\n");\
    printf("#          #           #  ########  ########   #      #         #\n");\
    printf("#          #           #  #      #  #          #      #         #\n");\
    printf("#          #           #  ########  ########   ########         #\n");\
    printf("#          #           #  #      #         #   #      #         #\n");\
    printf("#          #           #  #      #  ########   #      #         #\n");\
    printf("#                                                               #\n");\
    printf("#################################################################\n\n");\
    DEBUG_COLOR_RESET();\
    DEBUG_PRINT_LINE()
#else
    #define PRINT_TITLE()
#endif

static const char* execute_repl(char* source);

static const char* execute_repl(char* source) {
    
    #define HASH_REPL_BUFFER_SIZE 1024

    DEBUG_COLOR_SET(COLOR_GREEN);
    DEBUG_LOG("\nType # on a new line and press 'Enter' to exit repl.\n\n");
    DEBUG_COLOR_RESET();

    source = (char*)calloc(1, 1);
    char last_input[HASH_REPL_BUFFER_SIZE];

    DEBUG_COLOR_SET(COLOR_GREEN);
    DEBUG_LOG("#> ");
    DEBUG_COLOR_RESET();

    while(fgets(last_input, HASH_REPL_BUFFER_SIZE, stdin) && last_input[0] != '#') {
        source = (char*)realloc(source, strlen(source) + strlen(last_input) + 1);
        if(source == NULL) {
            DEBUG_LOG("Source Null\n");
        }
        strncat(source, last_input, strlen(last_input));
        DEBUG_COLOR_SET(COLOR_GREEN);
        DEBUG_LOG("\nCurrent Input: \n");
        DEBUG_COLOR_RESET();
        DEBUG_LOG("%s\n", source);

        DEBUG_COLOR_SET(COLOR_GREEN);
        DEBUG_LOG("#> ");
        DEBUG_COLOR_RESET();
    }

    return source;

    #undef HASH_REPL_BUFFER_SIZE
}


int main(int argc, char** argv) {

    PRINT_TITLE();

    execution_mode_t run_mode = MODE_FILE;
    const char* source_file_path = NULL;
    const char* repl_save_file_path = NULL;
    const char* file_content = NULL;
    uint8_t args_flags = 0;

    parse_args(argc, argv, &run_mode, &source_file_path, &repl_save_file_path, &args_flags);

    if(source_file_path == NULL) source_file_path = PATH;

    if(args_flags & (1 << H_ARGS_FLAG_COMPILE)) {
        DEBUG_LOG("Running in Compile Mode\n");
    }

    DEBUG_COLOR_SET(COLOR_BLUE);
    switch(run_mode) {
        case MODE_FILE:
            DEBUG_LOG("\nFile Mode\n");
            DEBUG_LOG("\nFile: %s\n", source_file_path);
            break;
        case MODE_REPL:
            DEBUG_LOG("\nRepl\n");
            file_content = execute_repl((char*)file_content);
            break;
        default:
            exit(HASH_INVALID_RUN_MODE);
    }
    DEBUG_COLOR_RESET();
    
    if(run_mode == MODE_REPL && args_flags & (1 << H_ARGS_FLAG_SAVE_FILE)) {
        DEBUG_LOG("Saving Repl input to %s\n", repl_save_file_path);
        write_file(repl_save_file_path, file_content, sizeof(char), strlen(file_content));
    }

    #if DEBUG_TIMERS
        clock_t preprocessor_timer = timer_start("Preprocessor");
    #endif
    h_std_t* std = h_std_init();

    h_preprocessor_t* preprocessor = preprocessor_init(source_file_path, std);

    if(preprocessor_run(preprocessor) == 0) return 0;

    preprocessor_destroy(preprocessor);

    #if DEBUG_TIMERS
        timer_stop_log("Preprocessor", preprocessor_timer, COLOR_CYAN);
    #endif

    const char* preprocessed_file_content = read_file("preprocessed");

    interpreter_result_t result = pipeline_start(preprocessed_file_content, args_flags, std);

    h_std_free(std);

    free((void*)preprocessed_file_content);
    free((void*)file_content);
    return 0;

}