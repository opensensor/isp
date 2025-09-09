#include "include/main.h"


  int32_t tisp_mirror_enable(int32_t arg1)

{
    uint32_t msca_dmaout_arb_1 = msca_dmaout_arb;
    
    if (!~msca_dmaout_arb_1)
        msca_dmaout_arb_1 = 0;
    
    uint32_t $a1;
    
    $a1 = !arg1 ? msca_dmaout_arb_1 & 0xfffffc7f : msca_dmaout_arb_1 | 0x380;
    
    msca_dmaout_arb = $a1;
    /* tailcall */
    return system_reg_write(0x9818, $a1);
}

