#include "include/main.h"


  int32_t apical_isp_hvflip_update(void* arg1, int32_t arg2)

{
    int32_t $v1 = *(arg1 + 0x134);
    char $s0_1;
    
    if ($v1 != 1)
        $s0_1 = arg2 & 0xff;
    else
    {
        int32_t $a3_1 = 1;
        int32_t $a2_1;
        
        if (arg2 == $v1)
            $a2_1 = 0;
        else
        {
            $a3_1 = 0;
            
            if (arg2 == 2)
                $a2_1 = 1;
            else if (arg2 != 3)
                $a2_1 = 0;
            else
            {
                $a3_1 = 1;
                $a2_1 = 1;
            }
        }
        
        tisp_lsc_hvflip(*(arg1 + 0x124), *(arg1 + 0x128), $a2_1, $a3_1);
        *(arg1 + 0x1ac) = arg2;
        *(arg1 + 0x1a8) = 1;
        $s0_1 = arg2 & 0xfd;
    }
    
    tisp_s_mscaler_hvflip_mask($s0_1);
    tisp_hv_flip_enable($s0_1);
    return 0;
}

