#include "tx_isp_t30_subdev.h"

/* T30 uses the legacy T23/T31 subdevice prefix and 16-entry graph. */
#define TX_ISP_T30_GRAPH_SUBDEVS_OFFSET 0x38U
#define TX_ISP_T30_GRAPH_SUBDEV_COUNT   16U

static void *tx_isp_t30_subdev_at(void *graph, unsigned int index)
{
	return *(void **)((char *)graph + TX_ISP_T30_GRAPH_SUBDEVS_OFFSET +
			 index * sizeof(void *));
}

static const char *tx_isp_t30_subdev_name(void *subdev)
{
	return *(const char **)((char *)subdev + TX_ISP_ABI_SUBDEV_NAME_OFFSET);
}

static unsigned int tx_isp_t30_pad_count(void *subdev, unsigned int type)
{
	unsigned int offset = type == TX_ISP_SUBDEV_PAD_OUTPUT ?
		TX_ISP_ABI_LEGACY_SUBDEV_PAD_COUNT1 :
		TX_ISP_ABI_LEGACY_SUBDEV_PAD_COUNT0;

	return *(unsigned short *)((char *)subdev + offset);
}

static void *tx_isp_t30_pad_base(void *subdev, unsigned int type)
{
	unsigned int offset = type == TX_ISP_SUBDEV_PAD_OUTPUT ?
		TX_ISP_ABI_LEGACY_SUBDEV_PAD_PTR1 :
		TX_ISP_ABI_LEGACY_SUBDEV_PAD_PTR0;

	return *(void **)((char *)subdev + offset);
}

static const struct tx_isp_subdev_resolver_ops tx_isp_t30_subdev_ops = {
	.subdev_count = TX_ISP_T30_GRAPH_SUBDEV_COUNT,
	.pad_stride = TX_ISP_ABI_PAD_STRIDE,
	.subdev_at = tx_isp_t30_subdev_at,
	.subdev_name = tx_isp_t30_subdev_name,
	.pad_count = tx_isp_t30_pad_count,
	.pad_base = tx_isp_t30_pad_base,
};

unsigned long tx_isp_t30_resolve_link_pad(
	unsigned long graph,
	unsigned long descriptor,
	enum tx_isp_subdev_resolve_status *status)
{
	struct tx_isp_subdev_pad_descriptor common_descriptor;

	if (!descriptor)
		return 0;
	common_descriptor.name =
		*(const char **)((char *)descriptor +
				TX_ISP_SUBDEV_DESC_NAME_OFFSET);
	common_descriptor.type =
		*(unsigned char *)((char *)descriptor +
				  TX_ISP_SUBDEV_DESC_TYPE_OFFSET);
	common_descriptor.index =
		*(unsigned char *)((char *)descriptor +
				  TX_ISP_SUBDEV_DESC_INDEX_OFFSET);
	return (unsigned long)tx_isp_subdev_resolve_pad(
		(void *)graph, &common_descriptor, &tx_isp_t30_subdev_ops,
		status);
}

static void *tx_isp_t30_remote_pad(void *local_pad)
{
	return *(void **)((char *)local_pad + TX_ISP_ABI_PAD_LINK_OFFSET +
			 TX_ISP_ABI_LINK_SINK_OFFSET);
}

static unsigned long tx_isp_t30_event_handler(void *remote_pad)
{
	return *(unsigned int *)((char *)remote_pad +
				TX_ISP_ABI_PAD_EVENT_OFFSET);
}

static const struct tx_isp_remote_event_ops tx_isp_t30_remote_event_ops = {
	.remote_pad = tx_isp_t30_remote_pad,
	.event_handler = tx_isp_t30_event_handler,
};

enum tx_isp_remote_event_status tx_isp_t30_resolve_remote_event(
	void *local_pad,
	tx_isp_remote_event_pointer_valid pointer_valid,
	struct tx_isp_remote_event_target *target)
{
	return tx_isp_resolve_remote_event(
		local_pad, &tx_isp_t30_remote_event_ops, pointer_valid, target);
}

#include "../common/tx_isp_subdev.c"
#include "../common/tx_isp_remote_event.c"
