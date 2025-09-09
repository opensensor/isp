#include "include/main.h"


  int32_t tisp_channel_stop(int32_t arg1)

{
    uint32_t msca_ch_en_1 = msca_ch_en;
    int32_t $s0 = 1 << (arg1 & 0x1f);
    
    if (!~msca_ch_en_1)
        msca_ch_en_1 = 0;
    
    int32_t $a1 = ~$s0 & msca_ch_en_1;
    msca_ch_en = $a1;
    int32_t $s2 = 0xbb9;
    system_reg_write(0x9804, $a1);
    int32_t $v0_2;
    
    do
    {
        $v0_2 = system_reg_read(0x9808);
        $s2 -= 1;
        private_msleep(1);
        
        if (!$s2)
        {
            int32_t var_28_1_3 = arg1;
            isp_printf(2, "VIC_CTRL : %08x\\n", "tisp_channel_stop");
            break;
        }
    } while ($s0 & $v0_2);
    return 0;
}

