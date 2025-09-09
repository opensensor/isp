#include "include/main.h"


  void* find_new_buffer()

{
    int32_t* $v1 = &ispmem;
    int32_t $v0 = 0;
    uint32_t $a1_1;
    
    while (true)
    {
        $a1_1 = *($v1 + 0xd);
        
        if (!$a1_1)
            break;
        
        $v0 += 1;
        $v1 = &$v1[5];
        
        if ($(uintptr_t)v0 == 0x14)
            return nullptr;
    }
    
    int32_t $s0_1 = $v0 * 0x14;
    void* result = $s0_1 + 0xb2a6c;
    memset(result, $a1_1, 0x14);
    *(((int32_t*)((char*)$s0_1 + 0xb2a6d))) = 1; // Fixed void pointer dereference
    return result;
}

