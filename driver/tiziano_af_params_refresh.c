#include "include/main.h"


  int32_t tiziano_af_params_refresh()

{
    memcpy(&stAFParam_Zone, 0xa675c, 0x90);
    memcpy(&stAFParam_ThresEnable, 0xa67ec, 0x34);
    memcpy(&stAFParam_FIR0_V, 0xa6820, 0x14);
    memcpy(&stAFParam_FIR0_Ldg, 0xa6834, 0x20);
    memcpy(&stAFParam_FIR0_Coring, 0xa6854, 0x10);
    memcpy(&stAFParam_FIR1_V, 0xa6864, 0x14);
    memcpy(&stAFParam_FIR1_Ldg, 0xa6878, 0x20);
    memcpy(&stAFParam_FIR1_Coring, 0xa6898, 0x10);
    memcpy(&stAFParam_IIR0_H, 0xa68a8, 0x28);
    memcpy(&stAFParam_IIR0_Ldg, 0xa68d0, 0x20);
    memcpy(&stAFParam_IIR0_Coring, 0xa68f0, 0x10);
    memcpy(&stAFParam_IIR1_H, 0xa6900, 0x28);
    memcpy(&stAFParam_IIR1_Ldg, 0xa6928, 0x20);
    memcpy(&stAFParam_IIR1_Coring, 0xa6948, 0x10);
    memcpy(&AFParam_PointPos, 0xa6958, 8);
    memcpy(&AFParam_Tilt, 0xa6960, 0x14);
    memcpy(&AFParam_FvWmean, 0xa6974, 0x3c);
    memcpy(&AFParam_Fv, 0xa69b0, 0xc);
    memcpy(&AFWeight_Param, 0xa69bc, 0x384);
    return 0;
}

