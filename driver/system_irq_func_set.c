#include "include/main.h"


  int32_t system_irq_func_set(int32_t arg1, int32_t arg2)

{
    *((arg1 << 2) + &irq_func_cb) = arg2;
    return 0;
}

