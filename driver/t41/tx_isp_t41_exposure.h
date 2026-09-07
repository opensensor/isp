#ifndef TX_ISP_T41_EXPOSURE_H
#define TX_ISP_T41_EXPOSURE_H

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdbool.h>
#include "../include/tx_isp/tx_isp_exposure.h"
#endif

/*
 * Apply or remove the T41 image-side half of the anti-flicker profile.
 * The caller owns the TOP bypass shadow and supplies it so hardware and
 * software remain in lockstep.
 */
int tx_isp_t41_flicker_profile_apply(u32 channel, bool enable,
				     u32 gib_gain_q10,
				     u32 green_correction_q10,
				     u32 blue_correction_q10,
				     u32 calibrated_ccm_bypass,
				     u32 *top_bypass);

/* Rebuild CCM from current calibration, neutral CT and exposure. */
void tx_isp_t41_calibrated_ccm_apply(void);

/* Restore black-range-normalized GIB gains after a diagnostic override. */
int tx_isp_t41_calibrated_gib_apply(void);

#endif /* TX_ISP_T41_EXPOSURE_H */
