#include "include/main.h"


  int32_t isp_core_tuning_event(void* arg1, int32_t arg2)

{
                uint32_t $s1_1 = *(arg1 + 0x40a4);
    if ((uintptr_t)arg2 == 0x4000001)
        *((int32_t*)((char*)arg1 + 0x40c4)) = 1; // Fixed void pointer dereference
    else
    {
        if ((uintptr_t)arg2 >= 0x4000002)
        {
            if ((uintptr_t)arg2 == 0x4000002)
                isp_frame_done_wakeup();
            else if ((uintptr_t)arg2 == 0x4000003)
            {
                tisp_day_or_night_s_ctrl($s1_1);
                *((int32_t*)((char*)arg1 + 0x40a4)) = $s1_1; // Fixed void pointer dereference
            }
            
            return 0;
        }
        
        if ((uintptr_t)arg2 == 0x4000000)
            *((int32_t*)((char*)arg1 + 0x40c4)) = 2; // Fixed void pointer dereference
    }
    
    return 0;
}

