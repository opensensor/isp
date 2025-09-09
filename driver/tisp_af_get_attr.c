#include "include/main.h"


  int32_t tisp_af_get_attr(uint32_t* arg1)

{
    *arg1 = data_d6540_2 >> (data_d6c6d_3 & 0x1f);
    arg1[1] = AFParam_Fv_Alt >> (data_d6c6d_4 & 0x1f);
    arg1[2] = AFParam_Fv >> (data_d6c6d_5 & 0x1f);
    arg1[3] = data_d653c_1 >> (data_d6c6d_6 & 0x1f);
    arg1[4] = AF_Enable;
    char $v1 = data_d6c6d_7;
    *(arg1 + 0x21) = data_b1358_2;
    *(arg1 + 0x22) = data_b135c_2;
    *(arg1 + 0x23) = data_b1360_2;
    char $a1_3 = data_b1364_2;
    *(arg1 + 0x11) = $v1;
    arg1[9] = $a1_3;
    *(arg1 + 0x12) = data_b11f8_2;
    arg1[5] = data_b11fc_2;
    *(arg1 + 0x16) = data_b1354_3;
    arg1[6] = AFParam_Tilt;
    *(arg1 + 0x1a) = data_b11f4_2;
    arg1[7] = data_b1380_3;
    *(arg1 + 0x1d) = stAFParam_Zone;
    *(arg1 + 0x1e) = data_b1384_6;
    *(arg1 + 0x1f) = data_b137c_5;
    arg1[8] = frame_num;
    *(arg1 + 0x26) = stAFParam_FIR0_Ldg;
    *(arg1 + 0x27) = data_b1314_2;
    arg1[0xa] = data_b1318_2;
    *(arg1 + 0x2a) = data_b131c_2;
    *(arg1 + 0x2b) = data_b1320_2;
    arg1[0xb] = data_b1324_2;
    *(arg1 + 0x2e) = data_b1328_2;
    arg1[0xc] = data_b132c_2;
    *(arg1 + 0x32) = stAFParam_FIR1_Ldg;
    *(arg1 + 0x33) = data_b12d0_2;
    arg1[0xd] = data_b12d4_2;
    *(arg1 + 0x36) = data_b12d8_2;
    *(arg1 + 0x37) = data_b12dc_2;
    arg1[0xe] = data_b12e0_2;
    *(arg1 + 0x3a) = data_b12e4_2;
    arg1[0xf] = data_b12e8_2;
    *(arg1 + 0x3e) = stAFParam_IIR0_Ldg;
    *(arg1 + 0x3f) = data_b1278_2;
    arg1[0x10] = data_b127c_2;
    *(arg1 + 0x42) = data_b1280_2;
    *(arg1 + 0x43) = data_b1284_2;
    arg1[0x11] = data_b1288_2;
    char $v0_16 = data_b1290_2;
    *(arg1 + 0x46) = data_b128c_2;
    arg1[0x12] = $v0_16;
    *(arg1 + 0x4a) = stAFParam_IIR1_Ldg;
    *(arg1 + 0x4b) = data_b1220_2;
    arg1[0x13] = data_b1224_2;
    *(arg1 + 0x4e) = data_b1228_2;
    *(arg1 + 0x4f) = data_b122c_2;
    arg1[0x14] = data_b1230_2;
    char $v0_17 = data_b1238_2;
    *(arg1 + 0x52) = data_b1234_2;
    arg1[0x15] = $v0_17;
    return 0;
}

