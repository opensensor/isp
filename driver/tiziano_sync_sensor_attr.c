#include "include/main.h"


  int32_t tiziano_sync_sensor_attr(int32_t* arg1)

{
    sensor_info = *arg1;
    int32_t $a0 = arg1[7];
    data_b2e1c_1 = arg1[1];
    uint32_t $v0_2 = tisp_math_exp2($a0, 0x10, 0xa);
    int32_t $a0_1 = arg1[8];
    data_b2e34_1 = $v0_2;
    data_b2e38_1 = tisp_math_exp2($a0_1, 0x10, 0xa);
    data_b2e44_1 = arg1[0xb];
    data_b2e48_2 = arg1[0xc];
    data_b2e62_2 = *(arg1 + 0x4a);
    data_b2e64_2 = arg1[0x13];
    data_b2e4a_1 = *(arg1 + 0x32);
    data_b2e4c_1 = arg1[0xd];
    data_b2e4e_1 = *(arg1 + 0x36);
    data_b2e54_2 = arg1[0xf];
    data_b2e56_2 = *(arg1 + 0x3e);
    data_b2e58_2 = arg1[0x10];
    data_b2e5a_1 = *(arg1 + 0x42);
    data_b2e5c_1 = arg1[0x11];
    data_b2e5e_1 = *(arg1 + 0x46);
    data_b2e60_1 = arg1[0x12];
    data_b2e62_3 = *(arg1 + 0x4a);
    data_b2e64_3 = arg1[0x13];
    uint32_t $v0_20 = tisp_math_exp2(arg1[0x15], 0x10, 0xa);
    int32_t $a0_3 = data_c46c0_1;
    data_b2e6c_1 = $v0_20;
    
    if (!$a0_3 || $a0_3 == data_b2e34_2)
        data_c46c0_2 = data_b2e34_3;
    else
        data_c46c0_3 = $a0_3;
    
    uint32_t $a2 = data_b2e58_3;
    data_c46c4_1 = data_b2e38_2;
    uint32_t $a3 = data_b2e48_3;
    uint32_t $a0_6 = data_b2e64_4;
    uint32_t $a1_2 = data_b2e62_4;
    data_c46fc_1 = $v0_20;
    data_c4700_1 = $a0_6;
    *dmsc_sp_d_ud_ns_opt = $a3;
    data_c4730_1 = $a1_2;
    data_c46c8_1 = $a2;
    data_b2e9c_1 = arg1[7];
    int32_t $v1_1 = arg1[8];
    data_b2ed0_1 = $a0_6;
    data_b2ea0_1 = $v1_1;
    char $v1_2 = data_b2e5a_2;
    data_b2ea4_1 = $a3;
    data_b2eb6_1 = $v1_2;
    char $v1_3 = data_b2e5c_2;
    data_b2ea8_1 = $a2;
    data_b2eb7_1 = $v1_3;
    char $v1_4 = data_b2e5e_2;
    data_b2ecc_1 = $a1_2;
    data_b2eb8_1 = $v1_4;
    data_b2ed4_1 = arg1[0x15];
    return 0;
}

