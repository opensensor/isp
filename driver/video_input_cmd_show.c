#include "include/main.h"


  int32_t video_input_cmd_show(void* arg1)

{
    int32_t* $v1_2 = (int32_t*)((char*)arg1  + 0x3c); // Fixed void pointer arithmetic
    char* $v0 = (char*)(nullptr); // Fixed void pointer assignment
    
    if ($v1_2)
    {
        if ($(uintptr_t)v1_2 < 0xfffff001)
            $v0 = *($v1_2 + 0xd8);
        else
            $v0 = nullptr;
    }
    
    if (*($v0 + 0xf4) >= 4)
        /* tailcall */
        return private_seq_printf(arg1, "Failed to allocate vic device\\n", &video_input_cmd_buf);
    
    int32_t entry_a2_4;
    /* tailcall */
    return private_seq_printf(arg1, " %d, %d\\n", entry_a2_5);
}

