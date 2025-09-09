#include "include/main.h"


  int32_t tisp_wdr_process()

{
    tiziano_wdr_algorithm();
    tiziano_wdr_soft_para_out();
    int32_t $v0_1 = mdns_y_pspa_ref_median_win_opt_array + 1;
    
    if ($v0_1 == 0x1e)
        $v0_1 = 0;
    
    mdns_y_pspa_ref_median_win_opt_array = $v0_1;
    return 0;
}

