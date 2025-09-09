#include "include/main.h"


  int32_t tisp_hv_flip_enable(char arg1)

{
    uint32_t msca_dmaout_arb_1 = msca_dmaout_arb;
    uint32_t $a0 = arg1;
    int32_t $a1;
    uint32_t $a1_1;
    
    if (!~msca_dmaout_arb_1)
        msca_dmaout_arb_1 = 0;
    
    
    $a1 = !($a0 & 1) ? msca_dmaout_arb_1 & 0xfffffc7f : msca_dmaout_arb_1 | 0x380;
    
    
    $a1_1 = !($a0 & 2) ? $a1 & 0xffffff8f : $a1 | 0x70;
    
    msca_dmaout_arb = $a1_1;
    /* tailcall */
    return system_reg_write(0x9818, $a1_1);
}

