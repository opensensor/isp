/*
 * Shared TX-ISP graph endpoint resolver.
 */

#ifdef __KERNEL__
#include <linux/string.h>
#else
#include <string.h>
#endif

#include "../include/tx_isp/tx_isp_subdev.h"
#include "../include/tx_isp/tx_isp_subdev_abi.h"

static void tx_isp_subdev_resolve_set_status(
	enum tx_isp_subdev_resolve_status *status,
	enum tx_isp_subdev_resolve_status value)
{
	if (status)
		*status = value;
}

void *tx_isp_subdev_resolve_pad(
	void *graph,
	const struct tx_isp_subdev_pad_descriptor *descriptor,
	const struct tx_isp_subdev_resolver_ops *ops,
	enum tx_isp_subdev_resolve_status *status)
{
	unsigned int i;

	tx_isp_subdev_resolve_set_status(
		status, TX_ISP_SUBDEV_RESOLVE_INVALID);
	if (!graph || !descriptor || !descriptor->name || !ops ||
	    !ops->subdev_count || !ops->pad_stride || !ops->subdev_at ||
	    !ops->subdev_name || !ops->pad_count || !ops->pad_base)
		return 0;

	if (descriptor->type != TX_ISP_SUBDEV_PAD_OUTPUT &&
	    descriptor->type != TX_ISP_SUBDEV_PAD_INPUT) {
		tx_isp_subdev_resolve_set_status(
			status, TX_ISP_SUBDEV_RESOLVE_BAD_TYPE);
		return 0;
	}

	for (i = 0; i < ops->subdev_count; ++i) {
		void *subdev = ops->subdev_at(graph, i);
		const char *name;
		unsigned int count;
		void *pads;

		if (!subdev)
			continue;
		name = ops->subdev_name(subdev);
		if (!name || strcmp(name, descriptor->name))
			continue;

		count = ops->pad_count(subdev, descriptor->type);
		pads = ops->pad_base(subdev, descriptor->type);
		if (!pads) {
			tx_isp_subdev_resolve_set_status(
				status, TX_ISP_SUBDEV_RESOLVE_NO_PADS);
			return 0;
		}
		if (descriptor->index >= count) {
			tx_isp_subdev_resolve_set_status(
				status, TX_ISP_SUBDEV_RESOLVE_INDEX_RANGE);
			return 0;
		}

		tx_isp_subdev_resolve_set_status(
			status, TX_ISP_SUBDEV_RESOLVE_OK);
		return (char *)pads + descriptor->index * ops->pad_stride;
	}

	tx_isp_subdev_resolve_set_status(
		status, TX_ISP_SUBDEV_RESOLVE_NOT_FOUND);
	return 0;
}

enum tx_isp_subdev_link_status tx_isp_subdev_validate_link(
	unsigned int source_links_type,
	unsigned int sink_links_type,
	unsigned int source_state,
	unsigned int sink_state,
	unsigned int flags,
	unsigned int *enabled_flags)
{
	if (!enabled_flags)
		return TX_ISP_SUBDEV_LINK_INVALID;

	*enabled_flags = 0;
	if (!(source_links_type & sink_links_type & flags))
		return TX_ISP_SUBDEV_LINK_TYPE_MISMATCH;
	if (source_state == TX_ISP_ABI_PADSTATE_STREAM ||
	    sink_state == TX_ISP_ABI_PADSTATE_STREAM)
		return TX_ISP_SUBDEV_LINK_BUSY;

	*enabled_flags = flags | TX_ISP_SUBDEV_LINK_ENABLED;
	return TX_ISP_SUBDEV_LINK_OK;
}

void tx_isp_subdev_init_link_record(void *pad)
{
	void *link;

	if (!pad)
		return;
	link = (char *)pad + TX_ISP_ABI_PAD_LINK_OFFSET;
	tx_isp_subdev_clear_link_endpoints(link);
	*(unsigned int *)((char *)link + TX_ISP_ABI_LINK_STATE_OFFSET) = 0;
}

void tx_isp_subdev_clear_link_endpoints(void *link)
{
	*(unsigned int *)((char *)link + TX_ISP_ABI_LINK_SOURCE_OFFSET) = 0;
	*(unsigned int *)((char *)link + TX_ISP_ABI_LINK_SINK_OFFSET) = 0;
	*(unsigned int *)((char *)link + TX_ISP_ABI_LINK_REVERSE_OFFSET) = 0;
	*(unsigned int *)((char *)link + TX_ISP_ABI_LINK_FLAG_OFFSET) = 0;
}

void tx_isp_subdev_detach_link(
	void *link,
	unsigned int *source,
	unsigned int *reverse,
	unsigned int *sink)
{
	*source = *(unsigned int *)((char *)link +
		TX_ISP_ABI_LINK_SOURCE_OFFSET);
	*reverse = *(unsigned int *)((char *)link +
		TX_ISP_ABI_LINK_REVERSE_OFFSET);
	*sink = *(unsigned int *)((char *)link +
		TX_ISP_ABI_LINK_SINK_OFFSET);
	tx_isp_subdev_clear_link_endpoints(link);
}

void tx_isp_subdev_connect_link_pair(
	void *source_pad,
	void *sink_pad,
	unsigned int flags)
{
	unsigned int source;
	unsigned int sink;
	void *source_link;
	void *sink_link;

	if (!source_pad || !sink_pad)
		return;

	source = (unsigned int)(unsigned long)source_pad;
	sink = (unsigned int)(unsigned long)sink_pad;
	source_link = (char *)source_pad + TX_ISP_ABI_PAD_LINK_OFFSET;
	sink_link = (char *)sink_pad + TX_ISP_ABI_PAD_LINK_OFFSET;

	*(unsigned int *)((char *)source_link +
		TX_ISP_ABI_LINK_SOURCE_OFFSET) = source;
	*(unsigned int *)((char *)source_link +
		TX_ISP_ABI_LINK_SINK_OFFSET) = sink;
	*(unsigned int *)((char *)source_link +
		TX_ISP_ABI_LINK_REVERSE_OFFSET) =
		sink + TX_ISP_ABI_PAD_LINK_OFFSET;
	*(unsigned int *)((char *)source_link +
		TX_ISP_ABI_LINK_FLAG_OFFSET) = flags;
	*(unsigned char *)((char *)source_pad +
		TX_ISP_ABI_PAD_STATE_OFFSET) = TX_ISP_ABI_PADSTATE_LINKED;

	*(unsigned int *)((char *)sink_link +
		TX_ISP_ABI_LINK_SOURCE_OFFSET) = sink;
	*(unsigned int *)((char *)sink_link +
		TX_ISP_ABI_LINK_SINK_OFFSET) = source;
	*(unsigned int *)((char *)sink_link +
		TX_ISP_ABI_LINK_REVERSE_OFFSET) =
		source + TX_ISP_ABI_PAD_LINK_OFFSET;
	*(unsigned int *)((char *)sink_link +
		TX_ISP_ABI_LINK_FLAG_OFFSET) = flags;
	*(unsigned char *)((char *)sink_pad +
		TX_ISP_ABI_PAD_STATE_OFFSET) = TX_ISP_ABI_PADSTATE_LINKED;
}
