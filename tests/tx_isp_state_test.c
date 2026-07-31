#include <stdio.h>

#include "../driver/include/tx_isp/tx_isp_state.h"

static void expect(int condition)
{
	if (!condition)
		__builtin_trap();
}

int main(void)
{
	const unsigned long object = 0x80001000UL;
	const unsigned long queue_self = object + 0x1fcUL;

	expect(!tx_isp_subdev_state_ready(
		0, queue_self, queue_self, 0));
	expect(tx_isp_subdev_state_ready(
		object, 0x80002000UL, queue_self, 0));
	expect(tx_isp_subdev_state_ready(
		object, 0x80002000UL, queue_self, 1));
	expect(tx_isp_subdev_state_ready(
		object, queue_self, queue_self, 0));
	expect(!tx_isp_subdev_state_ready(
		object, queue_self, queue_self, 1));
	expect(tx_isp_subdev_state_ready(
		object, queue_self, queue_self, 2));
	expect(!tx_isp_subdev_state_ready(
		object, queue_self, queue_self, 3));

	puts("tx_isp_state_test: ok");
	return 0;
}
