#include "include/main.h"


  uint32_t* defog_wei_interpcot(int32_t arg1, int32_t arg2, void* arg3, void* arg4, uint32_t* arg5)

{
    void var_98_129;
    memset(&var_98_130, 0, 0x80);
    int32_t var_118_11[0x20];
    memset(&var_118_12, 0, 0x80);
    tiziano_defog_set_reg_params();
    int32_t $v1 = 0;
    int32_t $t0 = 0;
    
    while (true)
    {
        int32_t* $a2 = arg3 + $v1;
        
        if ($t0 == arg1)
            break;
        
        int32_t* $a1 = arg4 + $v1;
        int32_t $a0_2 = 0;
        
        while ($a0_2 != arg2)
        {
            void* $v0_3 = &var_118_13[*$a2];
            int32_t $t2_1 = *$a1;
            *($v0_3 + 0x80) += 1;
            $a0_2 += 1;
            *$v0_3 += $t2_1;
            $a2 = &$a2[1];
            $a1 = &$a1[1];
        }
        
        $t0 += 1;
        $v1 += arg2 << 2;
    }
    
    uint32_t* result = arg5;
    void* $a2_1 = &var_98_131;
    int32_t (* $v1_1)[0x20] = &var_118_14;
    
    for (int32_t i = 0; i != 0x20; )
    {
        int32_t $a3_1 = *$a2_1;
        
        if (!$a3_1)
            *result = 0;
        else
        {
            uint32_t $lo_1 = (($a3_1 >> 1) + *$v1_1) / $a3_1;
            
            if ($lo_1 >= 0x20)
                *result = 0x1f;
            else
                *result = $lo_1;
        }
        
        if (!i)
            i += 1;
        else
        {
            uint32_t $a1_3 = *(result - 4);
            
            if (*result < $a1_3)
                *result = $a1_3;
            
            i += 1;
        }
        
        $a2_1 += 4;
        result = &result[1];
        $v1_1 = &(*$v1_1)[1];
    }
    
    return result;
}

