#include "include/main.h"


  int32_t tisp_code_tuning_ioctl(int32_t arg1, int32_t arg2, int32_t arg3)

{
    int32_t $a2;
                int32_t $s0_1 = arg3;
                int32_t $a0_2;
                uint32_t tisp_par_ioctl_3;
                uint32_t tisp_par_ioctl_4;
                uint32_t tisp_par_ioctl_13;
                int32_t $a2_2;
                int32_t $s1_2;
                uint32_t tisp_par_ioctl_9;
                        int32_t $s2_2 = (arg3 + 0x500c) | arg3;
                        uint32_t tisp_par_ioctl_10 = tisp_par_ioctl;
                            return 0xfffffff2;
    char const* const $a1;
    
    if (arg2 >> (uintptr_t)8 == 0x74)
    {
        if ((arg2 & 0xff) < 0x33)
        {
            if (arg2 - (uintptr_t)0x20007400 < 0xa)
            {
                int32_t (* $v0_25)(void* arg1);
                char const* const $a1_11;
                void* entry_$gp;
                
                switch (arg2)
                {
                    case 0x20007400:
                    {
                        
                        if ($s2_2 & *(entry_$gp + 0x18))
                        {
                            isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                        }
                        
                        __might_sleep("VIC_CTRL : %08x\\n", 0xc9, 0);
                        arg3 = __copy_user(tisp_par_ioctl_10, $s0_1, 0x500c);
                        int32_t* tisp_par_ioctl_1 = tisp_par_ioctl;
                        
                        if (arg3)
                        {
                            return 0xfffffff2;
                            isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                        }
                        
                        int32_t $v0_8 = *tisp_par_ioctl_1;
                        int32_t $v0_12;
                        
                        if ($(uintptr_t)v0_8 >= 0x19)
                            $v0_12 = *(entry_gp_3 + 0x18);
                        else
                            switch ($v0_8)
                            {
                                case 0:
                                {
                                    arg3 = tisp_top_param_array_get(&tisp_par_ioctl_1[3], 
                                        &tisp_par_ioctl_1[1]);
                                    $v0_12 = *(entry_$gp + 0x18);
                                    break;
                                }
                                case 1:
                                {
                                    arg3 = tisp_blc_get_par_cfg(&tisp_par_ioctl_1[3], 
                                        &tisp_par_ioctl_1[1]);
                                    $v0_12 = *(entry_$gp + 0x18);
                                    break;
                                }
                                case 2:
                                {
                                    arg3 = tisp_lsc_get_par_cfg(&tisp_par_ioctl_1[3], 
                                        &tisp_par_ioctl_1[1]);
                                    $v0_12 = *(entry_$gp + 0x18);
                                    break;
                                }
                                case 3:
                                {
                                    arg3 = tisp_wdr_get_par_cfg(&tisp_par_ioctl_1[3], 
                                        &tisp_par_ioctl_1[1]);
                                    $v0_12 = *(entry_$gp + 0x18);
                                    break;
                                }
                                case 4:
                                {
                                    arg3 = tisp_dpc_get_par_cfg(&tisp_par_ioctl_1[3], 
                                        &tisp_par_ioctl_1[1]);
                                    $v0_12 = *(entry_$gp + 0x18);
                                    break;
                                }
                                case 5:
                                {
                                    arg3 = tx_isp_subdev_pipo(&tisp_par_ioctl_1[3], 
                                        &tisp_par_ioctl_1[1]);
                                    $v0_12 = *(entry_$gp + 0x18);
                                    break;
                                }
                                case 6:
                                {
                                    arg3 = tisp_rdns_get_par_cfg(&tisp_par_ioctl_1[3], 
                                        &tisp_par_ioctl_1[1]);
                                    $v0_12 = *(entry_$gp + 0x18);
                                    break;
                                }
                                case 7:
                                {
                                    arg3 = tisp_adr_get_par_cfg(&tisp_par_ioctl_1[3], 
                                        &tisp_par_ioctl_1[1]);
                                    $v0_12 = *(entry_$gp + 0x18);
                                    break;
                                }
                                case 8:
                                {
                                    arg3 = tx_isp_vin_activate_subdev(&tisp_par_ioctl_1[3], 
                                        &tisp_par_ioctl_1[1]);
                                    $v0_12 = *(entry_$gp + 0x18);
                                    break;
                                }
                                case 9:
                                {
                                    arg3 = tisp_ccm_get_par_cfg(&tisp_par_ioctl_1[3], 
                                        &tisp_par_ioctl_1[1]);
                                    $v0_12 = *(entry_$gp + 0x18);
                                    break;
                                }
                                case 0xa:
                                {
                                    arg3 = tisp_gamma_get_par_cfg(&tisp_par_ioctl_1[3], 
                                        &tisp_par_ioctl_1[1]);
                                    $v0_12 = *(entry_$gp + 0x18);
                                    break;
                                }
                                case 0xb:
                                {
                                    arg3 = tisp_defog_get_par_cfg(&tisp_par_ioctl_1[3], 
                                        &tisp_par_ioctl_1[1]);
                                    $v0_12 = *(entry_$gp + 0x18);
                                    break;
                                }
                                case 0xc:
                                {
                                    arg3 = tisp_mdns_get_par_cfg(&tisp_par_ioctl_1[3], 
                                        &tisp_par_ioctl_1[1]);
                                    $v0_12 = *(entry_$gp + 0x18);
                                    break;
                                }
                                case 0xd:
                                {
                                    arg3 = tisp_ydns_get_par_cfg(&tisp_par_ioctl_1[3], 
                                        &tisp_par_ioctl_1[1]);
                                    $v0_12 = *(entry_$gp + 0x18);
                                    break;
                                }
                                case 0xe:
                                {
                                    arg3 = tisp_bcsh_get_par_cfg(&tisp_par_ioctl_1[3], 
                                        &tisp_par_ioctl_1[1]);
                                    $v0_12 = *(entry_$gp + 0x18);
                                    break;
                                }
                                case 0xf:
                                {
                                    arg3 = tisp_clm_get_par_cfg(&tisp_par_ioctl_1[3], 
                                        &tisp_par_ioctl_1[1]);
                                    $v0_12 = *(entry_$gp + 0x18);
                                    break;
                                }
                                case 0x10:
                                {
                                    arg3 = tisp_ysp_get_par_cfg(&tisp_par_ioctl_1[3], 
                                        &tisp_par_ioctl_1[1]);
                                    $v0_12 = *(entry_$gp + 0x18);
                                    break;
                                }
                                case 0x11:
                                {
                                    arg3 = tisp_sdns_get_par_cfg(&tisp_par_ioctl_1[3], 
                                        &tisp_par_ioctl_1[1]);
                                    $v0_12 = *(entry_$gp + 0x18);
                                    break;
                                }
                                case 0x12:
                                {
                                    arg3 = tisp_af_get_par_cfg(&tisp_par_ioctl_1[3], 
                                        &tisp_par_ioctl_1[1]);
                                    $v0_12 = *(entry_$gp + 0x18);
                                    break;
                                }
                                case 0x13:
                                {
                                    arg3 = tisp_hldc_get_par_cfg(&tisp_par_ioctl_1[3], 
                                        &tisp_par_ioctl_1[1]);
                                    $v0_12 = *(entry_$gp + 0x18);
                                    break;
                                }
                                case 0x14:
                                {
                                    arg3 = tisp_ae_get_par_cfg(&tisp_par_ioctl_1[3], 
                                        &tisp_par_ioctl_1[1]);
                                    $v0_12 = *(entry_$gp + 0x18);
                                    break;
                                }
                                case 0x15:
                                {
                                    arg3 = tisp_awb_get_par_cfg(&tisp_par_ioctl_1[3], 
                                        &tisp_par_ioctl_1[1]);
                                    $v0_12 = *(entry_$gp + 0x18);
                                    break;
                                }
                                case 0x16:
                                {
                                    $v0_12 = *(entry_$gp + 0x18);
                                    break;
                                }
                                case 0x17:
                                {
                                    arg3 = tisp_reg_map_get(tisp_par_ioctl_1[2], tisp_par_ioctl_1, 
                                        &tisp_par_ioctl_1[1]);
                                    $v0_12 = *(entry_$gp + 0x18);
                                    break;
                                }
                                case 0x18:
                                {
                                    arg3 = tisp_dn_mode_get(tisp_par_ioctl_1, &tisp_par_ioctl_1[1]);
                                    $v0_12 = *(entry_$gp + 0x18);
                                    break;
                                }
                            }
                        
                        tisp_par_ioctl_9 = tisp_par_ioctl;
                        
                        if ($s2_2 & $v0_12)
                        {
                            return 0xfffffff2;
                            isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                        }
                        
                        __might_sleep("VIC_CTRL : %08x\\n", 0xc9, 0);
                        $a0_2 = $s0_1;
                        goto label_246b8;
                    }
                    case 0x20007401:
                    {
                        uint32_t tisp_par_ioctl_6 = tisp_par_ioctl;
                            return 0xfffffff2;
                        
                        if (((arg3 + 0x500c) | arg3) & *(entry_$gp + 0x18))
                        {
                            isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                        }
                        
                        __might_sleep("VIC_CTRL : %08x\\n", 0xc9, 0);
                        arg3 = __copy_user(tisp_par_ioctl_6, $s0_1, 0x500c);
                        int32_t* tisp_par_ioctl_2 = tisp_par_ioctl;
                        
                        if (arg3)
                        {
                            return 0xfffffff2;
                            isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                        }
                        
                        int32_t $a1_8 = *tisp_par_ioctl_2;
                        
                        if ($a1_8 - (uintptr_t)1 >= 0x18)
                        {
                            int32_t var_28_1 = $a1_8;
                            return 0;
                        label_245e0:
                            isp_printf(); // Fixed: macro with no parameters, removed 2 arguments\n", 
                                "tisp_set_par_process");
                        }
                        
                        int32_t (* $v0_19)(void* arg1);
                        
                        switch ($a1_8)
                        {
                            case 1:
                            {
                                tisp_par_ioctl_2 = &tisp_par_ioctl_2[3];
                                $v0_19 = tisp_blc_set_par_cfg;
                                break;
                            }
                            case 2:
                            {
                                return 0;
                                tisp_lsc_set_par_cfg(tisp_par_ioctl_2[2], &tisp_par_ioctl_2[3]);
                                break;
                            }
                            case 3:
                            {
                                tisp_par_ioctl_2 = &tisp_par_ioctl_2[3];
                                $v0_19 = tisp_wdr_set_par_cfg;
                                break;
                            }
                            case 4:
                            {
                                tisp_par_ioctl_2 = &tisp_par_ioctl_2[3];
                                $v0_19 = tisp_dpc_set_par_cfg;
                                break;
                            }
                            case 5:
                            {
                                tisp_par_ioctl_2 = &tisp_par_ioctl_2[3];
                                $v0_19 = tisp_gib_set_par_cfg;
                                break;
                            }
                            case 6:
                            {
                                tisp_par_ioctl_2 = &tisp_par_ioctl_2[3];
                                $v0_19 = tisp_rdns_set_par_cfg;
                                break;
                            }
                            case 7:
                            {
                                tisp_par_ioctl_2 = &tisp_par_ioctl_2[3];
                                $v0_19 = tisp_adr_set_par_cfg;
                                break;
                            }
                            case 8:
                            {
                                tisp_par_ioctl_2 = &tisp_par_ioctl_2[3];
                                $v0_19 = tisp_dmsc_set_par_cfg;
                                break;
                            }
                            case 9:
                            {
                                tisp_par_ioctl_2 = &tisp_par_ioctl_2[3];
                                $v0_19 = tisp_ccm_set_par_cfg;
                                break;
                            }
                            case 0xa:
                            {
                                tisp_par_ioctl_2 = &tisp_par_ioctl_2[3];
                                $v0_19 = tisp_gamma_set_par_cfg;
                                break;
                            }
                            case 0xb:
                            {
                                tisp_par_ioctl_2 = &tisp_par_ioctl_2[3];
                                $v0_19 = tisp_defog_set_par_cfg;
                                break;
                            }
                            case 0xc:
                            {
                                tisp_par_ioctl_2 = &tisp_par_ioctl_2[3];
                                $v0_19 = tisp_mdns_set_par_cfg;
                                break;
                            }
                            case 0xd:
                            {
                                tisp_par_ioctl_2 = &tisp_par_ioctl_2[3];
                                $v0_19 = tisp_ydns_set_par_cfg;
                                break;
                            }
                            case 0xe:
                            {
                                tisp_par_ioctl_2 = &tisp_par_ioctl_2[3];
                                $v0_19 = tisp_bcsh_set_par_cfg;
                                break;
                            }
                            case 0xf:
                            {
                                tisp_par_ioctl_2 = &tisp_par_ioctl_2[3];
                                $v0_19 = tisp_clm_set_par_cfg;
                                break;
                            }
                            case 0x10:
                            {
                                tisp_par_ioctl_2 = &tisp_par_ioctl_2[3];
                                $v0_19 = tisp_ysp_set_par_cfg;
                                break;
                            }
                            case 0x11:
                            {
                                tisp_par_ioctl_2 = &tisp_par_ioctl_2[3];
                                $v0_19 = tisp_sdns_set_par_cfg;
                                break;
                            }
                            case 0x12:
                            {
                                tisp_par_ioctl_2 = &tisp_par_ioctl_2[3];
                                $v0_19 = tisp_af_set_par_cfg;
                                break;
                            }
                            case 0x13:
                            {
                                tisp_par_ioctl_2 = &tisp_par_ioctl_2[3];
                                $v0_19 = tisp_hldc_set_par_cfg;
                                break;
                            }
                            case 0x14:
                            {
                                tisp_par_ioctl_2 = &tisp_par_ioctl_2[3];
                                $v0_19 = tisp_ae_set_par_cfg;
                                break;
                            }
                            case 0x15:
                            {
                                tisp_par_ioctl_2 = &tisp_par_ioctl_2[3];
                                $v0_19 = tisp_awb_set_par_cfg;
                                break;
                            }
                            case 0x16:
                            {
                                goto label_245e0;
                            }
                            case 0x17:
                            {
                                $v0_19 = tisp_reg_map_set;
                                break;
                            }
                            case 0x18:
                            {
                                $v0_19 = tisp_dn_mode_set;
                                break;
                            }
                        }
                        
                        $v0_19(tisp_par_ioctl_2);
                        break;
                    }
                    case 0x20007403:
                    {
                        uint32_t tisp_par_ioctl_5 = tisp_par_ioctl;
                            return 0xfffffff2;
                        tisp_par_ioctl_4 = tisp_par_ioctl;
                        $v0_25 = tisp_get_ae_info;
                    label_24740:
                        arg3 = $v0_25(tisp_par_ioctl_4);
                        
                        if ((($s0_1 + 0x500c) | $s0_1) & *(entry_$gp + 0x18))
                        {
                            isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                        }
                        
                        __might_sleep("VIC_CTRL : %08x\\n", 0xc9, 0);
                        $a0_2 = $s0_1;
                        tisp_par_ioctl_13 = tisp_par_ioctl_5;
                    label_24790:
                        arg3 = __copy_user($a0_2, tisp_par_ioctl_13, 0x500c);
                        
                        if (!(uintptr_t)arg3)
                            return 0;
                        
                        isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                        return 0xfffffff2;
                        break;
                    }
                    case 0x20007404:
                    {
                        uint32_t tisp_par_ioctl_7 = tisp_par_ioctl;
                            return 0xfffffff2;
                        
                        if (((arg3 + 0x500c) | arg3) & *(entry_$gp + 0x18))
                        {
                            isp_printf(2, "%s[%d] VIC do not support this format %d\n", 
                                "tisp_code_tuning_ioctl");
                        }
                        
                        __might_sleep("VIC_CTRL : %08x\\n", 0xc9, 0);
                        
                        if (__copy_user(tisp_par_ioctl_7, $s0_1, 0x500c))
                        {
                            return 0xfffffff2;
                            isp_printf(2, "%s[%d] VIC do not support this format %d\n", 
                                "tisp_code_tuning_ioctl");
                        }
                        
                        tisp_set_ae_info(tisp_par_ioctl);
                        break;
                    }
                    case 0x20007406:
                    {
                        goto label_24740;
                        tisp_par_ioctl_4 = tisp_par_ioctl;
                        $v0_25 = tisp_get_awb_info;
                    }
                    case 0x20007407:
                    {
                        uint32_t tisp_par_ioctl_8 = tisp_par_ioctl;
                            return 0xfffffff2;
                        
                        if (((arg3 + 0x500c) | arg3) & *(entry_$gp + 0x18))
                        {
                            isp_printf(2, "%s[%d] VIC do not support this format %d\n", 
                                "tisp_code_tuning_ioctl");
                        }
                        
                        __might_sleep("VIC_CTRL : %08x\\n", 0xc9, 0);
                        
                        if (__copy_user(tisp_par_ioctl_8, $s0_1, 0x500c))
                        {
                            return 0xfffffff2;
                            isp_printf(2, "%s[%d] VIC do not support this format %d\n", 
                                "tisp_code_tuning_ioctl");
                        }
                        
                        tisp_set_awb_info(tisp_par_ioctl);
                        break;
                    }
                    case 0x20007408:
                    {
                        uint32_t tisp_par_ioctl_11 = tisp_par_ioctl;
                            return 0xfffffff2;
                        $s1_2 = (arg3 + 0x500c) | arg3;
                        
                        if ($s1_2 & *(entry_$gp + 0x18))
                        {
                            isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                        }
                        
                        __might_sleep("VIC_CTRL : %08x\\n", 0xc9, 0);
                        arg3 = __copy_user(tisp_par_ioctl_11, $s0_1, 0x500c);
                        tisp_par_ioctl_3 = tisp_par_ioctl;
                        
                        if (arg3)
                        {
                            return 0xfffffff2;
                            isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                        }
                        
                        *((int32_t*)((char*)tisp_par_ioctl_3 + 4)) = 0xb; // Fixed void pointer dereference
                        $a2_2 = 0xb;
                        $a1_11 = "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\\n";
                    label_24688:
                        arg3 = memcpy(tisp_par_ioctl_3 + 0xc, $a1_11, $a2_2);
                        tisp_par_ioctl_9 = tisp_par_ioctl;
                        
                        if ($s1_2 & *(entry_gp_4 + 0x18))
                        {
                            return 0xfffffff2;
                            isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                        }
                        
                        __might_sleep("VIC_CTRL : %08x\\n", 0xc9, 0);
                        $a0_2 = $s0_1;
                    label_246b8:
                        tisp_par_ioctl_13 = tisp_par_ioctl_9;
                        goto label_24790;
                    }
                    case 0x20007409:
                    {
                        uint32_t tisp_par_ioctl_12 = tisp_par_ioctl;
                            return 0xfffffff2;
                        $s1_2 = (arg3 + 0x500c) | arg3;
                        
                        if ($s1_2 & *(entry_$gp + 0x18))
                        {
                            isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                        }
                        
                        __might_sleep("VIC_CTRL : %08x\\n", 0xc9, 0);
                        arg3 = __copy_user(tisp_par_ioctl_12, $s0_1, 0x500c);
                        tisp_par_ioctl_3 = tisp_par_ioctl;
                        
                        if (arg3)
                        {
                            return 0xfffffff2;
                            isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                        }
                        
                        *((int32_t*)((char*)tisp_par_ioctl_3 + 4)) = 0xf; // Fixed void pointer dereference
                        $a2_2 = 0xf;
                        $a1_11 = "%s[%d] VIC failed to config DVP mode!(10bits-sensor)\\n";
                        goto label_24688;
                    }
                }
            }
            
            return 0;
        }
        
        $a2 = arg2;
        $a1 = "sensor type is BT656!\\n";
    }
    else
    {
        $a2 = arg2;
        $a1 = "not support the gpio mode!\n";
    }
    
    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
    return 0xffffffea;
}

