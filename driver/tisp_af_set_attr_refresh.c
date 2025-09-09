#include "include/main.h"


  int32_t tisp_af_set_attr_refresh()

{
    AF_Enable = data_d6c6c_1;
    data_b1358_3 = data_d6c7d_1;
    uint32_t $a2 = data_d6c6e_2;
    data_b135c_3 = data_d6c7e_1;
    data_b1360_3 = data_d6c7f_1;
    data_b1364_3 = data_d6c80_1;
    data_b11f8_3 = $a2;
    data_b11fc_3 = data_d6c70_2;
    data_b1354_4 = data_d6c72_2;
    AFParam_Tilt = data_d6c74_2;
    uint32_t $a1 = data_d6c78_2;
    data_b11f4_3 = data_d6c76_2;
    data_b1380_4 = $a1;
    stAFParam_Zone = data_d6c79_2;
    data_b1384_7 = data_d6c7a_2;
    data_b137c_6 = data_d6c7b_2;
    stAFParam_FIR0_Ldg = data_d6c82_1;
    data_b1314_3 = data_d6c83_1;
    data_b1318_3 = data_d6c84_1;
    data_b131c_3 = data_d6c86_1;
    data_b1320_3 = data_d6c87_1;
    data_b1324_3 = data_d6c88_1;
    data_b1328_3 = data_d6c8a_1;
    data_b132c_3 = data_d6c8c_1;
    stAFParam_FIR1_Ldg = data_d6c8e_1;
    data_b12d0_3 = data_d6c8f_1;
    data_b12d4_3 = data_d6c90_1;
    data_b12d8_3 = data_d6c92_1;
    data_b12dc_3 = data_d6c93_1;
    data_b12e0_3 = data_d6c94_1;
    data_b12e4_3 = data_d6c96_1;
    data_b12e8_3 = data_d6c98_1;
    stAFParam_IIR0_Ldg = data_d6c9a_1;
    data_b1278_3 = data_d6c9b_1;
    data_b127c_3 = data_d6c9c_1;
    data_b1280_3 = data_d6c9e_1;
    data_b1284_3 = data_d6c9f_1;
    data_b1288_3 = data_d6ca0_1;
    data_b128c_3 = data_d6ca2_1;
    data_b1290_3 = data_d6ca4_1;
    stAFParam_IIR1_Ldg = data_d6ca6_1;
    data_b1220_3 = data_d6ca7_1;
    data_b1224_3 = data_d6ca8_1;
    data_b1228_3 = data_d6caa_1;
    data_b122c_3 = data_d6cab_1;
    data_b1230_3 = data_d6cac_1;
    uint32_t $v0 = data_d6cb0_1;
    data_b1234_3 = data_d6cae_1;
    data_b1238_3 = $v0;
    /* tailcall */
    return tiziano_af_set_hardware_param();
}

