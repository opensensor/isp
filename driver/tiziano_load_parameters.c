#include "include/main.h"


  int32_t tiziano_load_parameters(int32_t arg1)

{
    char* $v0;
    int32_t $a2;
    $v0 = private_vmalloc(0x34);
    int32_t (* var_44_16)() = private_vmalloc;
    
    if (!$v0)
    {
        isp_printf(2, "%s[%d] VIC failed to config DVP mode!(8bits-sensor)\\n", $a2);
        return 0xffffffff;
    }
    
    memset($v0, 0, 0x34);
    snprintf($v0, 8, "%s[%d] VIC failed to config DVP mode!(10bits-sensor)\\n", 
        "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\\n");
    char* $s3_1 = &$v0[8];
    snprintf($s3_1, 8, "%s[%d] VIC do not support this format %d\\n", 0);
    void var_88_9;
    sprintf(&var_88_10, arg1);
    void* $v0_2 = private_filp_open(&var_88_11, 0, 0);
    void* $s1_1 = $v0_2;
    void* const var_40_1_6 = memset;
    void* const var_3c_1_4 = snprintf;
    int32_t (* var_38_1_3)(int32_t, int32_t, int16_t arg3) = private_filp_open;
    
    if ($v0_2 >= 0xfffff001)
    {
        isp_printf(2, "%s[%d] do not support this interface\\n", &var_88_12);
        return 0xffffffff;
    }
    
    void* $v0_5 = *(*($s1_1 + 0xc) + 0x28);
    int32_t $s4_1 = *($v0_5 + 0x30);
    int32_t $fp_1 = *($v0_5 + 0x34);
    int32_t var_48_20;
    private_get_fs(&var_48_21);
    int32_t $s7_1 = var_48_22;
    int32_t $a2_2 = private_set_fs(0);
    int32_t (* var_34_1_6)(int32_t arg1) = private_get_fs;
    void* $a0_22;
    
    if (!*($v0 + 0x28))
    {
        void* var_30_1_5 = $s1_1 + 0x28;
        int32_t $v0_7;
        $v0_7 = private_vmalloc($s4_1);
        *($v0 + 0x28) = $v0_7;
        
        if (!$v0_7)
        {
            int32_t var_90_1_3 = $fp_1 << 0x16 | $s4_1 >> 0xa;
            int32_t var_8c_1_2 = $fp_1 >> 0xa;
        label_25cf0:
            isp_printf(2, "%s:%d::linear mode\\n", "tiziano_load_parameters");
            $a0_22 = $s1_1;
        label_25f4c:
            private_filp_close($a0_22, 0);
            private_set_fs($s7_1);
        label_25f64:
            private_kfree(*($v0 + 0x28));
            private_kfree($v0);
            return 0xffffffff;
        }
        
        *($v0 + 0x2c) = $s4_1;
    }
    
    char const* const $a1_3;
    
    if ($fp_1 > 0)
    {
    label_25a54:
        $a1_3 = "%s:%d::wdr mode\\n";
    }
    else
    {
        int32_t $a1_2;
        
        if ($fp_1)
            $a1_2 = *($v0 + 0x28);
        else
        {
            if (*($v0 + 0x2c) < $s4_1)
                goto label_25a54;
            
            $a1_2 = *($v0 + 0x28);
        }
        
        private_vfs_read($s1_1, $a1_2, $s4_1);
        private_filp_close($s1_1, 0);
        private_set_fs($s7_1);
        char* $s4_2 = *($v0 + 0x28);
        int32_t $v1_2 = 8;
        char* $v0_11 = $v0;
        char* $a0_10 = $s4_2;
        int32_t $a1_4 = 8;
        uint32_t $at_1;
        uint32_t $a2_6;
        
        while (true)
        {
            $a2_6 = *$v0_11;
            $at_1 = *$a0_10;
            
            if ($a1_4)
            {
                $a1_4 -= 1;
                $v0_11 = &$v0_11[1];
                
                if ($at_1 != $a2_6)
                    break;
                
                $a0_10 = &$a0_10[1];
                
                if ($a2_6)
                    continue;
            }
            
            $a2_6 = $at_1;
            break;
        }
        
        uint32_t $a2_7 = $a2_6 - $at_1;
        int32_t (* var_30_2_1)() = private_vfs_read;
        
        if ($a2_7)
        {
            isp_printf(2, "bank no free\\n", isp_printf(2, "qbuffer null\\n", $a2_7));
            isp_printf(2, "qbuffer null\\n", 
                isp_printf(2, "Failed to allocate vic device\\n", $s4_2));
            goto label_25f64;
        }
        
        void* $v0_12 = &$s4_2[8];
        char* $a0_11 = $s3_1;
        uint32_t $at_2;
        uint32_t $a1_6;
        
        while (true)
        {
            $a1_6 = *$a0_11;
            $at_2 = *$v0_12;
            
            if ($v1_2)
            {
                $v1_2 -= 1;
                $a0_11 = &$a0_11[1];
                
                if ($at_2 != $a1_6)
                    break;
                
                $v0_12 += 1;
                
                if ($a1_6)
                    continue;
            }
            
            $a1_6 = $at_2;
            break;
        }
        
        char const* const $a1_8;
        int32_t $a2_10;
        
        if ($a1_6 - $at_2)
        {
            $a2_10 = isp_printf(2, "qbuffer null\\n", $a2_7);
            $a1_8 = "Failed to init isp module(%d.%d)\\n";
        label_25e58:
            isp_printf(2, "qbuffer null\\n", isp_printf(2, $a1_8, $a2_10));
            $a0_22 = $s1_1;
            goto label_25f4c;
        }
        
        int32_t $v0_13 = *($s4_2 + 0x10);
        *($v0 + 0x10) = $v0_13;
        int32_t $t1_1 = *($s4_2 + 0x14);
        *($v0 + 0x14) = $t1_1;
        void* $a2_11 = &$s4_2[0x18];
        int32_t $v1_3 = 0;
        
        while (true)
        {
            void* temp1_1 = $a2_11;
            $a2_11 += 4;
            
            if (temp1_1 == &$s4_2[0x18 + ($v0_13 >> 2 << 2)])
                break;
            
            int32_t $v1_4 = $v1_3 ^ *($a2_11 - 4);
            $v1_3 = $v1_4 ^ *((($v1_4 & 7) << 2) + &crc_table);
        }
        
        char const* const $a1_10;
        
        if ($t1_1 != $v1_3)
        {
            $a1_10 = "&vsd->mlock";
        label_25ee0:
            isp_printf(2, $a1_10, "tiziano_load_parameters");
            $a0_22 = $s1_1;
            goto label_25f4c;
        }
        
        uint32_t tparams_day_1 = tparams_day;
        *($v0 + 0x30) = &$s4_2[0x18];
        memcpy(tparams_day_1, &$s4_2[0x18], 0x137f0);
        memcpy(tparams_night, &$s4_2[0x13808], 0x137f0);
        var_3c_1_5(&var_88_13, 0x40, "&vsd->snap_mlock", 0xb2e24);
        void* $v0_19 = var_38_1_4(&var_88_14, 0, 0);
        $s1_1 = $v0_19;
        
        if ($v0_19 >= 0xfffff001)
        {
            isp_printf(0, " %d, %d\\n", &var_88_15);
        label_25f34:
            private_vfree(*($v0 + 0x28));
            private_vfree($v0);
            return 0;
        }
        
        void* $v0_22 = *(*($s1_1 + 0xc) + 0x28);
        int32_t $s4_3 = *($v0_22 + 0x30);
        int32_t $s6_1 = *($v0_22 + 0x34);
        var_34_1_7(&var_48_23);
        $s7_1 = var_48_24;
        $a2_2 = private_set_fs(0);
        
        if (!*($v0 + 0x28))
        {
            void* var_3c_2_2 = $s1_1 + 0x28;
            int32_t $v0_26;
            $v0_26 = var_44_17();
            *($v0 + 0x28) = $v0_26;
            
            if (!$v0_26)
            {
                int32_t var_90_2_2 = $s6_1 << 0x16 | $s4_3 >> 0xa;
                int32_t var_8c_2_2 = $s6_1 >> 0xa;
                goto label_25cf0;
            }
            
            *($v0 + 0x2c) = $s4_3;
        }
        
        if ($s6_1 > 0)
            $a1_3 = "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\\n";
        else
        {
            if ($s6_1)
            {
                *($v0 + 0x28);
            label_25d48:
                var_30_2_2();
                private_filp_close($s1_1, 0);
                private_set_fs($s7_1);
                char* $s6_2 = *($v0 + 0x28);
                int32_t $v1_6 = 8;
                void* $v0_31 = $v0;
                char* $a0_26 = $s6_2;
                int32_t $a1_14 = 8;
                uint32_t $at_3;
                uint32_t $a2_16;
                
                while (true)
                {
                    $a2_16 = *$v0_31;
                    $at_3 = *$a0_26;
                    
                    if ($a1_14)
                    {
                        $a1_14 -= 1;
                        $v0_31 += 1;
                        
                        if ($at_3 != $a2_16)
                            break;
                        
                        $a0_26 = &$a0_26[1];
                        
                        if ($a2_16)
                            continue;
                    }
                    
                    $a2_16 = $at_3;
                    break;
                }
                
                uint32_t $a2_17 = $a2_16 - $at_3;
                char* $v0_32 = &$s6_2[8];
                
                if ($a2_17)
                {
                    isp_printf(2, "The parameter is invalid!\\n", 
                        isp_printf(2, "qbuffer null\\n", $a2_17));
                    isp_printf(2, "qbuffer null\\n", isp_printf(2, "vic_done_gpio%d", $s6_2));
                    goto label_25f64;
                }
                
                uint32_t $at_4;
                uint32_t $a0_27;
                
                while (true)
                {
                    $a0_27 = *$s3_1;
                    $at_4 = *$v0_32;
                    
                    if ($v1_6)
                    {
                        $v1_6 -= 1;
                        $s3_1 = &$s3_1[1];
                        
                        if ($at_4 != $a0_27)
                            break;
                        
                        $v0_32 = &$v0_32[1];
                        
                        if ($a0_27)
                            continue;
                    }
                    
                    $a0_27 = $at_4;
                    break;
                }
                
                if ($a0_27 - $at_4)
                {
                    $a2_10 = isp_printf(2, "qbuffer null\\n", $a2_17);
                    $a1_8 = "register is 0x%x, value is 0x%x\\n";
                    goto label_25e58;
                }
                
                int32_t $v0_33 = *($s6_2 + 0x10);
                *($v0 + 0x10) = $v0_33;
                int32_t $a3_3 = *($s6_2 + 0x14);
                *($v0 + 0x14) = $a3_3;
                void* $a1_15 = &$s6_2[0x18];
                int32_t $v1_7 = 0;
                
                while (true)
                {
                    void* temp1_2 = $a1_15;
                    $a1_15 += 4;
                    
                    if (&$s6_2[0x18 + ($v0_33 >> 2 << 2)] == temp1_2)
                        break;
                    
                    int32_t $v1_8 = $v1_7 ^ *($a1_15 - 4);
                    $v1_7 = $v1_8 ^ *((($v1_8 & 7) << 2) + &crc_table);
                }
                
                if ($a3_3 != $v1_7)
                {
                    $a1_10 = "count is %d\\n";
                    goto label_25ee0;
                }
                
                uint32_t $v0_38 = var_44_18();
                tparams_cust = $v0_38;
                var_40_1_7($v0_38, 0, 0x137f0);
                uint32_t tparams_cust_1 = tparams_cust;
                *($v0 + 0x30) = &$s6_2[0x18];
                memcpy(tparams_cust_1, &$s6_2[0x18], 0x137f0);
                goto label_25f34;
            }
            
            if (*($v0 + 0x2c) >= $s4_3)
            {
                *($v0 + 0x28);
                goto label_25d48;
            }
            
            $a1_3 = "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\\n";
        }
    }
    
    isp_printf(2, $a1_3, $a2_2);
    $a0_22 = $s1_1;
    goto label_25f4c;
}

