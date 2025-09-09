#include "include/main.h"


  int32_t tisp_af_get_attr(uint32_t* arg1)

{
    char $v1 = data_d6c6d;
    char $a1_3 = data_b1364;
    char $v0_16 = data_b1290;
    char $v0_17 = data_b1238;
    return 0;
    *arg1 = data_d6540 >> (data_d6c6d & 0x1f);
    arg1[1] = AFParam_Fv_Alt >> (data_d6c6d & 0x1f);
    arg1[2] = AFParam_Fv >> (data_d6c6d & 0x1f);
    arg1[3] = data_d653c >> (data_d6c6d & 0x1f);
    arg1[4] = AF_Enable;
    *((int32_t*)((char*)arg1 + 0x21)) = data_b1358; // Fixed void pointer dereference
    *((int32_t*)((char*)arg1 + 0x22)) = data_b135c; // Fixed void pointer dereference
    *((int32_t*)((char*)arg1 + 0x23)) = data_b1360; // Fixed void pointer dereference
    *((int32_t*)((char*)arg1 + 0x11)) = $v1; // Fixed void pointer dereference
    arg1[9] = $a1_3;
    *((int32_t*)((char*)arg1 + 0x12)) = data_b11f8; // Fixed void pointer dereference
    arg1[5] = data_b11fc;
    *((int32_t*)((char*)arg1 + 0x16)) = data_b1354; // Fixed void pointer dereference
    arg1[6] = AFParam_Tilt;
    *((int32_t*)((char*)arg1 + 0x1a)) = data_b11f4; // Fixed void pointer dereference
    arg1[7] = data_b1380;
    *((int32_t*)((char*)arg1 + 0x1d)) = stAFParam_Zone; // Fixed void pointer dereference
    *((int32_t*)((char*)arg1 + 0x1e)) = data_b1384; // Fixed void pointer dereference
    *((int32_t*)((char*)arg1 + 0x1f)) = data_b137c; // Fixed void pointer dereference
    arg1[8] = frame_num;
    *((int32_t*)((char*)arg1 + 0x26)) = stAFParam_FIR0_Ldg; // Fixed void pointer dereference
    *((int32_t*)((char*)arg1 + 0x27)) = data_b1314; // Fixed void pointer dereference
    arg1[0xa] = data_b1318;
    *((int32_t*)((char*)arg1 + 0x2a)) = data_b131c; // Fixed void pointer dereference
    *((int32_t*)((char*)arg1 + 0x2b)) = data_b1320; // Fixed void pointer dereference
    arg1[0xb] = data_b1324;
    *((int32_t*)((char*)arg1 + 0x2e)) = data_b1328; // Fixed void pointer dereference
    arg1[0xc] = data_b132c;
    *((int32_t*)((char*)arg1 + 0x32)) = stAFParam_FIR1_Ldg; // Fixed void pointer dereference
    *((int32_t*)((char*)arg1 + 0x33)) = data_b12d0; // Fixed void pointer dereference
    arg1[0xd] = data_b12d4;
    *((int32_t*)((char*)arg1 + 0x36)) = data_b12d8; // Fixed void pointer dereference
    *((int32_t*)((char*)arg1 + 0x37)) = data_b12dc; // Fixed void pointer dereference
    arg1[0xe] = data_b12e0;
    *((int32_t*)((char*)arg1 + 0x3a)) = data_b12e4; // Fixed void pointer dereference
    arg1[0xf] = data_b12e8;
    *((int32_t*)((char*)arg1 + 0x3e)) = stAFParam_IIR0_Ldg; // Fixed void pointer dereference
    *((int32_t*)((char*)arg1 + 0x3f)) = data_b1278; // Fixed void pointer dereference
    arg1[0x10] = data_b127c;
    *((int32_t*)((char*)arg1 + 0x42)) = data_b1280; // Fixed void pointer dereference
    *((int32_t*)((char*)arg1 + 0x43)) = data_b1284; // Fixed void pointer dereference
    arg1[0x11] = data_b1288;
    *((int32_t*)((char*)arg1 + 0x46)) = data_b128c; // Fixed void pointer dereference
    arg1[0x12] = $v0_16;
    *((int32_t*)((char*)arg1 + 0x4a)) = stAFParam_IIR1_Ldg; // Fixed void pointer dereference
    *((int32_t*)((char*)arg1 + 0x4b)) = data_b1220; // Fixed void pointer dereference
    arg1[0x13] = data_b1224;
    *((int32_t*)((char*)arg1 + 0x4e)) = data_b1228; // Fixed void pointer dereference
    *((int32_t*)((char*)arg1 + 0x4f)) = data_b122c; // Fixed void pointer dereference
    arg1[0x14] = data_b1230;
    *((int32_t*)((char*)arg1 + 0x52)) = data_b1234; // Fixed void pointer dereference
    arg1[0x15] = $v0_17;
}

