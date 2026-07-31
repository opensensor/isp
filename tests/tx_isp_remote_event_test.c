#include <stdint.h>
#include <stdio.h>

#include "../driver/include/tx_isp/tx_isp_remote_event.h"

struct fake_pad {
	struct fake_pad *remote;
	unsigned long handler;
};

static void expect(int condition)
{
	if (!condition)
		__builtin_trap();
}

static int fake_handler(void *pad, unsigned int event, void *data)
{
	return pad && event == 7 && data ? 23 : -1;
}

static void *fake_remote_pad(void *local_pad)
{
	return ((struct fake_pad *)local_pad)->remote;
}

static unsigned long fake_event_handler(void *remote_pad)
{
	return ((struct fake_pad *)remote_pad)->handler;
}

static int fake_pointer_valid(unsigned long address)
{
	return address > 0x1000UL && address != 0xdeadbeefUL;
}

static const struct tx_isp_remote_event_ops fake_ops = {
	.remote_pad = fake_remote_pad,
	.event_handler = fake_event_handler,
};

int main(void)
{
	struct fake_pad local = { 0 };
	struct fake_pad remote = { 0 };
	struct tx_isp_remote_event_target target;
	tx_isp_remote_event_handler handler;
	int payload = 1;

	expect(tx_isp_resolve_remote_event(
		0, &fake_ops, fake_pointer_valid, &target) ==
	       TX_ISP_REMOTE_EVENT_INVALID);
	expect(!target.pad && !target.handler);
	expect(tx_isp_resolve_remote_event(
		&local, 0, fake_pointer_valid, &target) ==
	       TX_ISP_REMOTE_EVENT_INVALID);
	expect(tx_isp_resolve_remote_event(
		&local, &fake_ops, 0, &target) ==
	       TX_ISP_REMOTE_EVENT_INVALID);
	expect(tx_isp_resolve_remote_event(
		&local, &fake_ops, fake_pointer_valid, 0) ==
	       TX_ISP_REMOTE_EVENT_INVALID);

	expect(tx_isp_resolve_remote_event(
		&local, &fake_ops, fake_pointer_valid, &target) ==
	       TX_ISP_REMOTE_EVENT_UNLINKED);
	local.remote = (struct fake_pad *)(uintptr_t)0xdeadbeefUL;
	expect(tx_isp_resolve_remote_event(
		&local, &fake_ops, fake_pointer_valid, &target) ==
	       TX_ISP_REMOTE_EVENT_UNLINKED);

	local.remote = &remote;
	expect(tx_isp_resolve_remote_event(
		&local, &fake_ops, fake_pointer_valid, &target) ==
	       TX_ISP_REMOTE_EVENT_NO_HANDLER);
	remote.handler = 0xdeadbeefUL;
	expect(tx_isp_resolve_remote_event(
		&local, &fake_ops, fake_pointer_valid, &target) ==
	       TX_ISP_REMOTE_EVENT_NO_HANDLER);

	remote.handler = (unsigned long)(uintptr_t)fake_handler;
	expect(tx_isp_resolve_remote_event(
		&local, &fake_ops, fake_pointer_valid, &target) ==
	       TX_ISP_REMOTE_EVENT_OK);
	expect(target.pad == &remote);
	expect(target.handler == remote.handler);
	handler = (tx_isp_remote_event_handler)(uintptr_t)target.handler;
	expect(handler(target.pad, 7, &payload) == 23);

	puts("tx_isp_remote_event_test: ok");
	return 0;
}
