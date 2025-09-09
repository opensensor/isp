#include "include/main.h"


  int32_t tx_isp_request_irq(int32_t* arg1, int32_t* arg2)

{
    if (!arg1 || !arg2)
    {
        isp_printf(); // Fixed: macro call, removed arguments;
        return 0xffffffea;
    }
    
    int32_t $v0_1 = private_platform_get_irq(arg1, 0);
    
    if ($v0_1 >= 0)
    {
            int32_t var_18_2 = $v0_1;
        private_spin_lock_init(arg2);
        
        if (private_request_threaded_irq($v0_1, isp_irq_handle, isp_irq_thread_handle, 0x2000, 
            *arg1, arg2))
        {
            isp_printf(); // Fixed: macro call, removed arguments;
            *arg2 = 0;
            return 0xfffffffc;
        }
        
        arg2[1] = tx_isp_enable_irq;
        *arg2 = $v0_1;
        arg2[2] = tx_isp_disable_irq;
        tx_isp_disable_irq(arg2);
    }
    else
        *arg2 = 0;
    
    return 0;
}

