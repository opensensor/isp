#include "include/main.h"


  int32_t tiziano_adr_gamma_refresh()

{
    uint32_t adr_wdr_en_1 = adr_wdr_en;
    int32_t var_18_89 = 0;
    int32_t $a0 = 0x3c;
    
    if (adr_wdr_en_1)
        $a0 = 0x3d;
    
    int32_t $a2_1 = tisp_gamma_param_array_get($a0, &param_adr_gam_y_array_def, &var_18_90);
    
    if (var_18_91 != 0x102)
    {
        isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", $a2_1);
        return 0xffffffff;
    }
    
    void* $a3_1 = &histSub_4096_out;
    uint32_t $t5_1 = data_af514_1;
    uint32_t param_adr_gam_x_array_1 = param_adr_gam_x_array;
    uint32_t* $t0_1 = &histSub_4096_out;
    
    for (int32_t i = 0; i != 0x24; )
    {
        void* $a2_2 = &param_adr_gam_y_array_def;
        int32_t $v0_3 = *(&histSub_4096 + i);
        int32_t $v1_1 = 0;
        
        while (true)
        {
            uint32_t $a0_1 = *$a2_2;
            
            if ($a0_1 >= $v0_3)
            {
                if ($v1_1)
                {
                    int32_t $a1_2 = ($v1_1 - 1) << 1;
                    uint32_t $a2_5 = (&param_adr_gam_x_array)[$v1_1 - 1];
                    uint32_t $v1_4 = (&param_adr_gam_x_array)[$v1_1];
                    uint32_t $a2_6;
                    
                    if ($v1_4 >= $a2_5)
                    {
                        uint32_t $a1_5 = *(&param_adr_gam_y_array_def + $a1_2);
                        int32_t $v0_7 = $a1_5 - $v0_3;
                        
                        if ($a1_5 < $v0_3)
                            $v0_7 = $v0_3 - $a1_5;
                        
                        uint32_t $a0_3 = $a0_1 - $a1_5;
                        
                        if ($a0_1 < $a1_5)
                            $a0_3 = $a1_5 - $a0_1;
                        
                        $a2_6 = $v0_7 * ($v1_4 - $a2_5) / $a0_3 + $a2_5;
                    }
                    else
                    {
                        uint32_t $a1_4 = *(&param_adr_gam_y_array_def + $a1_2);
                        int32_t $v0_4 = $a1_4 - $v0_3;
                        
                        if ($a1_4 < $v0_3)
                            $v0_4 = $v0_3 - $a1_4;
                        
                        uint32_t $a0_2 = $a0_1 - $a1_4;
                        
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
            
            if ($v1_1 == 0x81)
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
    
    for (int32_t i_1 = 0; i_1 != 0x20; )
    {
        void* $a2_7 = &histSub_4096_diff + i_1;
        i_1 += 4;
        *$a2_7 = *($a3_1 + 4) - *$a3_1;
        $a3_1 += 4;
    }
    
    return 0;
}

