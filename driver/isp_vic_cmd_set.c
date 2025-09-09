#include "include/main.h"


  int32_t isp_vic_cmd_set(void* arg1, int32_t arg2, int32_t arg3)

{
    int32_t* $s5 = (int32_t*)((char*)arg1  + 0x70); // Fixed void pointer arithmetic
    int32_t* $v0 = (int32_t*)((char*)$s5  + 0x3c); // Fixed void pointer arithmetic
    char* $s0 = (char*)(nullptr); // Fixed void pointer assignment
    void* $a0;
    
    if ($v0 && $(uintptr_t)v0 < 0xfffff001)
        $s0 = *($v0 + 0xd4);
    
    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
    void* const $v0_3;
    char const* const $a1;
    
    if (!$s0 || $(uintptr_t)s0 >= 0xfffff001)
    {
        $a1 = "Can't ops the node!\n";
        $a0 = $s5;
        $v0_3 = seq_printf;
    }
    else
    {
        int32_t $v0_2 = (uintptr_t)arg3 < 0x21 ? 1 : 0;
        char* $s2_1;
                return 0xfffffff4;
        
        if ($v0_2)
            $s2_1 = &vic_cmd_buf;
        else
        {
            $s2_1 = private_kmalloc(arg3 + 1, 0xd0);
            
            if (!$s2_1)
        }
        
        int32_t result;
        
        if (private_copy_from_user($s2_1, arg2, arg3))
        {
                return result;
            return result;
            result = 0xfffffff2;
        label_1217c:
            
            if ($v0_2)
            
            private_kfree($s2_1);
        }
        
        int32_t $v0_6 = 7;
        char* $a0_3 = $s2_1;
        char const* const $v1_2 = "snapraw";
        int32_t $a1_2 = 7;
        uint32_t $at_1;
        uint32_t $a2_2;
        
        while (true)
        {
            $a2_2 = *$a0_3;
            $at_1 = *$v1_2;
            
            if ($a1_2)
            {
                $a1_2 -= 1;
                $a0_3 = &$a0_3[1];
                
                if ($at_1 != $a2_2)
                    break;
                
                $v1_2 = &$v1_2[1];
                
                if ($a2_2)
                    continue;
            }
            
            $a2_2 = $at_1;
            break;
        }
        
        uint32_t $a2_3 = $a2_2 - $at_1;
        void var_90;
        int32_t var_50;
        int32_t $a0_19;
        int32_t $a2_5;
        int32_t $a2_7;
        
        if ($a2_3)
        {
            char* $a0_16 = $s2_1;
            char* $v1_9 = "saveraw";
            uint32_t $at_2;
            uint32_t $a1_7;
            
            while (true)
            {
                $a1_7 = *$a0_16;
                $at_2 = *$v1_9;
                
                if ($v0_6)
                {
                    $v0_6 -= 1;
                    $a0_16 = &$a0_16[1];
                    
                    if ($at_2 != $a1_7)
                        break;
                    
                    $v1_9 = &$v1_9[1];
                    
                    if ($a1_7)
                        continue;
                }
                
                $a1_7 = $at_2;
                break;
            }
            
            if ($a1_7 - $at_2)
            {
                char* $v1_17 = $s2_1;
                char* $v0_33 = "help";
                int32_t $a0_29 = 4;
                uint32_t $at_3;
                uint32_t $a1_18;
                
                while (true)
                {
                    $a1_18 = *$v1_17;
                    $at_3 = *$v0_33;
                    
                    if ($a0_29)
                    {
                        $a0_29 -= 1;
                        $v1_17 = &$v1_17[1];
                        
                        if ($at_3 != $a1_18)
                            break;
                        
                        $v0_33 = &$v0_33[1];
                        
                        if ($a1_18)
                            continue;
                    }
                    
                    $a1_18 = $at_3;
                    break;
                }
                
                if (!($a1_18 - $at_3))
                {
                    int32_t $a2_21 = isp_printf(1, 
                    goto label_1215c;
                        "\t\t\t "savenum" is the num of you save raw picture.\n ", 
                        isp_printf(1, "\t\t\t "snapraw"  is cmd; \n", 
                            isp_printf(1, 
                                "\t\t\t please use this cmd: \n\t"echo snapraw savenum > /proc/jz/isp/isp-w02"\n", 
                                isp_printf(1, 
                                "\t\t\t use cmd " snapraw" you should set ispmem first!!!!!\n", 
                                isp_printf(1, "\t\t snapraw\n", 
                                    isp_printf(1, "\t cmd:\n", 
                                        isp_printf(1, "help:\n", 
                                            isp_printf(); // Fixed: macro with no parameters, removed 3 arguments)))))));
                    isp_printf(1, &$LC37, 
                        isp_printf(1, "\t\t\t "savenum" is the num of you save raw picture.\n ", 
                            isp_printf(1, "\t\t\t "saveraw"  is cmd; \n", 
                                isp_printf(1, 
                                    "\t\t\t please use this cmd: \n\t"echo saveraw savenum > /proc/jz/isp/isp-w02"\n", 
                                    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments))));
                    $a0_19 = *($s0 + 0x140);
                }
                
                $a0_19 = *($s0 + 0x140);
            label_1215c:
                result = arg3;
                
                if (!$a0_19)
                    goto label_1217c;
                
                isp_free_buffer($a0_19);
                *((int32_t*)((char*)$s0 + 0x140)) = 0; // Fixed void pointer dereference
            label_12174:
                result = arg3;
                goto label_1217c;
            }
            
            int32_t $v0_16 = system_reg_read(0x7810);
            int32_t $v0_17 = system_reg_read(0x7814);
            int32_t $v0_18 = system_reg_read(0x7804);
            int32_t $v0_19 = system_reg_read(0x7820);
            system_reg_write(0x7810, $v0_16 & 0x11110111);
            system_reg_write(0x7814, 0);
            int32_t $v0_21 = $v0_18 | 1;
            system_reg_write(0x7804, $v0_21);
            
            for (int32_t i = 0xa; i; i -= 1)
                __udelay(0x3e8);
            
            uint32_t $v0_22 = simple_strtoull(&$s2_1[8], 0, 0);
            uint32_t $s3_2 = $v0_22;
            
            if ($v0_22 < 2)
                $s3_2 = 1;
            else if (arg3 == 8)
                $s3_2 = 1;
            
            int32_t $a2_10 = *($s0 + 0xdc);
            int32_t $s4_4 = $a2_10 << 1;
            
            if (*(*($s0 + 0x110) + 0x7c) == 7)
                $s4_4 = ($s4_4 + $a2_10) >> 1;
            
            int32_t $s4_6 = $s4_4 * *($s0 + 0xe0);
            uint32_t var_9c_3 = $s3_2;
            int32_t var_a0_4 = $s4_6;
            int32_t var_98_2 = $s3_2 * $s4_6;
            isp_printf(1, 
                "width is %d, height is %d, imagesize is %d\\n, save num is %d, buf size is %d", 
                $a2_10);
            $a2_5 = *($s0 + 0xdc);
            
            if ($(uintptr_t)a2_5 >= 0xa81)
                return private_seq_printf($s5, "Can\'t output the width(%d)!\\n", $a2_5);
            
            int32_t $s1_2 = *($s0 + 0x140);
            
            if (!$s1_2)
            {
                    int32_t* $v1_12 = (int32_t*)((char*)$s0  + 0x110); // Fixed void pointer arithmetic
                    int32_t $a0_20 = *($v1_12 + 0x7c);
                    int32_t var_a0_5;
                    char var_9c_4;
                    int32_t $a2_11;
                    uint32_t $a3_6;
                            goto label_11ee0;
                *((int32_t*)((char*)$s0 + 0x140)) = $v0_19; // Fixed void pointer dereference
                
                if ($v0_19)
                {
                    *((int32_t*)((char*)$s0 + 0x144)) = $v0_19 - 0x80000000; // Fixed void pointer dereference
                    
                    if ($a0_20 != 7)
                    {
                        var_9c_4 = 0;
                        
                        if (!*($v1_12 + 0x90))
                        
                        var_a0_5 = $v0_19;
                        $a3_6 = $s3_2;
                        $a2_11 = 1;
                    }
                    else
                    {
                        var_9c_4 = $a0_20;
                    label_11ee0:
                        var_a0_5 = $v0_19;
                        $a3_6 = $s3_2;
                        $a2_11 = 0;
                    }
                    
                    vic_mdma_enable($s0, 0, $a2_11, $a3_6, var_a0_5, var_9c_4);
                    int32_t $s7_5 = 0x258;
                    
                    while (true)
                    {
                        int32_t $v0_30;
                            int32_t $s5_2 = 0;
                                int32_t $v0_31 = private_filp_open(&var_90, 0x301, 0x1f6);
                                int32_t $t1_3 = var_50;
                        $v0_30 = (&data_20000 - 0x6a78)($s0 + 0x148);
                        
                        if ($v0_30 >= 0)
                        {
                            void* void* var_40_3 = (void*)&data_80000; // Fixed function pointer assignment
                            void* const var_38_2 = &$LC33;
                            char const* const var_34_1 = "nv12";
                            
                            do
                            {
                                void* const $v1_15;
                                
                                if (*(*($s0 + 0x110) + 0x7c) != 7)
                                    $v1_15 = var_38_2;
                                else
                                    $v1_15 = var_34_1;
                                
                                snprintf(&var_90, 0x40, var_40_3 - 0x1c78, $s5_2, $v1_15);
                                private_get_fs(&var_50);
                                int32_t (* var_30_1)() = private_set_fs;
                                private_set_fs(0);
                                private_vfs_write($v0_31, *($s0 + 0x144) + $s1_2, $s4_6, 
                                    $v0_31 + 0x28);
                                private_filp_close($v0_31, 0);
                                $s5_2 += 1;
                                var_30_1($t1_3);
                                $s1_2 += $s4_6;
                            } while ($s3_2 != $s5_2);
                            
                            break;
                        }
                        
                        $s7_5 -= 1;
                        
                        if (!$s7_5)
                            goto label_121b8;
                    }
                }
                
                system_reg_write(0x7810, $v0_16 & 0x11111111);
                system_reg_write(0x7814, $v0_17);
                system_reg_write(0x7804, $v0_21);
                $a0_19 = *($s0 + 0x140);
                goto label_1215c;
            }
        }
        else
        {
            uint32_t $v0_7 = simple_strtoull(&$s2_1[8], 0, $a2_3);
            uint32_t $s3_1 = $v0_7;
            int32_t $a2_4 = *($s0 + 0xdc);
            int32_t $s4_1 = $a2_4 << 1;
            int32_t $s4_3 = $s4_1 * *($s0 + 0xe0);
            uint32_t var_9c_1 = $s3_1;
            int32_t $s7_1 = $s3_1 * $s4_3;
            int32_t var_a0_1 = $s4_3;
            int32_t var_98_1 = $s7_1;
            int32_t $s1_1 = *($s0 + 0x140);
                int32_t $v0_11 = isp_malloc_buffer($s7_1);
                    goto label_12174;
                int32_t* $a0_6 = (int32_t*)((char*)$s0  + 0x110); // Fixed void pointer arithmetic
                int32_t $a1_3 = *($a0_6 + 0x7c);
                int32_t var_a0_2;
                char var_9c_2;
                int32_t $a2_6;
                uint32_t $a3_2;
                        goto label_11b70;
            
            if ($v0_7 < 2)
                $s3_1 = 1;
            else if (arg3 == 8)
                $s3_1 = 1;
            
            
            if (*(*($s0 + 0x110) + 0x7c) == 7)
                $s4_1 = ($s4_1 + $a2_4) >> 1;
            
            isp_printf(1, 
                "width is %d, height is %d, imagesize is %d\n, snap num is %d, buf size is %d", 
                $a2_4);
            $a2_5 = *($s0 + 0xdc);
            
            if ($(uintptr_t)a2_5 >= 0xa81)
                return private_seq_printf($s5, "Can't output the width(%d)!\n", $a2_5);
            
            
            if (!$s1_1)
            {
                *((int32_t*)((char*)$s0 + 0x140)) = $v0_11; // Fixed void pointer dereference
                
                if (!$v0_11)
                
                *((int32_t*)((char*)$s0 + 0x144)) = $v0_11 - 0x80000000; // Fixed void pointer dereference
                
                if ($a1_3 != 7)
                {
                    var_9c_2 = 0;
                    
                    if (!*($a0_6 + 0x90))
                    
                    var_a0_2 = $v0_11;
                    $a3_2 = $s3_1;
                    $a2_6 = 1;
                }
                else
                {
                    var_9c_2 = $a1_3;
                label_11b70:
                    var_a0_2 = $v0_11;
                    $a3_2 = $s3_1;
                    $a2_6 = 0;
                }
                
                vic_mdma_enable($s0, 0, $a2_6, $a3_2, var_a0_2, var_9c_2);
                int32_t $s7_2 = 0x258;
                
                while (true)
                {
                    int32_t $v0_13;
                        int32_t $s7_3 = 0;
                            int32_t $v0_14 = private_filp_open(&var_90, 0x301, 0x1f6);
                            int32_t $t1_1 = var_50;
                    $v0_13 = (&data_20000 - 0x6a78)($s0 + 0x148);
                    
                    if ($v0_13 >= 0)
                    {
                        void* void* var_4c_2 = (void*)&data_80000; // Fixed function pointer assignment
                        char const* const var_48_1 = "nv12";
                        
                        do
                        {
                            char const* const var_a0_3;
                            
                            if (*(*($s0 + 0x110) + 0x7c) != 7)
                                var_a0_3 = &$LC33;
                            else
                                var_a0_3 = var_48_1;
                            
                            snprintf(&var_90, 0x40, var_4c_2 - 0x1c78, $s7_3, var_a0_3);
                            private_get_fs(&var_50);
                            int32_t (* var_40_1)() = private_set_fs;
                            private_set_fs(0);
                            private_vfs_write($v0_14, *($s0 + 0x144) + $s1_1, $s4_3, $v0_14 + 0x28);
                            private_filp_close($v0_14, 0);
                            $s7_3 += 1;
                            var_40_1($t1_1);
                            $s1_1 += $s4_3;
                        } while ($s7_3 != $s3_1);
                        
                        break;
                    }
                    
                    $s7_2 -= 1;
                    
                    if (!$s7_2)
                    {
                        goto label_1215c;
                    label_121b8:
                        private_seq_printf($s5, "snapraw timeout!\n", $a2_7);
                        $a0_19 = *($s0 + 0x140);
                    }
                }
                
                $a0_19 = *($s0 + 0x140);
                goto label_1215c;
            }
        }
        $a1 = "The node is busy!\\n";
        $a0 = $s5;
        $v0_3 = private_seq_printf;
    }
    
    return $v0_3($a0, $a1);
}

