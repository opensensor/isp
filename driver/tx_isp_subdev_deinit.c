#include "include/main.h"


  int32_t tx_isp_subdev_deinit(void* arg1)

{
    int32_t $a0_2 = *(arg1 + 0xcc);
    int32_t $a0_3 = *(arg1 + 0xd0);
    int32_t $a0_4 = *(arg1 + 0xb8);
    int32_t* $v0_1 = *(arg1 + 0xb4);
    int32_t $v0_2;
        int32_t $a0_5 = *$v0_1;
    if (*(arg1 + 0x30))
        private_misc_deregister(arg1 + 0xc);
    
    isp_subdev_release_clks(arg1);
    
    if ($a0_2)
        private_kfree($a0_2);
    
    
    if ($a0_3)
        private_kfree($a0_3);
    
    
    if ($a0_4)
        private_iounmap($a0_4);
    
    
    if (!$v0_1)
        $v0_2 = *(arg1 + 0x80);
    else
    {
        private_release_mem_region($a0_5, $v0_1[1] + 1 - $a0_5);
        *((int32_t*)((char*)arg1 + 0xb4)) = 0; // Fixed void pointer dereference
        $v0_2 = *(arg1 + 0x80);
    }
    
    if ($v0_2)
        tx_isp_free_irq(arg1 + 0x80);
    
    int32_t result = tx_isp_module_deinit(arg1);
    *((int32_t*)((char*)arg1 + 0xc4)) = 0; // Fixed void pointer dereference
    return result;
}

