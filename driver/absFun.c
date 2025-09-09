#include "include/main.h"

int32_t __attribute__((pure)) absFun(int32_t arg1, int32_t arg2)
{
    if (arg2 >= arg1)
        return arg2 - arg1;

    return arg1 - arg2;
}