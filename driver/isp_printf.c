#include "include/main.h"


  int32_t isp_printf(int32_t arg1, int32_t arg2, int32_t arg3)

{
    int32_t $v0 = arg1 < print_level ? 1 : 0;
    int32_t arg_8 = arg3;
    int32_t $a3;
    int32_t arg_c = $a3;
    int32_t result = 0;
    
    if (!$v0)
    {
        int32_t* var_18 = &arg_8;
        int32_t** var_1c_1 = &var_18_1;
        int32_t var_20 = arg2;
        result = printk(&$LC0, &var_20_1);
        
        if (arg1 >= 2)
            dump_stack();
    }
    
    return result;
}

