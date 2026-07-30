#ifndef TX_ISP_T41_EXPOSURE_H
#define TX_ISP_T41_EXPOSURE_H

#include <linux/types.h>

/*
 * Apply or remove the T41 image-side half of the anti-flicker profile.
 * The caller owns the TOP bypass shadow and supplies it so hardware and
 * software remain in lockstep.
 */
int tx_isp_t41_flicker_profile_apply(u32 channel, bool enable,
				     u32 gib_gain_q10,
				     u32 green_correction_q10,
				     u32 blue_correction_q10,
				     u32 *top_bypass);

#endif /* TX_ISP_T41_EXPOSURE_H */
