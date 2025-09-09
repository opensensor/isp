#include "include/main.h"


  uint32_t tiziano_ct_ccm_interpolation(int32_t arg1, int32_t arg2)

{
    int32_t var_4 = $ra;
        else if (arg2 + 0xaf0 >= arg1)
    int32_t $ra;
    
    if (0x1388 - arg2 >= arg1)
    {
        uint32_t $v0_3;
        
        if (arg2 + 0xed8 < arg1)
            $v0_3 = 1;
        else if (0xed8 - arg2 < arg1)
            $v0_3 = 2;
            $v0_3 = 4;
        else
            $v0_3 = 3;
        
        ct_flag.31782 = $v0_3;
    }
    else
        ct_flag.31782 = 0;
    
    uint32_t ct_flag.31782_1;
    
    if (ccm_real)
        ct_flag.31782_1 = ct_flag.31782;
    else
    {
        ct_flag.31782_1 = ct_flag.31782;
        
        if (!ct_flag.31782_1)
        {
            ct_flag.31782_1 = ct_flag.31782;
            
            if (!ct_flag_last.31783)
                return ct_flag.31782_1;
        }
        else if (ct_flag.31782_1 == 2 || ct_flag.31782_1 == 4)
        {
            if (ct_flag_last.31783 == ct_flag.31782_1)
                return ct_flag.31782_1;
            
            ct_flag.31782_1 = ct_flag.31782;
        }
        else
            ct_flag.31782_1 = ct_flag.31782;
    }
    
    ct_flag_last.31783 = ct_flag.31782_1;
    
    switch (ct_flag.31782_1)
    {
        case 0:
        {
            memcpy(&ccm_parameter, &_ccm_d_parameter, 0x24);
            break;
        }
        case 1:
        {
            int32_t $v1_1 = arg2 + 0xed8 - arg1;
            int32_t $v0_10 = 0x1388 - arg2 - (arg2 + 0xed8);
                int32_t $v1_3 = *(&_ccm_t_parameter + i);
                int32_t $v0_13 = *(&_ccm_d_parameter + i);
                void* $v1_4 = &ccm_parameter + i;
            
            if (arg2 + 0xed8 < arg1)
                $v1_1 = arg1 - (arg2 + 0xed8);
            
            
            if (0x1388 - arg2 < arg2 + 0xed8)
                $v0_10 = arg2 + 0xed8 - (0x1388 - arg2);
            
            for (int32_t i = 0; (uintptr_t)i != 0x24; )
            {
                int32_t $v0_18;
                
                $v0_18 = $v0_13 >= $v1_3 ? ($v0_13 - $v1_3) * $v1_1 / $v0_10 + $v1_3
                    : $v1_3 - ($v1_3 - $v0_13) * $v1_1 / $v0_10;
                
                i += 4;
                *$v1_4 = $v0_18;
            }
            break;
        }
        case 2:
        {
            memcpy(&ccm_parameter, &_ccm_t_parameter, 0x24);
            break;
        }
        case 3:
        {
            int32_t $a0_1 = arg2 + 0xaf0 - arg1;
            int32_t $v1_5 = 0xed8 - arg2 - (arg2 + 0xaf0);
                int32_t $a1_5 = *(&_ccm_a_parameter + i_1);
                int32_t $v0_25 = *(&_ccm_t_parameter + i_1);
                void* $a1_6 = &ccm_parameter + i_1;
            
            if (arg2 + 0xaf0 < arg1)
                $a0_1 = arg1 - (arg2 + 0xaf0);
            
            
            if (0xed8 - arg2 < arg2 + 0xaf0)
                $v1_5 = arg2 + 0xaf0 - (0xed8 - arg2);
            
            for (int32_t i_1 = 0; (uintptr_t)i_1 != 0x24; )
            {
                int32_t $v0_30;
                
                $v0_30 = $v0_25 >= $a1_5 ? ($v0_25 - $a1_5) * $a0_1 / $v1_5 + $a1_5
                    : $a1_5 - ($a1_5 - $v0_25) * $a0_1 / $v1_5;
                
                i_1 += 4;
                *$a1_6 = $v0_30;
            }
            break;
        }
        case 4:
        {
            memcpy(&ccm_parameter, &_ccm_a_parameter, 0x24);
            break;
        }
    }
    
    /* tailcall */
    return memcpy(0xc5308, &ccm_parameter, 0x24);
}

