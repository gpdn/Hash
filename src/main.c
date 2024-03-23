
#include "headers.h"
#include "debug.h"
#include "h_error_codes.h"
#include "h_file.h"
#include "bytecode_store.h"
#include "virtual_machine.h"
#include "h_pipeline.h"

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
#endif

int main(int argc, char** argv) {

    PRINT_TITLE();

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

    if(argc < 2) {
        fprintf(stderr, "No path specified. Using test.hash\n");
        //return HASH_NOT_SOURCE_FILE_SPECIFIED;
    }

    const char* source_file_path = argv[1] ? argv[1] : PATH;

    interpreter_result_t result = pipeline_start(source_file_path);

    return 0;
}