#include "include/main.h"


  int32_t isp_pre_frame_dequeue()

{
    char* $s0 = *((char*)mdns_y_pspa_cur_bi_wei0_array + 0xd4); // Fixed void pointer arithmetic
    uint32_t isp_ch0_pre_dequeue_time_1 = isp_ch0_pre_dequeue_time;
    int32_t var_24 = 1;
    uint32_t var_20 =
    int32_t var_30 = 0;
    int32_t var_2c = 0;
    int32_t var_28 = 0;
    void var_18;
    private_ktime_set(&var_18, isp_ch0_pre_dequeue_time_1 / 0x3e8, 
        isp_ch0_pre_dequeue_time_1 % 0x3e8 * 0xf4240);
    private_set_current_state(2);
    private_schedule_hrtimeout(&var_18, 1);
        *($s0 + 0x128) << 0x10 | *(*(mdns_y_pspa_cur_bi_wei0_array + 0xb8) + 0x9888) >> 0x10;
    void var_38;
    return tx_isp_send_event_to_remote(*(*($s0 + 0x150) + 0x78), 0x3000006, &var_38);
}

