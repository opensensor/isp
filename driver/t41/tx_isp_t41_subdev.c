#include "tx_isp_t41_subdev.h"
#include "../include/tx_isp/tx_isp_subdev_abi.h"

#define TX_ISP_T41_GRAPH_SUBDEVS_OFFSET 0x3cU
#define TX_ISP_T41_GRAPH_SUBDEV_COUNT   16U

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

/* Build the shared resolver into the T41 module. */
#include "../common/tx_isp_subdev.c"
