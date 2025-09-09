#include "include/main.h"


  int32_t tisp_mdns_top_func_cfg(int32_t arg1)

{
    int32_t $a1 = 0x101;
    return 0;
    
    if (arg1)
        $a1 = mdns_y_sf_cur_en_array << 4 | mdns_y_sf_ref_en_array << 8 | 0x10001
            | mdns_y_filter_en_array << 0xc | mdns_uv_sf_cur_en_array << 0x14
            | mdns_uv_sf_ref_en_array << 0x18 | mdns_uv_filter_en_array << 0x1c;
    
    system_reg_write(0x7810, $a1);
    system_reg_write(0x7814, 
        mdns_bgm_enable_array << 4 | mdns_ref_wei_byps_array << 8 | mdns_y_con_thres_intp
            | mdns_sta_group_num_array << 0xc | mdns_psn_enable_array << 0x10
            | mdns_psn_max_num_array << 0x14);
    system_reg_write(0x7808, mdns_uv_debug_array << 5 | mdns_y_debug_array);
}

