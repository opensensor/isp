#include "include/main.h"


  int32_t tisp_ydns_param_array_set(int32_t arg1, int32_t arg2, int32_t* arg3)

{
    if (arg1 - 0x3e6 >= 0xf)
    {
        int32_t var_10_1_13 = arg1;
        isp_printf(2, &$LC0, "tisp_ydns_param_array_set");
        return 0xffffffff;
    }
    
    int32_t $v0_1;
    
    switch (arg1)
    {
        case 0x3e6:
        {
            memcpy(&sdns_mv_num_thr_7x7_array, arg2, 4);
            $v0_1 = 4;
            break;
        }
        case 0x3e7:
        {
            memcpy(&sdns_mv_num_thr_9x9_array);
            $v0_1 = 0x24;
            break;
        }
        case 0x3e8:
        {
            memcpy(&sdns_mv_num_thr_11x11_array);
            $v0_1 = 0x24;
            break;
        }
        case 0x3e9:
        {
            memcpy(&ydns_mv_thres2_array);
            $v0_1 = 0x24;
            break;
        }
        case 0x3ea:
        {
            memcpy(&ydns_fus_level_array);
            $v0_1 = 0x24;
            break;
        }
        case 0x3eb:
        {
            memcpy(&ydns_fus_min_thres_array);
            $v0_1 = 0x24;
            break;
        }
        case 0x3ec:
        {
            memcpy(&ydns_fus_max_thres_array);
            $v0_1 = 0x24;
            break;
        }
        case 0x3ed:
        {
            memcpy(&ydns_fus_sswei_array);
            $v0_1 = 0x24;
            break;
        }
        case 0x3ee:
        {
            memcpy(&ydns_fus_sewei_array);
            $v0_1 = 0x24;
            break;
        }
        case 0x3ef:
        {
            memcpy(&ydns_fus_mswei_array);
            $v0_1 = 0x24;
            break;
        }
        case 0x3f0:
        {
            memcpy(&ydns_fus_mewei_array);
            $v0_1 = 0x24;
            break;
        }
        case 0x3f1:
        {
            memcpy(&ydns_fus_uvwei_array);
            $v0_1 = 0x24;
            break;
        }
        case 0x3f2:
        {
            memcpy(&ydns_edge_wei_array);
            $v0_1 = 0x24;
            break;
        }
        case 0x3f3:
        {
            memcpy(&ydns_edge_div_array);
            $v0_1 = 0x24;
            break;
        }
        case 0x3f4:
        {
            memcpy(&ydns_edge_thres_array, arg2, 0x24);
            tisp_ydns_intp_reg_refresh(ydns_gain_old + 0x200);
            $v0_1 = 0x24;
            break;
        }
    }
    
    *arg3 = $v0_1;
    return 0;
}

