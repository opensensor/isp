#include "include/main.h"


  int32_t tisp_bcsh_set_attr(int32_t arg1)

{
    memcpy(&bcsh_ctrl, arg1, 0x28);
    
    if (bcsh_ctrl != 1)
    {
        memcpy(&tisp_BCSH_au32CCMMatrix_d, 0xa6d40, 0x24);
        memcpy(&tisp_BCSH_au32CCMMatrix_t, 0xa6d64, 0x24);
        memcpy(&tisp_BCSH_au32CCMMatrix_a, 0xa6d88, 0x24);
        memcpy(&tisp_BCSH_au32Sthres, 0xa6dd0, 0xc);
        memcpy(&tisp_BCSH_au32CCMMatrix_d_wdr, 0xa6f6c, 0x24);
        memcpy(&tisp_BCSH_au32CCMMatrix_t_wdr, 0xa6f90, 0x24);
        memcpy(&tisp_BCSH_au32CCMMatrix_a_wdr, 0xa6fb4, 0x24);
        memcpy(&tisp_BCSH_au32Sthres_wdr, 0xa6ffc, 0xc);
    }
    else
    {
        memcpy(&tisp_BCSH_au32CCMMatrix_d, 0xc5490, 0x24);
        memcpy(&tisp_BCSH_au32CCMMatrix_t, 0xc5490, 0x24);
        memcpy(&tisp_BCSH_au32CCMMatrix_a, 0xc5490, 0x24);
        memcpy(&tisp_BCSH_au32CCMMatrix_d_wdr, 0xc5490, 0x24);
        memcpy(&tisp_BCSH_au32CCMMatrix_t_wdr, 0xc5490, 0x24);
        memcpy(&tisp_BCSH_au32CCMMatrix_a_wdr, 0xc5490, 0x24);
        
        if (!data_c548d)
        {
            tisp_BCSH_au32Sthres = 0;
            tisp_BCSH_au32Sthres_wdr = 0;
        }
    }
    
    BCSH_real = 1;
    /* tailcall */
    return tiziano_bcsh_update();
}

