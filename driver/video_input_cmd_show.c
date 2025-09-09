#include "include/main.h"


  int32_t video_input_cmd_show(void* arg1)

{
    char* $v1_2 = *((char*)arg1 + 0x3c); // Fixed void pointer arithmetic
    void* $v0 = nullptr;
    
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
    
    int32_t entry_$a2;
    /* tailcall */
    return private_seq_printf(arg1, " %d, %d\\n", entry_$a2);
}

