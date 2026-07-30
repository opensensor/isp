#include <assert.h>
#include <errno.h>
#include <stdio.h>

#include "tx_isp/tx_isp_callback_plan.h"

struct callback_log {
	unsigned int values[4];
	unsigned int count;
};

static void append_one(void *opaque)
{
	struct callback_log *log = opaque;

	log->values[log->count++] = 1U;
}

static void append_two(void *opaque)
{
	struct callback_log *log = opaque;

	log->values[log->count++] = 2U;
}

static void append_three(void *opaque)
{
	struct callback_log *log = opaque;

	log->values[log->count++] = 3U;
}

static void test_order(void)
{
	static const struct tx_isp_callback_step steps[] = {
		{ append_one },
		{ append_two },
		{ append_three },
	};
	const struct tx_isp_callback_plan plan = {
		.steps = steps,
		.count = sizeof(steps) / sizeof(steps[0]),
	};
	struct callback_log log = { { 0, 0, 0, 0 }, 0 };

	assert(tx_isp_callback_plan_run(&plan, &log) == 0);
	assert(log.count == 3U);
	assert(log.values[0] == 1U);
	assert(log.values[1] == 2U);
	assert(log.values[2] == 3U);
}

static void test_validation_is_atomic(void)
{
	static const struct tx_isp_callback_step malformed_steps[] = {
		{ append_one },
		{ NULL },
		{ append_three },
	};
	const struct tx_isp_callback_plan malformed = {
		.steps = malformed_steps,
		.count = sizeof(malformed_steps) / sizeof(malformed_steps[0]),
	};
	const struct tx_isp_callback_plan missing_steps = {
		.steps = NULL,
		.count = 1U,
	};
	const struct tx_isp_callback_plan empty = {
		.steps = NULL,
		.count = 0U,
	};
	struct callback_log log = { { 0, 0, 0, 0 }, 0 };

	assert(tx_isp_callback_plan_run(NULL, &log) == -EINVAL);
	assert(tx_isp_callback_plan_run(&missing_steps, &log) == -EINVAL);
	assert(tx_isp_callback_plan_run(&malformed, &log) == -EINVAL);
	assert(log.count == 0U);
	assert(tx_isp_callback_plan_run(&empty, &log) == 0);
}

int main(void)
{
	test_order();
	test_validation_is_atomic();
	puts("tx_isp_callback_plan tests passed");
	return 0;
}
