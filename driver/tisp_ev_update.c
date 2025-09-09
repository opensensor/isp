#include "include/main.h"


  int32_t tisp_ev_update(int32_t arg1, int32_t arg2)

{
    int32_t $v0 = system_reg_read(0xc);
        int32_t $v0_4 = $v0 & 0x800;
    
    if (*ev_last != arg1 || *(ev_last + 4) != arg2)
    {
        tisp_awb_ev_update(arg1);
        tisp_ccm_ev_update(arg1);
        
        if (!($v0 & 0x80))
        {
            tisp_adr_ev_update(arg1, arg2);
            $v0_4 = $v0 & 0x800;
        }
        
        if (!$v0_4)
            tisp_defog_ev_update(arg1, arg2);
        
        tisp_bcsh_ev_update(arg1);
        
        if ($v0 & 8)
            *ev_last = arg1;
        else
        {
            tisp_wdr_ev_update(arg1, arg2);
            *ev_last = arg1;
        }
        
        *(((void**)((char*)ev_last + 4))) = arg2; // Fixed void pointer dereference
    }
    
    return 0;
}

