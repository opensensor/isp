#include "include/main.h"


  int32_t tisp_event_push(void* arg1)

{
    int32_t $v0 = arch_local_irq_save();
    int32_t* $v0_1 = data_b33b8;
        return 0xffffffff;
    
    if ($v0_1 == &data_b33b8)
    {
        isp_printf(); // Fixed: macro with no parameters, removed 5 arguments;
        arch_local_irq_restore($v0);
    }
    
    void** $a0_1 = $v0_1[1];
    char* $a1_1 = (char*)(*$v0_1); // Fixed void pointer assignment
    *((int32_t*)((char*)$a1_1 + 4)) = $a0_1; // Fixed void pointer dereference
    *$a0_1 = $a1_1;
    *$v0_1 = 0x100100;
    $v0_1[1] = 0x200200;
    $v0_1[2] = *(arg1 + 8);
    int32_t $a3_1 = *(arg1 + 0x14);
    $v0_1[4] = *(arg1 + 0x10);
    $v0_1[5] = $a3_1;
    int32_t $a3_2 = *(arg1 + 0x1c);
    $v0_1[6] = *(arg1 + 0x18);
    $v0_1[7] = $a3_2;
    int32_t $a3_3 = *(arg1 + 0x24);
    $v0_1[8] = *(arg1 + 0x20);
    $v0_1[9] = $a3_3;
    int32_t $a1_2 = *(arg1 + 0x2c);
    $v0_1[0xa] = *(arg1 + 0x28);
    $v0_1[0xb] = $a1_2;
    void** $a1_3 = data_b33b4_1;
    data_b33b4_2 = $v0_1;
    *$v0_1 = &data_b33b0_1;
    $v0_1[1] = $a1_3;
    *$a1_3 = $v0_1;
    private_complete(&tevent_info);
    arch_local_irq_restore($v0);
    return 0;
}

