#include "include/main.h"


  int32_t tiziano_wdr_gamma_refresh()

{
    int32_t var_10_46 = 0;
    int32_t $a2_1 = tisp_gamma_param_array_get(0x3d, &wdr_gam_y129_array, &var_10_47);
    
    if (var_10_48 != 0x102)
    {
        isp_printf(2, "not support the gpio mode!\\n", $a2_1);
        return 0xffffffff;
    }
    
    void* $v0_1 = &wdr_gam_y129_array;
    
    for (int32_t i = 0; i != 0x84; )
    {
        uint32_t* $a2_2 = &wdr_gam_y33_array + i;
        i += 4;
        *$a2_2 = *$v0_1;
        $v0_1 += 8;
    }
    
    int32_t i_1 = data_b1594_4;
    
    if (!i_1)
    {
        do
        {
            int32_t $a2_4 = *(&wdr_gam_y33_array + i_1);
            void* $a1_1 = &param_wdr_gam_y_array_def + i_1;
            i_1 += 4;
            *$a1_1 = $a2_4;
        } while (i_1 != 0x84);
        
        return 0;
    }
    
    if (i_1 != 1)
        return 0;
    
    int32_t* $v0_2 = &param_wdr_gam_y_array_def;
    
    for (int32_t i_2 = 0; i_2 != 0x1000; )
    {
        *$v0_2 = i_2;
        i_2 += 0x80;
        $v0_2 = &$v0_2[1];
    }
    
    data_b22f8_3 = 0xfff;
    return 0;
}

