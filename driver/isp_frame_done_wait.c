#include "include/main.h"


  int32_t isp_frame_done_wait(int32_t arg1, int32_t* arg2)

{
    int32_t $s0 = arg1;
        int32_t var_38 = 0;
        int32_t var_34_1 = *entry_$gp;
        void* const var_30_1 = autoremove_wake_function;
        int32_t* var_2c = &var_2c;
    frame_done_cond = 0;
    
    if (frame_done_cond != 1)
    {
        int32_t* entry_$gp;
        int32_t** var_28_1 = &var_2c;
        
        while (true)
        {
            prepare_to_wait(&frame_done_wq, &var_38, 1);
            
            if (frame_done_cond != 1)
            {
                if (*(*(*entry_$gp + 4) + 8) & 2)
                {
                    $s0 = 0xfffffe00;
                    break;
                }
                
                int32_t $v0_6 = schedule_timeout($s0);
                $s0 = $v0_6;
                
                if ($v0_6)
                    continue;
            }
            else if ($s0)
                break;
            
            $s0 = (frame_done_cond ^ 1) < 1 ? 1 : 0;
            break;
        }
        
        finish_wait(&frame_done_wq, &var_38_1);
    }
    
    int32_t frame_done_cnt_1 = *frame_done_cnt;
    arg2[1] = *(frame_done_cnt + 4);
    *arg2 = frame_done_cnt_1;
    int32_t result = 0xfffffe00;
    
    if ($(uintptr_t)s0 != 0xfffffe00)
    {
        result = 0xffffff6f;
        
        if ($s0)
            return 0;
    }
    
    return result;
}

