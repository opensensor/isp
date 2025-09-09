#include "include/main.h"


  int32_t tisp_bcsh_param_array_set(int32_t arg1, int32_t* arg2, int32_t* arg3)

{
    if (arg1 - 0x3c0 >= 0x26)
    {
        int32_t var_18_1_11 = arg1;
        isp_printf(2, "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\\n", 
            "tisp_bcsh_param_array_set");
        return 0xffffffff;
    }
    
    int32_t $s1_1;
    
    switch (arg1)
    {
        case 0x3c0:
        {
            $s1_1 = 0x24;
            memcpy(&tisp_BCSH_au32CCMMatrix_d, arg2, $s1_1);
            break;
        }
        case 0x3c1:
        {
            $s1_1 = 0x24;
            memcpy(&tisp_BCSH_au32CCMMatrix_t, arg2, $s1_1);
            break;
        }
        case 0x3c2:
        {
            $s1_1 = 0x24;
            memcpy(&tisp_BCSH_au32CCMMatrix_a, arg2, $s1_1);
            break;
        }
        case 0x3c3:
        {
            $s1_1 = 0xc;
            memcpy(&tisp_BCSH_au32HDP, arg2, $s1_1);
            break;
        }
        case 0x3c4:
        {
            $s1_1 = 0xc;
            memcpy(&tisp_BCSH_au32HBP, arg2, $s1_1);
            break;
        }
        case 0x3c5:
        {
            $s1_1 = 0xc;
            memcpy(&tisp_BCSH_au32HLSP, arg2, $s1_1);
            break;
        }
        case 0x3c6:
        {
            $s1_1 = 0xc;
            memcpy(&tisp_BCSH_au32Sthres, arg2, $s1_1);
            break;
        }
        case 0x3c7:
        {
            $s1_1 = 0x24;
            memcpy(&tisp_BCSH_au32EvList, arg2, $s1_1);
            break;
        }
        case 0x3c8:
        {
            $s1_1 = 0x24;
            memcpy(&tisp_BCSH_au32SminListS, arg2, $s1_1);
            break;
        }
        case 0x3c9:
        {
            $s1_1 = 0x24;
            memcpy(&tisp_BCSH_au32SmaxListS, arg2, $s1_1);
            break;
        }
        case 0x3ca:
        {
            $s1_1 = 0x24;
            memcpy(&tisp_BCSH_au32SminListM, arg2, $s1_1);
            break;
        }
        case 0x3cb:
        {
            $s1_1 = 0x24;
            memcpy(&tisp_BCSH_au32SmaxListM, arg2, $s1_1);
            break;
        }
        case 0x3cc:
        {
            tisp_BCSH_au32C = *arg2;
            $s1_1 = 0x14;
            break;
        }
        case 0x3cd:
        {
            $s1_1 = 0x24;
            memcpy(&tisp_BCSH_au32Cxl, arg2, $s1_1);
            break;
        }
        case 0x3ce:
        {
            $s1_1 = 0x24;
            memcpy(&tisp_BCSH_au32Cxh, arg2, $s1_1);
            break;
        }
        case 0x3cf:
        {
            $s1_1 = 0x24;
            memcpy(&tisp_BCSH_au32Cyl, arg2, $s1_1);
            break;
        }
        case 0x3d0:
        {
            $s1_1 = 0x24;
            memcpy(&tisp_BCSH_au32Cyh, arg2, $s1_1);
            break;
        }
        case 0x3d1:
        {
            $s1_1 = 4;
            memcpy(&tisp_BCSH_u32B, arg2, $s1_1);
            break;
        }
        case 0x3d2:
        {
            $s1_1 = 0xc;
            memcpy(&tisp_BCSH_au32OffsetRGB, arg2, $s1_1);
            break;
        }
        case 0x3d3:
        {
            $s1_1 = 8;
            memcpy(&tisp_BCSH_au32OffsetYUVy, arg2, $s1_1);
            break;
        }
        case 0x3d4:
        {
            $s1_1 = 0x10;
            memcpy(&tisp_BCSH_au32clip0, arg2, $s1_1);
            break;
        }
        case 0x3d5:
        {
            $s1_1 = 0x10;
            memcpy(&tisp_BCSH_au32clip1, arg2, $s1_1);
            break;
        }
        case 0x3d6:
        {
            $s1_1 = 0x24;
            memcpy(&tisp_BCSH_au32CCMMatrix_d_wdr, arg2, $s1_1);
            break;
        }
        case 0x3d7:
        {
            $s1_1 = 0x24;
            memcpy(&tisp_BCSH_au32CCMMatrix_t_wdr, arg2, $s1_1);
            break;
        }
        case 0x3d8:
        {
            $s1_1 = 0x24;
            memcpy(&tisp_BCSH_au32CCMMatrix_a_wdr, arg2, $s1_1);
            break;
        }
        case 0x3d9:
        {
            $s1_1 = 0xc;
            memcpy(&tisp_BCSH_au32HDP_wdr, arg2, $s1_1);
            break;
        }
        case 0x3da:
        {
            $s1_1 = 0xc;
            memcpy(&tisp_BCSH_au32HBP_wdr, arg2, $s1_1);
            break;
        }
        case 0x3db:
        {
            $s1_1 = 0xc;
            memcpy(&tisp_BCSH_au32HLSP_wdr, arg2, $s1_1);
            break;
        }
        case 0x3dc:
        {
            $s1_1 = 0xc;
            memcpy(&tisp_BCSH_au32Sthres_wdr, arg2, $s1_1);
            break;
        }
        case 0x3dd:
        {
            $s1_1 = 0x24;
            memcpy(&tisp_BCSH_au32EvList_wdr, arg2, $s1_1);
            break;
        }
        case 0x3de:
        {
            $s1_1 = 0x24;
            memcpy(&tisp_BCSH_au32SminListS_wdr, arg2, $s1_1);
            break;
        }
        case 0x3df:
        {
            $s1_1 = 0x24;
            memcpy(&tisp_BCSH_au32SmaxListS_wdr, arg2, $s1_1);
            break;
        }
        case 0x3e0:
        {
            $s1_1 = 0x24;
            memcpy(&tisp_BCSH_au32SminListM_wdr, arg2, $s1_1);
            break;
        }
        case 0x3e1:
        {
            $s1_1 = 0x24;
            memcpy(&tisp_BCSH_au32SmaxListM_wdr, arg2, $s1_1);
            break;
        }
        case 0x3e2:
        {
            $s1_1 = 0xc;
            memcpy(&tisp_BCSH_au32OffsetRGB_wdr, arg2, $s1_1);
            break;
        }
        case 0x3e3:
        {
            $s1_1 = 0x24;
            memcpy(&MMatrix, arg2, $s1_1);
            break;
        }
        case 0x3e4:
        {
            $s1_1 = 0x24;
            memcpy(&MinvMatrix, arg2, $s1_1);
            break;
        }
        case 0x3e5:
        {
            $s1_1 = 0x10;
            memcpy(&tisp_BCSH_au32clip2, arg2, $s1_1);
            break;
        }
    }
    
    *arg3 = $s1_1;
    BCSH_real = 1;
    tiziano_bcsh_update();
    return 0;
}

