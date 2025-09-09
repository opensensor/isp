#include "include/main.h"


  int32_t tiziano_bcsh_params_refresh()

{
    if (!bcsh_ctrl)
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
    
    memcpy(&tisp_BCSH_au32HDP, 0xa6dac, 0xc);
    memcpy(&tisp_BCSH_au32HBP, 0xa6db8, 0xc);
    memcpy(&tisp_BCSH_au32HLSP, 0xa6dc4, 0xc);
    memcpy(&tisp_BCSH_au32EvList, 0xa6ddc, 0x24);
    memcpy(&tisp_BCSH_au32SminListS, 0xa6e00, 0x24);
    memcpy(&tisp_BCSH_au32SmaxListS, 0xa6e24, 0x24);
    memcpy(&tisp_BCSH_au32SminListM, 0xa6e48, 0x24);
    memcpy(&tisp_BCSH_au32SmaxListM, 0xa6e6c, 0x24);
    memcpy(&tisp_BCSH_au32C, 0xa6e90, 0x14);
    memcpy(&tisp_BCSH_au32Cxl, 0xa6ea4, 0x24);
    memcpy(&tisp_BCSH_au32Cxh, 0xa6ec8, 0x24);
    memcpy(&tisp_BCSH_au32Cyl, 0xa6eec, 0x24);
    memcpy(&tisp_BCSH_au32Cyh, 0xa6f10, 0x24);
    memcpy(&tisp_BCSH_u32B, 0xa6f34, 4);
    memcpy(&tisp_BCSH_au32OffsetRGB, 0xa6f38, 0xc);
    memcpy(&tisp_BCSH_au32OffsetYUVy, 0xa6f44, 8);
    memcpy(&tisp_BCSH_au32clip0, 0xa6f4c, 0x10);
    memcpy(&tisp_BCSH_au32clip1, 0xa6f5c, 0x10);
    memcpy(&tisp_BCSH_au32HDP_wdr, 0xa6fd8, 0xc);
    memcpy(&tisp_BCSH_au32HBP_wdr, 0xa6fe4, 0xc);
    memcpy(&tisp_BCSH_au32HLSP_wdr, 0xa6ff0, 0xc);
    memcpy(&tisp_BCSH_au32EvList_wdr, 0xa7008, 0x24);
    memcpy(&tisp_BCSH_au32SminListS_wdr, 0xa702c, 0x24);
    memcpy(&tisp_BCSH_au32SmaxListS_wdr, 0xa7050, 0x24);
    memcpy(&tisp_BCSH_au32SminListM_wdr, 0xa7074, 0x24);
    memcpy(&tisp_BCSH_au32SmaxListM_wdr, 0xa7098, 0x24);
    memcpy(&tisp_BCSH_au32OffsetRGB_wdr, 0xa70bc, 0xc);
    memcpy(&MMatrix, 0xa70c8, 0x24);
    memcpy(&MinvMatrix, 0xa70ec, 0x24);
    memcpy(&tisp_BCSH_au32clip2, 0xa7110, 0x10);
    return 0;
}

