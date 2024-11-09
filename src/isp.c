#include "isp.h"
#include "isp_regs.h"
#include <string.h>
#include <stdio.h>

static uint32_t isp_memopt = 1;
static uint32_t tparams_day[24] = {0};
static uint32_t tparams_night[24] = {0};
static uint32_t tparams_cust[24] = {0};

int isp_init(const isp_init_params_t *params) {
    if (!params) {
        fprintf(stderr, "Invalid parameters.\n");
        return -1;
    }

    // Initialize memory for day, night, and custom settings
    memset(tparams_day, 0, sizeof(tparams_day));
    memset(tparams_night, 0, sizeof(tparams_night));
    memset(tparams_cust, 0, sizeof(tparams_cust));

    // Set the ISP memory optimization option
    if (isp_memopt == 1) {
        tparams_day[0xbb58 / 4] = isp_memopt;
        tparams_night[0xbb58 / 4] = isp_memopt;
        tparams_cust[0xbb58 / 4] = isp_memopt;
    }

    // Write initial configuration to hardware registers
    uint32_t reg_value = (params->field_00 << 16) | params->field_04;
    REG_WRITE(ISP_REG_CONFIG, reg_value);

    // Set ISP mode based on field_08
    REG_WRITE(ISP_REG_MODE, params->field_08);

    // Configure color space conversion (CSC) version
    REG_WRITE(ISP_REG_CSC_VERSION, 0);

    // Final initialization step
    REG_WRITE(ISP_REG_INIT, 0xFFFFFFFF);

    printf("ISP initialized successfully.\n");
    return 0;
}
