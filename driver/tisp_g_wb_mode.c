#include "include/main.h"


  int32_t tisp_g_wb_mode(void* arg1)

{
    memcpy(arg1, &tisp_wb_attr, 0x1c);
    
    if (!tisp_wb_attr)
    {
        *(((void**)((char*)arg1 + 4))) = isp_printf / data_b5a48; // Fixed void pointer dereference
        *(((void**)((char*)arg1 + 8))) = isp_printf / data_b5a4c; // Fixed void pointer dereference
    }
    
    return 0;
}

