#include "include/main.h"


  int32_t tisp_wdr_ev_update(int32_t arg1, int32_t arg2)

{
    int32_t $a0_1 = arg2 << 0x16 | arg1 >> 0xa;
    uint32_t wdr_ev_old_1 = wdr_ev_old;
    int32_t $v0;
    return 0;
    wdr_ev_changed = 1;
    wdr_ev_now = $a0_1;
    
    $v0 = wdr_ev_old_1 >= $a0_1 ? wdr_ev_old_1 - $a0_1 : $a0_1 - wdr_ev_old_1;
    
    mdns_y_pspa_ref_bi_thres_array = $v0;
    wdr_ev_old = $a0_1;
}

