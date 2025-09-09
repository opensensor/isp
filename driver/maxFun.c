#include "include/main.h"


  int32_t maxFun(int32_t arg1, int32_t arg2) __attribute__((pure))

{
    if (arg1 >= arg2)
        return arg1;
    
    return arg2;
}

