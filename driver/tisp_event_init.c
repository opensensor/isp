#include "include/main.h"


  int32_t tisp_event_init()

{
    int32_t* $v0 = &data_b2ff0_1;
    data_b33b0_1 = &data_b33b0_2;
    data_b33b4_1 = &data_b33b0_3;
    data_b33b8_1 = &data_b33b8_2;
    data_b33bc_1 = &data_b33b8_3;
    int32_t* $a2 = &data_b2ff0_2;
    data_b2ff0_3 = &data_b2ff0_4;
    
    while (true)
    {
        $a2[1] = $a2;
        $a2 = &$a2[0xc];
        
        if ($a2 == &data_b33b0_4)
            break;
        
        *$a2 = $a2;
    }
    
    int32_t** $a2_1 = data_b33bc_2;
    
    while (true)
    {
        data_b33bc_3 = $v0;
        *$v0 = &data_b33b8_4;
        $v0[1] = $a2_1;
        *$a2_1 = $v0;
        $v0 = &$v0[0xc];
        
        if ($v0 == &data_b33b0_5)
            break;
        
        $a2_1 = data_b33bc_4;
    }
    
    tevent_info = 0;
    __init_waitqueue_head(0xb2fe4, &$LC0, 0);
    return 0;
}

