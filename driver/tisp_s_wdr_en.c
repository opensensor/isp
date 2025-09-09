#include "include/main.h"


  int32_t tisp_s_wdr_en(uint32_t arg1)

{
    int32_t i;
    int32_t $v0_2 = system_reg_read(0x20);
    int32_t $v0_3 = system_reg_read(0xc);
    int32_t var_2c = $v0_3;
    uint32_t var_30 = arg1;
    int32_t $a1_3;
    int32_t $s1;
        int32_t $s1_1 = data_94b3c;
    system_reg_write(0x24, system_reg_read(0x24) | 1);
    
    do
        i = system_reg_read(0x28) & 1;
     while (!i);
    system_reg_write(0x20, $v0_2 | 4);
    system_reg_write(0x20, $v0_2 & 0xfffffffb);
    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
    
    if (arg1 != 1)
    {
        data_b2e74 = 0;
        $s1 = ($s1_1 << 7) + (($v0_3 & 0xb577ff7d) | 0x34000009);
        $a1_3 = 0x1c;
    }
    else
    {
        int32_t var_2c_1 = $s1;
        uint32_t var_30_1 = arg1;
        $s1 = ($v0_3 & 0xa1ffdf76) | 0x880002;
        isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
        data_b2e74 = arg1;
        $a1_3 = 0x10;
    }
    
    system_reg_write(0x804, $a1_3);
    system_reg_write(0xc, $s1);
    tisp_dpc_wdr_en(arg1);
    tisp_lsc_wdr_en(arg1);
    tisp_gamma_wdr_en(arg1);
    tisp_sharpen_wdr_en(arg1);
    tisp_ccm_wdr_en(arg1);
    tisp_bcsh_wdr_en(arg1);
    tisp_rdns_wdr_en(arg1);
    tisp_adr_wdr_en(arg1);
    tisp_defog_wdr_en(arg1);
    tisp_mdns_wdr_en(arg1);
    tisp_dmsc_wdr_en(arg1);
    tisp_ae_wdr_en(arg1);
    tisp_sdns_wdr_en(arg1);
    tiziano_clm_init();
    tiziano_ydns_init();
    system_reg_write(0x800, 1);
    int32_t var_2c_2_1 = $s1;
    uint32_t var_30_2_3 = arg1;
    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
    return 0;
}

