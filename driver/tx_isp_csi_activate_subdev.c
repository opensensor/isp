#include "include/main.h"


  int32_t tx_isp_csi_activate_subdev(void* arg1)

{
    int32_t result = 0xffffffea;
    
    if (arg1)
    {
        if (arg1 >= 0xfffff001)
            return 0xffffffea;
        
        void* $s1_1 = *(arg1 + 0xd4);
        result = 0xffffffea;
        
        if ($s1_1 && $s1_1 < 0xfffff001)
        {
            private_mutex_lock($s1_1 + 0x12c);
            
            if (*($s1_1 + 0x128) == 1)
            {
                *($s1_1 + 0x128) = 2;
                int32_t* $s1_2 = *(arg1 + 0xbc);
                
                if ($s1_2)
                {
                    int32_t i = 0;
                    
                    if ($s1_2 < 0xfffff001)
                    {
                        while (i < *(arg1 + 0xc0))
                        {
                            private_clk_enable(*$s1_2);
                            i += 1;
                            $s1_2 = &$s1_2[1];
                        }
                    }
                }
            }
            
            private_mutex_unlock($s1_1 + 0x12c);
            return 0;
        }
    }
    
    return result;
}

