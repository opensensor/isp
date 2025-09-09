#include "include/main.h"


  int32_t tisp_s_defog_enable(int32_t arg1)

{
    int32_t $v0 = system_reg_read(0xc);
    
    if ((($v0 >> 0xb ^ 1) & 1) == arg1)
        return 0;
    
    int32_t (* $v0_2)(int32_t arg1, int32_t arg2);
    int32_t sensor_info_1;
    int32_t $a1_2;
    
    if (arg1 != 1)
    {
        if (arg1)
        {
            isp_printf(2, 
                "width is %d, height is %d, imagesize is %d\n, snap num is %d, buf size is %d", 
                arg1);
            return 0xffffffff;
        }
        
        $a1_2 = $v0 | 0x800;
        sensor_info_1 = 0xc;
        $v0_2 = system_reg_write;
    }
    else
    {
        system_reg_write(0xc, $v0 & 0xfffff7ff);
        sensor_info_1 = sensor_info;
        $a1_2 = data_b2e1c;
        $v0_2 = tiziano_defog_init;
    }
    
    $v0_2(sensor_info_1, $a1_2);
    return 0;
}

