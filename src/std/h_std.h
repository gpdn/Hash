#ifndef H_STD
#define H_STD

#include "h_std_flags.h"
#include "../h_values.h"
#include "../h_locals_stack.h"
#include "../h_ht_enums.h"
#include "../h_ht_types.h"

typedef int (h_std_import_fn)(h_locals_stack_t* locals_stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table);

#include "h_nfi.h"
#include "h_std_io.h"
#include "h_std_core.h"
#include "h_std_utils.h"
#include "h_std_math.h"
#include "h_std_ds.h"

typedef struct h_std_t {
    size_t import_flags;
} h_std_t;

h_std_t* h_std_init();
int h_std_set_flag(h_std_t* std, h_std_import_flag_t flag);
int h_std_set_flags(h_std_t* std, size_t bitmask);
int h_std_resolve_imports(h_std_t* std, h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table);
void h_std_free(h_std_t* std);

#endif