#ifndef TX_ISP_FRAME_CHANNEL_H
#define TX_ISP_FRAME_CHANNEL_H

#include "tx_isp_frame_abi.h"
#include "tx_isp_frame_format.h"

/* Frame-source remote event namespace shared by all recovered generations. */
#define TX_ISP_FRAME_EVENT_BYPASS_ISP		0x03000000U
#define TX_ISP_FRAME_EVENT_GET_FORMAT		0x03000001U
#define TX_ISP_FRAME_EVENT_SET_FORMAT		0x03000002U
#define TX_ISP_FRAME_EVENT_STREAM_ON		0x03000003U
#define TX_ISP_FRAME_EVENT_STREAM_OFF		0x03000004U
#define TX_ISP_FRAME_EVENT_QUEUE_BUFFER		0x03000005U
#define TX_ISP_FRAME_EVENT_BUFFER_DONE		0x03000006U

/* Buffer ownership values recovered from the legacy frame-channel ABI. */
enum tx_isp_frame_slot_state {
	TX_ISP_FRAME_SLOT_FREE = 0,
	TX_ISP_FRAME_SLOT_QUEUED = 1,
	TX_ISP_FRAME_SLOT_ACTIVE = 3,
	TX_ISP_FRAME_SLOT_DONE = 4,
};

/*
 * REQBUFS is the same five-word wire object in every recovered generation.
 * Allocation ownership and the returned count remain generation-specific.
 */
enum tx_isp_frame_request_word {
	TX_ISP_FRAME_REQUEST_WORD_COUNT = 0,
	TX_ISP_FRAME_REQUEST_WORD_TYPE,
	TX_ISP_FRAME_REQUEST_WORD_MEMORY,
	TX_ISP_FRAME_REQUEST_WORD_CAPABILITIES,
	TX_ISP_FRAME_REQUEST_WORD_RESERVED,
	TX_ISP_FRAME_REQUEST_WORD_COUNT_TOTAL,
};

struct tx_isp_frame_request_wire {
	u32 count;
	u32 type;
	u32 memory;
	u32 capabilities;
	u32 reserved;
};

#define TX_ISP_FRAME_REQUEST_BYTES		0x14U
#define TX_ISP_FRAME_STREAM_TYPE_BYTES		0x04U

/*
 * T23/T31 retain the V4L2 ('V') ioctl family.  T41 uses its private 'T'
 * family and the 116-byte extended format.
 */
#define TX_ISP_FRAME_IOCTL_LEGACY_SET_FORMAT	0xc07056c3U
#define TX_ISP_FRAME_IOCTL_LEGACY_GET_FORMAT	0x407056c4U
#define TX_ISP_FRAME_IOCTL_LEGACY_REQBUFS	0xc0145608U
#define TX_ISP_FRAME_IOCTL_LEGACY_QUERYBUF	0xc0445609U
#define TX_ISP_FRAME_IOCTL_LEGACY_QBUF		0xc044560fU
#define TX_ISP_FRAME_IOCTL_LEGACY_DQBUF		0xc0445611U
#define TX_ISP_FRAME_IOCTL_LEGACY_STREAM_ON	0x80045612U
#define TX_ISP_FRAME_IOCTL_LEGACY_STREAM_OFF	0x80045613U
#define TX_ISP_FRAME_IOCTL_LEGACY_WAIT		0x400456bfU

#define TX_ISP_FRAME_IOCTL_T41_SET_FORMAT	0xc0745451U
#define TX_ISP_FRAME_IOCTL_T41_GET_FORMAT	0xc0745452U
#define TX_ISP_FRAME_IOCTL_T41_REQBUFS		0xc0145453U
#define TX_ISP_FRAME_IOCTL_T41_QUERYBUF		0xc0445454U
#define TX_ISP_FRAME_IOCTL_T41_QBUF		0xc0445455U
#define TX_ISP_FRAME_IOCTL_T41_DQBUF		0xc0445456U
#define TX_ISP_FRAME_IOCTL_T41_STREAM_ON		0xc0045457U
#define TX_ISP_FRAME_IOCTL_T41_STREAM_OFF	0xc0045458U
#define TX_ISP_FRAME_IOCTL_T41_SET_BANKS		0xc0045459U
#define TX_ISP_FRAME_IOCTL_T41_GET_COUNT		0xc004545aU
#define TX_ISP_FRAME_IOCTL_T41_SET_ALIGN		0xc008545bU

#define TX_ISP_IOCTL_NUMBER_MASK		0xffU
#define TX_ISP_IOCTL_TYPE_SHIFT			8U
#define TX_ISP_IOCTL_SIZE_SHIFT			16U
#define TX_ISP_IOCTL_SIZE_MASK			0x3fffU
#define TX_ISP_IOCTL_DIRECTION_SHIFT		30U
#define TX_ISP_IOCTL_DIRECTION_MASK		0x3U

static inline u32 tx_isp_ioctl_number(u32 command)
{
	return command & TX_ISP_IOCTL_NUMBER_MASK;
}

static inline u32 tx_isp_ioctl_type(u32 command)
{
	return (command >> TX_ISP_IOCTL_TYPE_SHIFT) & 0xffU;
}

static inline u32 tx_isp_ioctl_payload_bytes(u32 command)
{
	return (command >> TX_ISP_IOCTL_SIZE_SHIFT) &
		TX_ISP_IOCTL_SIZE_MASK;
}

static inline u32 tx_isp_ioctl_direction(u32 command)
{
	return (command >> TX_ISP_IOCTL_DIRECTION_SHIFT) &
		TX_ISP_IOCTL_DIRECTION_MASK;
}

#endif /* TX_ISP_FRAME_CHANNEL_H */
