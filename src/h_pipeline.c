#include "h_pipeline.h"

interpreter_result_t pipeline_start(const char* source_path) {
    
    clock_t pipeline_timer = timer_start_time("Pipeline");
    
    const char* file_content = read_file("./test.hash");

    if(file_content == NULL) {
        fprintf(stderr, "Failed to open file. Path: %s", source_path);
        return HASH_FILE_OPEN_FAILED;
    }
    
    #if DEBUG_TRACE_LOG_FILE
        DEBUG_PRINT_LINE();
        DEBUG_TITLE("FILE");
        DEBUG_COLOR_SET(COLOR_BLUE);
        DEBUG_LOG("%s\n", file_content);
        DEBUG_COLOR_RESET();
        DEBUG_PRINT_LINE();
    #endif

    lexer_t* lexer = lexer_init(file_content); 

    token_t* tokens_array = lexer_tokenise(lexer);

    #if DEBUG_TRACE_LEXER_TOKEN
        DEBUG_PRINT_LINE();
        DEBUG_COLOR_SET(COLOR_CYAN);
        DEBUG_TITLE("Tokens List");
            for(token_t* temp = tokens_array; temp->type != H_TOKEN_EOF; ++temp) {
                token_print(temp);
            }
        DEBUG_COLOR_RESET();
        DEBUG_PRINT_LINE();
    #endif

    size_t tokens_count = lexer_get_tokens_count(lexer);
    unsigned int lexer_errors_count = lexer_get_errors_count(lexer);

    if(lexer_errors_count > 0) {
        lexer_report_tokenisation_errors(lexer);
        free((void*)file_content);
        free(tokens_array);
        lexer_free(lexer);
        return HASH_FAILURE;
    }

    icg_t* bytecode_generator = icg_init(tokens_array, tokens_count);

    bytecode_store_t* store = bs_init(50);

    /* disassemble_bytecode_store(store, "Bytecode Store");
    
    bs_write(store, OP_START);
    bs_write_constant(store, 10);
    bs_write_constant(store, 5);
    bs_write(store, OP_ADD);
    bs_write_constant(store, 10);
    bs_write(store, OP_SUB);
    bs_write(store, OP_NEGATE);
    bs_write(store, OP_RETURN);
    bs_write(store, OP_STOP);
 */
    virtual_machine_t* vm = vm_init(store);

    //interpreter_result_t result = vm_run(vm);

    free((void*)file_content);
    free(tokens_array);
    lexer_free(lexer);
    bs_free(store);
    vm_free(vm);

    timer_stop_log("Pipeline", pipeline_timer);

    return HASH_SUCCESS;
}