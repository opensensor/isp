#include "include/main.h"


  void* ispcore_irq_fs_work()

{
    void* result = &data_d0000_6;
    void* $s5 = *(mdns_y_pspa_cur_bi_wei0_array + 0xd4);
    int32_t var_30_28 = 0;
    
    if ($s5)
    {
        int32_t* $s2_1 = $s5 + 0x180;
        
        for (int32_t i = 0; i != 7; )
        {
            if (!*$s2_1)
                i += 1;
            else if (i == 5)
                i += 1;
            else
            {
                if (i < 7)
❓                    /* jump -> (&data_7ddf0_1)[i] */
                
                var_30_29 = $s2_1[1];
                
                if (*(*($s5 + 0x120) + 0xf0) != 1)
                    i += 1;
                else
                {
                    ispcore_sensor_ops_ioctl(mdns_y_pspa_cur_bi_wei0_array);
                    *$s2_1 = 0;
                    i += 1;
                }
            }
            
            result = 7;
            $s2_1 = &$s2_1[2];
        }
    }
    
    return result;
}

