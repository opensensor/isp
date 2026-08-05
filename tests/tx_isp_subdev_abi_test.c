#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "tx_isp/tx_isp_subdev_abi.h"
#include "tx_isp/tx_isp_subdev.h"

struct tx_isp_abi_link32 {
	uint32_t source;
	uint32_t sink;
	uint32_t reverse;
	uint32_t flag;
	uint32_t state;
};

struct tx_isp_abi_pad32 {
	uint32_t owner;
	uint8_t index;
	uint8_t type;
	uint8_t links_type;
	uint8_t state;
	struct tx_isp_abi_link32 link;
	uint32_t event;
	uint32_t private_data;
};

struct tx_isp_abi_legacy_subdev32 {
	uint8_t prefix[TX_ISP_ABI_LEGACY_SUBDEV_IRQDEV_OFFSET];
	uint8_t irqdev[TX_ISP_ABI_LEGACY_SUBDEV_CLOCKS_OFFSET -
		TX_ISP_ABI_LEGACY_SUBDEV_IRQDEV_OFFSET];
	uint32_t clocks;
	uint32_t clk_num;
	uint32_t ops;
	uint16_t pad_count0;
	uint16_t pad_count1;
	uint32_t pad_ptr0;
	uint32_t pad_ptr1;
	uint32_t private0;
	uint32_t private1;
};

struct tx_isp_abi_t40_subdev32 {
	uint8_t prefix[TX_ISP_ABI_T40_SUBDEV_IRQDEV_OFFSET];
	uint8_t irqdev[TX_ISP_ABI_T40_SUBDEV_RES_OFFSET -
		TX_ISP_ABI_T40_SUBDEV_IRQDEV_OFFSET];
	uint32_t resources[3];
	uint32_t bases[3];
	uint32_t clocks;
	uint32_t clk_num;
	uint32_t ops;
	uint16_t pad_count0;
	uint16_t pad_count1;
	uint32_t pad_ptr0;
	uint32_t pad_ptr1;
};

TX_ISP_ABI_ASSERT(host_link_source,
	offsetof(struct tx_isp_abi_link32, source) ==
	TX_ISP_ABI_LINK_SOURCE_OFFSET);
TX_ISP_ABI_ASSERT(host_link_sink,
	offsetof(struct tx_isp_abi_link32, sink) ==
	TX_ISP_ABI_LINK_SINK_OFFSET);
TX_ISP_ABI_ASSERT(host_link_reverse,
	offsetof(struct tx_isp_abi_link32, reverse) ==
	TX_ISP_ABI_LINK_REVERSE_OFFSET);
TX_ISP_ABI_ASSERT(host_link_flag,
	offsetof(struct tx_isp_abi_link32, flag) ==
	TX_ISP_ABI_LINK_FLAG_OFFSET);
TX_ISP_ABI_ASSERT(host_link_state,
	offsetof(struct tx_isp_abi_link32, state) ==
	TX_ISP_ABI_LINK_STATE_OFFSET);
TX_ISP_ABI_ASSERT(host_link_size,
	sizeof(struct tx_isp_abi_link32) == TX_ISP_ABI_LINK_SIZE);

TX_ISP_ABI_ASSERT(host_pad_owner,
	offsetof(struct tx_isp_abi_pad32, owner) == TX_ISP_ABI_PAD_OWNER_OFFSET);
TX_ISP_ABI_ASSERT(host_pad_index,
	offsetof(struct tx_isp_abi_pad32, index) == TX_ISP_ABI_PAD_INDEX_OFFSET);
TX_ISP_ABI_ASSERT(host_pad_type,
	offsetof(struct tx_isp_abi_pad32, type) == TX_ISP_ABI_PAD_TYPE_OFFSET);
TX_ISP_ABI_ASSERT(host_pad_links_type,
	offsetof(struct tx_isp_abi_pad32, links_type) == TX_ISP_ABI_PAD_LINKS_TYPE_OFFSET);
TX_ISP_ABI_ASSERT(host_pad_state,
	offsetof(struct tx_isp_abi_pad32, state) == TX_ISP_ABI_PAD_STATE_OFFSET);
TX_ISP_ABI_ASSERT(host_pad_link,
	offsetof(struct tx_isp_abi_pad32, link) == TX_ISP_ABI_PAD_LINK_OFFSET);
TX_ISP_ABI_ASSERT(host_pad_event,
	offsetof(struct tx_isp_abi_pad32, event) == TX_ISP_ABI_PAD_EVENT_OFFSET);
TX_ISP_ABI_ASSERT(host_pad_private,
	offsetof(struct tx_isp_abi_pad32, private_data) == TX_ISP_ABI_PAD_PRIVATE_OFFSET);
TX_ISP_ABI_ASSERT(host_pad_size,
	sizeof(struct tx_isp_abi_pad32) == TX_ISP_ABI_PAD_STRIDE);

TX_ISP_ABI_ASSERT(host_legacy_irqdev,
	offsetof(struct tx_isp_abi_legacy_subdev32, irqdev) ==
	TX_ISP_ABI_LEGACY_SUBDEV_IRQDEV_OFFSET);
TX_ISP_ABI_ASSERT(host_legacy_clocks,
	offsetof(struct tx_isp_abi_legacy_subdev32, clocks) ==
	TX_ISP_ABI_LEGACY_SUBDEV_CLOCKS_OFFSET);
