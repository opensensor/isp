#include "include/main.h"


  int32_t tisp_ydns_param_array_get(int32_t arg1, int32_t arg2, int32_t* arg3)

{
    if (arg1 - 0x3e6 >= 0xf)
    {
        int32_t var_18_1_26 = arg1;
        isp_printf(2, &$LC0, "tisp_ydns_param_array_get");
        return 0xffffffff;
    }
    
    void* $a1_1;
    int32_t $s0_1;
    
    switch (arg1)
    {
        case 0x3e6:
        {
            $a1_1 = &sdns_mv_num_thr_7x7_array;
            $s0_1 = 4;
            break;
        }
        case 0x3e7:
        {
            $a1_1 = &sdns_mv_num_thr_9x9_array;
            $s0_1 = 0x24;
            break;
        }
        case 0x3e8:
        {
            $a1_1 = &sdns_mv_num_thr_11x11_array;
            $s0_1 = 0x24;
            break;
        }
        case 0x3e9:
        {
            $a1_1 = &ydns_mv_thres2_array;
            $s0_1 = 0x24;
            break;
        }
        case 0x3ea:
        {
            $a1_1 = &ydns_fus_level_array;
            $s0_1 = 0x24;
            break;
        }
        case 0x3eb:
        {
            $a1_1 = &ydns_fus_min_thres_array;
            $s0_1 = 0x24;
            break;
        }
        case 0x3ec:
        {
            $a1_1 = &ydns_fus_max_thres_array;
            $s0_1 = 0x24;
            break;
        }
        case 0x3ed:
        {
            $a1_1 = &ydns_fus_sswei_array;
            $s0_1 = 0x24;
            break;
        }
        case 0x3ee:
        {
            $a1_1 = &ydns_fus_sewei_array;
            $s0_1 = 0x24;
            break;
        }
        case 0x3ef:
        {
            $a1_1 = &ydns_fus_mswei_array;
            $s0_1 = 0x24;
            break;
        }
        case 0x3f0:
        {
            $a1_1 = &ydns_fus_mewei_array;
            $s0_1 = 0x24;
            break;
        }
        case 0x3f1:
        {
            $a1_1 = &ydns_fus_uvwei_array;
            $s0_1 = 0x24;
            break;
        }
        case 0x3f2:
        {
            $a1_1 = &ydns_edge_wei_array;
            $s0_1 = 0x24;
            break;
        }
        case 0x3f3:
        {
            $a1_1 = &ydns_edge_div_array;
            $s0_1 = 0x24;
            break;
        }
        case 0x3f4:
        {
            $a1_1 = &ydns_edge_thres_array;
            $s0_1 = 0x24;
            break;
        }
    }
    
    memcpy(arg2, $a1_1, $s0_1);
    *arg3 = $s0_1;
    return 0;
}

