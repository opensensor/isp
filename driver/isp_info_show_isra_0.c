#include "include/main.h"


  int32_t isp_info_show.isra.0(void* arg1)

{
    char* $v0 = *((char*)arg1 + 0x3c); // Fixed void pointer arithmetic
    void* $s5 = nullptr;
    uint32_t var_30 = 0x1388;
    char* $s6 = *((char*)$s5 + 0x1bc); // Fixed void pointer arithmetic
    uint32_t var_34 = 0;
    int32_t* $v0_6 = private_kmalloc(0x1e0, 0xd0);
    int32_t $s4_1 = 0xffffffff;
    int32_t $v0_8 = *($s5 + 0xf4);
    else if ($(uintptr_t)v0_8 >= 0x3202)
                else if ($(uintptr_t)v0_8 >= 0x3309)
    
    if ($v0 && $(uintptr_t)v0 < 0xfffff001)
        $s5 = *($v0 + 0xd4);
    
    int32_t var_3c;
    int32_t $v0_1;
    int32_t $a2_1;
    $v0_1 = private_seq_printf(arg1, 
        "width is %d, height is %d, imagesize is %d\n, snap num is %d, buf size is %d", 
        tisp_g_awb_start(&var_3c));
    
    if (*($s5 + 0xe8) < 4)
        return $v0_1 + private_seq_printf(arg1, "Can't output the width(%d)!\n", $a2_1);
    
    memset($v0_6, 0, 0x1e0);
    
    if ($v0_6)
        $s4_1 = tisp_get_antiflicker_step($v0_6, &var_34);
    
    int32_t var_e8;
    tisp_g_ev_attr(&var_e8);
    int32_t var_4c;
    tisp_g_ae_min(&var_4c);
    void var_68;
    tisp_g_wb_attr(&var_68);
    tisp_g_wb_ct(&var_30);
    char const* const $s7_1;
    
    if ($(uintptr_t)v0_8 == 0x3201)
        $s7_1 = "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\n";
    {
        if ($(uintptr_t)v0_8 == 0x3300)
            $s7_1 = "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\n";
        else
        {
            int32_t $v0_12;
            int32_t $v0_13;
            int32_t $v1_19;
            
            if ($(uintptr_t)v0_8 >= 0x3301)
            {
                if ($(uintptr_t)v0_8 == 0x3308)
                    $s7_1 = "qbuffer null\n";
                {
                    if ($(uintptr_t)v0_8 == 0x330c)
                        $s7_1 = "&vsd->mlock";
                    else if ($(uintptr_t)v0_8 < 0x330d)
                    {
                        $v0_13 = $(uintptr_t)v0_8 < 0x330b ? 1 : 0;
                        
                        if ($(uintptr_t)v0_8 == 0x330a)
                            $s7_1 = "Failed to allocate vic device\n";
                        else
                        {
                        label_77564:
                            
                            $s7_1 =
                                $v0_13 ? "bank no free\n" : "Failed to init isp module(%d.%d)\n";
                        }
                    }
                    else if ($(uintptr_t)v0_8 == 0x330e)
                        $s7_1 = " %d, %d\\n";
                    else
                    {
                        $v1_19 = 0x330f;
                        
                        if ($(uintptr_t)v0_8 < 0x330e)
                            $s7_1 = "&vsd->snap_mlock";
                        else
                        {
                        label_77588:
                            
                            $s7_1 = $v0_8 == $v1_19
                                ? "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n"
                                : "The parameter is invalid!\n";
                        }
                    }
                }
                else if ($(uintptr_t)v0_8 == 0x3304)
                    $s7_1 = "%s[%d] VIC do not support this format %d\\n";
                else if ($(uintptr_t)v0_8 >= 0x3305)
                {
                    $v0_12 = $(uintptr_t)v0_8 < 0x3307 ? 1 : 0;
                    
                    if ($(uintptr_t)v0_8 == 0x3306)
                        $s7_1 = "%s:%d::linear mode\n";
                    else
                    {
                    label_77538:
                        
                        $s7_1 =
                            $v0_12 ? "%s[%d] do not support this interface\n" : "%s:%d::wdr mode\n";
                    }
                }
                else if ($(uintptr_t)v0_8 == 0x3302 || $(uintptr_t)v0_8 < 0x3303)
                    $s7_1 = "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\\n";
                else
                    $s7_1 = "%s[%d] VIC failed to config DVP mode!(10bits-sensor)\\n";
            }
            else if ($(uintptr_t)v0_8 == 0x3208)
                $s7_1 = "qbuffer null\\n";
            else if ($(uintptr_t)v0_8 >= 0x3209)
            {
                if ($(uintptr_t)v0_8 == 0x320c)
                    $s7_1 = "&vsd->mlock";
                else if ($(uintptr_t)v0_8 < 0x320d)
                {
                    $v0_13 = $(uintptr_t)v0_8 < 0x320b ? 1 : 0;
                    
                    if ($(uintptr_t)v0_8 != 0x320a)
                        goto label_77564;
                    
                    $s7_1 = "Failed to allocate vic device\n";
                }
                else if ($(uintptr_t)v0_8 == 0x320e)
                    $s7_1 = " %d, %d\\n";
                else
                {
                    $v1_19 = 0x320f;
                    
                    if ($(uintptr_t)v0_8 >= 0x320e)
                        goto label_77588;
                    
                    $s7_1 = "&vsd->snap_mlock";
                }
            }
            else if ($(uintptr_t)v0_8 == 0x3204)
                $s7_1 = "%s[%d] VIC do not support this format %d\\n";
            else if ($(uintptr_t)v0_8 >= 0x3205)
            {
                $v0_12 = $(uintptr_t)v0_8 < 0x3207 ? 1 : 0;
                
                if ($(uintptr_t)v0_8 != 0x3206)
                    goto label_77538;
                
                $s7_1 = "%s:%d::linear mode\n";
            }
            else if ($(uintptr_t)v0_8 == 0x3202)
                $s7_1 = "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\\n";
            else if ($(uintptr_t)v0_8 == 0x3203)
                $s7_1 = "%s[%d] VIC failed to config DVP mode!(10bits-sensor)\\n";
            else
                $s7_1 = "The parameter is invalid!\\n";
        }
    }
    else if ($(uintptr_t)v0_8 == 0x3102)
        $s7_1 = "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\\n";
    else if ($(uintptr_t)v0_8 >= 0x3103)
    {
        else if ($(uintptr_t)v0_8 >= 0x310a)
            else if ($(uintptr_t)v0_8 >= 0x310e)
                else if ($(uintptr_t)v0_8 == 0x3200)
        if ($(uintptr_t)v0_8 == 0x3109)
            $s7_1 = "bank no free\n";
        {
            if ($(uintptr_t)v0_8 == 0x310d)
                $s7_1 = "&vsd->snap_mlock";
            {
                if ($(uintptr_t)v0_8 == 0x310f)
                    $s7_1 = "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n";
                else if ($(uintptr_t)v0_8 < 0x310f)
                    $s7_1 = " %d, %d\n";
                    $s7_1 = "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\n";
                else
                    $s7_1 = "The parameter is invalid!\n";
            }
            else if ($(uintptr_t)v0_8 == 0x310b)
                $s7_1 = "Failed to init isp module(%d.%d)\\n";
            else if ($(uintptr_t)v0_8 < 0x310c)
                $s7_1 = "Failed to allocate vic device\\n";
            else
                $s7_1 = "&vsd->mlock";
        }
        else if ($(uintptr_t)v0_8 == 0x3105)
            $s7_1 = "%s[%d] do not support this interface\\n";
        else if ($(uintptr_t)v0_8 >= 0x3106)
        {
            if ($(uintptr_t)v0_8 == 0x3107)
                $s7_1 = "%s:%d::wdr mode\n";
            else if ($(uintptr_t)v0_8 < 0x3108)
                $s7_1 = "%s:%d::linear mode\n";
            else
                $s7_1 = "qbuffer null\n";
        }
        else if ($(uintptr_t)v0_8 == 0x3103)
            $s7_1 = "%s[%d] VIC failed to config DVP mode!(10bits-sensor)\\n";
        else if ($(uintptr_t)v0_8 == 0x3104)
            $s7_1 = "%s[%d] VIC do not support this format %d\\n";
        else
            $s7_1 = "The parameter is invalid!\\n";
    }
    else if ($(uintptr_t)v0_8 == 0x300f)
        $s7_1 = "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\\n";
    else if ($(uintptr_t)v0_8 >= 0x3010)
    {
        else if ($(uintptr_t)v0_8 >= 0x3014)
            else if ($(uintptr_t)v0_8 >= 0x3101 || $(uintptr_t)v0_8 == 0x3014)
        if ($(uintptr_t)v0_8 == 0x3013)
            $s7_1 = "sensor type is BT601!\n";
        {
            if ($(uintptr_t)v0_8 == 0x3100)
                $s7_1 = "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\n";
                $s7_1 = "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\n";
            else
                $s7_1 = "The parameter is invalid!\n";
        }
        else if ($(uintptr_t)v0_8 == 0x3011)
            $s7_1 = "%s[%d] VIC failed to config DVP mode!(8bits-sensor)\\n";
        else if ($(uintptr_t)v0_8 < 0x3012)
            $s7_1 = "sensor type is BT601!\\n";
        else
            $s7_1 = "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\\n";
    }
    else if ($(uintptr_t)v0_8 >= 0x3009)
    {
        else if ($(uintptr_t)v0_8 == 0x300e)
        if ($(uintptr_t)v0_8 == 0x300a)
            $s7_1 = "%s[%d] VIC failed to config DVP mode!(8bits-sensor)\n";
            $s7_1 = "sensor type is BT601!\n";
        else
            $s7_1 = "The parameter is invalid!\n";
    }
    else if ($(uintptr_t)v0_8 >= 0x3007 || $(uintptr_t)v0_8 == 0x3001)
        $s7_1 = "sensor type is BT656!\\n";
    else if ($(uintptr_t)v0_8 == 0x3002)
        $s7_1 = "%s[%d] VIC failed to config DVP mode!(8bits-sensor)\\n";
    else
        $s7_1 = "The parameter is invalid!\\n";
    
    int32_t $s0_7 = $v0_1 + private_seq_printf(arg1, "/tmp/snap%d.%s", "The node is busy!\\n")
        + private_seq_printf(arg1, "nv12", **($s5 + 0x120))
        + private_seq_printf(arg1, &$LC33, *($s5 + 0xec))
        + private_seq_printf(arg1, "saveraw", *($s5 + 0xf0)) + private_seq_printf(arg1, 
        "width is %d, height is %d, imagesize is %d\\n, save num is %d, buf size is %d", 
        *($s5 + 0x12c) >> 0x10) + private_seq_printf(arg1, "help", $s7_1)
        + private_seq_printf(arg1, &$LC37, tisp_top_read());
    char const* const $a2_9;
    
    if (tisp_day_or_night_g_ctrl() != 1)
        $a2_9 = "register is 0x%x, value is 0x%x\\n";
    else
        $a2_9 = "vic_done_gpio%d";
    
    int32_t $s0_8 = $s0_7 + private_seq_printf(arg1, "help:\\n", $a2_9);
    char const* const $a2_10;
    
    if (tisp_cust_mode_g_ctrl() != 1)
        $a2_10 = "snapraw";
    else
        $a2_10 = "count is %d\\n";
    
    int32_t $v0_27 = private_seq_printf(arg1, "\\t cmd:\\n", $a2_10);
    char const* const $a2_11;
    
    if (*($s5 + 0x17c) != 1)
        $a2_11 = "snapraw";
    else
        $a2_11 = "count is %d\\n";
    
    int32_t var_d8_2;
    int32_t var_c8_4;
    int16_t var_7a;
    int32_t $s0_14 = $s0_8 + $v0_27 + private_seq_printf(arg1, "\\t\\t snapraw\\n", $a2_11) +
        private_seq_printf(arg1, "\\t\\t\\t use cmd " snapraw" you should set ispmem first!!!!!\\n", 
        var_e8_4) + private_seq_printf(arg1, 
        "\\t\\t\\t please use this cmd: \\n\\t"echo snapraw savenum > /proc/jz/isp/isp-w02"\\n", var_7a_1)
        + private_seq_printf(arg1, "\\t\\t\\t "snapraw"  is cmd; \\n", var_d8_3) +
        private_seq_printf(arg1, "\\t\\t\\t "savenum" is the num of you save raw picture.\\n ", var_c8_5);
    int32_t var_e4;
    int32_t var_d4_8;
    int32_t var_d0_9;
    int32_t var_c4_7;
    int32_t var_c0_4;
    int32_t var_bc_3;
    int32_t $s0_20 = $s0_14 + private_seq_printf(arg1, "\\t\\t saveraw\\n", var_c0_5) +
        private_seq_printf(arg1, 
        "\\t\\t\\t please use this cmd: \\n\\t"echo saveraw savenum > /proc/jz/isp/isp-w02"\\n", var_bc_4)
        + private_seq_printf(arg1, "\\t\\t\\t "saveraw"  is cmd; \\n", var_d4_9)
        + private_seq_printf(arg1, "Can\'t ops the node!\\n", var_c4_8)
        + private_seq_printf(arg1, "snapraw timeout!\\n", var_d0_10)
        + private_seq_printf(arg1, "streamon", var_e4_1);
    int32_t var_e0_4;
    int32_t var_dc_1;
    int32_t var_54_12;
    int32_t var_48_21;
    int32_t $s0_25 = $s0_20 + private_seq_printf(arg1, "streamoff", var_dc_2)
        + private_seq_printf(arg1, "%s[%d]: invalid parameter\\n", var_e0_5)
        + private_seq_printf(arg1, "%s[%d]: %s\\n", var_4c_8)
        + private_seq_printf(arg1, "%s[%d] SET ERR GPIO(%d),STATE(%d),%d", var_48_22)
        + private_seq_printf(arg1, "line : %d; bank_addr:0x%x; addr:0x%x\\n", isp_printf / var_54_13);
    int32_t var_50_11;
    int32_t $s0_31 = $s0_25 + private_seq_printf(arg1, 
        "line = %d, i=%d ;num = %d;busy_buf_count %d\\n", isp_printf / var_50_12)
        + private_seq_printf(arg1, "function: %s ; vic dma addrrss error!!!\\n", var_30_12)
        + private_seq_printf(arg1, "VIC_ADDR_DMA_CONTROL : 0x%x\\n", var_3c_6)
        + private_seq_printf(arg1, "busy_buf null; busy_buf_count= %d\\n", *($s6 + 0x4090))
        + private_seq_printf(arg1, "busy_buf null; busy_buf_count= %d\\n", tisp_get_saturation()) +
        private_seq_printf(arg1, "Info[VIC_MDAM_IRQ] : channel[%d] frame done\\n", 
        tisp_get_sharpness());
    int32_t $s0_32 = $s0_31
        + private_seq_printf(arg1, "Err [VIC_INT] : frame asfifo ovf!!!!!\\n", tisp_get_contrast());
    int32_t $v0_55 = private_seq_printf(arg1, "Err [VIC_INT] : hor err ch0 !!!!! 0x3a8 = 0x%08x\\n", 
        tisp_get_brightness());
    int32_t $v0_56 =
        private_seq_printf(arg1, "Err [VIC_INT] : hor err ch1 !!!!!\\n", *($s6 + 0xefc));
    char const* const $a2_38;
    
    if (*($s5 + 0x170) != 1)
        $a2_38 = "snapraw";
    else
        $a2_38 = "count is %d\\n";
    
    *($s5 + 0x168);
    int32_t $s0_35 = $s0_32 + $v0_55 + $v0_56
        + private_seq_printf(arg1, "Err [VIC_INT] : hor err ch2 !!!!!\\n", $a2_38);
    
    if (!$s4_1)
    {
        int32_t $s0_36 = $s0_35 + $v0_58;
        int32_t* $s5_1 = $v0_6;
        int32_t $v0_58;
        int32_t $a2_41;
        $v0_58 = private_seq_printf(arg1, "Err [VIC_INT] : hor err ch3 !!!!!\n", var_34 + 1);
        
        while (var_34 >= $s4_1)
        {
            int32_t $v0_61;
            $v0_61 = private_seq_printf(arg1, tisp_day_or_night_s_ctrl, *$s5_1);
            $s0_36 += $v0_61;
            $s4_1 += 1;
            $s5_1 = &$s5_1[1];
        }
        
        $s0_35 = $s0_36 + private_seq_printf(arg1, "Err [VIC_INT] : ver err ch1 !!!!!\\n", $a2_41);
    }
    
    int32_t var_ec_1_2 = data_ca574_1;
    int32_t var_f0_1_1 = data_ca578_1;
    uint32_t isp_err3_1 = isp_err3;
    int32_t var_f8_1_1 = 0;
    int32_t var_fc_1_1 = 0;
    int32_t var_100_1_1 = data_ca57c_1;
    int32_t $s0_37 =
        private_seq_printf(arg1, "Err [VIC_INT] : ver err ch2 !!!!!\\n", data_ca584_1) + $s0_35;
    uint32_t isp_ch0_pre_dequeue_valid_lines_1 = isp_ch0_pre_dequeue_valid_lines;
    int32_t result = $s0_37 +
        private_seq_printf(arg1, "Err [VIC_INT] : ver err ch3 !!!!!\\n", isp_ch0_pre_dequeue_time);
    private_kfree($v0_6);
    return result;
}

