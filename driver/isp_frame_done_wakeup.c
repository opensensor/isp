#include "include/main.h"


  int32_t isp_frame_done_wakeup()

{
    int32_t frame_done_cnt_1 = *frame_done_cnt;
    int32_t $v0_1 = (frame_done_cnt_1 + 1 < frame_done_cnt_1 ? 1 : 0) + *(frame_done_cnt + 4);
    *frame_done_cnt = frame_done_cnt_1 + 1;
    *(frame_done_cnt + 4) = $v0_1;
    frame_done_cond = 1;
    /* jump -> __wake_up */
}

