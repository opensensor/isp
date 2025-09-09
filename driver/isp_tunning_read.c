#include "include/main.h"


  int32_t isp_tunning_read(void* arg1, int32_t arg2, int32_t arg3)

{
    int32_t $v0 = 0xfffffff5;
        int32_t $s0_1 = arg3;
    
    if (!(*(arg1 + 0x1c) & 0x80) && *tispPollValue)
    {
        void* entry_$gp;
        
        if (!(((arg2 + arg3) | arg2 | arg3) & *(entry_$gp + 0x18)))
        {
            __might_sleep("VIC_CTRL : %08x\n", 0xc9, 0);
            arg3 = __copy_user(arg2, &tispPollValue, $s0_1);
        }
        
        $v0 = 0xfffffff2;
        
        if (!(uintptr_t)arg3)
        {
            return $s0_1;
            *tispPollValue = 0;
        }
    }
    
    return $v0;
}

