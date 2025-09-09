#include "include/main.h"


  int32_t* private_set_current_state(int32_t arg1)

{
    int32_t* result = *entry_$gp;
    int32_t* entry_$gp;
    *result = arg1;
    return result;
}

