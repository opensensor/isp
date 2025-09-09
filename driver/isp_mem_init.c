#include "include/main.h"


  void* isp_mem_init()

{
    void* $v0 = find_new_buffer();
    int32_t ispmem_1 = ispmem;
    void* result = data_b2bfc;
    memset(&ispmem, 0, 0x1ac);
    private_get_isp_priv_mem(&ispmem, &data_b2a64);
    private_raw_mutex_init(0xb2c00, &$LC0, 0);
    data_b2bfc = $v0;
    *$v0 = 0;
    *(((int32_t*)((char*)data_b2bfc + 4))) = 0; // Fixed void pointer dereference
    *(((int32_t*)((char*)data_b2bfc + 8))) = 0; // Fixed void pointer dereference
    *(((void**)((char*)data_b2bfc + 0xc))) = ispmem_1; // Fixed void pointer dereference
    *(((void**)((char*)result + 0x10))) = data_b2a64; // Fixed void pointer dereference
    return result;
}

