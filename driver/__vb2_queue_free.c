#include "include/main.h"


  int32_t __vb2_queue_free(void* arg1, int32_t arg2)

{
    int32_t $s1_1 = *(arg1 + 0x1e8) - arg2;
    int32_t var_20_18 = 0;
    void* $s2_1 = arg1 + ($s1_1 << 2);
    int32_t $v0_1;
    
    while (true)
    {
        $v0_1 = *(arg1 + 0x1e8);
        $s2_1 += 4;
        
        if ($s1_1 >= $v0_1)
            break;
        
        private_kfree(*($s2_1 + 0xe4));
        $s1_1 += 1;
        *($s2_1 + 0xe4) = 0;
    }
    
    *(arg1 + 0x1e8) = $v0_1 - arg2;
    *(arg1 + 0x1ec) = arg1 + 0x1ec;
    *(arg1 + 0x1f0) = arg1 + 0x1ec;
    __private_spin_lock_irqsave(arg1 + 0x200, &var_20_19);
    *(arg1 + 0x1f8) = arg1 + 0x1f8;
    *(arg1 + 0x1fc) = arg1 + 0x1f8;
    private_spin_unlock_irqrestore(arg1 + 0x200, var_20_20);
    return tx_isp_send_event_to_remote(*(arg1 + 0x298), 0x3000007, 0);
}

