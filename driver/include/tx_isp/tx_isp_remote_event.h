#ifndef TX_ISP_REMOTE_EVENT_H
#define TX_ISP_REMOTE_EVENT_H

typedef int (*tx_isp_remote_event_handler)(
	void *pad,
	unsigned int event,
	void *data);

typedef int (*tx_isp_remote_event_pointer_valid)(unsigned long address);

struct tx_isp_remote_event_ops {
	void *(*remote_pad)(void *local_pad);
	unsigned long (*event_handler)(void *remote_pad);
};

struct tx_isp_remote_event_target {
	void *pad;
	unsigned long handler;
};

enum tx_isp_remote_event_status {
	TX_ISP_REMOTE_EVENT_OK = 0,
	TX_ISP_REMOTE_EVENT_INVALID,
	TX_ISP_REMOTE_EVENT_UNLINKED,
	TX_ISP_REMOTE_EVENT_NO_HANDLER,
};

/*
 * Resolve the common pad -> active-link sink -> event-handler route. Pointer
 * policy and callback invocation remain generation-local.
 */
enum tx_isp_remote_event_status tx_isp_resolve_remote_event(
	void *local_pad,
	const struct tx_isp_remote_event_ops *ops,
	tx_isp_remote_event_pointer_valid pointer_valid,
	struct tx_isp_remote_event_target *target);

#endif /* TX_ISP_REMOTE_EVENT_H */
