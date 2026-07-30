#ifndef TX_ISP_T41_SCALER_H
#define TX_ISP_T41_SCALER_H

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include "../include/tx_isp/tx_isp_scaler.h"
#endif

#define TX_ISP_T41_SCALER_COEFFICIENTS	33U

int tx_isp_t41_scaler_curve_generate(u32 target, u32 source,
				     s16 *coefficients,
				     u32 coefficient_capacity);

#endif /* TX_ISP_T41_SCALER_H */
