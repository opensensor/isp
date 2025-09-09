#include "include/main.h"


  int32_t sensor_init(void* arg1)

{
    char* $v0 = *((char*)g_ispcore + 0x120); // Fixed void pointer arithmetic
    *(((void**)((char*)arg1 + 0x20))) = *($v0 + 0x94); // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0x24))) = *($v0 + 0x98); // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0x3a))) = *($v0 + 0xb6); // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0x3b))) = *($v0 + 0xb8); // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0x3c))) = *($v0 + 0xba); // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0x28))) = *($v0 + 0xa4); // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0x2c))) = *($v0 + 0xb4); // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0x50))) = *($v0 + 0xd8); // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0x54))) = *($v0 + 0xda); // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0x58))) = *($v0 + 0xe0); // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0x5c))) = sensor_hw_reset_disable; // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0x60))) = sensor_hw_reset_enable; // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0x64))) = sensor_alloc_analog_gain; // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0x68))) = sensor_alloc_analog_gain_short; // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0x6c))) = sensor_alloc_digital_gain; // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0x70))) = sensor_alloc_integration_time; // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0x74))) = sensor_alloc_integration_time_short; // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0x78))) = sensor_set_integration_time; // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0x7c))) = sensor_set_integration_time_short; // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0x80))) = sensor_start_changes; // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0x84))) = sensor_end_changes; // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0x88))) = sensor_set_analog_gain; // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0x8c))) = sensor_set_analog_gain_short; // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0x90))) = sensor_set_digital_gain; // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0x94))) = sensor_get_normal_fps; // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0x98))) = sensor_read_black_pedestal; // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0x9c))) = sensor_set_mode; // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0xa0))) = sensor_set_wdr_mode; // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0xa4))) = sensor_fps_control; // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0xa8))) = sensor_get_id; // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0xac))) = sensor_disable_isp; // Fixed void pointer dereference
    *(((void**)((char*)arg1 + 0xb0))) = sensor_get_lines_per_second; // Fixed void pointer dereference
    return sensor_get_lines_per_second;
}

