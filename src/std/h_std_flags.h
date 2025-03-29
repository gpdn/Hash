#ifndef H_STD_IMPORT_FLAGS
#define H_STD_IMPORT_FLAGS

#include <stdint.h>

typedef enum h_std_import_flag_t {
    H_STD_FLAG_CMD,
    H_STD_FLAG_FILE,
    H_STD_FLAG_STR,
    H_STD_FLAG_ARR,
    H_STD_FLAG_TYPE,
    H_STD_FLAG_TIME,
    H_STD_FLAG_TIMER,
    H_STD_FLAG_SYSTEM,
    H_STD_FLAG_MATH,
    H_STD_FLAG_DS_SLL,
    H_STD_FLAG_DS_DLL,
    H_STD_FLAG_DS_STACK,
    H_STD_FLAG_DS_MAP,
    H_STD_FLAG_DS_QUEUE
} h_std_import_flag_t;

const char* h_std_resolve_import_flag(size_t flag);

#endif