#include "include/main.h"


  void* vic_pipo_mdma_enable(void* arg1)

{
    int32_t $v1 = *(arg1 + 0xdc);
    *(*(arg1 + 0xb8) + 0x308) = 1;
    int32_t $v1_1 = $v1 << 1;
    *(*(arg1 + 0xb8) + 0x304) = *(arg1 + 0xdc) << 0x10 | *(arg1 + 0xe0);
    *(*(arg1 + 0xb8) + 0x310) = $v1_1;
    void* result = *(arg1 + 0xb8);
    *(result + 0x314) = $v1_1;
    return result;
}

