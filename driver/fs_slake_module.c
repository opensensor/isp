#include "include/main.h"


  int32_t fs_slake_module(void* arg1)

{
    int32_t result = 0xffffffea;
                char* $s1_2 = (char*)(i * 0x2ec + *(arg1 + 0xdc)); // Fixed void pointer assignment
    
    if (arg1)
    {
        if ((uintptr_t)arg1 >= 0xfffff001)
            return 0xffffffea;
        
        result = 0;
        
        if (*(arg1 + 0xe4) != 1)
        {
            for (int32_t i = 0; i < *(arg1 + 0xe0); i += 1)
            {
                
                if (*($s1_2 + 0x2d0) != 4)
                    *(((int32_t*)((char*)$s1_2 + 0x2d0))) = 1; // Fixed void pointer dereference
                else
                {
                    int32_t entry_$a2;
                    __frame_channel_vb2_streamoff($s1_2, *($s1_2 + 0x24), entry_$a2);
                    entry_$a2 = __vb2_queue_free($s1_2 + 0x24, *($s1_2 + 0x20c));
                    *(((int32_t*)((char*)$s1_2 + 0x2d0))) = 1; // Fixed void pointer dereference
                }
            }
            
            *(((int32_t*)((char*)arg1 + 0xe4))) = 1; // Fixed void pointer dereference
            return 0;
        }
    }
    
    return result;
}

