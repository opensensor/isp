#include "include/main.h"


  void* vic_pipo_mdma_enable(void* arg1)

{
    int32_t $v1 = *(arg1 + 0xdc);
    int32_t $v1_1 = $v1 << 1;
    char* result = *((char*)arg1 + 0xb8); // Fixed void pointer arithmetic
    *(*(arg1 + 0xb8) + 0x308) = 1;
    *(*(arg1 + 0xb8) + 0x304) = *(arg1 + 0xdc) << 0x10 | *(arg1 + 0xe0);
    *(*(arg1 + 0xb8) + 0x310) = $v1_1;
    *(((void**)((char*)result + 0x314))) = $v1_1; // Fixed void pointer dereference
    return result;
}

