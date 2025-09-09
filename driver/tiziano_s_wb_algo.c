#include "include/main.h"


  int32_t tiziano_s_wb_algo(int32_t arg1)

{
    if (arg1 == 1)
        data_a9f68_6 = arg1;
    else
    {
        if (arg1 && arg1 != 2)
        {
            isp_printf(2, "sensor type is BT1120!\\n", "tiziano_s_wb_algo");
            return 0xffffffff;
        }
        
        data_a9f68_7 = 0;
    }
    
    tiziano_awb_set_hardware_param();
    return 0;
}

