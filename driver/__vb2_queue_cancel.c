#include "include/main.h"


  void __vb2_queue_cancel(void* arg1)

{
    int32_t var_18 = 0;
    int32_t $a1_2 = var_18;
    char* $a0_5 = (char*)(arg1 + 0xe8); // Fixed void pointer assignment
    int32_t $v1_1 = 0;
        int32_t $v0 = $v1_1 < *(arg1 + 0x1e8) ? 1 : 0;
    
    if (!(uintptr_t)arg1 || (uintptr_t)arg1 >= 0xfffff001)
        return;
    
    if (*(arg1 + 0x20c) & 1)
        tx_isp_send_event_to_remote(*(arg1 + 0x298), 0x3000004, 0);
    
    *(arg1 + 0x20c) &= 0xfe;
    *((int32_t*)((char*)arg1 + 0x1ec)) = arg1 + 0x1ec; // Fixed void pointer dereference
    *((int32_t*)((char*)arg1 + 0x1f0)) = arg1 + 0x1ec; // Fixed void pointer dereference
    __private_spin_lock_irqsave(arg1 + 0x200, &var_18);
    *((int32_t*)((char*)arg1 + 0x1f8)) = arg1 + 0x1f8; // Fixed void pointer dereference
    *((int32_t*)((char*)arg1 + 0x1fc)) = arg1 + 0x1f8; // Fixed void pointer dereference
    *((int32_t*)((char*)arg1 + 0x1f4)) = 0; // Fixed void pointer dereference
    *((int32_t*)((char*)arg1 + 0x200)) = 0; // Fixed void pointer dereference
    private_spin_unlock_irqrestore(arg1 + 0x200, $a1_2);
    tx_isp_send_event_to_remote(*(arg1 + 0x298), 0x3000007, 0);
    private_wake_up_all(arg1 + 0x204);
    
    while (true)
    {
        $v1_1 += 1;
        
        if (!$v0)
            break;
        
        *(*$a0_5 + 0x48) = 0;
        $a0_5 += 4;
    }
}

