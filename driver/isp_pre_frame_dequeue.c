#include "include/main.h"


  int32_t isp_pre_frame_dequeue()

{
    void* $s0 = *(mdns_y_pspa_cur_bi_wei0_array + 0xd4);
    uint32_t isp_ch0_pre_dequeue_time_1 = isp_ch0_pre_dequeue_time;
    void var_18_118;
    private_ktime_set(&var_18_119, isp_ch0_pre_dequeue_time_1 / 0x3e8, 
        isp_ch0_pre_dequeue_time_1 % 0x3e8 * 0xf4240);
    private_set_current_state(2);
    private_schedule_hrtimeout(&var_18_120, 1);
    int32_t var_24_16 = 1;
    uint32_t var_20_214 =
        *($s0 + 0x128) << 0x10 | *(*(mdns_y_pspa_cur_bi_wei0_array + 0xb8) + 0x9888) >> 0x10;
    int32_t var_30_30 = 0;
    int32_t var_2c_23 = 0;
    int32_t var_28_40 = 0;
    void var_38_71;
    return tx_isp_send_event_to_remote(*(*($s0 + 0x150) + 0x78), 0x3000006, &var_38_72);
}

