#include "include/main.h"


  void tx_vic_disable_irq()

{
    void* dump_vsd_2 = dump_vsd;
    void* const dump_vsd_5 = nullptr;
    
    if (dump_vsd_2)
    {
        void* const dump_vsd_4 = dump_vsd_2;
        
        if (dump_vsd_2 >= 0xfffff001)
            dump_vsd_4 = nullptr;
        
        dump_vsd_5 = dump_vsd_4;
    }
    
    int32_t var_14 = 0;
    int32_t var_18_18 = 0;
    
    if (!dump_vsd_5 || dump_vsd_5 >= 0xfffff001)
        return;
    
    __private_spin_lock_irqsave(dump_vsd_2 + 0x130, &var_18_19);
    uint32_t dump_vsd_1 = dump_vsd;
    uint32_t dump_vsd_3;
    
    if (var_14_1)
        dump_vsd_3 = dump_vsd;
    else
    {
        dump_vsd_3 = dump_vsd;
        
        if (*(dump_vsd_1 + 0x13c))
        {
            *(dump_vsd_1 + 0x13c) = 0;
            int32_t $v0_2 = *(dump_vsd_5 + 0x88);
            dump_vsd_3 = dump_vsd;
            
            if ($v0_2)
            {
                $v0_2(dump_vsd_5 + 0x80);
                dump_vsd_3 = dump_vsd;
            }
        }
    }
    
    private_spin_unlock_irqrestore(dump_vsd_3 + 0x130, var_18_20);
}

