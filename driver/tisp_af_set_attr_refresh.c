#include "include/main.h"


  int32_t tisp_af_set_attr_refresh()

{
    uint32_t $a2 = data_d6c6e;
    uint32_t $a1 = data_d6c78;
    uint32_t $v0 = data_d6cb0;
    AF_Enable = data_d6c6c;
    data_b1358 = data_d6c7d;
    data_b135c = data_d6c7e;
    data_b1360 = data_d6c7f;
    data_b1364 = data_d6c80;
    data_b11f8 = $a2;
    data_b11fc = data_d6c70;
    data_b1354 = data_d6c72;
    AFParam_Tilt = data_d6c74;
    data_b11f4 = data_d6c76;
    data_b1380 = $a1;
    stAFParam_Zone = data_d6c79;
    data_b1384 = data_d6c7a;
    data_b137c = data_d6c7b;
    stAFParam_FIR0_Ldg = data_d6c82;
    data_b1314 = data_d6c83;
    data_b1318 = data_d6c84;
    data_b131c = data_d6c86;
    data_b1320 = data_d6c87;
    data_b1324 = data_d6c88;
    data_b1328 = data_d6c8a;
    data_b132c = data_d6c8c;
    stAFParam_FIR1_Ldg = data_d6c8e;
    data_b12d0 = data_d6c8f;
    data_b12d4 = data_d6c90;
    data_b12d8 = data_d6c92;
    data_b12dc = data_d6c93;
    data_b12e0 = data_d6c94;
    data_b12e4 = data_d6c96;
    data_b12e8 = data_d6c98;
    stAFParam_IIR0_Ldg = data_d6c9a;
    data_b1278 = data_d6c9b;
    data_b127c = data_d6c9c;
    data_b1280 = data_d6c9e;
    data_b1284 = data_d6c9f;
    data_b1288 = data_d6ca0;
    data_b128c = data_d6ca2;
    data_b1290 = data_d6ca4;
    stAFParam_IIR1_Ldg = data_d6ca6;
    data_b1220 = data_d6ca7;
    data_b1224 = data_d6ca8;
    data_b1228 = data_d6caa;
    data_b122c = data_d6cab;
    data_b1230 = data_d6cac;
    data_b1234 = data_d6cae;
    data_b1238 = $v0;
    /* tailcall */
    return tiziano_af_set_hardware_param();
}