TX_ISP_ABI_ASSERT(host_legacy_clk_num,
	offsetof(struct tx_isp_abi_legacy_subdev32, clk_num) ==
	TX_ISP_ABI_LEGACY_SUBDEV_CLK_NUM_OFFSET);
TX_ISP_ABI_ASSERT(host_legacy_ops,
	offsetof(struct tx_isp_abi_legacy_subdev32, ops) ==
	TX_ISP_ABI_LEGACY_SUBDEV_OPS_OFFSET);
TX_ISP_ABI_ASSERT(host_legacy_pad_count0,
	offsetof(struct tx_isp_abi_legacy_subdev32, pad_count0) ==
	TX_ISP_ABI_LEGACY_SUBDEV_PAD_COUNT0);
TX_ISP_ABI_ASSERT(host_legacy_pad_count1,
	offsetof(struct tx_isp_abi_legacy_subdev32, pad_count1) ==
	TX_ISP_ABI_LEGACY_SUBDEV_PAD_COUNT1);
TX_ISP_ABI_ASSERT(host_legacy_pad_ptr0,
	offsetof(struct tx_isp_abi_legacy_subdev32, pad_ptr0) ==
	TX_ISP_ABI_LEGACY_SUBDEV_PAD_PTR0);
TX_ISP_ABI_ASSERT(host_legacy_pad_ptr1,
	offsetof(struct tx_isp_abi_legacy_subdev32, pad_ptr1) ==
	TX_ISP_ABI_LEGACY_SUBDEV_PAD_PTR1);
TX_ISP_ABI_ASSERT(host_legacy_private0,
	offsetof(struct tx_isp_abi_legacy_subdev32, private0) ==
	TX_ISP_ABI_LEGACY_SUBDEV_PRIVATE0);
TX_ISP_ABI_ASSERT(host_legacy_private1,
	offsetof(struct tx_isp_abi_legacy_subdev32, private1) ==
	TX_ISP_ABI_LEGACY_SUBDEV_PRIVATE1);
TX_ISP_ABI_ASSERT(host_legacy_size,
	sizeof(struct tx_isp_abi_legacy_subdev32) ==
	TX_ISP_ABI_LEGACY_SUBDEV_SIZE);

TX_ISP_ABI_ASSERT(host_t40_irqdev,
	offsetof(struct tx_isp_abi_t40_subdev32, irqdev) ==
	TX_ISP_ABI_T40_SUBDEV_IRQDEV_OFFSET);
TX_ISP_ABI_ASSERT(host_t40_resources,
	offsetof(struct tx_isp_abi_t40_subdev32, resources) ==
	TX_ISP_ABI_T40_SUBDEV_RES_OFFSET);
TX_ISP_ABI_ASSERT(host_t40_bases,
	offsetof(struct tx_isp_abi_t40_subdev32, bases) ==
	TX_ISP_ABI_T40_SUBDEV_BASE_OFFSET);
TX_ISP_ABI_ASSERT(host_t40_clocks,
	offsetof(struct tx_isp_abi_t40_subdev32, clocks) ==
	TX_ISP_ABI_T40_SUBDEV_CLOCKS_OFFSET);
TX_ISP_ABI_ASSERT(host_t40_clk_num,
	offsetof(struct tx_isp_abi_t40_subdev32, clk_num) ==
	TX_ISP_ABI_T40_SUBDEV_CLK_NUM_OFFSET);
TX_ISP_ABI_ASSERT(host_t40_ops,
	offsetof(struct tx_isp_abi_t40_subdev32, ops) ==
	TX_ISP_ABI_T40_SUBDEV_OPS_OFFSET);
TX_ISP_ABI_ASSERT(host_t40_pad_count0,
	offsetof(struct tx_isp_abi_t40_subdev32, pad_count0) ==
	TX_ISP_ABI_T40_SUBDEV_PAD_COUNT0);
TX_ISP_ABI_ASSERT(host_t40_pad_count1,
	offsetof(struct tx_isp_abi_t40_subdev32, pad_count1) ==
	TX_ISP_ABI_T40_SUBDEV_PAD_COUNT1);
TX_ISP_ABI_ASSERT(host_t40_pad_ptr0,
	offsetof(struct tx_isp_abi_t40_subdev32, pad_ptr0) ==
	TX_ISP_ABI_T40_SUBDEV_PAD_PTR0);
TX_ISP_ABI_ASSERT(host_t40_pad_ptr1,
	offsetof(struct tx_isp_abi_t40_subdev32, pad_ptr1) ==
	TX_ISP_ABI_T40_SUBDEV_PAD_PTR1);

static void test_link_detach(void)
{
	struct tx_isp_abi_link32 link = {
		.source = 0x11111111U,
		.sink = 0x22222222U,
		.reverse = 0x33333333U,
		.flag = 0x44444444U,
		.state = 0x55555555U,
	};
	uint32_t source = 0;
	uint32_t reverse = 0;
	uint32_t sink = 0;

	tx_isp_subdev_detach_link(&link, &source, &reverse, &sink);
	if (source != 0x11111111U || reverse != 0x33333333U ||
	    sink != 0x22222222U)
		__builtin_trap();
	if (link.source || link.sink || link.reverse || link.flag)
		__builtin_trap();
	if (link.state != 0x55555555U)
		__builtin_trap();
}

int main(void)
{
	test_link_detach();
	puts("tx_isp_subdev_abi_test: ok");
	return 0;
}
