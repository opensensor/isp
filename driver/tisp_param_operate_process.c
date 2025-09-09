#include "include/main.h"


  int32_t tisp_param_operate_process(int32_t* arg1)

{
    int32_t $v1 = *arg1;
    int16_t var_20_25 = 0;
    int32_t* opmsg_1;
    int16_t $a1_45;
    
    if (!$v1)
    {
        int32_t $a0 = arg1[1];
        int32_t (* $v0_4)(int32_t arg1, int32_t arg2, int32_t* arg3);
        
        if ($a0 && $a0 - 1 >= 0x22)
        {
            if ($a0 - 0x23 < 0x19)
            {
                opmsg;
                $v0_4 = tisp_awb_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x3c < 2)
            {
                opmsg;
                $v0_4 = tisp_gamma_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x3e < 0x16)
            {
                opmsg;
                $v0_4 = tisp_gib_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x5f < 0x4a)
            {
                opmsg;
                $v0_4 = tisp_dmsc_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x54 < 0xb)
            {
                opmsg;
                $v0_4 = tisp_lsc_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0xa9 < 0xc)
            {
                opmsg;
                $v0_4 = tisp_ccm_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0xb5 < 0x31)
            {
                opmsg;
                $v0_4 = tisp_sharpen_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0xe6 < 0x1f)
            {
                opmsg;
                $v0_4 = tisp_dpc_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x105 < 0x7b)
            {
                opmsg;
                $v0_4 = tisp_sdns_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x180 < 0x1d7)
            {
                opmsg;
                $v0_4 = tisp_mdns_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x357 < 3)
            {
                opmsg;
                $v0_4 = tisp_clm_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x35a < 0x26)
            {
                opmsg;
                $v0_4 = tisp_defog_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x380 < 0x2c)
            {
                opmsg;
                $v0_4 = tisp_adr_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 == 0x3ac)
            {
                opmsg;
                $v0_4 = tisp_hldc_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x3ad < 0x13)
            {
                opmsg;
                $v0_4 = tisp_af_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x3c0 < 0x26)
            {
                opmsg;
                $v0_4 = tisp_bcsh_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x3e6 < 0xf)
            {
                opmsg;
                $v0_4 = tisp_ydns_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x3f5 < 0xa)
            {
                opmsg;
                $v0_4 = tisp_gb_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x3ff < 0x33)
            {
                opmsg;
                $v0_4 = tisp_wdr_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x432 < 0x15)
            {
                opmsg;
                $v0_4 = tisp_rdns_param_array_get;
                goto label_21d34;
            }
            
        label_22068:
            isp_printf(2, &$LC0, "tisp_param_operate_process");
            return 0xffffffff;
        }
        
        opmsg;
        $v0_4 = tisp_ae_param_array_get;
    label_21d34:
        $v0_4();
        opmsg_1 = opmsg;
        *opmsg_1 = *arg1;
        opmsg_1[1] = arg1[1];
        opmsg_1[2] = arg1[2];
        $a1_45 = var_20_26 + 0x18;
        opmsg_1[3] = arg1[3];
        opmsg_1[4] = arg1[4];
    }
    else if ($v1 == 1)
    {
        int32_t $a0_2 = arg1[1];
        int32_t var_20_1_4 = arg1[2];
        int32_t (* $v0_51)(int32_t arg1, int32_t arg2, int32_t* arg3);
        
        if ($a0_2 && $a0_2 - 1 >= 0x22)
        {
            if ($a0_2 - 0x23 < 0x19)
            {
                $v0_51 = tisp_awb_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_2 - 0x3c < 2)
            {
                $v0_51 = tisp_gamma_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_2 - 0x3e < 0x16)
            {
                $v0_51 = tisp_gib_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_2 - 0x54 < 0xb)
            {
                $v0_51 = tisp_lsc_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_2 - 0x5f < 0x4a)
            {
                $v0_51 = tisp_dmsc_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_2 - 0xa9 < 0xc)
            {
                $v0_51 = Tiziano_awb_set_gain;
                goto label_2203c;
            }
            
            if ($a0_2 - 0xb5 < 0x31)
            {
                $v0_51 = tisp_sharpen_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_2 - 0xe6 < 0x1f)
            {
                $v0_51 = tisp_dpc_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_2 - 0x105 < 0x7b)
            {
                $v0_51 = tisp_sdns_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_2 - 0x180 < 0x1d7)
            {
                $v0_51 = tisp_mdns_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_2 - 0x357 < 3)
            {
                $v0_51 = tisp_clm_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_2 - 0x35a < 0x26)
            {
                $v0_51 = tisp_defog_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_2 - 0x380 < 0x2c)
            {
                $v0_51 = tisp_adr_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_2 == 0x3ac)
            {
                $v0_51 = tisp_hldc_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_2 - 0x3ad < 0x13)
            {
                $v0_51 = tisp_af_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_2 - 0x3c0 < 0x26)
            {
                $v0_51 = tisp_bcsh_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_2 - 0x3e6 < 0xf)
            {
                $v0_51 = tisp_ydns_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_2 - 0x3f5 < 0xa)
            {
                $v0_51 = tisp_gb_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_2 - 0x3ff < 0x33)
            {
                $v0_51 = tisp_wdr_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_2 - 0x432 >= 0x15)
                goto label_22068;
            
            $v0_51 = tisp_rdns_param_array_set;
            goto label_2203c;
        }
        
        $v0_51 = tisp_ae_param_array_set;
    label_2203c:
        $v0_51();
        $a1_45 = 0x18;
        opmsg_1 = opmsg;
    }
    else
    {
        int32_t var_b8_10;
        
        if ($v1 != 2)
        {
            if ($v1 != 3)
            {
                isp_printf(2, "sensor type is BT1120!\\n", "tisp_param_operate_process");
                return 0xffffffff;
            }
            
            int32_t $v0_100 = arg1[1];
            int32_t* opmsg_3 = opmsg;
            *opmsg_3 = $v1;
            opmsg_3[1] = arg1[1];
            opmsg_3[2] = arg1[2];
            opmsg_3[3] = arg1[3];
            int32_t $a3_8 = arg1[4];
            opmsg_3[5] = 0;
            opmsg_3[4] = $a3_8;
            
            if ($v0_100)
            {
                if ($v0_100 == 0xd)
                {
                    int32_t $v0_102 = private_kmalloc(0x42c, 0xd0);
                    var_b8_11 = $v0_102;
                    tisp_g_ae_hist($v0_102);
                    memcpy(&opmsg_3[6], &var_b8_12, 0x42c);
                    netlink_send_msg(opmsg, 0x444);
                    private_kfree(var_b8_13);
                    return 0;
                }
                
                if ($v0_100 == 1)
                {
                    int32_t $v0_103 = system_reg_read(0xc);
                    opmsg_1 = opmsg;
                    opmsg_1[2] = $v0_103;
                    $a1_45 = 0x18;
                }
                else if ($v0_100 == 0x10)
                {
                    var_b8_14 = 0;
                    tisp_g_wdr_en(&var_b8_15);
                    opmsg_1 = opmsg;
                label_22568:
                    opmsg_1[3] = var_b8_16;
                    $a1_45 = 0x18;
                }
                else if ($v0_100 == 2)
                {
                    int32_t $v0_104 = system_reg_read(arg1[2] & 0xfffff);
                    opmsg_1 = opmsg;
                    opmsg_1[3] = $v0_104;
                    $a1_45 = 0x18;
                }
                else if ($v0_100 != $v1)
                {
                    if ($v0_100 == 0xc)
                    {
                        var_b8_17 = 0;
                        tisp_g_Hilightdepress(&var_b8_18);
                        opmsg_1 = opmsg;
                        goto label_22568;
                    }
                    
                    if ($v0_100 != 4)
                    {
                        if ($v0_100 != 5)
                        {
                            isp_printf(2, &$LC0, "tisp_param_operate_process");
                            *(opmsg + 0x14) = 1;
                            return 0;
                        }
                        
                        tisp_g_wb_attr(&var_b8_19);
                        memcpy(&opmsg_3[6], &var_b8_20, 0x1c);
                        $a1_45 = 0x34;
                    }
                    else
                    {
                        memcpy(&opmsg_3[6], &sensor_info, 0x60);
                        $a1_45 = 0x78;
                    }
                    
                    opmsg_1 = opmsg;
                }
                else
                {
                    int32_t $v0_105;
                    int32_t $a2_11;
                    $v0_105 = tisp_day_or_night_g_ctrl();
                    int32_t $v1_7 = 0;
                    
                    if (!$v0_105)
                        opmsg_1 = opmsg;
                    else
                    {
                        $v1_7 = 1;
                        
                        if ($v0_105 == 1)
                            opmsg_1 = opmsg;
                        else
                        {
                            isp_printf(2, "Can not support this frame mode!!!\\n", $a2_11);
                            $v1_7 = 0;
                            opmsg_1 = opmsg;
                        }
                    }
                    
                    opmsg_1[3] = $v1_7;
                    $a1_45 = 0x18;
                }
            }
            else
            {
                memcpy(&opmsg_3[6], &dmsc_sp_d_w_stren_wdr_array, 0x98);
                $a1_45 = 0xb0;
                opmsg_1 = opmsg;
            }
        }
        else
        {
            int32_t $v0_89 = arg1[1];
            int32_t* opmsg_2 = opmsg;
            *opmsg_2 = $v1;
            opmsg_2[1] = arg1[1];
            opmsg_2[2] = arg1[2];
            opmsg_2[3] = arg1[3];
            int32_t $a2_4 = arg1[4];
            opmsg_2[5] = 0;
            opmsg_2[4] = $a2_4;
            int32_t var_4d8;
            int32_t var_a8_17;
            
            if (!$v0_89)
            {
                memcpy(&var_b8_21, &arg1[6], 0x98);
                
                for (int32_t i = 0; i < 0x88; i += 1)
                    *(&var_4d8_1 + i) = *(&var_a8_18 + i);
                
                tisp_ae_manual_set();
                $a1_45 = 0x18;
            }
            else if ($v0_89 == 1)
            {
                system_reg_write(0xc, arg1[2]);
                $a1_45 = 0x18;
            }
            else if ($v0_89 == 0xb)
            {
                memcpy(&var_b8_22, &arg1[6], 0x98);
                
                for (int32_t i_1 = 0; i_1 < 0x88; i_1 += 1)
                    *(&var_4d8_2 + i_1) = *(&var_a8_19 + i_1);
                
                tisp_ae_min_max_set();
                $a1_45 = 0x18;
            }
            else if ($v0_89 == 0xd)
            {
                int32_t* $v0_91 = private_kmalloc(0x42c, 0xd0);
                memcpy($v0_91, &arg1[6], 0x42c);
                
                for (int32_t i_2 = 0; i_2 < 0x41c; i_2 += 1)
                    *(&var_4d8_3 + i_2) = *(&$v0_91[4] + i_2);
                
                tisp_s_ae_hist();
                private_kfree();
                $a1_45 = 0x18;
            }
            else if ($v0_89 == $v1)
            {
                system_reg_write(arg1[2] & 0x1ffff, arg1[3]);
                $a1_45 = 0x18;
            }
            else if ($v0_89 == 3)
            {
                int32_t $v0_93 = arg1[2];
                
                if ($v0_93 && $v0_93 != 1)
                    isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", 
                        "tisp_param_operate_process");
                
                tisp_day_or_night_s_ctrl();
                $a1_45 = 0x18;
            }
            else if ($v0_89 == 0xc)
            {
                arg1[2];
                tisp_s_Hilightdepress();
                $a1_45 = 0x18;
            }
            else if ($v0_89 == 0x10)
            {
                arg1[2];
                tisp_s_wdr_en();
                $a1_45 = 0x18;
            }
            else if ($v0_89 == 9)
            {
                arg1[2];
                tisp_set_brightness();
                $a1_45 = 0x18;
            }
            else if ($v0_89 == 6)
            {
                arg1[2];
                tisp_set_saturation();
                $a1_45 = 0x18;
            }
            else if ($v0_89 == 7)
            {
                arg1[2];
                tisp_set_sharpness();
                $a1_45 = 0x18;
            }
            else if ($v0_89 == 8)
            {
                arg1[2];
                tisp_set_contrast();
                $a1_45 = 0x18;
            }
            else
            {
                int32_t var_b4_2;
                int32_t var_b0_49;
                int32_t var_ac_12;
                
                if ($v0_89 == 5)
                {
                    memcpy(&var_b8_23, &arg1[6], 0x1c);
                    int32_t $a0_21 = var_b8_24;
                    var_4d8_4 = var_a8_20;
                    int32_t var_a0_21;
                    int32_t var_4d0_1_1 = var_a0_22;
                    int32_t var_a4_12;
                    tisp_s_wb_attr($a0_21, var_b4_3, var_b0_50, var_ac_13, var_4d8_5, var_a4_13);
                    $a1_45 = 0x18;
                }
                else if ($v0_89 != 0xa)
                {
                    isp_printf(2, &$LC0, "tisp_param_operate_process");
                    *(opmsg + 0x14) = 1;
                    $a1_45 = 0x18;
                }
                else
                {
                    memcpy(&var_b8_25, &arg1[6], 0x14);
                    int32_t $a0_23 = var_b8_26;
                    var_4d8_6 = var_a8_21;
                    tisp_s_fcrop_control($a0_23, var_b4_4, var_b0_51, var_ac_14, var_4d8_7);
                    $a1_45 = 0x18;
                }
            }
            opmsg_1 = opmsg;
        }
    }
    
    netlink_send_msg(opmsg_1, $a1_45);
    return 0;
}

