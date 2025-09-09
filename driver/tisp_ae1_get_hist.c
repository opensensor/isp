#include "include/main.h"


  void* tisp_ae1_get_hist(int32_t* arg1)

{
    char* result = (char*)(&IspAeStatic); // Fixed void pointer assignment
        int32_t $a1_1 = *arg1;
    return result;
    
    do
    {
        arg1 = &arg1[2];
        *((int32_t*)((char*)result + 0x2eac)) = $a1_1 & 0x1fffff; // Fixed void pointer dereference
        result += 8;
        *((int32_t*)((char*)result + 0x2ea8)) = *(arg1 - 4) & 0x1fffff; // Fixed void pointer dereference
    } while (arg1 != &arg1[0x100]);
    
}

