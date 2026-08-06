#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "tx_isp/tx_isp_video_queue.h"

#define TEST_TYPE 1U
#define TEST_MEMORY 1U
#define TEST_SIZE 0x546000U

static void configure_queue(struct tx_isp_video_queue *queue,
			    struct tx_isp_video_slot *slots)
{
	assert(tx_isp_video_queue_init(queue, slots, 4) == 0);
	assert(tx_isp_video_queue_configure(queue, 3, TEST_TYPE, TEST_MEMORY,
					    TEST_SIZE) == 0);
	assert(tx_isp_video_queue_prepare(queue, 0, 0x06052000U,
					  TEST_SIZE) == 0);
	assert(tx_isp_video_queue_prepare(queue, 1, 0x06598000U,
					  TEST_SIZE) == 0);
	assert(tx_isp_video_queue_prepare(queue, 2, 0x06ae0000U,
					  TEST_SIZE) == 0);
}

static void test_validation(void)
{
	struct tx_isp_video_queue queue;
	struct tx_isp_video_slot slots[4];
	u32 index;

	assert(tx_isp_video_queue_init(NULL, slots, 4) == -EINVAL);
	assert(tx_isp_video_queue_init(&queue, NULL, 4) == -EINVAL);
	assert(tx_isp_video_queue_init(&queue, slots,
				       TX_ISP_VIDEO_QUEUE_MAX_BUFFERS + 1) == -EINVAL);
	configure_queue(&queue, slots);
	assert(tx_isp_video_queue_prepare(&queue, 3, 0, TEST_SIZE) == -EINVAL);
	assert(tx_isp_video_queue_prepare(&queue, 0, 0, TEST_SIZE - 1) ==
	       -EINVAL);
	assert(tx_isp_video_queue_qbuf(&queue, 0) == 0);
	assert(tx_isp_video_queue_take(&queue, &index) == -EINVAL);
	assert(tx_isp_video_queue_stream_on(&queue) == 0);
	assert(tx_isp_video_queue_stream_on(&queue) == -EINVAL);
	assert(tx_isp_video_queue_qbuf(&queue, 3) == -EINVAL);
	assert(tx_isp_video_queue_take(&queue, &index) == 0 && index == 0);
}

static void test_completion_order_and_wire_metadata(void)
{
	struct tx_isp_video_queue queue;
	struct tx_isp_video_slot slots[4];
	struct tx_isp_frame_buffer_wire buffer;
	u32 index;

	configure_queue(&queue, slots);
	assert(tx_isp_video_queue_qbuf(&queue, 0) == 0);
	assert(tx_isp_video_queue_qbuf(&queue, 1) == 0);
	assert(tx_isp_video_queue_qbuf(&queue, 0) == -EBUSY);
	assert(tx_isp_video_queue_stream_on(&queue) == 0);
	assert(tx_isp_video_queue_take(&queue, &index) == 0 && index == 0);
	assert(tx_isp_video_queue_take(&queue, &index) == 0 && index == 1);

	/* DQBUF follows hardware completion order, not original QBUF order. */
	assert(tx_isp_video_queue_complete(&queue, 1, TEST_SIZE,
					   1200000345000ULL, 0) == 0);
	assert(tx_isp_video_queue_complete(&queue, 0, TEST_SIZE,
					   1200040567000ULL, 1) == 0);
	assert(tx_isp_video_queue_dqbuf(&queue, &buffer) == 0);
	assert(buffer.index == 1);
	assert(buffer.type == TEST_TYPE);
	assert(buffer.memory == TEST_MEMORY);
	assert(buffer.bytesused == TEST_SIZE);
	assert(buffer.flags == TX_ISP_FRAME_FLAG_DONE);
	assert(buffer.sequence == 0);
	assert(buffer.timestamp_sec == 1200);
	assert(buffer.timestamp_usec == 345);
	assert(buffer.dma == 0x06598000U);
	assert(buffer.length == TEST_SIZE);

	memset(&buffer, 0xa5, sizeof(buffer));
	assert(tx_isp_video_queue_dqbuf(&queue, &buffer) == 0);
	assert(buffer.index == 0);
	assert(buffer.flags == TX_ISP_FRAME_FLAG_ERROR);
	assert(buffer.sequence == 1);
	assert(buffer.timestamp_sec == 1200);
	assert(buffer.timestamp_usec == 40567);
	assert(tx_isp_video_queue_dqbuf(&queue, &buffer) == -EAGAIN);

	assert(queue.stats.queued == 2);
	assert(queue.stats.completed == 2);
	assert(queue.stats.dequeued == 2);
	assert(queue.stats.errors == 1);
}

static void test_stream_off_returns_ownership(void)
{
	struct tx_isp_video_queue queue;
	struct tx_isp_video_slot slots[4];
	u32 index;

	configure_queue(&queue, slots);
	assert(tx_isp_video_queue_stream_on(&queue) == 0);
	assert(tx_isp_video_queue_qbuf(&queue, 0) == 0);
	assert(tx_isp_video_queue_qbuf(&queue, 1) == 0);
	assert(tx_isp_video_queue_take(&queue, &index) == 0);
	tx_isp_video_queue_stream_off(&queue);
	assert(queue.streaming == 0);
	assert(queue.queued_count == 0);
	assert(queue.done_count == 0);
	assert(slots[0].state == TX_ISP_FRAME_SLOT_FREE);
	assert(slots[1].state == TX_ISP_FRAME_SLOT_FREE);
	assert(slots[0].dma == 0x06052000U);
	assert(queue.stats.stream_stops == 1);
	assert(tx_isp_video_queue_stream_on(&queue) == 0);
	assert(tx_isp_video_queue_qbuf(&queue, 0) == 0);
}

int main(void)
{
	test_validation();
	test_completion_order_and_wire_metadata();
	test_stream_off_returns_ownership();
	puts("tx_isp_video_queue tests passed");
	return 0;
}
