#include "include/main.h"


  int32_t tiziano_adr_gamma_refresh()

{
    uint32_t adr_wdr_en_1 = adr_wdr_en;
    int32_t var_18 = 0;
    int32_t $a0 = 0x3c;
    int32_t $a2_1 = tisp_gamma_param_array_get($a0, &param_adr_gam_y_array_def, &var_18);
        return 0xffffffff;
    
    if (adr_wdr_en_1)
        $a0 = 0x3d;
    
    
    if ((uintptr_t)var_18 != 0x102)
    {
        isp_printf(); // Fixed: macro with no parameters, removed 5 arguments;
    }
    
    char* $a3_1 = (char*)(&histSub_4096_out); // Fixed void pointer assignment
    uint32_t $t5_1 = data_af514_1;
    uint32_t param_adr_gam_x_array_1 = param_adr_gam_x_array;
    uint32_t* $t0_1 = &histSub_4096_out;
    
    for (int32_t i = 0; (uintptr_t)i != 0x24; )
    {
        char* $a2_2 = (char*)(&param_adr_gam_y_array_def); // Fixed void pointer assignment
        int32_t $v0_3 = *(&histSub_4096 + i);
        int32_t $v1_1 = 0;
            uint32_t $a0_1 = *$a2_2;
                    int32_t $a1_2 = ($v1_1 - 1) << 1;
                    uint32_t $a2_5 = (&param_adr_gam_x_array)[$v1_1 - 1];
                    uint32_t $v1_4 = (&param_adr_gam_x_array)[$v1_1];
                    uint32_t $a2_6;
                        uint32_t $a1_5 = *(&param_adr_gam_y_array_def + $a1_2);
                        int32_t $v0_7 = $a1_5 - $v0_3;
                        uint32_t $a0_3 = $a0_1 - $a1_5;
        
        while (true)
        {
            
            if ($a0_1 >= $v0_3)
            {
                if ($v1_1)
                {
                    
                    if ($v1_4 >= $a2_5)
                    {
                        
                        if ($a1_5 < $v0_3)
                            $v0_7 = $v0_3 - $a1_5;
                        
                        
                        if ($a0_1 < $a1_5)
                            $a0_3 = $a1_5 - $a0_1;
                        
                        $a2_6 = $v0_7 * ($v1_4 - $a2_5) / $a0_3 + $a2_5;
                    }
                    else
                    {
                        uint32_t $a1_4 = *(&param_adr_gam_y_array_def + $a1_2);
                        int32_t $v0_4 = $a1_4 - $v0_3;
                        uint32_t $a0_2 = $a0_1 - $a1_4;
                        
                        if ($a1_4 < $v0_3)
                            $v0_4 = $v0_3 - $a1_4;
                        
                        
                        if ($a0_1 < $a1_4)
                            $a0_2 = $a1_4 - $a0_1;
                        
                        $a2_6 = $a2_5 - $v0_4 * ($a2_5 - $v1_4) / $a0_2;
                    }
                    
                    *$t0_1 = $a2_6;
                }
                else
                    *$t0_1 = param_adr_gam_x_array_1;
                
                break;
            }
            
            $v1_1 += 1;
            $a2_2 += 2;
            
            if ($(uintptr_t)v1_1 == 0x81)
            {
                *$t0_1 = $t5_1;
                break;
            }
        }
        
        uint32_t* $v0_10 = &adr_tm_base_lut + i;
        i += 4;
        *$v0_10 = *$t0_1;
        $t0_1 = &$t0_1[1];
    }
    
    for (int32_t i_1 = 0; (uintptr_t)i_1 != 0x20; )
    {
        char* $a2_7 = (char*)(&histSub_4096_diff + i_1); // Fixed void pointer assignment
        i_1 += 4;
        *((int32_t*)$a2_7) = *($a3_1 + 4) - *$a3_1; // Fixed void pointer dereference
        $a3_1 += 4;
    }
    
    return 0;
}

