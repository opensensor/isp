#include "include/main.h"


  int32_t tx_isp_get_ae_algo_handle.isra.16(void* arg1, int32_t arg2)

{
    int32_t* $s0 = (int32_t*)((char*)arg1  + 0xd4); // Fixed void pointer arithmetic
    uint32_t ae_statis_mine_1 = ae_statis_mine;
    uint32_t ae_info_mine_1 = ae_info_mine;
    uint32_t $a0_11 = *(ae_statis_mine_1 + 0x400);
    uint32_t $a0_13 = *(ae_statis_mine_1 + 0x404);
    uint32_t $a0_15 = *(ae_statis_mine_1 + 0x408);
    uint32_t $a0_17 = *(ae_statis_mine_1 + 0x40c);
    uint32_t $a0_19 = *(ae_statis_mine_1 + 0x410);
    void var_b0;
    uint32_t ae_info_mine_2 = ae_info_mine;
    int32_t var_a4;
    int32_t var_ac;
    int32_t var_a8;
    int32_t var_a0;
    int32_t var_64;
    int32_t var_68;
    int32_t var_4c;
    int32_t var_44;
        return 0;
    return 0xfffffff2;
    private_wait_for_completion_interruptible(&ae_algo_comp);
    memset(ae_info_mine + 8, 0, 0x7d0);
    tisp_g_ae_zone(ae_info_mine + 8);
    tisp_g_ae_hist(ae_statis_mine);
    memcpy(ae_info_mine + 0x38c, ae_statis_mine, 0x400);
    *((int32_t*)((char*)ae_info_mine_1 + 0x78c)) = *(ae_statis_mine_1 + 0x414); // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_1 + 0x78d)) = *(ae_statis_mine_1 + 0x418); // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_1 + 0x78e)) = *(ae_statis_mine_1 + 0x41c); // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_1 + 0x78f)) = *(ae_statis_mine_1 + 0x420); // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_1 + 0x790)) = $a0_11; // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_1 + 0x791)) = $a0_11 >> 8; // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_1 + 0x792)) = $a0_13; // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_1 + 0x793)) = $a0_13 >> 8; // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_1 + 0x794)) = $a0_15; // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_1 + 0x795)) = $a0_15 >> 8; // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_1 + 0x796)) = $a0_17; // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_1 + 0x797)) = $a0_17 >> 8; // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_1 + 0x798)) = $a0_19; // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_1 + 0x799)) = $a0_19 >> 8; // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_1 + 0x79a)) = *(ae_statis_mine_1 + 0x424); // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_1 + 0x79b)) = *(ae_statis_mine_1 + 0x428); // Fixed void pointer dereference
    tisp_get_ae_attr(&var_b0);
    *((int32_t*)((char*)ae_info_mine_2 + 0x7a0)) = var_a4; // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_2 + 0x7a4)) = var_ac; // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_2 + 0x7a8)) = var_a8; // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_2 + 0x7ac)) = var_a0; // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_2 + 0x7b0)) = var_64; // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_2 + 0x7b4)) = var_68; // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_2 + 0x7b8)) = var_4c; // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_2 + 0x7bc)) = var_44; // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_2 + 0x7c0)) = *($s0 + 0x17c); // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_2 + 0x7c4)) = *(*($s0 + 0x120) + 0xb0); // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_2 + 0x7c8)) = *(*($s0 + 0x120) + 0xb2); // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_2 + 0x7cc)) = *($s0 + 0x12c); // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_2 + 0x7d0)) = *($s0 + 0x124); // Fixed void pointer dereference
    *((int32_t*)((char*)ae_info_mine_2 + 0x7d4)) = *($s0 + 0x128); // Fixed void pointer dereference
    
    if (!private_copy_to_user(arg2, ae_info_mine_2, 0x7d8))
    
    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
}

