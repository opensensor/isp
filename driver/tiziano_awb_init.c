#include "include/main.h"


  int32_t tiziano_awb_init(int32_t arg1, int32_t arg2)

{
    awb_first = 0;
    memset(&tisp_wb_attr, 0, 0x1c);
    tiziano_awb_params_refresh();
    int32_t* $v0 = &_awb_parameter;
    int32_t* $a2 = &_awb_parameter;
    int32_t $a0 = 0;
    
    while (true)
    {
        int32_t $v1_1 = data_a9fc8_3;
        $a2 = &$a2[1];
        
        if ($a0 >= $v1_1)
            break;
        
        $a0 += 1;
        $a2[3] = (arg2 >> 1) / $v1_1;
    }
    
    int32_t $a1_1 = 0;
    
    while (true)
    {
        int32_t $v1_3 = data_a9fc0_3;
        $v0 = &$v0[1];
        
        if ($a1_1 >= $v1_3)
            break;
        
        $a1_1 += 1;
        $v0[0x12] = (arg1 >> 1) / $v1_3;
    }
    
    if (!awb_frz)
    {
        tiziano_awb_set_hardware_param();
        Tiziano_awb_set_gain(&_awb_mf_para, *_AwbPointPos, &_wb_static);
    }
    
    tisp_event_set_cb(0xa, JZ_Isp_Awb);
    system_irq_func_set(0x1e, awb_interrupt_static);
    return 0;
}

