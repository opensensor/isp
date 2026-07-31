#include "tx_isp_t23_subdev.h"
#include "../include/tx_isp/tx_isp_subdev_abi.h"

#define TX_ISP_T23_GRAPH_SUBDEVS_OFFSET 0x38U
#define TX_ISP_T23_GRAPH_SUBDEV_COUNT   16U

static void *tx_isp_t23_subdev_at(void *graph, unsigned int index)
{
	return *(void **)((char *)graph + TX_ISP_T23_GRAPH_SUBDEVS_OFFSET +
			 index * sizeof(void *));
}

static const char *tx_isp_t23_subdev_name(void *subdev)
{
	return *(const char **)((char *)subdev + TX_ISP_ABI_SUBDEV_NAME_OFFSET);
}

static unsigned int tx_isp_t23_pad_count(void *subdev, unsigned int type)
{
	unsigned int offset = type == TX_ISP_SUBDEV_PAD_OUTPUT ?
		TX_ISP_ABI_LEGACY_SUBDEV_PAD_COUNT1 :
		TX_ISP_ABI_LEGACY_SUBDEV_PAD_COUNT0;

	return *(unsigned short *)((char *)subdev + offset);
}

static void *tx_isp_t23_pad_base(void *subdev, unsigned int type)
{
	unsigned int offset = type == TX_ISP_SUBDEV_PAD_OUTPUT ?
		TX_ISP_ABI_LEGACY_SUBDEV_PAD_PTR1 :
		TX_ISP_ABI_LEGACY_SUBDEV_PAD_PTR0;

	return *(void **)((char *)subdev + offset);
}

static const struct tx_isp_subdev_resolver_ops tx_isp_t23_subdev_ops = {
	.subdev_count = TX_ISP_T23_GRAPH_SUBDEV_COUNT,
	.pad_stride = TX_ISP_ABI_PAD_STRIDE,
	.subdev_at = tx_isp_t23_subdev_at,
	.subdev_name = tx_isp_t23_subdev_name,
	.pad_count = tx_isp_t23_pad_count,
	.pad_base = tx_isp_t23_pad_base,
};

unsigned long tx_isp_t23_resolve_link_pad(
	unsigned long graph,
	unsigned long descriptor,
	enum tx_isp_subdev_resolve_status *status)
{
	struct tx_isp_subdev_pad_descriptor common_descriptor;

	if (!descriptor)
		return 0;
	common_descriptor.name =
		*(const char **)((char *)descriptor + 0);
	common_descriptor.type =
		*(unsigned char *)((char *)descriptor + 4);
	common_descriptor.index =
		*(unsigned char *)((char *)descriptor + 5);
	return (unsigned long)tx_isp_subdev_resolve_pad(
		(void *)graph, &common_descriptor, &tx_isp_t23_subdev_ops,
		status);
}

/* Build the shared resolver into the T23 module. */
#include "../common/tx_isp_subdev.c"
