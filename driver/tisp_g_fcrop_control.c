#include "include/main.h"


  int32_t tisp_g_fcrop_control(char* arg1)

{
    int32_t $v1 = data_b2e04;
        int32_t tispinfo_1 = tispinfo;
    int32_t result;
    
    if ($v1 != 1)
    {
        *arg1 = 0;
        *(((int32_t*)((char*)arg1 + 4))) = 0; // Fixed void pointer dereference
        result = data_b2f34;
        *(((int32_t*)((char*)arg1 + 8))) = 0; // Fixed void pointer dereference
        *(((void**)((char*)arg1 + 0xc))) = tispinfo_1; // Fixed void pointer dereference
    }
    else
    {
        *arg1 = $v1;
        *(((void**)((char*)arg1 + 4))) = data_b2e0c; // Fixed void pointer dereference
        *(((void**)((char*)arg1 + 8))) = data_b2e08; // Fixed void pointer dereference
        *(((void**)((char*)arg1 + 0xc))) = data_b2e10; // Fixed void pointer dereference
        result = data_b2e14;
    }
    
    *(((void**)((char*)arg1 + 0x10))) = result; // Fixed void pointer dereference
    return result;
}

