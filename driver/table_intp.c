#include "include/main.h"


  int32_t table_intp(int32_t arg1, int32_t* arg2, int32_t arg3, int32_t arg4)

{
    if (arg2[1] >= arg4)
        return *arg2;
    
    uint32_t $v0_1 = 1;
    void* $t0_2;
    int32_t $t1_1;
    
    while (true)
    {
        if ($v0_1 >= arg3)
            return *(&arg2[arg3 * 2] - 8);
        
        $t0_2 = &arg2[$v0_1 * 2];
        $t1_1 = *($t0_2 + 4);
        
        if ($t1_1 >= arg4)
            break;
        
        $v0_1 = $v0_1 + 1;
    }
    
    void* $a1 = &arg2[($v0_1 - 1) * 2];
    int32_t var_c_3 = arg4;
    return fix_point_intp(arg1, *($a1 + 4), $t1_1, *$a1, *$t0_2);
}

