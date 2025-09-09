#include "include/main.h"


  int32_t tisp_ccm_param_array_set(int32_t arg1, int32_t arg2, int32_t* arg3)

{
        int32_t var_10_1 = arg1;
    if (arg1 - (uintptr_t)0xa9 >= 0xc)
    {
        isp_printf(); // Fixed: macro call, removed arguments;
        return 0xffffffff;
    }
    
    int32_t $v0_2;
    void* $a0;
    
    switch (arg1)
    {
        case 0xa9:
        {
            $a0 = &tiziano_ccm_dp_cfg;
            $v0_2 = 0x14;
            break;
        }
        case 0xaa:
        {
            $a0 = &tiziano_ccm_a_linear;
            $v0_2 = 0x24;
            break;
        }
        case 0xab:
        {
            $a0 = &tiziano_ccm_t_linear;
            $v0_2 = 0x24;
            break;
        }
        case 0xac:
        {
            $a0 = &tiziano_ccm_d_linear;
            $v0_2 = 0x24;
            break;
        }
        case 0xad:
        {
            $a0 = &cm_ev_list;
            $v0_2 = 0x24;
            break;
        }
        case 0xae:
        {
            $a0 = &cm_sat_list;
            $v0_2 = 0x24;
            break;
        }
        case 0xaf:
        {
            $a0 = &tiziano_ccm_a_wdr;
            $v0_2 = 0x24;
            break;
        }
        case 0xb0:
        {
            $a0 = &tiziano_ccm_t_wdr;
            $v0_2 = 0x24;
            break;
        }
        case 0xb1:
        {
            $a0 = &tiziano_ccm_d_wdr;
            $v0_2 = 0x24;
            break;
        }
        case 0xb2:
        {
            $a0 = &cm_ev_list_wdr;
            $v0_2 = 0x24;
            break;
        }
        case 0xb3:
        {
            $a0 = &cm_sat_list_wdr;
            $v0_2 = 0x24;
            break;
        }
        case 0xb4:
        {
            $a0 = &cm_awb_list;
            $v0_2 = 8;
            break;
        }
    }
    
    *arg3 = $v0_2;
    memcpy($a0, arg2, $v0_2);
    ccm_real = 1;
    jz_isp_ccm();
    return 0;
}

