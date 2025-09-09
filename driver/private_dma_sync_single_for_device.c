#include "include/main.h"


  void* private_dma_sync_single_for_device(void* arg1)

{
    void* result;
    
    if (arg1)
        result = *(arg1 + 0x80);
    
    if (!arg1 || !result)
        result = nullptr;
    
    int32_t $t9 = *(result + 0x24);
    
    if (!$t9)
        return result;
    
❓    /* jump -> $t9 */
}

