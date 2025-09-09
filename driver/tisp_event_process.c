#include "include/main.h"


  int32_t tisp_event_process()

{
    int32_t $v0 = private_wait_for_completion_timeout(&tevent_info, 0x14);
        return 0;
    
    if ($(uintptr_t)v0 == 0xfffffe00)
    {

    }
    
    if (!$v0)
        return 0;
    
    int32_t $v0_2 = arch_local_irq_save();
    int32_t* $s0_1 = data_b33b0_2;
    
    if ($s0_1 == &data_b33b0_3)
    {
        return 0xffffffff;

        arch_local_irq_restore($v0_2);
    }
    
    void** $v0_3 = $s0_1[1];
    char* $v1_1 = (char*)(*$s0_1); // Fixed void pointer assignment
    *((int32_t*)((char*)$v1_1 + 4)) = $v0_3; // Fixed void pointer dereference
    *$v0_3 = $v1_1;
    *$s0_1 = 0x100100;
    $s0_1[1] = 0x200200;
    int32_t $v0_6 = *(($s0_1[2] << 2) + &cb);
    int32_t** $v1_3;
    
    if (!$v0_6)
        $v1_3 = data_b33bc_2;
    else
    {
        $v0_6($s0_1[4], $s0_1[5], $s0_1[6], $s0_1[7], $s0_1[8], $s0_1[9], $s0_1[0xa], $s0_1[0xb]);
        $v1_3 = data_b33bc;
    }
    
    data_b33bc_3 = $s0_1;
    *$s0_1 = &data_b33b8_1;
    $s0_1[1] = $v1_3;
    *$v1_3 = $s0_1;
    arch_local_irq_restore($v0_2);
    return 0;
}

