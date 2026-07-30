#ifndef TX_ISP_T31_EXPOSURE_H
#define TX_ISP_T31_EXPOSURE_H

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include "../include/tx_isp/tx_isp_exposure.h"
#endif

#define TX_ISP_T31_FLICKER_LUT_ENTRIES	120U

/*
 * Expand a mains half-cycle step into the T31 AE table ABI.
 *
 * The table contains 120 u32 words, repeats its last valid node through the
 * unused tail, and publishes the last valid index rather than a node count.
 */
int tx_isp_t31_flicker_lut_build(u32 step_lines, u32 max_integration,
				 u32 *lut, u32 lut_capacity,
				 u32 *last_index);

#endif /* TX_ISP_T31_EXPOSURE_H */
