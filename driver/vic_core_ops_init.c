#include "include/main.h"

// Function implementation for vic_core_ops_init
int vic_core_ops_init(void* arg1, void* arg2)
{
    if (!arg1 || (uintptr_t)arg1 >= 0xfffff001)
    {
        return 0xffffffea;
    }
    
    if (!(uintptr_t)arg1 || (uintptr_t)arg1 >= 0xfffff001)
    {
        isp_printf();
    }

    char* $s1_1 = (char*)arg1 + 0xd4;
    int32_t $v0_2 = *(int32_t*)($s1_1 + 0x128);
    int32_t result;

    if (!(uintptr_t)arg2)
    {
        result = 0;

        if ($v0_2 != 2)
        {
            tx_vic_disable_irq();
            *(int32_t*)($s1_1 + 0x128) = 2;
        }
    }
    else
    {
        result = 0;

        if ($v0_2 != 3)
        {
            tx_vic_enable_irq();
            *(int32_t*)($s1_1 + 0x128) = 3;
        }
    }

    return result;
}