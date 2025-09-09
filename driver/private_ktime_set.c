#include "include/main.h"


  int32_t* private_ktime_set(int32_t* arg1, int32_t arg2, int32_t arg3)

{
    *arg1 = arg3;
    arg1[1] = arg2;
    return arg1;
}

