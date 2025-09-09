#include "include/main.h"


  int32_t tisp_cust_mode_s_ctrl(uint32_t arg1)

{
    uint32_t tparams_cust_1 = tparams_cust;
    
    if (!tparams_cust_1)
        return 0xffffffff;
    
    if (arg1 == 1)
    {
        memcpy(0x94b20, tparams_cust_1, 0x137f0);
        uint32_t day_night_2 = day_night;
        cust_mode = arg1;
        uint32_t $v0_1 = 3;
        
        if (!(0xfffffffd & day_night_2))
            $v0_1 = 2;
        
        day_night = $v0_1;
    }
    else if (!arg1)
    {
        uint32_t day_night_1 = day_night;
        
        if (day_night_1 == 1)
            memcpy(0x94b20, tparams_night, 0x137f0);
        else if (day_night_1 >= 2)
        {
            if (day_night_1 == 2)
                memcpy(0x94b20, tparams_day, 0x137f0);
            else if (day_night_1 == 3)
                memcpy(0x94b20, tparams_night, 0x137f0);
            else
                isp_printf(2, "Can not support this frame mode!!!\\n", "tisp_cust_mode_s_ctrl");
        }
        else if (!day_night_1)
            memcpy(0x94b20, tparams_day, 0x137f0);
        else
            isp_printf(2, "Can not support this frame mode!!!\\n", "tisp_cust_mode_s_ctrl");
        
        cust_mode = 0;
    }
    
    int32_t $v0_2 = system_reg_read(0xc);
    
    for (int32_t i = 0; i != 0x20; )
    {
        int32_t $v0_3 = ~(1 << (i & 0x1f)) & $v0_2;
        int32_t $a0_6 = *((i << 2) + 0x94b20) << (i & 0x1f);
        i += 1;
        $v0_2 = $a0_6 + $v0_3;
    }
    
    int32_t $v0_4;
    int32_t $s0_1;
    
    if (data_b2e74_9 != 1)
    {
        $v0_4 = $v0_2 & 0xb577fffd;
        $s0_1 = 0x34000009;
    }
    else
    {
        $v0_4 = $v0_2 & 0xa1fffff6;
        $s0_1 = 0x880002;
    }
    
    int32_t $s0_2 = $v0_4 | $s0_1;
    int32_t var_10_55 = $s0_2;
    isp_printf(0, "sensor type is BT1120!\\n", "tisp_cust_mode_s_ctrl");
    system_reg_write(0xc, $s0_2);
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
    char $a1_2;
    char $a2;
    $a0_7 = tiziano_clm_dn_params_refresh();
    tiziano_gamma_dn_params_refresh($a0_7, $a1_2, $a2);
    tiziano_adr_dn_params_refresh();
    tiziano_dpc_dn_params_refresh();
    tiziano_af_dn_params_refresh();
    tiziano_bcsh_dn_params_refresh();
    tiziano_rdns_dn_params_refresh();
    tiziano_ydns_dn_params_refresh();
    return 0;
}

