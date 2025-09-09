#include "include/main.h"


  int32_t ispvic_frame_channel_s_stream(void* arg1, int32_t arg2)

{
    void* $s0 = nullptr;
    
    if (arg1 && arg1 < 0xfffff001)
        $s0 = *(arg1 + 0xd4);
    
    int32_t var_18_21 = 0;
    
    if (!arg1)
    {
        isp_printf(2, "%s[%d]: invalid parameter\\n", "ispvic_frame_channel_s_stream");
        return 0xffffffea;
    }
    
    char const* const $v0_3;
    
    $v0_3 = arg2 ? "streamon" : "streamoff";
    
    char const* const var_20_1_2 = $v0_3;
    isp_printf(0, "%s[%d]: %s\\n", "ispvic_frame_channel_s_stream");
    
    if (arg2 == *($s0 + 0x210))
        return 0;
    
    __private_spin_lock_irqsave($s0 + 0x1f4, &var_18_22);
    
    if (!arg2)
    {
        *(*($s0 + 0xb8) + 0x300) = 0;
        *($s0 + 0x210) = 0;
    }
    else
    {
        vic_pipo_mdma_enable($s0);
        *(*($s0 + 0xb8) + 0x300) = *($s0 + 0x218) << 0x10 | 0x80000020;
        *($s0 + 0x210) = 1;
    }
    
    private_spin_unlock_irqrestore($s0 + 0x1f4, var_18_23);
    return 0;
}

