#include "include/main.h"


  void* tisp_ae1_get_hist(int32_t* arg1)

{
    void* result = &IspAeStatic;
        int32_t $a1_1 = *arg1;
    
    do
    {
        arg1 = &arg1[2];
        *(((void**)((char*)result + 0x2eac))) = $a1_1 & 0x1fffff; // Fixed void pointer dereference
        result += 8;
        *(((void**)((char*)result + 0x2ea8))) = *(arg1 - 4) & 0x1fffff; // Fixed void pointer dereference
    } while (arg1 != &arg1[0x100]);
    
    return result;
}

