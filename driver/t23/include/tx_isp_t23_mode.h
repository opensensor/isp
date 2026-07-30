#ifndef TX_ISP_T23_MODE_H
#define TX_ISP_T23_MODE_H

/*
 * Apply the T23 top-bypass flags and refresh every tuning block affected by a
 * day/night, custom-mode, or tuning-bin bank change.
 */
void tx_isp_t23_mode_profile_apply(const unsigned int *flags);

#endif /* TX_ISP_T23_MODE_H */
