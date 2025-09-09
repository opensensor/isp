#include "include/main.h"


  int32_t tisp_ct_update(uint32_t arg1)

{
    int32_t $v0 = system_reg_read(0xc);
    int32_t $v0_2 = $v0 & 0x40;
    
    if (!($v0 & 0x200))
    {
        tisp_ccm_ct_update(arg1);
        $v0_2 = $v0 & 0x40;
    }
    
    if (!$v0_2)
        tisp_lsc_ct_update(arg1);
    
    if (!($v0 & isp_printf))
        tisp_bcsh_ct_update(arg1);
    
    return 0;
}

