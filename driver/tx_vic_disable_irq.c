#include "include/main.h"


  void tx_vic_disable_irq()

{
    char* dump_vsd_2 = (char*)(dump_vsd); // Fixed void pointer assignment
    void* const dump_vsd_5 = nullptr;
    
    if (dump_vsd_2)
    {
        void* const dump_vsd_4 = dump_vsd_2;
        
        if ((uintptr_t)dump_vsd_2 >= 0xfffff001)
            dump_vsd_4 = nullptr;
        
        dump_vsd_5 = dump_vsd_4;
    }
    
    int32_t var_14 = 0;
    int32_t var_18_7 = 0;
    
    if (!(uintptr_t)dump_vsd_5 || (uintptr_t)dump_vsd_5 >= 0xfffff001)
        return;
    
    __private_spin_lock_irqsave(dump_vsd_2 + 0x130, &var_18_8);
    uint32_t dump_vsd_1 = dump_vsd;
    uint32_t dump_vsd_3;
    
    if (var_14_1)
        dump_vsd_3 = dump_vsd;
    else
    {
            int32_t $v0_2 = *(dump_vsd_5 + 0x88);
        dump_vsd_3 = dump_vsd;
        
        if (*(dump_vsd_1 + 0x13c))
        {
            *((int32_t*)((char*)dump_vsd_1 + 0x13c)) = 0; // Fixed void pointer dereference
            dump_vsd_3 = dump_vsd;
            
            if ($v0_2)
            {
                $v0_2(dump_vsd_5 + 0x80);
                dump_vsd_3 = dump_vsd;
            }
        }
    }
    
    private_spin_unlock_irqrestore(dump_vsd_3 + 0x130, var_18_9);
}

