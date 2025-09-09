#include "include/main.h"


  int32_t tisp_get_awb_info(void* arg1)

{
    void var_28;
    tisp_g_wb_attr(&var_28);
    *(((void**)((char*)arg1 + 4))) = 0x1c; // Fixed void pointer dereference
    memcpy(arg1 + 0xc, &var_28, 0x1c);
    return 0;
}

