#include "include/main.h"


  int32_t private_request_module(int32_t arg1, int32_t arg2, int32_t arg3)

{
    int32_t arg_8;
    int32_t* var_10 = &arg_8;
    uint32_t pfaces_1 = pfaces;
    int32_t $a3;
    int32_t arg_c = $a3;
    int32_t var_18 = arg2;
    int32_t** var_14 = &var_10;
    arg_8 = arg3;
    return (*(pfaces_1 + 0x130))(1, &$LC0, &var_18);
}

