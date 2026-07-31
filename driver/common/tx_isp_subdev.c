/*
 * Shared TX-ISP graph endpoint resolver.
 */

#ifdef __KERNEL__
#include <linux/string.h>
#else
#include <string.h>
#endif

#include "../include/tx_isp/tx_isp_subdev.h"

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
