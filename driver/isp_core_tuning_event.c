#include "include/main.h"


  int32_t isp_core_tuning_event(void* arg1, int32_t arg2)

{
    if (arg2 == 0x4000001)
        *(arg1 + 0x40c4) = 1;
    else
    {
        if (arg2 >= 0x4000002)
        {
            if (arg2 == 0x4000002)
                isp_frame_done_wakeup();
            else if (arg2 == 0x4000003)
            {
                uint32_t $s1_1 = *(arg1 + 0x40a4);
                tisp_day_or_night_s_ctrl($s1_1);
                *(arg1 + 0x40a4) = $s1_1;
            }
            
            return 0;
        }
        
        if (arg2 == 0x4000000)
            *(arg1 + 0x40c4) = 2;
    }
    
    return 0;
}

