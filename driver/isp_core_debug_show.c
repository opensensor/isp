#include "include/main.h"


  int32_t isp_core_debug_show(void* arg1)

{
    if (isp_core_debug_type != 1)
        /* tailcall */
        return isp_info_show.isra.0(arg1);
    
    isp_core_debug_type = 0;
    int32_t entry_$a2;
    
    if (data_ca554_2 != 4)
        /* tailcall */
        return private_seq_printf(arg1, "Err [VIC_INT] : dvp hcomp err!!!!\\n", entry_$a2);
    /* tailcall */
    return private_seq_printf(arg1, "Err [VIC_INT] : hvf err !!!!!\\n", 
        data_ca568_1 * 0xfff0bdc0 + data_ca558_1 * 0xf4240 + data_ca55c_1
            - mdns_y_pspa_cur_bi_wei_seg_array);
}

