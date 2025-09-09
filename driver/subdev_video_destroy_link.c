#include "include/main.h"


  int32_t subdev_video_destroy_link(int32_t* arg1)

{
    if (arg1[3])
    {
        void* $a1_1 = *arg1;
        int32_t* $v0 = arg1[2];
        void* $v1_1 = arg1[1];
        *arg1 = 0;
        arg1[1] = 0;
        arg1[2] = 0;
        arg1[3] = 0;
        *($a1_1 + 7) = 2;
        
        if ($v0)
        {
            *$v0 = 0;
            $v0[1] = 0;
            $v0[2] = 0;
            $v0[3] = 0;
        }
        
        if ($v1_1)
            *($v1_1 + 7) = 2;
    }
    
    return 0;
}

