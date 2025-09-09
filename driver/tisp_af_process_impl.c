#include "include/main.h"


  int32_t tisp_af_process_impl()

{
    IspAfStaticParam = &stAFParam_Zone;
    data_d6500_1 = &af_array_fird0;
    data_d6504_1 = &af_array_fird1;
    data_d6508_1 = &af_array_iird0;
    data_d650c_1 = &y_sp_fl_sl_0_array;
    data_d6510_1 = &tisp_BCSH_u32HLSPslope;
    data_d6514_1 = &af_array_high_luma_cnt;
    data_d6518_1 = &AFWeight_Param;
    data_d651c_1 = &AFParam_Fv;
    data_d6520_1 = &AFParam_FvWmean;
    data_d6524_1 = &AFParam_Tilt;
    data_d6528_1 = &AFParam_PointPos;
    
    for (int32_t i = 0; i < 0x28; i += 1)
    {
        char var_30_24[0x2c];
        var_30_25[i] = *(&data_d650c_2 + i);
    }
    
    Tiziano_af_fpga(IspAfStaticParam, data_d6500_2, data_d6504_2, data_d6508_2);
    return 0;
}

