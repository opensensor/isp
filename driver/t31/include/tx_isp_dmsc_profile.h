#ifndef TX_ISP_T31_DMSC_PROFILE_H
#define TX_ISP_T31_DMSC_PROFILE_H

#include <linux/types.h>

/*
 * Apply the measured shipping SC2336 DMSC correction profile and latch it.
 * The generic tuning decoder still programs every other DMSC register.
 */
int tx_isp_t31_sc2336_dmsc_profile_apply(bool night_mode);

#endif /* TX_ISP_T31_DMSC_PROFILE_H */
