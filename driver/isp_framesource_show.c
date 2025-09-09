#include "include/main.h"


  int32_t isp_framesource_show(void* arg1)

{
    char* $v0 = *((char*)arg1 + 0x3c); // Fixed void pointer arithmetic
    void* $s4 = nullptr;
    int32_t var_40 = 0;
    
    if ($v0 && $(uintptr_t)v0 < 0xfffff001)
        $s4 = *($v0 + 0xd4);
    
    
    if (!$s4 || $(uintptr_t)s4 >= 0xfffff001)
    {
        int32_t entry_$a2;
        isp_printf(); // Fixed: macro call, removed arguments;
        return 0;
    }
    
    int32_t i = 0;
    int32_t result = 0;
    
    while (i < *($s4 + 0xe0))
    {
        int32_t $s0_1 = result + private_seq_printf(arg1, "sensor type is BT656!\n", i);
        char* $s1_1 = (char*)(i * 0x2ec + *($s4 + 0xdc)); // Fixed void pointer assignment
            int32_t var_4c_1 = *($s1_1 + 0x24b);
            int32_t var_50_1 = *($s1_1 + 0x24a);
            int32_t $s0_3 = result + private_seq_printf(arg1, 
            int32_t $s0_4 = $s0_3 + private_seq_printf(arg1, 
        char const* const $a2_1;
        
        if (*($s1_1 + 0x2d0) != 4)
            $a2_1 = "sensor type is BT1120!\n";
        else
            $a2_1 = "Can not support this frame mode!!!\n";
        
        result = $s0_1 + private_seq_printf(arg1, "sensor type is BT601!\n", $a2_1);
        
        if (*($s1_1 + 0x2d0) != 4)
            i += 1;
        else
        {
                "%s[%d] VIC failed to config DVP mode!(8bits-sensor)\n", *($s1_1 + 0x248)) +
                private_seq_printf(arg1, 
                "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\n", *($s1_1 + 0x240));
            char const* const $a2_4;
            
            if (!*($s1_1 + 0x284))
                $a2_4 = "not support the gpio mode!\n";
            else
                $a2_4 = "VIC_CTRL : %08x\n";
            
                "%s[%d] VIC failed to config DVP mode!(10bits-sensor)\n", $a2_4);
            uint32_t $v0_14;
            
            if (!*($s1_1 + 0x284))
                $v0_14 = *($s1_1 + 0x270);
            else
            {
                $s0_4 = $s0_4 + private_seq_printf(arg1, 
                    "%s[%d] VIC do not support this format %d\n", *($s1_1 + 0x288)) +
                    private_seq_printf(arg1, "%s[%d] do not support this interface\n", 
                    *($s1_1 + 0x28c));
                $v0_14 = *($s1_1 + 0x270);
            }
            
            char const* const $a2_7;
            
            $a2_7 = !$v0_14 ? "not support the gpio mode!\\n" : "VIC_CTRL : %08x\\n";
            
            int32_t $v0_17;
            int32_t $a2_8;
            $v0_17 = private_seq_printf(arg1, "%s:%d::linear mode\\n", $a2_7);
            int32_t $s0_6 = $s0_4 + $v0_17;
            
            if (*($s1_1 + 0x270))
            {
                int32_t $v0_19 = private_seq_printf(arg1, "%s:%d::wdr mode\n", *($s1_1 + 0x274));
                int32_t $v0_20 = private_seq_printf(arg1, "qbuffer null\n", *($s1_1 + 0x278));
                int32_t $v0_21 = private_seq_printf(arg1, "bank no free\n", *($s1_1 + 0x27c));
                int32_t $v0_22;
                $v0_22 =
                    private_seq_printf(arg1, "Failed to allocate vic device\n", *($s1_1 + 0x280));
                $s0_6 = $s0_6 + $v0_19 + $v0_20 + $v0_21 + $v0_22;
            }
            
            int32_t $s0_10 =
                $s0_6 + private_seq_printf(arg1, "Failed to init isp module(%d.%d)\\n", $a2_8);
            __private_spin_lock_irqsave($s1_1 + 0x2c4, &var_40_4);
            int32_t $s0_11 = $s0_10 + private_seq_printf(arg1, "&vsd->mlock", *($s1_1 + 0x218));
            char* $v1_6 = (char*)(*($s1_1 + 0x210) - 0x58); // Fixed void pointer assignment
            
            while ($v1_6 + 0x58 != $s1_1 + 0x210)
            {
                $s0_11 += private_seq_printf(arg1, "&vsd->snap_mlock", *($v1_6 + 0x34));
                $v1_6 = *($v1_6 + 0x58) - 0x58;
            }
            
            int32_t $s0_12 = $s0_11 + private_seq_printf(arg1, " %d, %d\\n", *($s1_1 + 0x224));
            char* $v1_10 = (char*)(*($s1_1 + 0x21c) - 0x60); // Fixed void pointer assignment
            
            while ($v1_10 + 0x60 != $s1_1 + 0x21c)
            {
                $s0_12 += private_seq_printf(arg1, 
                    "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n", *($v1_10 + 0x34));
                $v1_10 = *($v1_10 + 0x60) - 0x60;
            }
            
            private_spin_unlock_irqrestore($s1_1 + 0x2c4, var_40_5);
            int32_t $v0_30 =
                private_seq_printf(arg1, "The parameter is invalid!\\n", *($s1_1 + 0x2e0));
            int32_t $v0_31;
            int32_t $a2_19;
            $v0_31 = private_seq_printf(arg1, "vic_done_gpio%d", *($s1_1 + 0x2e4));
            int32_t $s0_14 = $s0_12 + $v0_30 + $v0_31;
            
            if (!i)
            {
                int32_t $s0_16 = $s0_14 + private_seq_printf(arg1, 
                int32_t var_50_2 = 0;
                    "register is 0x%x, value is 0x%x\n", isp_ch0_pre_dequeue_drop) +
                    private_seq_printf(arg1, "count is %d\n", isp_ch0_pre_dequeue_intc_ahead_cnt);
                int32_t $v0_34;
                $v0_34 = private_seq_printf(arg1, "snapraw", isp_ch0_pdq_cnt);
                $s0_14 = $s0_16 + $v0_34;
            }
            
            result = $s0_14 + private_seq_printf(arg1, 
                "width is %d, height is %d, imagesize is %d\\n, snap num is %d, buf size is %d", 
                $a2_19);
            void* $s1_2 = $s1_1 + 0x10c;
            
            for (int32_t j = 0; (uintptr_t)j != 0x40; )
            {
                void* $v0_36 = *$s1_2;
                    int32_t var_48_1 = *($v0_36 + 0x54);
                    int32_t var_4c_2 = *($v0_36 + 0x50);
                    uint32_t var_50_3 = *($v0_36 + 0x4c);
                
                if ($v0_36)
                {
                    result += private_seq_printf(arg1, "Can't output the width(%d)!\n", j);
                }
                
                j += 1;
                $s1_2 += 4;
            }
            
            i += 1;
        }
    }
    
    return result;
}

