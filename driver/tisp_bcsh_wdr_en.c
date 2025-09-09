#include "include/main.h"


  int32_t tisp_bcsh_wdr_en(uint32_t arg1)

{
    void* $v0;
    bcsh_wdr_en = arg1;
    
    if (arg1)
    {
        tisp_BCSH_au32CCMMatrix_d_now = &tisp_BCSH_au32CCMMatrix_d_wdr;
        tisp_BCSH_au32CCMMatrix_t_now = &tisp_BCSH_au32CCMMatrix_t_wdr;
        tisp_BCSH_au32CCMMatrix_a_now = &tisp_BCSH_au32CCMMatrix_a_wdr;
        tisp_BCSH_au32HDP_now = &tisp_BCSH_au32HDP_wdr;
        tisp_BCSH_au32HBP_now = &tisp_BCSH_au32HBP_wdr;
        tisp_BCSH_au32HLSP_now = &tisp_BCSH_au32HLSP_wdr;
        tisp_BCSH_au32Sthres_now = &tisp_BCSH_au32Sthres_wdr;
        tisp_BCSH_au32EvList_now = &tisp_BCSH_au32EvList_wdr;
        tisp_BCSH_au32SminListS_now = &tisp_BCSH_au32SminListS_wdr;
        tisp_BCSH_au32SmaxListS_now = &tisp_BCSH_au32SmaxListS_wdr;
        tisp_BCSH_au32SminListM_now = &tisp_BCSH_au32SminListM_wdr;
        tisp_BCSH_au32SmaxListM_now = &tisp_BCSH_au32SmaxListM_wdr;
        $v0 = &tisp_BCSH_au32OffsetRGB_wdr;
    }
    else
    {
        tisp_BCSH_au32CCMMatrix_d_now = &tisp_BCSH_au32CCMMatrix_d;
        tisp_BCSH_au32CCMMatrix_t_now = &tisp_BCSH_au32CCMMatrix_t;
        tisp_BCSH_au32CCMMatrix_a_now = &tisp_BCSH_au32CCMMatrix_a;
        tisp_BCSH_au32HDP_now = &tisp_BCSH_au32HDP;
        tisp_BCSH_au32HBP_now = &tisp_BCSH_au32HBP;
        tisp_BCSH_au32HLSP_now = &tisp_BCSH_au32HLSP;
        tisp_BCSH_au32Sthres_now = &tisp_BCSH_au32Sthres;
        tisp_BCSH_au32EvList_now = &tisp_BCSH_au32EvList;
        tisp_BCSH_au32SminListS_now = &tisp_BCSH_au32SminListS;
        tisp_BCSH_au32SmaxListS_now = &tisp_BCSH_au32SmaxListS;
        tisp_BCSH_au32SminListM_now = &tisp_BCSH_au32SminListM;
        tisp_BCSH_au32SmaxListM_now = &tisp_BCSH_au32SmaxListM;
        $v0 = &tisp_BCSH_au32OffsetRGB;
    }
    
    tisp_BCSH_au32OffsetRGB_now = $v0;
    BCSH_real = 1;
    tiziano_bcsh_update();
    return 0;
}

