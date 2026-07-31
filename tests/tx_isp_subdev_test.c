#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "../driver/include/tx_isp/tx_isp_subdev.h"

#define FAKE_SUBDEV_COUNT 4
#define FAKE_PAD_STRIDE   36

struct fake_subdev {
	const char *name;
	unsigned int output_count;
	unsigned int input_count;
	unsigned char *output_pads;
	unsigned char *input_pads;
};

struct fake_graph {
	struct fake_subdev *subdevs[FAKE_SUBDEV_COUNT];
};

static void *fake_subdev_at(void *opaque, unsigned int index)
{
	struct fake_graph *graph = opaque;

	return graph->subdevs[index];
}

static const char *fake_subdev_name(void *opaque)
{
	struct fake_subdev *subdev = opaque;

	return subdev->name;
}

static unsigned int fake_pad_count(void *opaque, unsigned int type)
{
	struct fake_subdev *subdev = opaque;

	return type == TX_ISP_SUBDEV_PAD_OUTPUT ?
		subdev->output_count : subdev->input_count;
}

static void *fake_pad_base(void *opaque, unsigned int type)
{
	struct fake_subdev *subdev = opaque;

	return type == TX_ISP_SUBDEV_PAD_OUTPUT ?
		subdev->output_pads : subdev->input_pads;
}

static const struct tx_isp_subdev_resolver_ops fake_ops = {
	.subdev_count = FAKE_SUBDEV_COUNT,
	.pad_stride = FAKE_PAD_STRIDE,
	.subdev_at = fake_subdev_at,
	.subdev_name = fake_subdev_name,
	.pad_count = fake_pad_count,
	.pad_base = fake_pad_base,
};

static void expect(int condition)
{
	if (!condition)
		__builtin_trap();
}

static void test_wire_layout(void)
{
	expect(sizeof(struct tx_isp_subdev_pad_descriptor32) ==
	       TX_ISP_SUBDEV_DESC_SIZE);
	expect(offsetof(struct tx_isp_subdev_pad_descriptor32, name) ==
	       TX_ISP_SUBDEV_DESC_NAME_OFFSET);
	expect(offsetof(struct tx_isp_subdev_pad_descriptor32, type) ==
	       TX_ISP_SUBDEV_DESC_TYPE_OFFSET);
	expect(offsetof(struct tx_isp_subdev_pad_descriptor32, index) ==
	       TX_ISP_SUBDEV_DESC_INDEX_OFFSET);

	expect(sizeof(struct tx_isp_subdev_link_config32) ==
	       TX_ISP_SUBDEV_LINK_CONFIG_SIZE);
	expect(offsetof(struct tx_isp_subdev_link_config32, source) ==
	       TX_ISP_SUBDEV_LINK_SOURCE_OFFSET);
	expect(offsetof(struct tx_isp_subdev_link_config32, sink) ==
	       TX_ISP_SUBDEV_LINK_SINK_OFFSET);
	expect(offsetof(struct tx_isp_subdev_link_config32, flag) ==
	       TX_ISP_SUBDEV_LINK_FLAG_OFFSET);

	expect(sizeof(struct tx_isp_subdev_link_set32) ==
	       TX_ISP_SUBDEV_LINK_SET_SIZE);
	expect(offsetof(struct tx_isp_subdev_link_set32, records) ==
	       TX_ISP_SUBDEV_LINK_SET_RECORDS_OFFSET);
	expect(offsetof(struct tx_isp_subdev_link_set32, count) ==
	       TX_ISP_SUBDEV_LINK_SET_COUNT_OFFSET);
}

int main(void)
{
	unsigned char source_pads[2][FAKE_PAD_STRIDE] = { { 0 } };
	unsigned char sink_pads[1][FAKE_PAD_STRIDE] = { { 0 } };
	struct fake_subdev source = {
		.name = "isp-source",
		.output_count = 2,
		.output_pads = &source_pads[0][0],
	};
	struct fake_subdev sink = {
		.name = "isp-sink",
		.input_count = 1,
		.input_pads = &sink_pads[0][0],
	};
	struct fake_subdev empty = {
		.name = "isp-empty",
		.output_count = 1,
	};
	struct fake_graph graph = {
		.subdevs = { 0, &source, &sink, &empty },
	};
	struct tx_isp_subdev_pad_descriptor descriptor = {
		.name = "isp-source",
		.type = TX_ISP_SUBDEV_PAD_OUTPUT,
		.index = 1,
	};
	enum tx_isp_subdev_resolve_status status;
	void *pad;

	test_wire_layout();
	pad = tx_isp_subdev_resolve_pad(
		&graph, &descriptor, &fake_ops, &status);
	expect(pad == &source_pads[1][0]);
	expect(status == TX_ISP_SUBDEV_RESOLVE_OK);

	descriptor.name = "isp-sink";
	descriptor.type = TX_ISP_SUBDEV_PAD_INPUT;
	descriptor.index = 0;
	pad = tx_isp_subdev_resolve_pad(
		&graph, &descriptor, &fake_ops, &status);
	expect(pad == &sink_pads[0][0]);
	expect(status == TX_ISP_SUBDEV_RESOLVE_OK);

	descriptor.index = 1;
	expect(!tx_isp_subdev_resolve_pad(
		&graph, &descriptor, &fake_ops, &status));
	expect(status == TX_ISP_SUBDEV_RESOLVE_INDEX_RANGE);

	descriptor.name = "isp-empty";
	descriptor.type = TX_ISP_SUBDEV_PAD_OUTPUT;
	descriptor.index = 0;
	expect(!tx_isp_subdev_resolve_pad(
		&graph, &descriptor, &fake_ops, &status));
	expect(status == TX_ISP_SUBDEV_RESOLVE_NO_PADS);

	descriptor.name = "missing";
	expect(!tx_isp_subdev_resolve_pad(
		&graph, &descriptor, &fake_ops, &status));
	expect(status == TX_ISP_SUBDEV_RESOLVE_NOT_FOUND);

	descriptor.name = "isp-source";
	descriptor.type = 9;
	expect(!tx_isp_subdev_resolve_pad(
		&graph, &descriptor, &fake_ops, &status));
	expect(status == TX_ISP_SUBDEV_RESOLVE_BAD_TYPE);

	expect(!tx_isp_subdev_resolve_pad(
		0, &descriptor, &fake_ops, &status));
	expect(status == TX_ISP_SUBDEV_RESOLVE_INVALID);

	puts("tx_isp_subdev_test: ok");
	return 0;
}
