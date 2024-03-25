
#include "headers.h"
#include "debug.h"
#include "h_args_parser.h"
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

    execution_mode_t run_mode = MODE_FILE;
    const char* source_file_path = PATH;

    parse_args(argc, argv, &run_mode, &source_file_path);

    printf("%d", run_mode);

    interpreter_result_t result = pipeline_start(source_file_path);

    return 0;
}