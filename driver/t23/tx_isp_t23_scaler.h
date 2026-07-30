#ifndef TX_ISP_T23_SCALER_H
#define TX_ISP_T23_SCALER_H

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include "../include/tx_isp/tx_isp_scaler.h"
#endif

#define TX_ISP_T23_SCALER_COEFFICIENTS	17U

int tx_isp_t23_scaler_curve_generate(u32 ratio_q14, s16 *coefficients,
				     u32 coefficient_capacity);
const s16 *tx_isp_t23_scaler_sinc_lut_get(u32 *entry_count);

#endif /* TX_ISP_T23_SCALER_H */
