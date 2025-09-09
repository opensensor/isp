#include "include/main.h"


  int32_t tisp_af_process_impl()

{
        char var_30[0x2c];
    IspAfStaticParam = &stAFParam_Zone;
    data_d6500 = &af_array_fird0;
    data_d6504 = &af_array_fird1;
    data_d6508 = &af_array_iird0;
    data_d650c = &y_sp_fl_sl_0_array;
    data_d6510 = &tisp_BCSH_u32HLSPslope;
    data_d6514 = &af_array_high_luma_cnt;
    data_d6518 = &AFWeight_Param;
    data_d651c = &AFParam_Fv;
    data_d6520 = &AFParam_FvWmean;
    data_d6524 = &AFParam_Tilt;
    data_d6528 = &AFParam_PointPos;
    
    for (int32_t i = 0; (uintptr_t)i < 0x28; i += 1)
    {
        var_30[i] = *(&data_d650c + i);
    }
    
    Tiziano_af_fpga(IspAfStaticParam, data_d6500_1, data_d6504_1, data_d6508_1);
    return 0;
}

