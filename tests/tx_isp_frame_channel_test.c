#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "tx_isp/tx_isp_frame_channel.h"

static void test_events(void)
{
	assert(TX_ISP_FRAME_EVENT_BYPASS_ISP == 0x03000000U);
	assert(TX_ISP_FRAME_EVENT_GET_FORMAT ==
	       TX_ISP_FRAME_EVENT_BYPASS_ISP + 1);
	assert(TX_ISP_FRAME_EVENT_SET_FORMAT ==
	       TX_ISP_FRAME_EVENT_GET_FORMAT + 1);
	assert(TX_ISP_FRAME_EVENT_STREAM_ON ==
	       TX_ISP_FRAME_EVENT_SET_FORMAT + 1);
	assert(TX_ISP_FRAME_EVENT_STREAM_OFF ==
	       TX_ISP_FRAME_EVENT_STREAM_ON + 1);
	assert(TX_ISP_FRAME_EVENT_QUEUE_BUFFER ==
	       TX_ISP_FRAME_EVENT_STREAM_OFF + 1);
	assert(TX_ISP_FRAME_EVENT_BUFFER_DONE ==
	       TX_ISP_FRAME_EVENT_QUEUE_BUFFER + 1);
}

static void assert_ioctl(uint32_t command, uint32_t type, uint32_t number,
			 uint32_t bytes, uint32_t direction)
{
	assert(tx_isp_ioctl_type(command) == type);
	assert(tx_isp_ioctl_number(command) == number);
	assert(tx_isp_ioctl_payload_bytes(command) == bytes);
	assert(tx_isp_ioctl_direction(command) == direction);
}

static void test_request_wire(void)
{
	assert(sizeof(struct tx_isp_frame_request_wire) ==
	       TX_ISP_FRAME_REQUEST_BYTES);
	assert(offsetof(struct tx_isp_frame_request_wire, count) == 0);
	assert(offsetof(struct tx_isp_frame_request_wire, type) == 4);
	assert(offsetof(struct tx_isp_frame_request_wire, memory) == 8);
	assert(offsetof(struct tx_isp_frame_request_wire, capabilities) == 12);
	assert(offsetof(struct tx_isp_frame_request_wire, reserved) == 16);
	assert(TX_ISP_FRAME_REQUEST_WORD_COUNT_TOTAL == 5);
}

static void test_legacy_ioctls(void)
{
	assert_ioctl(TX_ISP_FRAME_IOCTL_LEGACY_SET_FORMAT, 'V', 0xc3,
		     TX_ISP_FRAME_FORMAT_BYTES, 3);
	assert_ioctl(TX_ISP_FRAME_IOCTL_LEGACY_GET_FORMAT, 'V', 0xc4,
		     TX_ISP_FRAME_FORMAT_BYTES, 1);
	assert_ioctl(TX_ISP_FRAME_IOCTL_LEGACY_REQBUFS, 'V', 0x08,
		     TX_ISP_FRAME_REQUEST_BYTES, 3);
	assert_ioctl(TX_ISP_FRAME_IOCTL_LEGACY_QUERYBUF, 'V', 0x09,
		     TX_ISP_FRAME_BUFFER_BYTES, 3);
	assert_ioctl(TX_ISP_FRAME_IOCTL_LEGACY_QBUF, 'V', 0x0f,
		     TX_ISP_FRAME_BUFFER_BYTES, 3);
	assert_ioctl(TX_ISP_FRAME_IOCTL_LEGACY_DQBUF, 'V', 0x11,
		     TX_ISP_FRAME_BUFFER_BYTES, 3);
	assert_ioctl(TX_ISP_FRAME_IOCTL_LEGACY_STREAM_ON, 'V', 0x12,
		     TX_ISP_FRAME_STREAM_TYPE_BYTES, 2);
	assert_ioctl(TX_ISP_FRAME_IOCTL_LEGACY_STREAM_OFF, 'V', 0x13,
		     TX_ISP_FRAME_STREAM_TYPE_BYTES, 2);
	assert_ioctl(TX_ISP_FRAME_IOCTL_LEGACY_WAIT, 'V', 0xbf, 4, 1);
}

static void test_t41_ioctls(void)
{
	assert_ioctl(TX_ISP_FRAME_IOCTL_T41_SET_FORMAT, 'T', 0x51,
		     TX_ISP_T41_FRAME_FORMAT_BYTES, 3);
	assert_ioctl(TX_ISP_FRAME_IOCTL_T41_GET_FORMAT, 'T', 0x52,
		     TX_ISP_T41_FRAME_FORMAT_BYTES, 3);
	assert_ioctl(TX_ISP_FRAME_IOCTL_T41_REQBUFS, 'T', 0x53,
		     TX_ISP_FRAME_REQUEST_BYTES, 3);
	assert_ioctl(TX_ISP_FRAME_IOCTL_T41_QUERYBUF, 'T', 0x54,
		     TX_ISP_FRAME_BUFFER_BYTES, 3);
	assert_ioctl(TX_ISP_FRAME_IOCTL_T41_QBUF, 'T', 0x55,
		     TX_ISP_FRAME_BUFFER_BYTES, 3);
	assert_ioctl(TX_ISP_FRAME_IOCTL_T41_DQBUF, 'T', 0x56,
		     TX_ISP_FRAME_BUFFER_BYTES, 3);
	assert_ioctl(TX_ISP_FRAME_IOCTL_T41_STREAM_ON, 'T', 0x57,
		     TX_ISP_FRAME_STREAM_TYPE_BYTES, 3);
	assert_ioctl(TX_ISP_FRAME_IOCTL_T41_STREAM_OFF, 'T', 0x58,
		     TX_ISP_FRAME_STREAM_TYPE_BYTES, 3);
	assert_ioctl(TX_ISP_FRAME_IOCTL_T41_SET_BANKS, 'T', 0x59, 4, 3);
	assert_ioctl(TX_ISP_FRAME_IOCTL_T41_GET_COUNT, 'T', 0x5a, 4, 3);
	assert_ioctl(TX_ISP_FRAME_IOCTL_T41_SET_ALIGN, 'T', 0x5b, 8, 3);
}

int main(void)
{
	test_events();
	test_request_wire();
	test_legacy_ioctls();
	test_t41_ioctls();
	puts("tx_isp_frame_channel tests passed");
	return 0;
}
