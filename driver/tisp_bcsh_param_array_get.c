#include "include/main.h"


  int32_t tisp_bcsh_param_array_get(int32_t arg1, int32_t arg2, int32_t* arg3)

{
        int32_t var_18_1 = arg1;
        return 0xffffffff;
    if (arg1 - (uintptr_t)0x3c0 >= 0x26)
    {
        isp_printf(); // Fixed: macro with no parameters, removed 2 arguments\n", 
            "tisp_bcsh_param_array_get");
    }
    
    void* $a1_1;
    int32_t $s1_1;
    
    switch (arg1)
    {
        case 0x3c0:
        {
            $a1_1 = &tisp_BCSH_au32CCMMatrix_d;
            $s1_1 = 0x24;
            break;
        }
        case 0x3c1:
        {
            $a1_1 = &tisp_BCSH_au32CCMMatrix_t;
            $s1_1 = 0x24;
            break;
        }
        case 0x3c2:
        {
            $a1_1 = &tisp_BCSH_au32CCMMatrix_a;
            $s1_1 = 0x24;
            break;
        }
        case 0x3c3:
        {
            $a1_1 = &tisp_BCSH_au32HDP;
            $s1_1 = 0xc;
            break;
        }
        case 0x3c4:
        {
            $a1_1 = &tisp_BCSH_au32HBP;
            $s1_1 = 0xc;
            break;
        }
        case 0x3c5:
        {
            $a1_1 = &tisp_BCSH_au32HLSP;
            $s1_1 = 0xc;
            break;
        }
        case 0x3c6:
        {
            $a1_1 = &tisp_BCSH_au32Sthres;
            $s1_1 = 0xc;
            break;
        }
        case 0x3c7:
        {
            $a1_1 = &tisp_BCSH_au32EvList;
            $s1_1 = 0x24;
            break;
        }
        case 0x3c8:
        {
            $a1_1 = &tisp_BCSH_au32SminListS;
            $s1_1 = 0x24;
            break;
        }
        case 0x3c9:
        {
            $a1_1 = &tisp_BCSH_au32SmaxListS;
            $s1_1 = 0x24;
            break;
        }
        case 0x3ca:
        {
            $a1_1 = &tisp_BCSH_au32SminListM;
            $s1_1 = 0x24;
            break;
        }
        case 0x3cb:
        {
            $a1_1 = &tisp_BCSH_au32SmaxListM;
            $s1_1 = 0x24;
            break;
        }
        case 0x3cc:
        {
            $a1_1 = &tisp_BCSH_au32C;
            $s1_1 = 0x14;
            break;
        }
        case 0x3cd:
        {
            $a1_1 = &tisp_BCSH_au32Cxl;
            $s1_1 = 0x24;
            break;
        }
        case 0x3ce:
        {
            $a1_1 = &tisp_BCSH_au32Cxh;
            $s1_1 = 0x24;
            break;
        }
        case 0x3cf:
        {
            $a1_1 = &tisp_BCSH_au32Cyl;
            $s1_1 = 0x24;
            break;
        }
        case 0x3d0:
        {
            $a1_1 = &tisp_BCSH_au32Cyh;
            $s1_1 = 0x24;
            break;
        }
        case 0x3d1:
        {
            $a1_1 = &tisp_BCSH_u32B;
            $s1_1 = 4;
            break;
        }
        case 0x3d2:
        {
            $a1_1 = &tisp_BCSH_au32OffsetRGB;
            $s1_1 = 0xc;
            break;
        }
        case 0x3d3:
        {
            $a1_1 = &tisp_BCSH_au32OffsetYUVy;
            $s1_1 = 8;
            break;
        }
        case 0x3d4:
        {
            $a1_1 = &tisp_BCSH_au32clip0;
            $s1_1 = 0x10;
            break;
        }
        case 0x3d5:
        {
            $a1_1 = &tisp_BCSH_au32clip1;
            $s1_1 = 0x10;
            break;
        }
        case 0x3d6:
        {
            $a1_1 = &tisp_BCSH_au32CCMMatrix_d_wdr;
            $s1_1 = 0x24;
            break;
        }
        case 0x3d7:
        {
            $a1_1 = &tisp_BCSH_au32CCMMatrix_t_wdr;
            $s1_1 = 0x24;
            break;
        }
        case 0x3d8:
        {
            $a1_1 = &tisp_BCSH_au32CCMMatrix_a_wdr;
            $s1_1 = 0x24;
            break;
        }
        case 0x3d9:
        {
            $a1_1 = &tisp_BCSH_au32HDP_wdr;
            $s1_1 = 0xc;
            break;
        }
        case 0x3da:
        {
            $a1_1 = &tisp_BCSH_au32HBP_wdr;
            $s1_1 = 0xc;
            break;
        }
        case 0x3db:
        {
            $a1_1 = &tisp_BCSH_au32HLSP_wdr;
            $s1_1 = 0xc;
            break;
        }
        case 0x3dc:
        {
            $a1_1 = &tisp_BCSH_au32Sthres_wdr;
            $s1_1 = 0xc;
            break;
        }
        case 0x3dd:
        {
            $a1_1 = &tisp_BCSH_au32EvList_wdr;
            $s1_1 = 0x24;
            break;
        }
        case 0x3de:
        {
            $a1_1 = &tisp_BCSH_au32SminListS_wdr;
            $s1_1 = 0x24;
            break;
        }
        case 0x3df:
        {
            $a1_1 = &tisp_BCSH_au32SmaxListS_wdr;
            $s1_1 = 0x24;
            break;
        }
        case 0x3e0:
        {
            $a1_1 = &tisp_BCSH_au32SminListM_wdr;
            $s1_1 = 0x24;
            break;
        }
        case 0x3e1:
        {
            $a1_1 = &tisp_BCSH_au32SmaxListM_wdr;
            $s1_1 = 0x24;
            break;
        }
        case 0x3e2:
        {
            $a1_1 = &tisp_BCSH_au32OffsetRGB_wdr;
            $s1_1 = 0xc;
            break;
        }
        case 0x3e3:
        {
            $a1_1 = &MMatrix;
            $s1_1 = 0x24;
            break;
        }
        case 0x3e4:
        {
            $a1_1 = &MinvMatrix;
            $s1_1 = 0x24;
            break;
        }
        case 0x3e5:
        {
            $a1_1 = &tisp_BCSH_au32clip2;
            $s1_1 = 0x10;
            break;
        }
    }
    
    memcpy(arg2, $a1_1, $s1_1);
    *arg3 = $s1_1;
    return 0;
}

