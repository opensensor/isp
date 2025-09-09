#include "include/main.h"


  int32_t tisp_rdns_param_array_set(int32_t arg1, int32_t arg2, int32_t* arg3)

{
    if (arg1 - 0x432 >= 0x15)
    {
        int32_t var_10_1_14 = arg1;
        isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", "tisp_rdns_param_array_set");
        return 0xffffffff;
    }
    
    int32_t $v0_2;
    
    switch (arg1)
    {
        case 0x432:
        {
            memcpy(&rdns_out_opt_array, arg2, 4);
            $v0_2 = 4;
            break;
        }
        case 0x433:
        {
            memcpy(&rdns_awb_gain_par_cfg_array, arg2, 0x10);
            $v0_2 = 0x10;
            break;
        }
        case 0x434:
        {
            memcpy(&rdns_oe_num_array);
            $v0_2 = 0x24;
            break;
        }
        case 0x435:
        {
            memcpy(&rdns_opt_cfg_array, arg2, 0x14);
            $v0_2 = 0x14;
            break;
        }
        case 0x436:
        {
            memcpy(&rdns_gray_stren_array);
            $v0_2 = 0x24;
            break;
        }
        case 0x437:
        {
            memcpy(&rdns_slope_par_cfg_array);
            $v0_2 = 8;
            break;
        }
        case 0x438:
        {
            memcpy(&rdns_gray_std_thres_array);
            $v0_2 = 0x24;
            break;
        }
        case 0x439:
        {
            memcpy(&rdns_text_base_thres_array);
            $v0_2 = 0x24;
            break;
        }
        case 0x43a:
        {
            memcpy(&rdns_filter_sat_thres_array);
            $v0_2 = 0x24;
            break;
        }
        case 0x43b:
        {
            memcpy(&rdns_oe_thres_array);
            $v0_2 = 0x24;
            break;
        }
        case 0x43c:
        {
            memcpy(&rdns_flat_g_thres_array);
            $v0_2 = 0x24;
            break;
        }
        case 0x43d:
        {
            memcpy(&rdns_text_g_thres_array);
            $v0_2 = 0x24;
            break;
        }
        case 0x43e:
        {
            memcpy(&rdns_flat_rb_thres_array);
            $v0_2 = 0x24;
            break;
        }
        case 0x43f:
        {
            memcpy(&rdns_text_rb_thres_array);
            $v0_2 = 0x24;
            break;
        }
        case 0x440:
        {
            memcpy(&rdns_gray_np_array, arg2, 0x20);
            $v0_2 = 0x20;
            break;
        }
        case 0x441:
        {
            memcpy(&sdns_aa_mv_det_opt);
            $v0_2 = 0x40;
            break;
        }
        case 0x442:
        {
            memcpy(&rdns_lum_np_array);
            $v0_2 = 0x40;
            break;
        }
        case 0x443:
        {
            memcpy(&rdns_std_np_array);
            $v0_2 = 0x40;
            break;
        }
        case 0x444:
        {
            memcpy(&rdns_mv_text_thres_array);
            $v0_2 = 0x24;
            break;
        }
        case 0x445:
        {
            $v0_2 = 0x24;
            break;
        }
        case 0x446:
        {
            memcpy(&rdns_sl_par_cfg);
            $v0_2 = 8;
            break;
        }
    }
    
    *arg3 = $v0_2;
    tisp_rdns_all_reg_refresh(rdns_gain_old + 0x200);
    return 0;
}

