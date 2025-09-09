#include "include/main.h"


  int32_t tisp_init(int32_t* arg1, int32_t arg2)

{
    uint32_t $v0 = private_vmalloc(0x137f0);
    uint32_t $v0_1 = private_vmalloc(0x137f0);
    int32_t $v0_3;
    uint32_t tparams_cust_1;
    uint32_t isp_memopt_1 = isp_memopt;
        uint32_t tparams_day_1 = tparams_day;
        uint32_t tparams_night_1 = tparams_night;
    memset(&tispinfo, 0, 0x74);
    memset(&sensor_info, 0, 0x60);
    memset(&ds0_attr, 0, 0x34);
    memset(&ds1_attr, 0, 0x34);
    memset(&ds2_attr, 0, 0x34);
    memcpy(&sensor_info, arg1, 0x60);
    tparams_day = $v0;
    memset($v0, 0, 0x137f0);
    tparams_night = $v0_1;
    memset($v0_1, 0, 0x137f0);
    
    if (!strlen(arg2))
        snprintf(arg2, 0x40, "snapraw", 0xb2e24);
    
    $v0_3 = tiziano_load_parameters(arg2);
    
    if (isp_memopt_1 == 1)
    {
        *((int32_t*)((char*)tparams_day_1 + 0xbb50)) = 0; // Fixed void pointer dereference
        *((int32_t*)((char*)tparams_day_1 + 0xbb58)) = isp_memopt_1; // Fixed void pointer dereference
        *((int32_t*)((char*)tparams_day_1 + 0xbb68)) = 0; // Fixed void pointer dereference
        *((int32_t*)((char*)tparams_day_1 + 0xbb60)) = 0; // Fixed void pointer dereference
        *((int32_t*)((char*)tparams_night_1 + 0xbb50)) = 0; // Fixed void pointer dereference
        *((int32_t*)((char*)tparams_night_1 + 0xbb58)) = isp_memopt_1; // Fixed void pointer dereference
        *((int32_t*)((char*)tparams_night_1 + 0xbb68)) = 0; // Fixed void pointer dereference
        *((int32_t*)((char*)tparams_night_1 + 0xbb60)) = 0; // Fixed void pointer dereference
        tparams_cust_1 = tparams_cust;
        
        if (tparams_cust_1)
        {
            *((int32_t*)((char*)tparams_cust_1 + 0xbb50)) = 0; // Fixed void pointer dereference
            *((int32_t*)((char*)tparams_cust_1 + 0xbb58)) = isp_memopt_1; // Fixed void pointer dereference
            *((int32_t*)((char*)tparams_cust_1 + 0xbb68)) = 0; // Fixed void pointer dereference
            *((int32_t*)((char*)tparams_cust_1 + 0xbb60)) = 0; // Fixed void pointer dereference
        }
        
        int32_t* $a1_1 = tparams_day_1 + 0xd838;
        int32_t* $a0_6 = tparams_night_1 + 0xd838;
        isp_memopt_1 = tparams_cust_1 + 0xd85c;
        int32_t* $v1_2 = tparams_cust_1 + 0xd838;
        
        do
        {
            *$a1_1 = 0;
            *$a0_6 = 0;
            
            if (tparams_cust_1)
                *$v1_2 = 0;
            
            $v1_2 = &$v1_2[1];
            $a1_1 = &$a1_1[1];
            $a0_6 = &$a0_6[1];
        } while ($v1_2 != isp_memopt_1);
    }
    
    int32_t $v0_4;
    
    if (!$v0_3)
    {
        memcpy(0x94b20, tparams_day, 0x137f0, isp_memopt_1);
        $v0_4 = *arg1;
    }
    else
    {
        isp_printf(2, 
            "width is %d, height is %d, imagesize is %d\n, snap num is %d, buf size is %d", 
            tparams_cust_1);
        $v0_4 = *arg1;
    }
    
    system_reg_write(4, $v0_4 << 0x10 | arg1[1]);
    uint32_t $v0_6 = *arg1;
    tispinfo = $v0_6;
    uint32_t $v0_7 = arg1[1];
    data_b2f34_1 = $v0_7;
    int32_t $v0_8 = arg1[2];
    
    if ($(uintptr_t)v0_8 >= 0x15)
        isp_printf(); // Fixed: macro with no parameters, removed 2 arguments!\n", "tisp_init");
    else
        switch ($v0_8)
        {
            case 0:
            {
                system_reg_write(8, 0);
                deir_en = 0;
                break;
            }
            case 1:
            {
                system_reg_write(8, 1);
                deir_en = 0;
                break;
            }
            case 2:
            {
                system_reg_write(8, 2);
                deir_en = 0;
                break;
            }
            case 3:
            {
                system_reg_write(8, 3);
                deir_en = 0;
                break;
            }
            case 4:
            {
                system_reg_write(8, 8);
                deir_en = 1;
                break;
            }
            case 5:
            {
                system_reg_write(8, 9);
                deir_en = 1;
                break;
            }
            case 6:
            {
                system_reg_write(8, 0xa);
                deir_en = 1;
                break;
            }
            case 7:
            {
                system_reg_write(8, 0xb);
                deir_en = 1;
                break;
            }
            case 8:
            {
                system_reg_write(8, 0xc);
                deir_en = 1;
                break;
            }
            case 9:
            {
                system_reg_write(8, 0xd);
                deir_en = 1;
                break;
            }
            case 0xa:
            {
                system_reg_write(8, 0xe);
                deir_en = 1;
                break;
            }
            case 0xb:
            {
                system_reg_write(8, 0xf);
                deir_en = 1;
                break;
            }
            case 0xc:
            {
                system_reg_write(8, 0x10);
                deir_en = 1;
                break;
            }
            case 0xd:
            {
                system_reg_write(8, 0x11);
                deir_en = 1;
                break;
            }
            case 0xe:
            {
                system_reg_write(8, 0x12);
                deir_en = 1;
                break;
            }
            case 0xf:
            {
                system_reg_write(8, 0x13);
                deir_en = 1;
                break;
            }
            case 0x10:
            {
                system_reg_write(8, 0x14);
                deir_en = 1;
                break;
            }
            case 0x11:
            {
                system_reg_write(8, 0x15);
                deir_en = 1;
                break;
            }
            case 0x12:
            {
                system_reg_write(8, 0x16);
                deir_en = 1;
                break;
            }
            case 0x13:
            {
                system_reg_write(8, 0x17);
                deir_en = 1;
                break;
            }
            case 0x14:
            {
                deir_en = 1;
                break;
            }
        }
    
    int32_t $a1_7 = 0x3f00;
    
    if (deir_en == 1)
        $a1_7 = 0x10003f00;
    
    system_reg_write(0x1c, $a1_7);
    sensor_init(&sensor_ctrl);
    tisp_set_csc_version(0);
    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
    int32_t $s3 = 0x8077efff;
    
    for (int32_t i = 0; (uintptr_t)i != 0x20; )
    {
        int32_t $s3_1 = ~(1 << (i & 0x1f)) & $s3;
        int32_t $v1_12 = *((i << 2) + 0x94b20) << (i & 0x1f);
        i += 1;
        $s3 = $v1_12 + $s3_1;
    }
    
    int32_t var_70_7 = $s3;
    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
    int32_t $v0_12;
    int32_t $s3_2;
    
    if (data_b2e74_1 != 1)
    {
        $s3_2 = $s3 & 0xb577fffd;
        $v0_12 = 0x34000009;
    }
    else
    {
        $s3_2 = $s3 & 0xa1ffdf76;
        $v0_12 = 0x880002;
    }
    
    int32_t $s3_3 = $s3_2 | $v0_12;
    system_reg_write(0xc, $s3_3);
    int32_t var_70_1_2 = data_b2e74_2;
    int32_t var_6c_4 = $s3_3;
    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
    system_reg_write(0x30, 0xffffffff);
    int32_t $a1_9 = 0x33f;
    
    if (data_b2e74_3 != 1)
        $a1_9 = 0x133;
    
    system_reg_write(0x10, $a1_9);
    int32_t $v0_14 = private_kmalloc(0x6000, 0xd0);
    
    if ($v0_14)
    {
        char* var_30_1 = (char*)(&data_b0000); // Fixed void pointer assignment
        int32_t $v0_15 = private_kmalloc(0x6000, 0xd0);
            int32_t $v0_16 = private_kmalloc(0x4000, 0xd0);
                int32_t $v0_17 = private_kmalloc(0x4000, 0xd0);
                    int32_t $v0_18 = private_kmalloc(0x4000, 0xd0);
                        uint32_t var_5c_1 = data_b2e62;
                        uint32_t var_60_1 = data_b2e64;
                        uint32_t var_64_1 = data_b2e56;
                        uint32_t var_68_1 = data_b2e54;
                        uint32_t var_6c_1 = data_b2e58;
                        uint32_t $v0_24 = data_b2e48;
                        uint32_t var_70_2 = $v0_24;
                        int32_t $v0_25 = private_kmalloc(0x4000, 0xd0);
                            int32_t $v0_26 = private_kmalloc(0x8000, 0xd0);
                                uint32_t var_70_3 = *(arg1 + 0x4a);
                                int32_t tispinfo_1 = tispinfo;
                                int32_t var_4c;
                                int32_t var_6c_2 = var_4c;
                                int32_t tispinfo_2 = tispinfo_1;
                                uint32_t $a0_8;
                                char $a1_48;
                                char $a2_6;
                                int32_t $a1_53;
        system_reg_write(0xa02c, $v0_14 - 0x80000000);
        system_reg_write(0xa030, $v0_14 - 0x7ffff000);
        system_reg_write(0xa034, $v0_14 - 0x7fffe000);
        system_reg_write(0xa038, $v0_14 - 0x7fffd000);
        system_reg_write(0xa03c, $v0_14 - 0x7fffc000);
        system_reg_write(0xa040, $v0_14 - 0x7fffb800);
        system_reg_write(0xa044, $v0_14 - 0x7fffb000);
        system_reg_write(0xa048, $v0_14 - 0x7fffa800);
        system_reg_write(0xa04c, 0x33);
        data_b2f3c = $v0_14;
        data_b2f48 = $v0_14 + 0x4000;
        data_b2f38 = 4;
        data_b2f40 = $v0_14 - 0x80000000;
        data_b2f44 = 4;
        data_b2f4c = $v0_14 - 0x7fffc000;
        
        if ($v0_15)
        {
            system_reg_write(0xa82c, $v0_15 - 0x80000000);
            system_reg_write(0xa830, $v0_15 - 0x7ffff000);
            system_reg_write(0xa834, $v0_15 - 0x7fffe000);
            system_reg_write(0xa838, $v0_15 - 0x7fffd000);
            system_reg_write(0xa83c, $v0_15 - 0x7fffc000);
            system_reg_write(0xa840, $v0_15 - 0x7fffb800);
            system_reg_write(0xa844, $v0_15 - 0x7fffb000);
            system_reg_write(0xa848, $v0_15 - 0x7fffa800);
            system_reg_write(0xa84c, 0x33);
            data_b2f54 = $v0_15;
            data_b2f60 = $v0_15 + 0x4000;
            data_b2f50 = 4;
            data_b2f58 = $v0_15 - 0x80000000;
            data_b2f5c = 4;
            data_b2f64 = $v0_15 - 0x7fffc000;
            
            if ($v0_16)
            {
                system_reg_write(0xb03c, $v0_16 - 0x80000000);
                system_reg_write(0xb040, $v0_16 - 0x7ffff000);
                system_reg_write(0xb044, $v0_16 - 0x7fffe000);
                system_reg_write(0xb048, $v0_16 - 0x7fffd000);
                system_reg_write(0xb04c, 3);
                data_b2f6c = $v0_16;
                data_b2f68 = 4;
                data_b2f70 = $v0_16 - 0x80000000;
                
                if ($v0_17)
                {
                    system_reg_write(0x4494, $v0_17 - 0x80000000);
                    system_reg_write(0x4498, $v0_17 - 0x7ffff000);
                    system_reg_write(0x449c, $v0_17 - 0x7fffe000);
                    system_reg_write(0x44a0, $v0_17 - 0x7fffd000);
                    system_reg_write(0x4490, 3);
                    data_b2f78 = $v0_17;
                    data_b2f74 = 4;
                    data_b2f7c = $v0_17 - 0x80000000;
                    
                    if ($v0_18)
                    {
                        system_reg_write(0x5b84, $v0_18 - 0x80000000);
                        system_reg_write(0x5b88, $v0_18 - 0x7ffff000);
                        system_reg_write(0x5b8c, $v0_18 - 0x7fffe000);
                        system_reg_write(0x5b90, $v0_18 - 0x7fffd000);
                        system_reg_write(0x5b80, 3);
                        data_b2f84 = $v0_18;
                        data_b2f88 = $v0_18 - 0x80000000;
                        data_b2f80 = 4;
                        isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                        
                        if ($v0_25)
                        {
                            system_reg_write(0xb8a8, $v0_25 - 0x80000000);
                            system_reg_write(0xb8ac, $v0_25 - 0x7ffff000);
                            system_reg_write(0xb8b0, $v0_25 - 0x7fffe000);
                            system_reg_write(0xb8b4, $v0_25 - 0x7fffd000);
                            system_reg_write(0xb8b8, 3);
                            data_b2f90 = $v0_25;
                            data_b2f8c = 4;
                            data_b2f94 = $v0_25 - 0x80000000;
                            
                            if ($v0_26)
                            {
                                system_reg_write(0x2010, $v0_26 - 0x80000000);
                                system_reg_write(0x2014, $v0_26 - 0x7fffe000);
                                system_reg_write(0x2018, $v0_26 - 0x7fffc000);
                                system_reg_write(0x201c, $v0_26 - 0x7fffa000);
                                system_reg_write(0x2020, 0x400);
                                system_reg_write(0x2024, 3);
                                data_b2f98 = 4;
                                data_b2fa0 = $v0_26 - 0x80000000;
                                data_b2f9c = $v0_26;
                                tiziano_ae_init(data_b2f34, tispinfo_1, arg1[0xc]);
                                $a0_8 = tiziano_awb_init(data_b2f34, tispinfo);
                                tiziano_gamma_init($a0_8, $a1_48, $a2_6);
                                tiziano_gib_init();
                                tiziano_lsc_init();
                                tiziano_ccm_init();
                                tiziano_dmsc_init();
                                tiziano_sharpen_init();
                                tiziano_sdns_init();
                                tiziano_mdns_init($v0_6, $v0_7);
                                tiziano_clm_init();
                                tiziano_dpc_init();
                                tiziano_hldc_init();
                                tiziano_defog_init($v0_6, $v0_7);
                                tiziano_adr_init($v0_6, $v0_7);
                                tiziano_af_init($v0_7, $v0_6);
                                tiziano_bcsh_init();
                                tiziano_ydns_init();
                                tiziano_rdns_init();
                                
                                if (data_b2e74 != 1)
                                    $a1_53 = arg1[2];
                                else
                                {
                                    tiziano_wdr_init($v0_6, $v0_7);
                                    tisp_gb_init();
                                    tisp_dpc_wdr_en(1);
                                    tisp_lsc_wdr_en(1);
                                    tisp_gamma_wdr_en(1);
                                    tisp_sharpen_wdr_en(1);
                                    tisp_ccm_wdr_en(1);
                                    tisp_bcsh_wdr_en(1);
                                    tisp_rdns_wdr_en(1);
                                    tisp_adr_wdr_en(1);
                                    tisp_defog_wdr_en(1);
                                    tisp_mdns_wdr_en(1);
                                    tisp_dmsc_wdr_en(1);
                                    tisp_ae_wdr_en(1);
                                    tisp_sdns_wdr_en(1);
                                    $a1_53 = arg1[2];
                                }
                                
                                int32_t $v0_30;
                                int32_t $v1_29;
                                
                                if (data_b2e74_4)
                                {
                                    $v0_30 = 0x10;
                                    $v1_29 = 0x12;
                                }
                                else
                                {
                                    $v0_30 = 0x1c;
                                    $v1_29 = 0x1e;
                                }
                                
                                if ($(uintptr_t)a1_53 == 0x14)
                                    $v0_30 = $v1_29;
                                
                                system_reg_write(0x804, $v0_30);
                                system_reg_write(0x1c, 8);
                                system_reg_write(0x800, 1);
                                tisp_event_init();
                                tisp_event_set_cb(4, tisp_tgain_update);
                                tisp_event_set_cb(5, tisp_again_update);
                                tisp_event_set_cb(7, tisp_ev_update);
                                tisp_event_set_cb(9, tisp_ct_update);
                                tisp_event_set_cb(8, tisp_ae_ir_update);
                                system_irq_func_set(0xd, ip_done_interrupt_static);
                                int32_t $v0_31 = tisp_param_operate_init();
                                
                                if ($v0_31)
                                {
                                    int32_t var_70_4 = $v0_31;
                                    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                                }
                                
                                return 0;
                            }
                        }
                    }
                }
            }
        }
    }
    
    return 0xffffffff;
}

