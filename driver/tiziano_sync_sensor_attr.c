#include "include/main.h"


  int32_t tiziano_sync_sensor_attr(int32_t* arg1)

{
    int32_t $a0 = arg1[7];
    uint32_t $v0_2 = tisp_math_exp2($a0, 0x10, 0xa);
    int32_t $a0_1 = arg1[8];
    uint32_t $v0_20 = tisp_math_exp2(arg1[0x15], 0x10, 0xa);
    int32_t $a0_3 = data_c46c0;
    uint32_t $a2 = data_b2e58;
    uint32_t $a3 = data_b2e48;
    uint32_t $a0_6 = data_b2e64;
    uint32_t $a1_2 = data_b2e62;
    int32_t $v1_1 = arg1[8];
    char $v1_2 = data_b2e5a;
    char $v1_3 = data_b2e5c;
    char $v1_4 = data_b2e5e;
    sensor_info = *arg1;
    data_b2e1c = arg1[1];
    data_b2e34 = $v0_2;
    data_b2e38 = tisp_math_exp2($a0_1, 0x10, 0xa);
    data_b2e44 = arg1[0xb];
    data_b2e48 = arg1[0xc];
    data_b2e62 = *(arg1 + 0x4a);
    data_b2e64 = arg1[0x13];
    data_b2e4a = *(arg1 + 0x32);
    data_b2e4c = arg1[0xd];
    data_b2e4e = *(arg1 + 0x36);
    data_b2e54 = arg1[0xf];
    data_b2e56 = *(arg1 + 0x3e);
    data_b2e58 = arg1[0x10];
    data_b2e5a = *(arg1 + 0x42);
    data_b2e5c = arg1[0x11];
    data_b2e5e = *(arg1 + 0x46);
    data_b2e60 = arg1[0x12];
    data_b2e62 = *(arg1 + 0x4a);
    data_b2e64 = arg1[0x13];
    data_b2e6c = $v0_20;
    
    if (!$a0_3 || $a0_3 == data_b2e34)
        data_c46c0 = data_b2e34;
    else
        data_c46c0 = $a0_3;
    
    data_c46c4 = data_b2e38;
    data_c46fc = $v0_20;
    data_c4700 = $a0_6;
    *dmsc_sp_d_ud_ns_opt = $a3;
    data_c4730 = $a1_2;
    data_c46c8 = $a2;
    data_b2e9c = arg1[7];
    data_b2ed0 = $a0_6;
    data_b2ea0 = $v1_1;
    data_b2ea4 = $a3;
    data_b2eb6 = $v1_2;
    data_b2ea8 = $a2;
    data_b2eb7 = $v1_3;
    data_b2ecc = $a1_2;
    data_b2eb8 = $v1_4;
    data_b2ed4 = arg1[0x15];
    return 0;
}

