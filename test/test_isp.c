#include "isp.h"
#include "sensor.h"
#include <stdio.h>

int main() {
    isp_init_params_t init_params = {
        .field_00 = 0x1234,
        .field_04 = 0xABCD,
        .field_08 = 2
    };

    if (isp_init(&init_params) == 0) {
        printf("ISP initialization successful.\n");
    } else {
        printf("ISP initialization failed.\n");
    }

    sensor_init();

    return 0;
}