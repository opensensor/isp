#include "tx_isp_t31_subdev_resolver.h"
#include "include/tx_isp_subdev_helpers.h"

TX_ISP_ABI_ASSERT(t31_link_desc_name,
	__builtin_offsetof(struct link_pad_description, name) ==
	TX_ISP_SUBDEV_DESC_NAME_OFFSET);
TX_ISP_ABI_ASSERT(t31_link_desc_type,
	__builtin_offsetof(struct link_pad_description, type) ==
	TX_ISP_SUBDEV_DESC_TYPE_OFFSET);
TX_ISP_ABI_ASSERT(t31_link_desc_index,
	__builtin_offsetof(struct link_pad_description, index) ==
	TX_ISP_SUBDEV_DESC_INDEX_OFFSET);
TX_ISP_ABI_ASSERT(t31_link_desc_size,
	sizeof(struct link_pad_description) == TX_ISP_SUBDEV_DESC_SIZE);
TX_ISP_ABI_ASSERT(t31_link_config_source,
	__builtin_offsetof(struct tx_isp_link_config, src) ==
	TX_ISP_SUBDEV_LINK_SOURCE_OFFSET);
TX_ISP_ABI_ASSERT(t31_link_config_sink,
	__builtin_offsetof(struct tx_isp_link_config, dst) ==
	TX_ISP_SUBDEV_LINK_SINK_OFFSET);
TX_ISP_ABI_ASSERT(t31_link_config_flag,
	__builtin_offsetof(struct tx_isp_link_config, flag) ==
	TX_ISP_SUBDEV_LINK_FLAG_OFFSET);
TX_ISP_ABI_ASSERT(t31_link_config_size,
	sizeof(struct tx_isp_link_config) == TX_ISP_SUBDEV_LINK_CONFIG_SIZE);
TX_ISP_ABI_ASSERT(t31_link_set_records,
	__builtin_offsetof(struct tx_isp_link_configs, config) ==
	TX_ISP_SUBDEV_LINK_SET_RECORDS_OFFSET);
TX_ISP_ABI_ASSERT(t31_link_set_count,
	__builtin_offsetof(struct tx_isp_link_configs, length) ==
	TX_ISP_SUBDEV_LINK_SET_COUNT_OFFSET);
TX_ISP_ABI_ASSERT(t31_link_set_size,
	sizeof(struct tx_isp_link_configs) == TX_ISP_SUBDEV_LINK_SET_SIZE);

static void *tx_isp_t31_subdev_at(void *graph, unsigned int index)
{
	struct tx_isp_dev *isp_dev = graph;

	return isp_dev->subdevs[index];
}

static const char *tx_isp_t31_subdev_name(void *subdev)
{
	struct tx_isp_subdev *sd = subdev;
	const char *name = tx_isp_subdev_raw_name_get(sd);

	if (name)
		return name;
	if (sd->module.name)
		return sd->module.name;
	if (sd->module.miscdev.name)
		return sd->module.miscdev.name;
	if (sd->module.dev && sd->module.dev->kobj.name)
		return sd->module.dev->kobj.name;
	return 0;
}

static unsigned int tx_isp_t31_pad_count(void *subdev, unsigned int type)
{
	struct tx_isp_subdev *sd = subdev;

	return type == TX_ISP_SUBDEV_PAD_OUTPUT ?
		tx_isp_subdev_raw_num_outpads_get(sd) :
		tx_isp_subdev_raw_num_inpads_get(sd);
}

static void *tx_isp_t31_pad_base(void *subdev, unsigned int type)
{
	struct tx_isp_subdev *sd = subdev;

	return type == TX_ISP_SUBDEV_PAD_OUTPUT ?
		tx_isp_subdev_raw_outpads_get(sd) :
		tx_isp_subdev_raw_inpads_get(sd);
}

static const struct tx_isp_subdev_resolver_ops tx_isp_t31_subdev_ops = {
	.subdev_count = ISP_MAX_SUBDEVS,
	.pad_stride = TX_ISP_OEM_SUBDEV_PAD_STRIDE,
	.subdev_at = tx_isp_t31_subdev_at,
	.subdev_name = tx_isp_t31_subdev_name,
	.pad_count = tx_isp_t31_pad_count,
	.pad_base = tx_isp_t31_pad_base,
};

struct tx_isp_subdev_pad *tx_isp_t31_resolve_link_pad(
	struct tx_isp_dev *isp_dev,
	const struct link_pad_description *descriptor,
	enum tx_isp_subdev_resolve_status *status)
{
	struct tx_isp_subdev_pad_descriptor common_descriptor;

	if (!descriptor)
		return 0;
	common_descriptor.name = descriptor->name;
	common_descriptor.type = descriptor->type;
	common_descriptor.index = descriptor->index;
	return tx_isp_subdev_resolve_pad(
		isp_dev, &common_descriptor, &tx_isp_t31_subdev_ops, status);
}

/* Build the shared resolver into the T31 module. */
#include "../common/tx_isp_subdev.c"
