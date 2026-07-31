#ifndef TX_ISP_T23_SUBDEV_H
#define TX_ISP_T23_SUBDEV_H

#include "../include/tx_isp/tx_isp_subdev.h"

unsigned long tx_isp_t23_resolve_link_pad(
	unsigned long graph,
	unsigned long descriptor,
	enum tx_isp_subdev_resolve_status *status);

#endif /* TX_ISP_T23_SUBDEV_H */
