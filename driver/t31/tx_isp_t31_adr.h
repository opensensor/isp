#ifndef TX_ISP_T31_ADR_H
#define TX_ISP_T31_ADR_H

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include "../include/tx_isp/tx_isp_exposure.h"
#endif

#define TX_ISP_T31_ADR_MAP_CHANNELS 4U
#define TX_ISP_T31_ADR_MAP_POINTS   9U

/*
 * Scale the immutable ADR map tables into caller-owned working tables.
 * Keeping source and destination separate is important: strength changes are
 * absolute controls, not deltas, and therefore must not compound.
 */
int tx_isp_t31_adr_scale_mapb(
	u32 strength,
	const u32 minimum[TX_ISP_T31_ADR_MAP_CHANNELS],
	const u32 *const source[TX_ISP_T31_ADR_MAP_CHANNELS],
	u32 *const output[TX_ISP_T31_ADR_MAP_CHANNELS]);

#endif /* TX_ISP_T31_ADR_H */
