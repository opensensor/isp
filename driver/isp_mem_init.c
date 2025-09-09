#include "include/main.h"


  void* isp_mem_init()

{
    memset(&ispmem, 0, 0x1ac);
    private_get_isp_priv_mem(&ispmem, &data_b2a64_1);
    private_raw_mutex_init(0xb2c00, &$LC0, 0);
    void* $v0 = find_new_buffer();
    data_b2bfc_1 = $v0;
    *$v0 = 0;
    int32_t ispmem_1 = ispmem;
    *(data_b2bfc_2 + 4) = 0;
    *(data_b2bfc_3 + 8) = 0;
    *(data_b2bfc_4 + 0xc) = ispmem_1;
    void* result = data_b2bfc_5;
    *(result + 0x10) = data_b2a64_2;
    return result;
}

