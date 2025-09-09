#include "include/main.h"


  int32_t tx_isp_video_link_destroy.isra.5(void* arg1)

{
    int32_t $v1_1 = *(arg1 + 0x118);
    int32_t result = 0;
        int32_t $s3_1 = *(($v1_1 << 3) + 0x7ad50);
        void* $s1_1 = (&configs)[$v1_1 * 2];
        int32_t $s2_1 = 0;
    
    if ($v1_1 >= 0)
    {
        
        while (true)
        {
            if ($s2_1 >= $s3_1)
            {
                *(((void**)((char*)arg1 + 0x118))) = 0xffffffff; // Fixed void pointer dereference
                return 0;
            }
            
            void* $v0_2 = find_subdev_link_pad(arg1, $s1_1);
            
            if ($v0_2 && find_subdev_link_pad(arg1, $s1_1 + 8))
            {
                int32_t $a2_1;
                int32_t $a3_1;
                $a2_1 = subdev_video_destroy_link($v0_2 + 8);
                result = $a2_1($a3_1 + 8);
                $s2_1 += 1;
                
                if (result)
                {
                    $s1_1 += 0x14;
                    
                    if ((uintptr_t)result != 0xfffffdfd)
                        break;
                    
                    continue;
                }
            }
            else
                $s2_1 += 1;
            
            $s1_1 += 0x14;
        }
    }
    
    return result;
}

