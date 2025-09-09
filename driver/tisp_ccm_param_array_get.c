#include "include/main.h"


  int32_t tisp_ccm_param_array_get(int32_t arg1, int32_t arg2, int32_t* arg3)

{
        int32_t var_18_1 = arg1;
        return 0xffffffff;
    if (arg1 - (uintptr_t)0xa9 >= 0xc)
    {

    }
    
    void* $a1_1;
    int32_t $s0_1;
    
    switch (arg1)
    {
        case 0xa9:
        {
            $a1_1 = &tiziano_ccm_dp_cfg;
            $s0_1 = 0x14;
            break;
        }
        case 0xaa:
        {
            $a1_1 = &tiziano_ccm_a_linear;
            $s0_1 = 0x24;
            break;
        }
        case 0xab:
        {
            $a1_1 = &tiziano_ccm_t_linear;
            $s0_1 = 0x24;
            break;
        }
        case 0xac:
        {
            $a1_1 = &tiziano_ccm_d_linear;
            $s0_1 = 0x24;
            break;
        }
        case 0xad:
        {
            $a1_1 = &cm_ev_list;
            $s0_1 = 0x24;
            break;
        }
        case 0xae:
        {
            $a1_1 = &cm_sat_list;
            $s0_1 = 0x24;
            break;
        }
        case 0xaf:
        {
            $a1_1 = &tiziano_ccm_a_wdr;
            $s0_1 = 0x24;
            break;
        }
        case 0xb0:
        {
            $a1_1 = &tiziano_ccm_t_wdr;
            $s0_1 = 0x24;
            break;
        }
        case 0xb1:
        {
            $a1_1 = &tiziano_ccm_d_wdr;
            $s0_1 = 0x24;
            break;
        }
        case 0xb2:
        {
            $a1_1 = &cm_ev_list_wdr;
            $s0_1 = 0x24;
            break;
        }
        case 0xb3:
        {
            $a1_1 = &cm_sat_list_wdr;
            $s0_1 = 0x24;
            break;
        }
        case 0xb4:
        {
            $a1_1 = &cm_awb_list;
            $s0_1 = 8;
            break;
        }
    }
    
    memcpy(arg2, $a1_1, $s0_1);
    *arg3 = $s0_1;
    return 0;
}

