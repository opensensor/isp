#ifndef TX_ISP_FRAME_ABI_H
#define TX_ISP_FRAME_ABI_H

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
typedef uint32_t u32;
#endif

/*
 * T23, T31, and T41 all expose the 32-bit MIPS v4l2_buffer layout through
 * their private frame-channel ioctls.  Keep the wire layout independent of
 * the build host's pointer and timeval sizes.
 */
enum tx_isp_frame_buffer_word {
	TX_ISP_FRAME_WORD_INDEX = 0,
	TX_ISP_FRAME_WORD_TYPE,
	TX_ISP_FRAME_WORD_BYTESUSED,
	TX_ISP_FRAME_WORD_FLAGS,
	TX_ISP_FRAME_WORD_FIELD,
	TX_ISP_FRAME_WORD_TIMESTAMP_SEC,
	TX_ISP_FRAME_WORD_TIMESTAMP_USEC,
	TX_ISP_FRAME_WORD_TIMECODE_TYPE,
	TX_ISP_FRAME_WORD_TIMECODE_FLAGS,
	TX_ISP_FRAME_WORD_TIMECODE_FRAMES,
	TX_ISP_FRAME_WORD_TIMECODE_USERBITS,
	TX_ISP_FRAME_WORD_SEQUENCE,
	TX_ISP_FRAME_WORD_MEMORY,
	TX_ISP_FRAME_WORD_DMA,
	TX_ISP_FRAME_WORD_LENGTH,
	TX_ISP_FRAME_WORD_RESERVED2,
	TX_ISP_FRAME_WORD_RESERVED,
	TX_ISP_FRAME_WORD_COUNT,
};

struct tx_isp_frame_buffer_wire {
	u32 index;
	u32 type;
	u32 bytesused;
	u32 flags;
	u32 field;
	u32 timestamp_sec;
	u32 timestamp_usec;
	u32 timecode_type;
	u32 timecode_flags;
	u32 timecode_frames;
	u32 timecode_userbits;
	u32 sequence;
	u32 memory;
	u32 dma;
	u32 length;
	u32 reserved2;
	u32 reserved;
};

#define TX_ISP_FRAME_BUFFER_BYTES		0x44U
#define TX_ISP_FRAME_BUFFER_PREFIX_BYTES	0x34U

/* V4L2 buffer flags used by every recovered frame-channel implementation. */
#define TX_ISP_FRAME_FLAG_QUEUED		0x00000002U
#define TX_ISP_FRAME_FLAG_DONE			0x00000004U
#define TX_ISP_FRAME_FLAG_ERROR			0x00000040U
#define TX_ISP_FRAME_FLAG_RETAIN_MASK		0xffff1bb8U

#define TX_ISP_FRAME_STATE_BIT(state) \
	((state) < 32U ? 1U << (state) : 0U)

#define TX_ISP_FRAME_T31_QUEUED_STATES		0U
#define TX_ISP_FRAME_T31_DONE_STATES		TX_ISP_FRAME_STATE_BIT(3U)
#define TX_ISP_FRAME_T31_ERROR_STATES		TX_ISP_FRAME_STATE_BIT(4U)

#define TX_ISP_FRAME_T41_QUEUED_STATES \
	(TX_ISP_FRAME_STATE_BIT(1U) | TX_ISP_FRAME_STATE_BIT(3U))
#define TX_ISP_FRAME_T41_DONE_STATES		TX_ISP_FRAME_STATE_BIT(4U)
#define TX_ISP_FRAME_T41_ERROR_STATES		TX_ISP_FRAME_STATE_BIT(5U)

/*
 * Recovered monoliths can use this statement form when an inline call changes
 * register allocation beyond the flag block.  Its expansion deliberately
 * matches the T41 vendor operation order.
 */
#define TX_ISP_FRAME_FLAGS_T41_UPDATE(					\
	flags, queue_flags, state, state_value) do {			\
	(flags) &= TX_ISP_FRAME_FLAG_RETAIN_MASK;			\
	(flags) |= (queue_flags);					\
	(state) = (state_value);						\
	if ((state) == 1U || (state) == 3U)				\
		(flags) |= TX_ISP_FRAME_FLAG_QUEUED;			\
	else if ((state) == 4U)						\
		(flags) |= TX_ISP_FRAME_FLAG_DONE;			\
	else if ((state) == 5U)						\
		(flags) |= TX_ISP_FRAME_FLAG_ERROR;			\
} while (0)

/*
 * Merge persistent/user flags, queue flags, and one generation's state map.
 * Priority matches the vendor implementations when malformed policies overlap:
 * QUEUED, then DONE, then ERROR.
 */
static inline u32 tx_isp_frame_flags_build(
	u32 flags, u32 queue_flags, u32 state, u32 queued_states,
	u32 done_states, u32 error_states)
{
	u32 state_bit = TX_ISP_FRAME_STATE_BIT(state);

	flags = (flags & TX_ISP_FRAME_FLAG_RETAIN_MASK) | queue_flags;
	if (state_bit & queued_states)
		flags |= TX_ISP_FRAME_FLAG_QUEUED;
	else if (state_bit & done_states)
		flags |= TX_ISP_FRAME_FLAG_DONE;
	else if (state_bit & error_states)
		flags |= TX_ISP_FRAME_FLAG_ERROR;
	return flags;
}

static inline u32 tx_isp_frame_flags_t31(u32 flags, u32 queue_flags,
					 u32 state)
{
	flags = (flags & TX_ISP_FRAME_FLAG_RETAIN_MASK) | queue_flags;
	if (state == 3U)
		flags |= TX_ISP_FRAME_FLAG_DONE;
	else if (state == 4U)
		flags |= TX_ISP_FRAME_FLAG_ERROR;
	return flags;
}

static inline u32 tx_isp_frame_flags_t41(u32 flags, u32 queue_flags,
					 u32 state)
{
	flags = (flags & TX_ISP_FRAME_FLAG_RETAIN_MASK) | queue_flags;
	if (state == 1U || state == 3U)
		flags |= TX_ISP_FRAME_FLAG_QUEUED;
	else if (state == 4U)
		flags |= TX_ISP_FRAME_FLAG_DONE;
	else if (state == 5U)
		flags |= TX_ISP_FRAME_FLAG_ERROR;
	return flags;
}

#endif /* TX_ISP_FRAME_ABI_H */
