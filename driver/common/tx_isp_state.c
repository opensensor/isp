/*
 * Shared TX-ISP subdevice state policy.
 */

#include "../include/tx_isp/tx_isp_state.h"

int tx_isp_subdev_state_ready(
	unsigned long object,
	unsigned long queue_next,
	unsigned long queue_self,
	unsigned int state)
{
	if (!object)
		return 0;
	if (queue_next != queue_self)
		return 1;

	return (state & 1U) ^ 1U;
}
