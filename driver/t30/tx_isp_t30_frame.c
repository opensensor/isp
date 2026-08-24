/* T30 layout adapter for the shared 32-bit frame-buffer ABI. */

#include <linux/string.h>
#include <linux/types.h>

#include "../include/tx_isp/tx_isp_frame_abi.h"
#include "tx_isp_t30_frame.h"

#define TX_ISP_T30_VB_QUEUE_OFFSET		0x44U
#define TX_ISP_T30_VB_STATE_OFFSET		0x48U
#define TX_ISP_T30_QUEUE_FLAGS_OFFSET		0x14U

s32 __fill_v4l2_buffer(void *source, void *destination)
{
	u32 *source_words = source;
	u32 *destination_words = destination;
	u32 *queue;
	u32 state;
	u32 flags;

	queue = *(u32 **)((char *)source + TX_ISP_T30_VB_QUEUE_OFFSET);
	memcpy(destination, source, TX_ISP_FRAME_BUFFER_PREFIX_BYTES);
	destination_words[TX_ISP_FRAME_WORD_RESERVED2] =
		source_words[TX_ISP_FRAME_WORD_RESERVED2];
	destination_words[TX_ISP_FRAME_WORD_RESERVED] =
		source_words[TX_ISP_FRAME_WORD_RESERVED];

	state = *(u32 *)((char *)source + TX_ISP_T30_VB_STATE_OFFSET);
	flags = tx_isp_frame_flags_t30(
		destination_words[TX_ISP_FRAME_WORD_FLAGS],
		*(u32 *)((char *)queue + TX_ISP_T30_QUEUE_FLAGS_OFFSET), state);
	destination_words[TX_ISP_FRAME_WORD_FLAGS] = flags;
	return (s32)flags;
}
