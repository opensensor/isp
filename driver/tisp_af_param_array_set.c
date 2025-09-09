#include "include/main.h"


  int32_t tisp_af_param_array_set(int32_t arg1, int32_t arg2, int32_t* arg3)

{
        int32_t var_18_1 = arg1;
    if (arg1 - (uintptr_t)0x3ad >= 0x13)
    {
        isp_printf(); // Fixed: macro call, removed arguments;
        return 0xffffffff;
    }
    
    int32_t* $a0;
    int32_t $s1_1;
    
    switch (arg1)
    {
        case 0x3ad:
        {
            $a0 = &stAFParam_Zone;
            $s1_1 = 0x90;
            break;
        }
        case 0x3ae:
        {
            $a0 = &stAFParam_ThresEnable;
            $s1_1 = 0x34;
            break;
        }
        case 0x3af:
        {
            $a0 = &stAFParam_FIR0_V;
            $s1_1 = 0x14;
            break;
        }
        case 0x3b0:
        {
            $a0 = &stAFParam_FIR0_Ldg;
            $s1_1 = 0x20;
            break;
        }
        case 0x3b1:
        {
            $a0 = &stAFParam_FIR0_Coring;
            $s1_1 = 0x10;
            break;
        }
        case 0x3b2:
        {
            $a0 = &stAFParam_FIR1_V;
            $s1_1 = 0x14;
            break;
        }
        case 0x3b3:
        {
            $a0 = &stAFParam_FIR1_Ldg;
            $s1_1 = 0x20;
            break;
        }
        case 0x3b4:
        {
            $a0 = &stAFParam_FIR1_Coring;
            $s1_1 = 0x10;
            break;
        }
        case 0x3b5:
        {
            $a0 = &stAFParam_IIR0_H;
            $s1_1 = 0x28;
            break;
        }
        case 0x3b6:
        {
            $a0 = &stAFParam_IIR0_Ldg;
            $s1_1 = 0x20;
            break;
        }
        case 0x3b7:
        {
            $a0 = &stAFParam_IIR0_Coring;
            $s1_1 = 0x10;
            break;
        }
        case 0x3b8:
        {
            $a0 = &stAFParam_IIR1_H;
            $s1_1 = 0x28;
            break;
        }
        case 0x3b9:
        {
            $a0 = &stAFParam_IIR1_Ldg;
            $s1_1 = 0x20;
            break;
        }
        case 0x3ba:
        {
            $a0 = &stAFParam_IIR1_Coring;
            $s1_1 = 0x10;
            break;
        }
        case 0x3bb:
        {
            $a0 = &AFParam_PointPos;
            $s1_1 = 8;
            break;
        }
        case 0x3bc:
        {
            $a0 = &AFParam_Tilt;
            $s1_1 = 0x14;
            break;
        }
        case 0x3bd:
        {
            $a0 = &AFParam_FvWmean;
            $s1_1 = 0x3c;
            break;
        }
        case 0x3be:
        {
            $a0 = &AFParam_Fv;
            $s1_1 = 0xc;
            break;
        }
        case 0x3bf:
        {
            $a0 = &AFWeight_Param;
            $s1_1 = 0x384;
            break;
        }
    }
    
    memcpy($a0, arg2, $s1_1);
    *arg3 = $s1_1;
    tiziano_af_set_hardware_param();
    return 0;
}

