#include "include/main.h"


  int32_t tisp_event_init()

{
    int32_t* $v0 = &data_b2ff0;
    int32_t* $a2 = &data_b2ff0;
    data_b33b0 = &data_b33b0;
    data_b33b4 = &data_b33b0;
    data_b33b8 = &data_b33b8;
    data_b33bc = &data_b33b8;
    data_b2ff0 = &data_b2ff0;
    
    while (true)
    {
        $a2[1] = $a2;
        $a2 = &$a2[0xc];
        
        if ($a2 == &data_b33b0)
            break;
        
        *$a2 = $a2;
    }
    
    int32_t** $a2_1 = data_b33bc_1;
    
    while (true)
    {
        data_b33bc = $v0;
        *$v0 = &data_b33b8;
        $v0[1] = $a2_1;
        *$a2_1 = $v0;
        $v0 = &$v0[0xc];
        
        if ($v0 == &data_b33b0)
            break;
        
        $a2_1 = data_b33bc;
    }
    
    tevent_info = 0;
    __init_waitqueue_head(0xb2fe4, &$LC0, 0);
    return 0;
}

