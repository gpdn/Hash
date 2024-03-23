#include "h_pipeline.h"

interpreter_result_t pipeline_start(const char* source_path) {
    
    clock_t pipeline_timer = timer_start_time("Pipeline");
    
    const char* file_content = read_file("./test.hash");

    if(file_content == NULL) {
        fprintf(stderr, "Failed to open file. Path: %s", source_path);
        return HASH_FILE_OPEN_FAILED;
    }
    
    DEBUG_COLOR_SET(COLOR_BLUE);
    DEBUG_TITLE("FILE");
    DEBUG_LOG("%s\n", file_content);
    DEBUG_COLOR_RESET();
    DEBUG_PRINT_LINE();
    DEBUG_PRINT_LINE();

    lexer_t* lexer = lexer_init(file_content); 

    token_t* tokens_array = lexer_tokenise(lexer);

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
    lexer_free(lexer);
    bs_free(store);
    vm_free(vm);

    timer_stop_log("Pipeline", pipeline_timer);

    return HASH_SUCCESS;
}