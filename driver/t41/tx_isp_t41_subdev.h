#ifndef TX_ISP_T41_SUBDEV_H
#define TX_ISP_T41_SUBDEV_H

#include "../include/tx_isp/tx_isp_subdev.h"
#include "../include/tx_isp/tx_isp_remote_event.h"

unsigned long tx_isp_t41_resolve_link_pad(
	unsigned long graph,
	unsigned long descriptor,
	enum tx_isp_subdev_resolve_status *status);

enum tx_isp_remote_event_status tx_isp_t41_resolve_remote_event(
	void *local_pad,
	tx_isp_remote_event_pointer_valid pointer_valid,
	struct tx_isp_remote_event_target *target);

#endif /* TX_ISP_T41_SUBDEV_H */
