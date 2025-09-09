#include "include/main.h"


  int32_t tisp_param_operate_process(int32_t* arg1)

{
    int32_t $v1 = *arg1;
    int32_t var_20_25 = 0;
    int32_t* opmsg_1;
    int16_t $a1_25;
    
    if (!$v1)
    {
        int32_t $a0 = arg1[1];
        int32_t (* $v0_4)(int32_t arg1, int32_t arg2, int32_t* arg3);
        int32_t $a1_2;
        int32_t* $a2_1;
        
        if ($a0 && $a0 - 1 >= 0x22)
        {
            if ($a0 - 0x23 < 0x19)
            {
                $a2_1 = &var_20_26;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_awb_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x3c < 2)
            {
                $a2_1 = &var_20_27;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_gamma_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x3e < 0x16)
            {
                $a2_1 = &var_20_28;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_gib_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x5f < 0x4a)
            {
                $a2_1 = &var_20_29;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_dmsc_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x54 < 0xb)
            {
                $a2_1 = &var_20_30;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_lsc_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0xa9 < 0xc)
            {
                $a2_1 = &var_20_31;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_ccm_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0xb5 < 0x31)
            {
                $a2_1 = &var_20_32;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_sharpen_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0xe6 < 0x1f)
            {
                $a2_1 = &var_20_33;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_dpc_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x105 < 0x7b)
            {
                $a2_1 = &var_20_34;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_sdns_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x180 < 0x1d7)
            {
                $a2_1 = &var_20_35;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_mdns_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x357 < 3)
            {
                $a2_1 = &var_20_36;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_clm_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x35a < 0x26)
            {
                $a2_1 = &var_20_37;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_defog_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x380 < 0x2c)
            {
                $a2_1 = &var_20_38;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_adr_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 == 0x3ac)
            {
                $a2_1 = &var_20_39;
                $a1_2 = opmsg + 0x18;
                $a0 = 0x3ac;
                $v0_4 = tisp_hldc_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x3ad < 0x13)
            {
                $a2_1 = &var_20_40;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_af_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x3c0 < 0x26)
            {
                $a2_1 = &var_20_41;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_bcsh_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x3e6 < 0xf)
            {
                $a2_1 = &var_20_42;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_ydns_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x3f5 < 0xa)
            {
                $a2_1 = &var_20_43;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_gb_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x3ff < 0x33)
            {
                $a2_1 = &var_20_44;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_wdr_param_array_get;
                goto label_21d34;
            }
            
            if ($a0 - 0x432 >= 0x15)
            {
            label_22068:
                isp_printf(2, &$LC0, "tisp_param_operate_process");
                return 0xffffffff;
            }
            
            $a2_1 = &var_20_45;
            $a1_2 = opmsg + 0x18;
            $v0_4 = tisp_rdns_param_array_get;
            goto label_21d34;
        }
        
        $a2_1 = &var_20_46;
        $a1_2 = opmsg + 0x18;
        $v0_4 = tisp_ae_param_array_get;
    label_21d34:
        $v0_4($a0, $a1_2, $a2_1);
        opmsg_1 = opmsg;
        *opmsg_1 = *arg1;
        int16_t $a1_23 = var_20_47;
        opmsg_1[1] = arg1[1];
        opmsg_1[2] = arg1[2];
        $a1_25 = $a1_23 + 0x18;
        opmsg_1[3] = arg1[3];
        opmsg_1[4] = arg1[4];
    }
    else if ($v1 == 1)
    {
        int32_t $a0_1 = arg1[1];
        var_20_48 = arg1[2];
        int32_t (* $v0_51)(int32_t arg1, int32_t arg2, int32_t* arg3);
        void* $a1_26;
        int32_t* $a2_2;
        
        if ($a0_1 && $a0_1 - 1 >= 0x22)
        {
            if ($a0_1 - 0x23 < 0x19)
            {
                $a2_2 = &var_20_49;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_awb_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_1 - 0x3c < 2)
            {
                $a2_2 = &var_20_50;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_gamma_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_1 - 0x3e < 0x16)
            {
                $a2_2 = &var_20_51;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_gib_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_1 - 0x54 < 0xb)
            {
                $a2_2 = &var_20_52;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_lsc_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_1 - 0x5f < 0x4a)
            {
                $a2_2 = &var_20_53;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_dmsc_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_1 - 0xa9 < 0xc)
            {
                $a2_2 = &var_20_54;
                $a1_26 = &arg1[6];
                $v0_51 = Tiziano_awb_set_gain;
                goto label_2203c;
            }
            
            if ($a0_1 - 0xb5 < 0x31)
            {
                $a2_2 = &var_20_55;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_sharpen_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_1 - 0xe6 < 0x1f)
            {
                $a2_2 = &var_20_56;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_dpc_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_1 - 0x105 < 0x7b)
            {
                $a2_2 = &var_20_57;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_sdns_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_1 - 0x180 < 0x1d7)
            {
                $a2_2 = &var_20_58;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_mdns_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_1 - 0x357 < 3)
            {
                $a2_2 = &var_20_59;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_clm_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_1 - 0x35a < 0x26)
            {
                $a2_2 = &var_20_60;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_defog_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_1 - 0x380 < 0x2c)
            {
                $a2_2 = &var_20_61;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_adr_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_1 == 0x3ac)
            {
                $a2_2 = &var_20_62;
                $a1_26 = &arg1[6];
                $a0_1 = 0x3ac;
                $v0_51 = tisp_hldc_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_1 - 0x3ad < 0x13)
            {
                $a2_2 = &var_20_63;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_af_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_1 - 0x3c0 < 0x26)
            {
                $a2_2 = &var_20_64;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_bcsh_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_1 - 0x3e6 < 0xf)
            {
                $a2_2 = &var_20_65;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_ydns_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_1 - 0x3f5 < 0xa)
            {
                $a2_2 = &var_20_66;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_gb_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_1 - 0x3ff < 0x33)
            {
                $a2_2 = &var_20_67;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_wdr_param_array_set;
                goto label_2203c;
            }
            
            if ($a0_1 - 0x432 >= 0x15)
                goto label_22068;
            
            $a2_2 = &var_20_68;
            $a1_26 = &arg1[6];
            $v0_51 = tisp_rdns_param_array_set;
            goto label_2203c;
        }
        
        $a2_2 = &var_20_69;
        $a1_26 = &arg1[6];
        $v0_51 = tisp_ae_param_array_set;
    label_2203c:
        $v0_51($a0_1, $a1_26, $a2_2);
        $a1_25 = 0x18;
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
            int32_t $a3_7 = arg1[4];
            opmsg_3[5] = 0;
            opmsg_3[4] = $a3_7;
            
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
                    $a1_25 = 0x18;
                }
                else if ($v0_100 == 0x10)
                {
                    var_b8_14 = 0;
                    tisp_g_wdr_en(&var_b8_15);
                    opmsg_1 = opmsg;
                label_22568:
                    opmsg_1[3] = var_b8_16;
                    $a1_25 = 0x18;
                }
                else if ($v0_100 == 2)
                {
                    int32_t $v0_104 = system_reg_read(arg1[2] & 0xfffff);
                    opmsg_1 = opmsg;
                    opmsg_1[3] = $v0_104;
                    $a1_25 = 0x18;
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
                        $a1_25 = 0x34;
                    }
                    else
                    {
                        memcpy(&opmsg_3[6], &sensor_info, 0x60);
                        $a1_25 = 0x78;
                    }
                    
                    opmsg_1 = opmsg;
                }
                else
                {
                    int32_t $v0_105;
                    int32_t $a2_12;
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
                            isp_printf(2, "Can not support this frame mode!!!\\n", $a2_12);
                            $v1_7 = 0;
                            opmsg_1 = opmsg;
                        }
                    }
                    
                    opmsg_1[3] = $v1_7;
                    $a1_25 = 0x18;
                }
            }
            else
            {
                memcpy(&opmsg_3[6], &dmsc_sp_d_w_stren_wdr_array, 0x98);
                $a1_25 = 0xb0;
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
            int32_t $a2_6 = arg1[4];
            opmsg_2[5] = 0;
            opmsg_2[4] = $a2_6;
            int32_t var_4d8;
            int32_t var_b4_2;
            int32_t var_b0_49;
            int32_t var_ac_12;
            int32_t var_a8_17;
            
            if (!$v0_89)
            {
                memcpy(&var_b8_21, &arg1[6], 0x98);
                
                for (int32_t i = 0; i < 0x88; i += 1)
                    *(&var_4d8_1 + i) = *(&var_a8_18 + i);
                
                tisp_ae_manual_set(var_b8_22, var_b4_3, var_b0_50, var_ac_13);
                $a1_25 = 0x18;
            }
            else if ($v0_89 == 1)
            {
                system_reg_write(0xc, arg1[2]);
                $a1_25 = 0x18;
            }
            else if ($v0_89 == 0xb)
            {
                memcpy(&var_b8_23, &arg1[6], 0x98);
                
                for (int32_t i_1 = 0; i_1 < 0x88; i_1 += 1)
                    *(&var_4d8_2 + i_1) = *(&var_a8_19 + i_1);
                
                tisp_ae_min_max_set(var_b8_24, var_b4_4, var_b0_51, var_ac_14);
                $a1_25 = 0x18;
            }
            else if ($v0_89 == 0xd)
            {
                uint32_t $v0_91 = private_kmalloc(0x42c, 0xd0);
                memcpy($v0_91, &arg1[6], 0x42c);
                
                for (int32_t i_2 = 0; i_2 < 0x41c; i_2 += 1)
                    *(&var_4d8_3 + i_2) = *($v0_91 + 0x10 + i_2);
                
                tisp_s_ae_hist();
                private_kfree($v0_91);
                $a1_25 = 0x18;
            }
            else if ($v0_89 == $v1)
            {
                system_reg_write(arg1[2] & 0x1ffff, arg1[3]);
                $a1_25 = 0x18;
            }
            else if ($v0_89 == 3)
            {
                int32_t $v0_93 = arg1[2];
                int32_t $a0_9 = 0;
                
                if ($v0_93)
                {
                    $a0_9 = 1;
                    
                    if ($v0_93 != 1)
                    {
                        isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", 
                            "tisp_param_operate_process");
                        $a0_9 = 0;
                    }
                }
                
                tisp_day_or_night_s_ctrl($a0_9);
                $a1_25 = 0x18;
            }
            else if ($v0_89 == 0xc)
            {
                tisp_s_Hilightdepress(arg1[2]);
                $a1_25 = 0x18;
            }
            else if ($v0_89 == 0x10)
            {
                tisp_s_wdr_en(arg1[2]);
                $a1_25 = 0x18;
            }
            else if ($v0_89 == 9)
            {
                tisp_set_brightness(arg1[2]);
                $a1_25 = 0x18;
            }
            else if ($v0_89 == 6)
            {
                tisp_set_saturation(arg1[2]);
                $a1_25 = 0x18;
            }
            else if ($v0_89 == 7)
            {
                tisp_set_sharpness(arg1[2]);
                $a1_25 = 0x18;
            }
            else if ($v0_89 == 8)
            {
                tisp_set_contrast(arg1[2]);
                $a1_25 = 0x18;
            }
            else if ($v0_89 == 5)
            {
                memcpy(&var_b8_25, &arg1[6], 0x1c);
                int32_t $a0_11 = var_b8_26;
                var_4d8_4 = var_a8_20;
                int32_t var_a0_21;
                int32_t var_4d0_1_1 = var_a0_22;
                int32_t var_a4_12;
                tisp_s_wb_attr($a0_11, var_b4_5, var_b0_52, var_ac_15, var_4d8_5, var_a4_13);
                $a1_25 = 0x18;
            }
            else if ($v0_89 != 0xa)
            {
                isp_printf(2, &$LC0, "tisp_param_operate_process");
                *(opmsg + 0x14) = 1;
                $a1_25 = 0x18;
            }
            else
            {
                memcpy(&var_b8_27, &arg1[6], 0x14);
                int32_t $a0_13 = var_b8_28;
                var_4d8_6 = var_a8_21;
                tisp_s_fcrop_control($a0_13, var_b4_6, var_b0_53, var_ac_16, var_4d8_7);
                $a1_25 = 0x18;
            }
            opmsg_1 = opmsg;
        }
    }
    
    netlink_send_msg(opmsg_1, $a1_25);
    return 0;
}

