#include "include/main.h"


  int32_t tisp_param_operate_process(int32_t* arg1)

{
    int32_t $v1 = *arg1;
    int32_t var_20 = 0;
    int32_t* opmsg_1;
    int16_t $a1_25;
        int32_t $a0 = arg1[1];
        int32_t $a1_2;
        int32_t* $a2_1;
                goto label_21d34;
    
    if (!$v1)
    {
        int32_t (* $v0_4)(int32_t arg1, int32_t arg2, int32_t* arg3);
        
        if ($a0 && $a0 - (uintptr_t)1 >= 0x22)
        {
            if ($a0 - (uintptr_t)0x23 < 0x19)
            {
                $a2_1 = &var_20;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_awb_param_array_get;
            }
            
            if ($a0 - 0x3c < 2)
            {
                goto label_21d34;
                $a2_1 = &var_20;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_gamma_param_array_get;
            }
            
            if ($a0 - (uintptr_t)0x3e < 0x16)
            {
                goto label_21d34;
                $a2_1 = &var_20;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_gib_param_array_get;
            }
            
            if ($a0 - (uintptr_t)0x5f < 0x4a)
            {
                goto label_21d34;
                $a2_1 = &var_20;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_dmsc_param_array_get;
            }
            
            if ($a0 - (uintptr_t)0x54 < 0xb)
            {
                goto label_21d34;
                $a2_1 = &var_20;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_lsc_param_array_get;
            }
            
            if ($a0 - (uintptr_t)0xa9 < 0xc)
            {
                goto label_21d34;
                $a2_1 = &var_20;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_ccm_param_array_get;
            }
            
            if ($a0 - (uintptr_t)0xb5 < 0x31)
            {
                goto label_21d34;
                $a2_1 = &var_20;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_sharpen_param_array_get;
            }
            
            if ($a0 - (uintptr_t)0xe6 < 0x1f)
            {
                goto label_21d34;
                $a2_1 = &var_20;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_dpc_param_array_get;
            }
            
            if ($a0 - (uintptr_t)0x105 < 0x7b)
            {
                goto label_21d34;
                $a2_1 = &var_20;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_sdns_param_array_get;
            }
            
            if ($a0 - (uintptr_t)0x180 < 0x1d7)
            {
                goto label_21d34;
                $a2_1 = &var_20;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_mdns_param_array_get;
            }
            
            if ($a0 - 0x357 < 3)
            {
                goto label_21d34;
                $a2_1 = &var_20;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_clm_param_array_get;
            }
            
            if ($a0 - (uintptr_t)0x35a < 0x26)
            {
                goto label_21d34;
                $a2_1 = &var_20;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_defog_param_array_get;
            }
            
            if ($a0 - (uintptr_t)0x380 < 0x2c)
            {
                goto label_21d34;
                $a2_1 = &var_20;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_adr_param_array_get;
            }
            
            if ($(uintptr_t)a0 == 0x3ac)
            {
                goto label_21d34;
                $a2_1 = &var_20;
                $a1_2 = opmsg + 0x18;
                $a0 = 0x3ac;
                $v0_4 = tisp_hldc_param_array_get;
            }
            
            if ($a0 - (uintptr_t)0x3ad < 0x13)
            {
                goto label_21d34;
                $a2_1 = &var_20;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_af_param_array_get;
            }
            
            if ($a0 - (uintptr_t)0x3c0 < 0x26)
            {
                goto label_21d34;
                $a2_1 = &var_20;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_bcsh_param_array_get;
            }
            
            if ($a0 - (uintptr_t)0x3e6 < 0xf)
            {
                goto label_21d34;
                $a2_1 = &var_20;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_ydns_param_array_get;
            }
            
            if ($a0 - (uintptr_t)0x3f5 < 0xa)
            {
                goto label_21d34;
                $a2_1 = &var_20;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_gb_param_array_get;
            }
            
            if ($a0 - (uintptr_t)0x3ff < 0x33)
            {
                goto label_21d34;
                $a2_1 = &var_20;
                $a1_2 = opmsg + 0x18;
                $v0_4 = tisp_wdr_param_array_get;
            }
            
            if ($a0 - (uintptr_t)0x432 >= 0x15)
            {
                return 0xffffffff;
            label_22068:

            }
            
            $a2_1 = &var_20_6;
            $a1_2 = opmsg + 0x18;
            $v0_4 = tisp_rdns_param_array_get;
            goto label_21d34;
        }
        
        $a2_1 = &var_20_7;
        $a1_2 = opmsg + 0x18;
        $v0_4 = tisp_ae_param_array_get;
    label_21d34:
        $v0_4($a0, $a1_2, $a2_1);
        opmsg_1 = opmsg;
        *opmsg_1 = *arg1;
        int16_t $a1_23 = var_20_8;
        opmsg_1[1] = arg1[1];
        opmsg_1[2] = arg1[2];
        $a1_25 = $a1_23 + 0x18;
        opmsg_1[3] = arg1[3];
        opmsg_1[4] = arg1[4];
    }
    else if ($v1 == 1)
    {
        int32_t $a0_1 = arg1[1];
        void* $a1_26;
        int32_t* $a2_2;
                goto label_2203c;
        var_20 = arg1[2];
        int32_t (* $v0_51)(int32_t arg1, int32_t arg2, int32_t* arg3);
        
        if ($a0_1 && $a0_1 - (uintptr_t)1 >= 0x22)
        {
            if ($a0_1 - (uintptr_t)0x23 < 0x19)
            {
                $a2_2 = &var_20;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_awb_param_array_set;
            }
            
            if ($a0_1 - 0x3c < 2)
            {
                goto label_2203c;
                $a2_2 = &var_20;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_gamma_param_array_set;
            }
            
            if ($a0_1 - (uintptr_t)0x3e < 0x16)
            {
                goto label_2203c;
                $a2_2 = &var_20;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_gib_param_array_set;
            }
            
            if ($a0_1 - (uintptr_t)0x54 < 0xb)
            {
                goto label_2203c;
                $a2_2 = &var_20;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_lsc_param_array_set;
            }
            
            if ($a0_1 - (uintptr_t)0x5f < 0x4a)
            {
                goto label_2203c;
                $a2_2 = &var_20;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_dmsc_param_array_set;
            }
            
            if ($a0_1 - (uintptr_t)0xa9 < 0xc)
            {
                goto label_2203c;
                $a2_2 = &var_20;
                $a1_26 = &arg1[6];
                $v0_51 = Tiziano_awb_set_gain;
            }
            
            if ($a0_1 - (uintptr_t)0xb5 < 0x31)
            {
                goto label_2203c;
                $a2_2 = &var_20;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_sharpen_param_array_set;
            }
            
            if ($a0_1 - (uintptr_t)0xe6 < 0x1f)
            {
                goto label_2203c;
                $a2_2 = &var_20;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_dpc_param_array_set;
            }
            
            if ($a0_1 - (uintptr_t)0x105 < 0x7b)
            {
                goto label_2203c;
                $a2_2 = &var_20;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_sdns_param_array_set;
            }
            
            if ($a0_1 - (uintptr_t)0x180 < 0x1d7)
            {
                goto label_2203c;
                $a2_2 = &var_20;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_mdns_param_array_set;
            }
            
            if ($a0_1 - 0x357 < 3)
            {
                goto label_2203c;
                $a2_2 = &var_20;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_clm_param_array_set;
            }
            
            if ($a0_1 - (uintptr_t)0x35a < 0x26)
            {
                goto label_2203c;
                $a2_2 = &var_20;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_defog_param_array_set;
            }
            
            if ($a0_1 - (uintptr_t)0x380 < 0x2c)
            {
                goto label_2203c;
                $a2_2 = &var_20;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_adr_param_array_set;
            }
            
            if ($(uintptr_t)a0_1 == 0x3ac)
            {
                goto label_2203c;
                $a2_2 = &var_20;
                $a1_26 = &arg1[6];
                $a0_1 = 0x3ac;
                $v0_51 = tisp_hldc_param_array_set;
            }
            
            if ($a0_1 - (uintptr_t)0x3ad < 0x13)
            {
                goto label_2203c;
                $a2_2 = &var_20;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_af_param_array_set;
            }
            
            if ($a0_1 - (uintptr_t)0x3c0 < 0x26)
            {
                goto label_2203c;
                $a2_2 = &var_20;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_bcsh_param_array_set;
            }
            
            if ($a0_1 - (uintptr_t)0x3e6 < 0xf)
            {
                goto label_2203c;
                $a2_2 = &var_20;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_ydns_param_array_set;
            }
            
            if ($a0_1 - (uintptr_t)0x3f5 < 0xa)
            {
                goto label_2203c;
                $a2_2 = &var_20;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_gb_param_array_set;
            }
            
            if ($a0_1 - (uintptr_t)0x3ff < 0x33)
            {
                goto label_2203c;
                $a2_2 = &var_20;
                $a1_26 = &arg1[6];
                $v0_51 = tisp_wdr_param_array_set;
            }
            
            if ($a0_1 - (uintptr_t)0x432 >= 0x15)
                goto label_22068;
            
            $a2_2 = &var_20_9;
            $a1_26 = &arg1[6];
            $v0_51 = tisp_rdns_param_array_set;
            goto label_2203c;
        }
        
        $a2_2 = &var_20_10;
        $a1_26 = &arg1[6];
        $v0_51 = tisp_ae_param_array_set;
    label_2203c:
        $v0_51($a0_1, $a1_26, $a2_2);
        $a1_25 = 0x18;
        opmsg_1 = opmsg;
    }
    else
    {
        int32_t var_b8;
                return 0xffffffff;
        
        if ($v1 != 2)
        {
            if ($v1 != 3)
            {

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
                    int32_t $v0_102 = private_kmalloc(0x42c, 0xd0);
                    return 0;
                if ($(uintptr_t)v0_100 == 0xd)
                {
                    var_b8 = $v0_102;
                    tisp_g_ae_hist($v0_102);
                    memcpy(&opmsg_3[6], &var_b8, 0x42c);
                    netlink_send_msg(opmsg, 0x444);
                    private_kfree(var_b8);
                }
                
                if ($v0_100 == 1)
                {
                    int32_t $v0_103 = system_reg_read(0xc);
                    opmsg_1 = opmsg;
                    opmsg_1[2] = $v0_103;
                    $a1_25 = 0x18;
                }
                else if ($(uintptr_t)v0_100 == 0x10)
                {
                    var_b8 = 0;
                    tisp_g_wdr_en(&var_b8);
                    opmsg_1 = opmsg;
                label_22568:
                    opmsg_1[3] = var_b8;
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
                        goto label_22568;
                    if ($(uintptr_t)v0_100 == 0xc)
                    {
                        var_b8 = 0;
                        tisp_g_Hilightdepress(&var_b8);
                        opmsg_1 = opmsg;
                    }
                    
                    if ($v0_100 != 4)
                    {
                            return 0;
                        if ($v0_100 != 5)
                        {

                            *((int32_t*)((char*)opmsg + 0x14)) = 1; // Fixed void pointer dereference
                        }
                        
                        tisp_g_wb_attr(&var_b8_3);
                        memcpy(&opmsg_3[6], &var_b8_4, 0x1c);
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
                    int32_t $v1_7 = 0;
                    $v0_105 = tisp_day_or_night_g_ctrl();
                    
                    if (!$v0_105)
                        opmsg_1 = opmsg;
                    else
                    {
                        $v1_7 = 1;
                        
                        if ($v0_105 == 1)
                            opmsg_1 = opmsg;
                        else
                        {

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
            int32_t $a2_6 = arg1[4];
            int32_t var_4d8;
            int32_t var_b4;
            int32_t var_b0;
            int32_t var_ac;
            int32_t var_a8;
            *opmsg_2 = $v1;
            opmsg_2[1] = arg1[1];
            opmsg_2[2] = arg1[2];
            opmsg_2[3] = arg1[3];
            opmsg_2[5] = 0;
            opmsg_2[4] = $a2_6;
            
            if (!$v0_89)
            {
                memcpy(&var_b8, &arg1[6], 0x98);
                
                for (int32_t i = 0; (uintptr_t)i < 0x88; i += 1)
                    *(&var_4d8 + i) = *(&var_a8 + i);
                
                tisp_ae_manual_set(var_b8, var_b4, var_b0, var_ac);
                $a1_25 = 0x18;
            }
            else if ($v0_89 == 1)
            {
                system_reg_write(0xc, arg1[2]);
                $a1_25 = 0x18;
            }
            else if ($(uintptr_t)v0_89 == 0xb)
            {
                memcpy(&var_b8, &arg1[6], 0x98);
                
                for (int32_t i_1 = 0; (uintptr_t)i_1 < 0x88; i_1 += 1)
                    *(&var_4d8 + i_1) = *(&var_a8 + i_1);
                
                tisp_ae_min_max_set(var_b8, var_b4, var_b0, var_ac);
                $a1_25 = 0x18;
            }
            else if ($(uintptr_t)v0_89 == 0xd)
            {
                uint32_t $v0_91 = private_kmalloc(0x42c, 0xd0);
                memcpy($v0_91, &arg1[6], 0x42c);
                
                for (int32_t i_2 = 0; (uintptr_t)i_2 < 0x41c; i_2 += 1)
                    *(&var_4d8 + i_2) = *($v0_91 + 0x10 + i_2);
                
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
            else if ($(uintptr_t)v0_89 == 0xc)
            {
                tisp_s_Hilightdepress(arg1[2]);
                $a1_25 = 0x18;
            }
            else if ($(uintptr_t)v0_89 == 0x10)
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
                int32_t $a0_11 = var_b8;
                int32_t var_a0;
                int32_t var_4d0_1 = var_a0;
                int32_t var_a4;
                memcpy(&var_b8, &arg1[6], 0x1c);
                var_4d8 = var_a8;
                tisp_s_wb_attr($a0_11, var_b4, var_b0, var_ac, var_4d8, var_a4);
                $a1_25 = 0x18;
            }
            else if ($(uintptr_t)v0_89 != 0xa)
            {

                *((int32_t*)((char*)opmsg + 0x14)) = 1; // Fixed void pointer dereference
                $a1_25 = 0x18;
            }
            else
            {
                int32_t $a0_13 = var_b8;
                memcpy(&var_b8, &arg1[6], 0x14);
                var_4d8 = var_a8;
                tisp_s_fcrop_control($a0_13, var_b4, var_b0, var_ac, var_4d8);
                $a1_25 = 0x18;
            }
            opmsg_1 = opmsg;
        }
    }
    
    netlink_send_msg(opmsg_1, $a1_25);
    return 0;
}

