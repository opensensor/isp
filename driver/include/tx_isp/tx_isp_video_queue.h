#ifndef TX_ISP_VIDEO_QUEUE_H
#define TX_ISP_VIDEO_QUEUE_H

#include "tx_isp_frame_channel.h"

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
typedef uint64_t u64;
#endif

/*
 * Allocation-free queue core shared by the private frame-channel ABI and a
 * future public V4L2 capture node.  The embedding adapter owns serialization:
 * kernel callers normally hold the vb2/frame-channel queue lock.
 */
#define TX_ISP_VIDEO_QUEUE_MAX_BUFFERS 32U

struct tx_isp_video_slot {
	u32 dma;
	u32 length;
	u32 bytesused;
	u32 sequence;
	u64 timestamp_ns;
	u32 state;
};

struct tx_isp_video_queue_stats {
	u32 queued;
	u32 completed;
	u32 dequeued;
	u32 errors;
	u32 stream_starts;
	u32 stream_stops;
};

struct tx_isp_video_queue {
	struct tx_isp_video_slot *slots;
	u32 capacity;
	u32 count;
	u32 type;
	u32 memory;
	u32 sizeimage;
	u32 streaming;
	u32 next_sequence;
	u32 queued_ring[TX_ISP_VIDEO_QUEUE_MAX_BUFFERS];
	u32 queued_head;
	u32 queued_tail;
	u32 queued_count;
	u32 done_ring[TX_ISP_VIDEO_QUEUE_MAX_BUFFERS];
	u32 done_head;
	u32 done_tail;
	u32 done_count;
	struct tx_isp_video_queue_stats stats;
};

int tx_isp_video_queue_init(struct tx_isp_video_queue *queue,
			    struct tx_isp_video_slot *slots, u32 capacity);
int tx_isp_video_queue_configure(struct tx_isp_video_queue *queue, u32 count,
				 u32 type, u32 memory, u32 sizeimage);
int tx_isp_video_queue_prepare(struct tx_isp_video_queue *queue, u32 index,
			       u32 dma, u32 length);
int tx_isp_video_queue_stream_on(struct tx_isp_video_queue *queue);
void tx_isp_video_queue_stream_off(struct tx_isp_video_queue *queue);
int tx_isp_video_queue_qbuf(struct tx_isp_video_queue *queue, u32 index);
int tx_isp_video_queue_take(struct tx_isp_video_queue *queue, u32 *index);
int tx_isp_video_queue_complete(struct tx_isp_video_queue *queue, u32 index,
				u32 bytesused, u64 timestamp_ns,
				int buffer_error);
int tx_isp_video_queue_dqbuf(struct tx_isp_video_queue *queue,
			     struct tx_isp_frame_buffer_wire *buffer);

#endif /* TX_ISP_VIDEO_QUEUE_H */
