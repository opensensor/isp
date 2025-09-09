#include "include/main.h"


  int32_t tisp_dpc_param_array_get(int32_t arg1, int32_t arg2, int32_t* arg3)

{
        int32_t var_18_1 = arg1;
        return 0xffffffff;
    if (arg1 - (uintptr_t)0xe6 >= 0x1f)
    {
        isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
    }
    
    int32_t* $a1_1;
    int32_t $s1_1;
    
    switch (arg1)
    {
        case 0xe6:
        {
            $a1_1 = &ctr_md_np_array;
            $s1_1 = 0x40;
            break;
        }
        case 0xe7:
        {
            $a1_1 = &ctr_std_np_array;
            $s1_1 = 0x40;
            break;
        }
        case 0xe8:
        {
            $a1_1 = &dpc_s_con_par_array;
            $s1_1 = 0x14;
            break;
        }
        case 0xe9:
        {
            $a1_1 = &dpc_d_m1_fthres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xea:
        {
            $a1_1 = &dpc_d_m1_dthres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xeb:
        {
            $a1_1 = &dpc_d_m1_con_par_array;
            $s1_1 = 0xc;
            break;
        }
        case 0xec:
        {
            $a1_1 = &dpc_d_m2_level_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xed:
        {
            $a1_1 = &dpc_d_m2_hthres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xee:
        {
            $a1_1 = &dpc_d_m2_lthres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xef:
        {
            $a1_1 = &dpc_d_m2_p0_d1_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xf0:
        {
            $a1_1 = &dpc_d_m2_p1_d1_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xf1:
        {
            $a1_1 = &dpc_d_m2_p2_d1_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xf2:
        {
            $a1_1 = &dpc_d_m2_p3_d1_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xf3:
        {
            $a1_1 = &dpc_d_m2_p0_d2_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xf4:
        {
            $a1_1 = &dpc_d_m2_p1_d2_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xf5:
        {
            $a1_1 = &dpc_d_m2_p2_d2_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xf6:
        {
            $a1_1 = &dpc_d_m2_p3_d2_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xf7:
        {
            $a1_1 = &dpc_d_m2_con_par_array;
            $s1_1 = 0x14;
            break;
        }
        case 0xf8:
        {
            $a1_1 = &dpc_d_m3_fthres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xf9:
        {
            $a1_1 = &dpc_d_m3_dthres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xfa:
        {
            $a1_1 = &dpc_d_m3_con_par_array;
            $s1_1 = 0x10;
            break;
        }
        case 0xfb:
        {
            $a1_1 = &dpc_d_cor_par_array;
            $s1_1 = 0x2c;
            break;
        }
        case 0xfc:
        {
            $a1_1 = &ctr_stren_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xfd:
        {
            $a1_1 = &ctr_md_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xfe:
        {
            $a1_1 = &ctr_el_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xff:
        {
            $a1_1 = &ctr_eh_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x100:
        {
            $a1_1 = &dpc_d_m1_fthres_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x101:
        {
            $a1_1 = &dpc_d_m1_dthres_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x102:
        {
            $a1_1 = &dpc_d_m3_fthres_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x103:
        {
            $a1_1 = &dpc_d_m3_dthres_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x104:
        {
            $a1_1 = &ctr_con_par_array;
            $s1_1 = 0x1c;
            break;
        }
    }
    
    memcpy(arg2, $a1_1, $s1_1);
    *arg3 = $s1_1;
    return 0;
}

