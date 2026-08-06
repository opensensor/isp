#ifdef __KERNEL__
#include <linux/errno.h>
#include <linux/math64.h>
#include <linux/string.h>
#else
#include <errno.h>
#include <string.h>
#endif

#include "../include/tx_isp/tx_isp_video_queue.h"

static void tx_isp_video_ring_reset(struct tx_isp_video_queue *queue)
{
	queue->queued_head = 0;
	queue->queued_tail = 0;
	queue->queued_count = 0;
	queue->done_head = 0;
	queue->done_tail = 0;
	queue->done_count = 0;
}

int tx_isp_video_queue_init(struct tx_isp_video_queue *queue,
			    struct tx_isp_video_slot *slots, u32 capacity)
{
	if (!queue || !slots || !capacity ||
	    capacity > TX_ISP_VIDEO_QUEUE_MAX_BUFFERS)
		return -EINVAL;

	memset(queue, 0, sizeof(*queue));
	memset(slots, 0, capacity * sizeof(*slots));
	queue->slots = slots;
	queue->capacity = capacity;
	return 0;
}

int tx_isp_video_queue_configure(struct tx_isp_video_queue *queue, u32 count,
				 u32 type, u32 memory, u32 sizeimage)
{
	u32 index;

	if (!queue || !queue->slots || queue->streaming || !count ||
	    count > queue->capacity || !type || !memory || !sizeimage)
		return -EINVAL;

	for (index = 0; index < queue->capacity; index++)
		memset(&queue->slots[index], 0, sizeof(queue->slots[index]));
	queue->count = count;
	queue->type = type;
	queue->memory = memory;
	queue->sizeimage = sizeimage;
	queue->next_sequence = 0;
	memset(&queue->stats, 0, sizeof(queue->stats));
	tx_isp_video_ring_reset(queue);
	return 0;
}

int tx_isp_video_queue_prepare(struct tx_isp_video_queue *queue, u32 index,
			       u32 dma, u32 length)
{
	struct tx_isp_video_slot *slot;

	if (!queue || !queue->slots || queue->streaming ||
	    index >= queue->count || length < queue->sizeimage)
		return -EINVAL;

	slot = &queue->slots[index];
	if (slot->state != TX_ISP_FRAME_SLOT_FREE)
		return -EBUSY;
	slot->dma = dma;
	slot->length = length;
	return 0;
}

int tx_isp_video_queue_stream_on(struct tx_isp_video_queue *queue)
{
	if (!queue || !queue->slots || !queue->count || queue->streaming)
		return -EINVAL;
	queue->next_sequence = 0;
	queue->streaming = 1;
	queue->stats.stream_starts++;
	return 0;
}

void tx_isp_video_queue_stream_off(struct tx_isp_video_queue *queue)
{
	u32 index;

	if (!queue || !queue->slots)
		return;
	for (index = 0; index < queue->count; index++) {
		queue->slots[index].state = TX_ISP_FRAME_SLOT_FREE;
		queue->slots[index].bytesused = 0;
		queue->slots[index].timestamp_ns = 0;
	}
	tx_isp_video_ring_reset(queue);
	if (queue->streaming)
		queue->stats.stream_stops++;
	queue->streaming = 0;
}

int tx_isp_video_queue_qbuf(struct tx_isp_video_queue *queue, u32 index)
{
	struct tx_isp_video_slot *slot;

	if (!queue || !queue->slots || !queue->count ||
	    index >= queue->count || queue->queued_count >= queue->count)
		return -EINVAL;
	slot = &queue->slots[index];
	if (slot->state != TX_ISP_FRAME_SLOT_FREE ||
	    slot->length < queue->sizeimage)
		return -EBUSY;

	slot->state = TX_ISP_FRAME_SLOT_QUEUED;
	slot->bytesused = 0;
	queue->queued_ring[queue->queued_tail] = index;
	queue->queued_tail = (queue->queued_tail + 1U) % queue->capacity;
	queue->queued_count++;
	queue->stats.queued++;
	return 0;
}

