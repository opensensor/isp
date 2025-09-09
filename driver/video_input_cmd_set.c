#include "include/main.h"


  int32_t video_input_cmd_set(void* arg1, int32_t arg2, int32_t arg3)

{
    void* $s3 = *(arg1 + 0x70);
    void* $v0 = *($s3 + 0x3c);
    
    if ($v0 && $v0 < 0xfffff001)
    {
        void* $v0_1 = *($v0 + 0xd8);
        
        if ($v0_1 && $v0_1 < 0xfffff001)
        {
            void* $s1_1 = *($v0_1 + 0xe4);
            
            if ($s1_1)
            {
                int32_t $s4_1 = arg3 < 0x81 ? 1 : 0;
                
                if ($s1_1 < 0xfffff001)
                {
                    int32_t result;
                    char* $s0_1;
                    
                    if ($s4_1)
                    {
                        $s0_1 = &video_input_cmd_buf;
                    label_13998:
                        int32_t $a2_1 = arg3;
                        void* entry_$gp;
                        
                        if (!((arg3 | arg2 | (arg3 + arg2)) & *(entry_$gp + 0x18)))
                        {
                            __might_sleep("sensor type is BT601!\\n", 0xc9, 0);
                            $a2_1 = __copy_user($s0_1, arg2, arg3);
                        }
                        
                        result = 0xfffffff2;
                        
                        if (!$a2_1)
                        {
                            int32_t $v0_8 = 9;
                            char* $a0_4 = $s0_1;
                            char const* const $v1_4 =
                                "%s[%d] VIC failed to config DVP mode!(8bits-sensor)\\n";
                            int32_t $a1_1 = 9;
                            uint32_t $at_2;
                            uint32_t $a2_3;
                            
                            while (true)
                            {
                                $a2_3 = *$a0_4;
                                $at_2 = *$v1_4;
                                
                                if ($a1_1)
                                {
                                    $a1_1 -= 1;
                                    $a0_4 = &$a0_4[1];
                                    
                                    if ($at_2 != $a2_3)
                                        break;
                                    
                                    $v1_4 = &$v1_4[1];
                                    
                                    if ($a2_3)
                                        continue;
                                }
                                
                                $a2_3 = $at_2;
                                break;
                            }
                            
                            uint32_t $a2_4 = $a2_3 - $at_2;
                            void* var_48;
                            
                            if ($a2_4)
                            {
                                char* $a0_8 = $s0_1;
                                char const* const $v1_5 = "%s[%d] do not support this interface\\n";
                                int32_t $a1_3 = 6;
                                uint32_t $at_3;
                                uint32_t $a2_7;
                                
                                while (true)
                                {
                                    $a2_7 = *$a0_8;
                                    $at_3 = *$v1_5;
                                    
                                    if ($a1_3)
                                    {
                                        $a1_3 -= 1;
                                        $a0_8 = &$a0_8[1];
                                        
                                        if ($at_3 != $a2_7)
                                            break;
                                        
                                        $v1_5 = &$v1_5[1];
                                        
                                        if ($a2_7)
                                            continue;
                                    }
                                    
                                    $a2_7 = $at_3;
                                    break;
                                }
                                
                                if ($a2_7 - $at_3)
                                {
                                    char* $a0_10 = $s0_1;
                                    char* $v1_6 = "%s:%d::linear mode\\n";
                                    int32_t $a1_5 = 5;
                                    uint32_t $at_4;
                                    uint32_t $a2_9;
                                    
                                    while (true)
                                    {
                                        $a2_9 = *$a0_10;
                                        $at_4 = *$v1_6;
                                        
                                        if ($a1_5)
                                        {
                                            $a1_5 -= 1;
                                            $a0_10 = &$a0_10[1];
                                            
                                            if ($at_4 != $a2_9)
                                                break;
                                            
                                            $v1_6 = &$v1_6[1];
                                            
                                            if ($a2_9)
                                                continue;
                                        }
                                        
                                        $a2_9 = $at_4;
                                        break;
                                    }
                                    
                                    if ($a2_9 - $at_4)
                                    {
                                        char* $a0_12 = $s0_1;
                                        char* $v1_7 = "%s:%d::wdr mode\\n";
                                        uint32_t $at_5;
                                        uint32_t $a1_7;
                                        
                                        while (true)
                                        {
                                            $a1_7 = *$a0_12;
                                            $at_5 = *$v1_7;
                                            
                                            if ($v0_8)
                                            {
                                                $v0_8 -= 1;
                                                $a0_12 = &$a0_12[1];
                                                
                                                if ($at_5 != $a1_7)
                                                    break;
                                                
                                                $v1_7 = &$v1_7[1];
                                                
                                                if ($a1_7)
                                                    continue;
                                            }
                                            
                                            $a1_7 = $at_5;
                                            break;
                                        }
                                        
                                        if ($a1_7 - $at_5)
                                        {
                                            sprintf(&video_input_cmd_buf, "&vsd->mlock");
                                            result = arg3;
                                        }
                                        else
                                        {
                                            int32_t var_30 = 0;
                                            int32_t $v0_25 =
                                                private_simple_strtoull(&$s0_1[0xa], &var_30_2, 0);
                                            int32_t $v0_26 =
                                                private_simple_strtoull(var_30_3 + 1, 0, 0);
                                            private_seq_printf($s3, "qbuffer null\\n", $v0_25);
                                            var_48_2 = $s1_1 + 0x8c;
                                            int32_t var_40_2_1 = $v0_25;
                                            int32_t var_3c_2 = 0;
                                            int32_t var_38_1_1 = $v0_26;
                                            int32_t var_34_1_3 = 0;
                                            void* $v0_29 = **($s1_1 + 0xc4);
                                            char const* const $a2_12;
                                            
                                            if (!$v0_29)
                                                $a2_12 = "Failed to init isp module(%d.%d)\\n";
                                            else
                                            {
                                                int32_t $v0_30 = *($v0_29 + 0x18);
                                                
                                                if (!$v0_30)
                                                    $a2_12 = "Failed to init isp module(%d.%d)\\n";
                                                else if ($v0_30($s1_1, &var_48_3))
                                                    $a2_12 = "Failed to init isp module(%d.%d)\\n";
                                                else
                                                    $a2_12 = "bank no free\\n";
                                            }
                                            
                                            sprintf(&video_input_cmd_buf, 
                                                "Failed to allocate vic device\\n", $a2_12);
                                            result = arg3;
                                        }
                                    }
                                    else
                                    {
                                        var_48_4 = $s1_1 + 0x8c;
                                        void* $v0_22 = **($s1_1 + 0xc4);
                                        
                                        if ($v0_22)
                                        {
                                            int32_t $v0_23 = *($v0_22 + 0x14);
                                            
                                            if (!$v0_23)
                                            {
                                                isp_printf(2, 
                                                    "%s[%d] VIC failed to config DVP SONY "
                                                "mode!(10bits-sensor)\\n", 
                                                    "video_input_cmd_set");
                                                result = arg3;
                                            }
                                            else if (!$v0_23($s1_1, &var_48_5))
                                                result = arg3;
                                            else
                                            {
                                                isp_printf(2, 
                                                    "%s[%d] VIC failed to config DVP SONY "
                                                "mode!(10bits-sensor)\\n", 
                                                    "video_input_cmd_set");
                                                result = arg3;
                                            }
                                        }
                                        else
                                        {
                                            isp_printf(2, 
                                                "%s[%d] VIC failed to config DVP SONY "
                                            "mode!(10bits-sensor)\\n", 
                                                "video_input_cmd_set");
                                            result = arg3;
                                        }
                                    }
                                }
                                else
                                {
                                    var_48_6 = $s1_1 + 0x8c;
                                    void* $v0_17 = **($s1_1 + 0xc4);
                                    
                                    if ($v0_17)
                                    {
                                        int32_t $v0_18 = *($v0_17 + 0x10);
                                        
                                        if (!$v0_18)
                                        {
                                            isp_printf(2, 
                                                "%s[%d] VIC failed to config DVP SONY "
                                            "mode!(10bits-sensor)\\n", 
                                                "video_input_cmd_set");
                                            result = arg3;
                                        }
                                        else if (!$v0_18($s1_1, &var_48_7))
                                            result = arg3;
                                        else
                                        {
                                            isp_printf(2, 
                                                "%s[%d] VIC failed to config DVP SONY "
                                            "mode!(10bits-sensor)\\n", 
                                                "video_input_cmd_set");
                                            result = arg3;
                                        }
                                    }
                                    else
                                    {
                                        isp_printf(2, 
                                            "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\\n", 
                                            "video_input_cmd_set");
                                        result = arg3;
                                    }
                                }
                            }
                            else
                            {
                                int32_t $v0_9 = private_simple_strtoull(&$s0_1[0xa], 0, $a2_4);
                                var_48_8 = $s1_1 + 0x8c;
                                int32_t var_40_1_4 = $v0_9;
                                int32_t var_3c_1_2 = 0;
                                void* $v0_12 = **($s1_1 + 0xc4);
                                int32_t $s1_2;
                                int32_t var_38_4;
                                
                                if ($v0_12)
                                {
                                    int32_t $v0_13 = *($v0_12 + 0xc);
                                    
                                    if (!$v0_13)
                                    {
                                        isp_printf(2, 
                                            "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\\n", 
                                            "video_input_cmd_set");
                                        $s1_2 = var_38_5;
                                    }
                                    else if ($v0_13($s1_1, &var_48_9, &data_80000_4))
                                    {
                                        isp_printf(2, 
                                            "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\\n", 
                                            "video_input_cmd_set");
                                        $s1_2 = var_38_6;
                                    }
                                    else
                                        $s1_2 = var_38_7;
                                }
                                else
                                {
                                    isp_printf(2, 
                                        "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\\n", 
                                        "video_input_cmd_set");
                                    $s1_2 = var_38_8;
                                }
                                private_seq_printf($s3, 
                                    "%s[%d] VIC failed to config DVP mode!(10bits-sensor)\\n", 
                                    $v0_9);
                                sprintf(&video_input_cmd_buf, 
                                    "%s[%d] VIC do not support this format %d\\n", $s1_2);
                                result = arg3;
                            }
                        }
                        
                        if (!$s4_1)
                            private_kfree($s0_1);
                    }
                    else
                    {
                        char* $v0_4 = private_kmalloc(arg3 + 1, 0xd0);
                        $s0_1 = $v0_4;
                        result = 0xfffffff4;
                        
                        if ($v0_4)
                        {
                            memset($v0_4, 0, arg3 + 1);
                            goto label_13998;
                        }
                    }
                    return result;
                }
            }
        }
    }
    
    return private_seq_printf($s3, "&vsd->snap_mlock", arg3);
}

