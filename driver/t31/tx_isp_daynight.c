/*
 * Build the shared day/night state machine into the monolithic T31 module.
 * T31 control IDs, register offsets, and tuning hooks remain in the T31
 * adapter call sites.
 */

#include "../common/tx_isp_daynight.c"
