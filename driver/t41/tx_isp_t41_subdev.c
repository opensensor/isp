#include "tx_isp_t41_subdev.h"
#include "../include/tx_isp/tx_isp_subdev_abi.h"

#define TX_ISP_T41_GRAPH_SUBDEVS_OFFSET 0x3cU
#define TX_ISP_T41_GRAPH_SUBDEV_COUNT   16U
#define TX_ISP_T41_STATE_QUEUE_OFFSET   0x1fcU
#define TX_ISP_T41_STATE_FLAGS_OFFSET   0x224U

static int tx_isp_t41_valid_pointer(const void *pointer)
{
	unsigned long value = (unsigned long)pointer;

	return value && value < (unsigned long)-4095;
}

static void *tx_isp_t41_subdev_at(void *graph, unsigned int index)
{
	void *subdev = *(void **)((char *)graph +
		TX_ISP_T41_GRAPH_SUBDEVS_OFFSET + index * sizeof(void *));

	return tx_isp_t41_valid_pointer(subdev) ? subdev : 0;
}

static const char *tx_isp_t41_subdev_name(void *subdev)
{
	const char *name = *(const char **)((char *)subdev +
		TX_ISP_ABI_SUBDEV_NAME_OFFSET);

	return tx_isp_t41_valid_pointer(name) ? name : 0;
}

static unsigned int tx_isp_t41_pad_count(void *subdev, unsigned int type)
{
	unsigned int offset = type == TX_ISP_SUBDEV_PAD_OUTPUT ?
		TX_ISP_ABI_EXTENDED_SUBDEV_PAD_COUNT1 :
		TX_ISP_ABI_EXTENDED_SUBDEV_PAD_COUNT0;

	return *(unsigned short *)((char *)subdev + offset);
}

static void *tx_isp_t41_pad_base(void *subdev, unsigned int type)
{
	unsigned int offset = type == TX_ISP_SUBDEV_PAD_OUTPUT ?
		TX_ISP_ABI_EXTENDED_SUBDEV_PAD_PTR1 :
		TX_ISP_ABI_EXTENDED_SUBDEV_PAD_PTR0;
	void *pads = *(void **)((char *)subdev + offset);

	return tx_isp_t41_valid_pointer(pads) ? pads : 0;
}

static const struct tx_isp_subdev_resolver_ops tx_isp_t41_subdev_ops = {
	.subdev_count = TX_ISP_T41_GRAPH_SUBDEV_COUNT,
	.pad_stride = TX_ISP_ABI_PAD_STRIDE,
	.subdev_at = tx_isp_t41_subdev_at,
	.subdev_name = tx_isp_t41_subdev_name,
	.pad_count = tx_isp_t41_pad_count,
	.pad_base = tx_isp_t41_pad_base,
};

unsigned long tx_isp_t41_resolve_link_pad(
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
		(void *)graph, &common_descriptor, &tx_isp_t41_subdev_ops,
		status);
}

static void *tx_isp_t41_remote_pad(void *local_pad)
{
	return *(void **)((char *)local_pad + TX_ISP_ABI_PAD_LINK_OFFSET +
			 TX_ISP_ABI_LINK_SINK_OFFSET);
}

static unsigned long tx_isp_t41_event_handler(void *remote_pad)
{
	return *(unsigned int *)((char *)remote_pad +
				TX_ISP_ABI_PAD_EVENT_OFFSET);
}

static const struct tx_isp_remote_event_ops tx_isp_t41_remote_event_ops = {
	.remote_pad = tx_isp_t41_remote_pad,
	.event_handler = tx_isp_t41_event_handler,
};

enum tx_isp_remote_event_status tx_isp_t41_resolve_remote_event(
	void *local_pad,
	tx_isp_remote_event_pointer_valid pointer_valid,
	struct tx_isp_remote_event_target *target)
{
	return tx_isp_resolve_remote_event(
		local_pad, &tx_isp_t41_remote_event_ops, pointer_valid, target);
}

int tx_isp_t41_subdev_state_ready(void *object)
{
	unsigned long queue_next;
	unsigned int state;

	if (!object)
		return 0;
	queue_next = *(unsigned int *)((char *)object +
				      TX_ISP_T41_STATE_QUEUE_OFFSET);
	state = *(unsigned char *)((char *)object +
				  TX_ISP_T41_STATE_FLAGS_OFFSET);
	return tx_isp_subdev_state_ready(
		(unsigned long)object, queue_next,
		(unsigned long)((char *)object + TX_ISP_T41_STATE_QUEUE_OFFSET),
		state);
}

/* Build the shared resolver into the T41 module. */
#include "../common/tx_isp_subdev.c"
#include "../common/tx_isp_remote_event.c"
#include "../common/tx_isp_state.c"
