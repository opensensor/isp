#ifndef TX_ISP_T40_SUBDEV_H
#define TX_ISP_T40_SUBDEV_H

#include "../include/tx_isp/tx_isp_subdev.h"
#include "../include/tx_isp/tx_isp_remote_event.h"
#include "../include/tx_isp/tx_isp_state.h"

unsigned long tx_isp_t40_resolve_link_pad(
	unsigned long graph,
	unsigned long descriptor,
	enum tx_isp_subdev_resolve_status *status);

enum tx_isp_remote_event_status tx_isp_t40_resolve_remote_event(
	void *local_pad,
	tx_isp_remote_event_pointer_valid pointer_valid,
	struct tx_isp_remote_event_target *target);

int tx_isp_t40_subdev_state_ready(void *object);

#endif /* TX_ISP_T40_SUBDEV_H */
