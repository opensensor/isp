#include "include/main.h"


  int32_t sensor_init(void* arg1)

{
    void* $v0 = *(g_ispcore + 0x120);
    *(arg1 + 0x20) = *($v0 + 0x94);
    *(arg1 + 0x24) = *($v0 + 0x98);
    *(arg1 + 0x3a) = *($v0 + 0xb6);
    *(arg1 + 0x3b) = *($v0 + 0xb8);
    *(arg1 + 0x3c) = *($v0 + 0xba);
    *(arg1 + 0x28) = *($v0 + 0xa4);
    *(arg1 + 0x2c) = *($v0 + 0xb4);
    *(arg1 + 0x50) = *($v0 + 0xd8);
    *(arg1 + 0x54) = *($v0 + 0xda);
    *(arg1 + 0x58) = *($v0 + 0xe0);
    *(arg1 + 0x5c) = sensor_hw_reset_disable;
    *(arg1 + 0x60) = sensor_hw_reset_enable;
    *(arg1 + 0x64) = sensor_alloc_analog_gain;
    *(arg1 + 0x68) = sensor_alloc_analog_gain_short;
    *(arg1 + 0x6c) = sensor_alloc_digital_gain;
    *(arg1 + 0x70) = sensor_alloc_integration_time;
    *(arg1 + 0x74) = sensor_alloc_integration_time_short;
    *(arg1 + 0x78) = sensor_set_integration_time;
    *(arg1 + 0x7c) = sensor_set_integration_time_short;
    *(arg1 + 0x80) = sensor_start_changes;
    *(arg1 + 0x84) = sensor_end_changes;
    *(arg1 + 0x88) = sensor_set_analog_gain;
    *(arg1 + 0x8c) = sensor_set_analog_gain_short;
    *(arg1 + 0x90) = sensor_set_digital_gain;
    *(arg1 + 0x94) = sensor_get_normal_fps;
    *(arg1 + 0x98) = sensor_read_black_pedestal;
    *(arg1 + 0x9c) = sensor_set_mode;
    *(arg1 + 0xa0) = sensor_set_wdr_mode;
    *(arg1 + 0xa4) = sensor_fps_control;
    *(arg1 + 0xa8) = sensor_get_id;
    *(arg1 + 0xac) = sensor_disable_isp;
    *(arg1 + 0xb0) = sensor_get_lines_per_second;
    return sensor_get_lines_per_second;
}

