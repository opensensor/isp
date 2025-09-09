#include "include/main.h"


  int32_t isp_ch1_frame_dequeue_delay()

{
    void* $s0 = *(mdns_y_pspa_cur_bi_wei0_array + 0xd4);
    uint32_t isp_ch1_dequeue_delay_time_1 = isp_ch1_dequeue_delay_time;
    void var_10_78;
    private_ktime_set(&var_10_79, isp_ch1_dequeue_delay_time_1 / 0x3e8, 
        isp_ch1_dequeue_delay_time_1 * 0xf4240);
    private_set_current_state(2);
    private_schedule_hrtimeout(&var_10_80, 1);
    return tx_isp_send_event_to_remote(*(*($s0 + 0x150) + 0x13c), 0x3000006, &ch1_buf);
}

