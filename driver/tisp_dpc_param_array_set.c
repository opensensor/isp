#include "include/main.h"


  int32_t tisp_dpc_param_array_set(int32_t arg1, int32_t arg2, int32_t* arg3)

{
    if (arg1 - 0xe6 >= 0x1f)
    {
        int32_t var_10_1_9 = arg1;
        isp_printf(2, &$LC0, "tisp_dpc_param_array_set");
        return 0xffffffff;
    }
    
    int32_t* $a0_1;
    int32_t $a2_1;
    
    switch (arg1)
    {
        case 0xe6:
        {
            $a0_1 = &ctr_md_np_array;
            $a2_1 = 0x40;
            break;
        }
        case 0xe7:
        {
            $a0_1 = &ctr_std_np_array;
            $a2_1 = 0x40;
            break;
        }
        case 0xe8:
        {
            $a0_1 = &dpc_s_con_par_array;
            $a2_1 = 0x14;
            break;
        }
        case 0xe9:
        {
            $a0_1 = &dpc_d_m1_fthres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0xea:
        {
            $a0_1 = &dpc_d_m1_dthres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0xeb:
        {
            $a0_1 = &dpc_d_m1_con_par_array;
            $a2_1 = 0xc;
            break;
        }
        case 0xec:
        {
            $a0_1 = &dpc_d_m2_level_array;
            $a2_1 = 0x24;
            break;
        }
        case 0xed:
        {
            $a0_1 = &dpc_d_m2_hthres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0xee:
        {
            $a0_1 = &dpc_d_m2_lthres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0xef:
        {
            $a0_1 = &dpc_d_m2_p0_d1_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0xf0:
        {
            $a0_1 = &dpc_d_m2_p1_d1_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0xf1:
        {
            $a0_1 = &dpc_d_m2_p2_d1_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0xf2:
        {
            $a0_1 = &dpc_d_m2_p3_d1_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0xf3:
        {
            $a0_1 = &dpc_d_m2_p0_d2_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0xf4:
        {
            $a0_1 = &dpc_d_m2_p1_d2_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0xf5:
        {
            $a0_1 = &dpc_d_m2_p2_d2_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0xf6:
        {
            $a0_1 = &dpc_d_m2_p3_d2_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0xf7:
        {
            $a0_1 = &dpc_d_m2_con_par_array;
            $a2_1 = 0x14;
            break;
        }
        case 0xf8:
        {
            $a0_1 = &dpc_d_m3_fthres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0xf9:
        {
            $a0_1 = &dpc_d_m3_dthres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0xfa:
        {
            $a0_1 = &dpc_d_m3_con_par_array;
            $a2_1 = 0x10;
            break;
        }
        case 0xfb:
        {
            $a0_1 = &dpc_d_cor_par_array;
            $a2_1 = 0x2c;
            break;
        }
        case 0xfc:
        {
            $a0_1 = &ctr_stren_array;
            $a2_1 = 0x24;
            break;
        }
        case 0xfd:
        {
            $a0_1 = &ctr_md_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0xfe:
        {
            $a0_1 = &ctr_el_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0xff:
        {
            $a0_1 = &ctr_eh_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x100:
        {
            $a0_1 = &dpc_d_m1_fthres_wdr_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x101:
        {
            $a0_1 = &dpc_d_m1_dthres_wdr_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x102:
        {
            $a0_1 = &dpc_d_m3_fthres_wdr_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x103:
        {
            $a0_1 = &dpc_d_m3_dthres_wdr_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x104:
        {
            $a0_1 = &ctr_con_par_array;
            $a2_1 = 0x1c;
            break;
        }
    }
    
    *arg3 = $a2_1;
    memcpy($a0_1);
    tisp_dpc_all_reg_refresh(data_9ab10_4 + 0x200);
    return 0;
}

