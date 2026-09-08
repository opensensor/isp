#ifndef TX_ISP_T41_SCALER_H
#define TX_ISP_T41_SCALER_H

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include "../include/tx_isp/tx_isp_scaler.h"
#endif

#define TX_ISP_T41_SCALER_COEFFICIENTS	33U
#define TX_ISP_T41_SCALER_SUB_COEFFICIENTS	257U
#define TX_ISP_T41_SCALER_PARAMS_BYTES	0xe8U

typedef int (*tx_isp_t41_scaler_write_fn)(void *context, u32 reg, u32 value);

/* Channel 1 has 64 fractional phases; channels 0/2 have eight. */
int tx_isp_t41_scaler_channel_curve_generate(u32 channel, u32 target,
					   u32 source, s16 *coefficients,
					   u32 coefficient_capacity);

/* Emit shadow-port value/address pairs. Returns the DMA pair count, or errno.
 * The caller owns serialization, shadow mode selection and the final DMA kick.
 * No writes occur for invalid arguments. The calibration's optional fixed
 * first phase is respected; all other phases come from the generated curves.
 */
int tx_isp_t41_scaler_curve_write(u32 channel, const u8 *params,
				u32 params_bytes, const s16 *vertical,
				const s16 *horizontal, u32 curve_capacity,
				tx_isp_t41_scaler_write_fn write, void *context);

int tx_isp_t41_scaler_curve_generate(u32 target, u32 source,
				     s16 *coefficients,
				     u32 coefficient_capacity);

#endif /* TX_ISP_T41_SCALER_H */
