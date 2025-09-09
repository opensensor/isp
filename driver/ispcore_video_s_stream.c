#include "include/main.h"


  int32_t ispcore_video_s_stream(int32_t* arg1, int32_t arg2)

{
    void* $s0 = arg1[0x35];
    int32_t var_28_44 = 0;
    __private_spin_lock_irqsave($s0 + 0xdc, &var_28_45);
    
    if (*($s0 + 0xe8) < 3)
    {
        isp_printf(2, "Err [VIC_INT] : mipi ch2 hcomp err !!!\\n", "ispcore_video_s_stream");
        private_spin_unlock_irqrestore($s0 + 0xdc, var_28_46);
        return 0xffffffff;
    }
    
    private_spin_unlock_irqrestore($s0 + 0xdc, var_28_47);
    *($s0 + 0x164) = 0;
    *($s0 + 0x168) = 0;
    *($s0 + 0x170) = 0;
    *($s0 + 0x160) = 0;
    int32_t $v0_3 = *($s0 + 0xe8);
    void* $s3_1;
    
    if (!arg2)
    {
        $s3_1 = &arg1[0xe];
        
        if ($v0_3 == 4)
        {
            int32_t $s2_1 = 0;
            void* $v0_5 = *($s0 + 0x150);
            
            while (true)
            {
                void* $v0_6 = $v0_5 + $s2_1;
                $s2_1 += 0xc4;
                
                if (*($v0_6 + 0x74) == 4)
                    ispcore_frame_channel_streamoff(*($v0_6 + 0x78));
                
                if ($s2_1 == 0x24c)
                    break;
                
                $v0_5 = *($s0 + 0x150);
            }
            
            *($s0 + 0xe8) = 3;
            $s3_1 = &arg1[0xe];
        }
    }
    else if ($v0_3 != 3)
        $s3_1 = &arg1[0xe];
    else
    {
        *($s0 + 0xe8) = 4;
        $s3_1 = &arg1[0xe];
    }
    
    int32_t result = 0;
    int32_t $a0_4;
    
    while (true)
    {
        void* $a0_5 = *$s3_1;
        
        if ($a0_5)
        {
            int32_t* $v0_7 = *(*($a0_5 + 0xc4) + 4);
            
            if ($v0_7)
            {
                int32_t $v0_8 = *$v0_7;
                
                if (!$v0_8)
                    result = 0xfffffdfd;
                else
                {
                    int32_t result_1 = $v0_8($a0_5, arg2);
                    result = result_1;
                    
                    if (result_1)
                    {
                        if (result_1 != 0xfffffdfd)
                        {
                            $a0_4 = *($s0 + 0x15c);
                            break;
                        }
                        
                        result = 0xfffffdfd;
                    }
                }
            }
            else
                result = 0xfffffdfd;
            
            $s3_1 += 4;
        }
        else
            $s3_1 += 4;
        
        if (&arg1[0x1e] == $s3_1)
        {
            $a0_4 = *($s0 + 0x15c);
            break;
        }
    }
    
    void* $v0_10 = arg1[0x2e];
    int32_t (* $v0_11)(int32_t* arg1);
    int32_t* $a0_6;
    
    if ($a0_4 == 1 || !arg2)
    {
        *($v0_10 + 0xb0) = 0;
        $a0_6 = arg1;
        $v0_11 = tx_isp_disable_irq;
    }
    else
    {
        *($v0_10 + 0xb0) = 0xffffffff;
        $a0_6 = arg1;
        $v0_11 = tx_isp_enable_irq;
    }
    
    $v0_11($a0_6);
    
    if (result == 0xfffffdfd)
        return 0;
    
    return result;
}

