#include <assert.h>
#include <errno.h>
#include <stdio.h>

#include "tx_isp/tx_isp_daynight.h"

struct write_log {
	u32 reg;
	u32 value;
	u32 count;
};

static void record_write(void *opaque, u32 reg, u32 value)
{
	struct write_log *log = opaque;

	log->reg = reg;
	log->value = value;
	log->count++;
}

static void test_custom_transition_staging(void)
{
	u32 state = TX_ISP_DAY_STATE;
	u32 pending = TX_ISP_DAYNIGHT_IDLE;

	assert(tx_isp_daynight_stage_custom(&state, &pending, 1) == 1);
	assert(state == TX_ISP_CUSTOM_DAY_STATE);
	assert(pending == TX_ISP_DAYNIGHT_FILL_DAY);

	assert(tx_isp_daynight_stage_custom(&state, &pending, 0) == 1);
	assert(state == TX_ISP_CUSTOM_DAY_STATE);
	assert(pending == TX_ISP_DAYNIGHT_FILL_DAY);

	state = TX_ISP_NIGHT_STATE;
	assert(tx_isp_daynight_stage_custom(&state, &pending, 1) == 1);
	assert(state == TX_ISP_CUSTOM_NIGHT_STATE);
	assert(pending == TX_ISP_DAYNIGHT_FILL_DAY);

	assert(tx_isp_daynight_stage_custom(&state, &pending, 0) == 1);
	assert(state == TX_ISP_CUSTOM_NIGHT_STATE);
	assert(pending == TX_ISP_DAYNIGHT_FILL_NIGHT);

	assert(tx_isp_daynight_stage_custom(NULL, &pending, 1) == -EINVAL);
	assert(tx_isp_daynight_stage_custom(&state, NULL, 1) == -EINVAL);
	assert(tx_isp_daynight_stage_custom(&state, &pending, 2) == -ERANGE);
	state = 4;
	assert(tx_isp_daynight_stage_custom(&state, &pending, 1) == -ERANGE);
}

static void test_custom_fill_application(void)
{
	const struct tx_isp_daynight_registers registers = {
		.fill = 0x6030,
		.day_fill = 0xff00ff00,
		.night_fill = 0xff008080,
	};
	struct write_log log = { 0, 0, 0 };
	u32 mode = TX_ISP_DAY_MODE;
	u32 pending = TX_ISP_DAYNIGHT_FILL_DAY;
	u32 commit = 0;
	struct tx_isp_daynight_runtime runtime = {
		.running_mode = &mode,
		.pending = &pending,
		.commit_pending = &commit,
		.registers = &registers,
		.write = record_write,
		.opaque = &log,
	};

	assert(tx_isp_daynight_apply(&runtime) == TX_ISP_DAYNIGHT_FILL_DAY);
	assert(log.reg == 0x6030);
	assert(log.value == 0xff00ff00);
	assert(log.count == 1);
	assert(pending == TX_ISP_DAYNIGHT_IDLE);

	pending = TX_ISP_DAYNIGHT_FILL_NIGHT;
	assert(tx_isp_daynight_apply(&runtime) == TX_ISP_DAYNIGHT_FILL_NIGHT);
	assert(log.value == 0xff008080);
	assert(log.count == 2);
}

int main(void)
{
	test_custom_transition_staging();
	test_custom_fill_application();
	puts("tx_isp_daynight tests passed");
	return 0;
}
