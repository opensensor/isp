/*
 * Shared TX-ISP remote pad event resolver.
 */

#include "../include/tx_isp/tx_isp_remote_event.h"

enum tx_isp_remote_event_status tx_isp_resolve_remote_event(
	void *local_pad,
	const struct tx_isp_remote_event_ops *ops,
	tx_isp_remote_event_pointer_valid pointer_valid,
	struct tx_isp_remote_event_target *target)
{
	void *remote_pad;
	unsigned long handler;

	if (target) {
		target->pad = 0;
		target->handler = 0;
	}
	if (!target || !ops || !ops->remote_pad || !ops->event_handler ||
	    !pointer_valid || !pointer_valid((unsigned long)local_pad))
		return TX_ISP_REMOTE_EVENT_INVALID;

	remote_pad = ops->remote_pad(local_pad);
	if (!pointer_valid((unsigned long)remote_pad))
		return TX_ISP_REMOTE_EVENT_UNLINKED;

	handler = ops->event_handler(remote_pad);
	if (!pointer_valid(handler))
		return TX_ISP_REMOTE_EVENT_NO_HANDLER;

	target->pad = remote_pad;
	target->handler = handler;
	return TX_ISP_REMOTE_EVENT_OK;
}