int tx_isp_video_queue_take(struct tx_isp_video_queue *queue, u32 *index)
{
	struct tx_isp_video_slot *slot;
	u32 next;

	if (!queue || !queue->slots || !index || !queue->streaming)
		return -EINVAL;
	if (!queue->queued_count)
		return -EAGAIN;

	next = queue->queued_ring[queue->queued_head];
	queue->queued_head = (queue->queued_head + 1U) % queue->capacity;
	queue->queued_count--;
	if (next >= queue->count)
		return -EIO;
	slot = &queue->slots[next];
	if (slot->state != TX_ISP_FRAME_SLOT_QUEUED)
		return -EIO;
	slot->state = TX_ISP_FRAME_SLOT_ACTIVE;
	*index = next;
	return 0;
}

int tx_isp_video_queue_complete(struct tx_isp_video_queue *queue, u32 index,
				u32 bytesused, u64 timestamp_ns,
				int buffer_error)
{
	struct tx_isp_video_slot *slot;

	if (!queue || !queue->slots || !queue->streaming ||
	    index >= queue->count || queue->done_count >= queue->count)
		return -EINVAL;
	slot = &queue->slots[index];
	if (slot->state != TX_ISP_FRAME_SLOT_ACTIVE)
		return -EBUSY;
	if (bytesused > slot->length)
		return -ENOSPC;

	slot->bytesused = bytesused;
	slot->timestamp_ns = timestamp_ns;
	slot->sequence = queue->next_sequence++;
	slot->state = buffer_error ? TX_ISP_FRAME_SLOT_ERROR :
		TX_ISP_FRAME_SLOT_DONE;
	queue->done_ring[queue->done_tail] = index;
	queue->done_tail = (queue->done_tail + 1U) % queue->capacity;
	queue->done_count++;
	queue->stats.completed++;
	if (buffer_error)
		queue->stats.errors++;
	return 0;
}

int tx_isp_video_queue_dqbuf(struct tx_isp_video_queue *queue,
			     struct tx_isp_frame_buffer_wire *buffer)
{
	struct tx_isp_video_slot *slot;
	u32 index;
	u32 error;
#ifdef __KERNEL__
	u32 timestamp_remainder;
#endif

	if (!queue || !queue->slots || !buffer)
		return -EINVAL;
	if (!queue->done_count)
		return -EAGAIN;

	index = queue->done_ring[queue->done_head];
	if (index >= queue->count)
		return -EIO;
	slot = &queue->slots[index];
	if (slot->state != TX_ISP_FRAME_SLOT_DONE &&
	    slot->state != TX_ISP_FRAME_SLOT_ERROR)
		return -EIO;
	error = slot->state == TX_ISP_FRAME_SLOT_ERROR;
	queue->done_head = (queue->done_head + 1U) % queue->capacity;
	queue->done_count--;

	memset(buffer, 0, sizeof(*buffer));
	buffer->index = index;
	buffer->type = queue->type;
	buffer->bytesused = slot->bytesused;
	buffer->flags = error ? TX_ISP_FRAME_FLAG_ERROR : TX_ISP_FRAME_FLAG_DONE;
#ifdef __KERNEL__
	buffer->timestamp_sec = (u32)div_u64_rem(slot->timestamp_ns,
		1000000000U, &timestamp_remainder);
	buffer->timestamp_usec = timestamp_remainder / 1000U;
#else
	buffer->timestamp_sec = (u32)(slot->timestamp_ns / 1000000000ULL);
	buffer->timestamp_usec =
		(u32)((slot->timestamp_ns % 1000000000ULL) / 1000ULL);
#endif
	buffer->sequence = slot->sequence;
	buffer->memory = queue->memory;
	buffer->dma = slot->dma;
	buffer->length = slot->length;

	slot->state = TX_ISP_FRAME_SLOT_FREE;
	queue->stats.dequeued++;
	return 0;
}
