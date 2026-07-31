#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "tx_isp/tx_isp_frame_abi.h"

#define ABI_ASSERT(name, expression) \
	typedef char abi_assert_##name[(expression) ? 1 : -1]

ABI_ASSERT(buffer_size,
	sizeof(struct tx_isp_frame_buffer_wire) == TX_ISP_FRAME_BUFFER_BYTES);
ABI_ASSERT(word_count,
	TX_ISP_FRAME_WORD_COUNT * sizeof(uint32_t) ==
	TX_ISP_FRAME_BUFFER_BYTES);
ABI_ASSERT(flags_offset,
	offsetof(struct tx_isp_frame_buffer_wire, flags) ==
	TX_ISP_FRAME_WORD_FLAGS * sizeof(uint32_t));
ABI_ASSERT(timestamp_offset,
	offsetof(struct tx_isp_frame_buffer_wire, timestamp_sec) ==
	TX_ISP_FRAME_WORD_TIMESTAMP_SEC * sizeof(uint32_t));
ABI_ASSERT(sequence_offset,
	offsetof(struct tx_isp_frame_buffer_wire, sequence) ==
	TX_ISP_FRAME_WORD_SEQUENCE * sizeof(uint32_t));
ABI_ASSERT(dma_offset,
	offsetof(struct tx_isp_frame_buffer_wire, dma) ==
	TX_ISP_FRAME_WORD_DMA * sizeof(uint32_t));
ABI_ASSERT(reserved_offset,
	offsetof(struct tx_isp_frame_buffer_wire, reserved) ==
	TX_ISP_FRAME_WORD_RESERVED * sizeof(uint32_t));

static void test_t31_states(void)
{
	const uint32_t source = 0xffffffffU;
	const uint32_t queue = 0x00000401U;
	const uint32_t base =
		(source & TX_ISP_FRAME_FLAG_RETAIN_MASK) | queue;

	assert(tx_isp_frame_flags_t31(source, queue, 0) == base);
	assert(tx_isp_frame_flags_t31(source, queue, 3) ==
	       (base | TX_ISP_FRAME_FLAG_DONE));
	assert(tx_isp_frame_flags_t31(source, queue, 4) ==
	       (base | TX_ISP_FRAME_FLAG_ERROR));
	assert(tx_isp_frame_flags_t31(source, queue, 5) == base);
	assert(tx_isp_frame_flags_t31(source, queue, 32) == base);
}

static void test_t41_states(void)
{
	const uint32_t source = 0x12345678U;
	const uint32_t queue = 0x00008001U;
	const uint32_t base =
		(source & TX_ISP_FRAME_FLAG_RETAIN_MASK) | queue;

	assert(tx_isp_frame_flags_t41(source, queue, 0) == base);
	assert(tx_isp_frame_flags_t41(source, queue, 1) ==
	       (base | TX_ISP_FRAME_FLAG_QUEUED));
	assert(tx_isp_frame_flags_t41(source, queue, 3) ==
	       (base | TX_ISP_FRAME_FLAG_QUEUED));
	assert(tx_isp_frame_flags_t41(source, queue, 4) ==
	       (base | TX_ISP_FRAME_FLAG_DONE));
	assert(tx_isp_frame_flags_t41(source, queue, 5) ==
	       (base | TX_ISP_FRAME_FLAG_ERROR));
	assert(tx_isp_frame_flags_t41(source, queue, 31) == base);
	assert(tx_isp_frame_flags_t41(source, queue, 32) == base);
}

static void test_t41_statement_adapter(void)
{
	const uint32_t queue_values[] = { 0x00000401U };
	const uint32_t state_values[] = { 4U };
	uint32_t flags = 0xffffffffU;
	unsigned int queue_index = 0;
	unsigned int state_index = 0;
	uint32_t state = 99U;

	TX_ISP_FRAME_FLAGS_T41_UPDATE(
		flags, queue_values[queue_index++], state,
		state_values[state_index++]);
	assert(queue_index == 1);
	assert(state_index == 1);
	assert(state == 4);
	assert(flags == ((0xffffffffU & TX_ISP_FRAME_FLAG_RETAIN_MASK) |
			 0x00000401U | TX_ISP_FRAME_FLAG_DONE));
}

static void test_policy_priority(void)
{
	const uint32_t overlap = TX_ISP_FRAME_STATE_BIT(7U);

	assert(tx_isp_frame_flags_build(0, 0, 7, overlap, overlap, overlap) ==
	       TX_ISP_FRAME_FLAG_QUEUED);
	assert(tx_isp_frame_flags_build(0, 0, 7, 0, overlap, overlap) ==
	       TX_ISP_FRAME_FLAG_DONE);
	assert(tx_isp_frame_flags_build(0, 0, 7, 0, 0, overlap) ==
	       TX_ISP_FRAME_FLAG_ERROR);
}

int main(void)
{
	test_t31_states();
	test_t41_states();
	test_t41_statement_adapter();
	test_policy_priority();
	puts("tx_isp_frame_abi tests passed");
	return 0;
}
