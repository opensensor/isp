#ifndef ISP_H
#define ISP_H

#include <stdint.h>

// Structure for ISP initialization parameters
typedef struct {
    uint32_t field_00;
    uint32_t field_04;
    uint32_t field_08;
} isp_init_params_t;

// Function declarations
int isp_init(const isp_init_params_t *params);

#endif // ISP_H
