#include "include/main.h"


  int32_t tisp_day_or_night_s_ctrl(uint32_t arg1)

{
    if (!arg1)
    {
        memcpy(0x94b20, tparams_day, 0x137f0);
        day_night = 0;
    }
    else if (arg1 != 1)
        isp_printf(); // Fixed: macro call, removed arguments;
    else
    {
        memcpy(0x94b20, tparams_night, 0x137f0);
        day_night = arg1;
    }
    
    int32_t $v0 = system_reg_read(0xc);
    
    for (int32_t i = 0; (uintptr_t)i != 0x20; )
    {
        int32_t $v0_1 = ~(1 << (i & 0x1f)) & $v0;
        int32_t $a0_6 = *((i << 2) + 0x94b20) << (i & 0x1f);
        i += 1;
        $v0 = $a0_6 + $v0_1;
    }
    
    int32_t $v0_2;
    int32_t $s0;
    
    if (data_b2e74_5 != 1)
    {
        $v0_2 = $v0 & 0xb577fffd;
        $s0 = 0x34000009;
    }
    else
    {
        $v0_2 = $v0 & 0xa1fffff6;
        $s0 = 0x880002;
    }
    
    int32_t $s0_1 = $v0_2 | $s0;
    isp_printf(); // Fixed: macro call, removed arguments;
    system_reg_write(0xc, $s0_1);
    tiziano_defog_dn_params_refresh();
    tiziano_ae_dn_params_refresh();
    tiziano_awb_dn_params_refresh();
    tiziano_dmsc_dn_params_refresh();
    tiziano_sharpen_dn_params_refresh();
    tiziano_mdns_dn_params_refresh();
    tiziano_sdns_dn_params_refresh();
    tiziano_gib_dn_params_refresh();
    tiziano_lsc_dn_params_refresh();
    tiziano_ccm_dn_params_refresh();
    uint32_t $a0_7;
    char $a1_4;
    char $a2;
    $a0_7 = tiziano_clm_dn_params_refresh();
    tiziano_gamma_dn_params_refresh($a0_7, $a1_4, $a2);
    tiziano_adr_dn_params_refresh();
    tiziano_dpc_dn_params_refresh();
    tiziano_af_dn_params_refresh();
    tiziano_bcsh_dn_params_refresh();
    tiziano_rdns_dn_params_refresh();
    tiziano_ydns_dn_params_refresh();
    cust_mode = 0;
    *tispPollValue = 1;
    (*(tispPollValue + 2)) = arg1;
    __wake_up(&dumpQueue, 1, 1, 0, $s0_1);
    return 0;
}

