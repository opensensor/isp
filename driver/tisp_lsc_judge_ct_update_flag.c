#include "include/main.h"


  int32_t tisp_lsc_judge_ct_update_flag()

{
    if (lsc_ct_update_flag != 1)
    {
        uint32_t $v0_1 = data_9a40c_2;
        
        if (data_9a410_3 >= $v0_1)
            data_9a408_1 = 0;
        else if (data_9a414_1 < $v0_1)
        {
            int32_t $a0_2;
            
            if (data_9a418_1 >= $v0_1)
                $a0_2 = 2;
            else if (data_9a41c_1 < $v0_1)
                $a0_2 = 4;
            else
                $a0_2 = 3;
            
            data_9a408_2 = $a0_2;
        }
        else
            data_9a408_3 = 1;
        
        int32_t $v1_1 = data_9a408_4;
        
        if ($v1_1 != data_9a404_1)
        {
            lsc_ct_last = $v0_1;
            data_9a404_2 = $v1_1;
            return 1;
        }
        
        uint32_t lsc_ct_last_1 = lsc_ct_last;
        
        if (!($v1_1 & 1))
            lsc_ct_last = $v0_1;
        else
        {
            int32_t $v1_4;
            
            $v1_4 = lsc_ct_last_1 >= $v0_1 ? lsc_ct_last_1 < $v0_1 + 0x1e ? 1 : 0
                : $v0_1 < lsc_ct_last_1 + 0x1e ? 1 : 0;
            
            if (!$v1_4)
            {
                lsc_ct_last = $v0_1;
                return 1;
            }
        }
    }
    
    return 0;
}

