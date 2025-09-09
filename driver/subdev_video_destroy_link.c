#include "include/main.h"


  int32_t subdev_video_destroy_link(int32_t* arg1)

{
        char* $a1_1 = (char*)(*arg1); // Fixed void pointer assignment
        int32_t* $v0 = arg1[2];
        char* $v1_1 = (char*)(arg1[1]); // Fixed void pointer assignment
    if (arg1[3])
    {
        *arg1 = 0;
        arg1[1] = 0;
        arg1[2] = 0;
        arg1[3] = 0;
        *((int32_t*)((char*)$a1_1 + 7)) = 2; // Fixed void pointer dereference
        
        if ($v0)
        {
            *$v0 = 0;
            $v0[1] = 0;
            $v0[2] = 0;
            $v0[3] = 0;
        }
        
        if ($v1_1)
            *((int32_t*)((char*)$v1_1 + 7)) = 2; // Fixed void pointer dereference
    }
    
    return 0;
}

