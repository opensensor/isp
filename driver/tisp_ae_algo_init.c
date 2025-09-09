#include "include/main.h"


  int32_t tisp_ae_algo_init(uint32_t arg1, void* arg2)

{
    void* $v0 = private_kmalloc(0x42c, 0xd0);
        int32_t $a0 = data_c46bc;
        int32_t dmsc_uu_thres_wdr_array_1 = dmsc_uu_thres_wdr_array;
        int32_t dmsc_awb_gain_1 = dmsc_awb_gain;
        int32_t $a0_2 = data_c46c0;
        int32_t $a0_8 = data_c471c;
        int32_t $a0_12 = data_c4730;
    ta_custom_en = arg1;
    
    if (arg1 == 1)
    {
        *(((int32_t*)((char*)arg2 + 8))) = 0; // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0xc))) = data_c46b8; // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x10))) = data_c46b0; // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x14))) = 0x400; // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x18))) = $a0; // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x3c))) = data_c46f8; // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x44))) = 0x400; // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x40))) = dmsc_uu_thres_wdr_array_1; // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x34))) = 0x400; // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x48))) = dmsc_awb_gain_1; // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x64))) = 0x400; // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x30))) = $a0_2; // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x2c))) = data_c46c8; // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x38))) = data_c46cc; // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x60))) = data_c46fc; // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x5c))) = data_c4700; // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x68))) = data_c4708; // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x24))) = 0x400; // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x20))) = $a0_8; // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x1c))) = *dmsc_sp_d_ud_ns_opt; // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x28))) = *(dmsc_sp_d_ud_ns_opt + 4); // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x50))) = dmsc_sp_ud_ns_thres_array; // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x54))) = 0x400; // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x4c))) = $a0_12; // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x58))) = data_c4738; // Fixed void pointer dereference
        tisp_ae_get_hist_custome($v0);
        *(((void**)((char*)arg2 + 0x70))) = *($v0 + 0x414); // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x71))) = *($v0 + 0x418); // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x72))) = *($v0 + 0x41c); // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x73))) = *($v0 + 0x420); // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x74))) = *($v0 + 0x400); // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x76))) = *($v0 + 0x404); // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x78))) = *($v0 + 0x408); // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x7a))) = *($v0 + 0x40c); // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x7c))) = *($v0 + 0x410); // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x7e))) = *($v0 + 0x424); // Fixed void pointer dereference
        *(((void**)((char*)arg2 + 0x7f))) = *($v0 + 0x428); // Fixed void pointer dereference
    }
    
    /* tailcall */
    return private_kfree($v0);
}

