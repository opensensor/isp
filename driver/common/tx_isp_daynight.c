/*
 * Shared frame-boundary day/night transition state machine.
 *
 * T31 and T41 use the same two-phase CSC fill sequence but expose different
 * userspace control IDs, register offsets, and tuning notification paths.
 * Those ABI details stay in small per-SoC adapters; this file owns the state
 * ordering and validation.
 */

#include <linux/errno.h>
#include <linux/kernel.h>

#include "../include/tx_isp/tx_isp_daynight.h"

static void
tx_isp_daynight_write(const struct tx_isp_daynight_runtime *runtime,
		      u32 reg, u32 value)
{
	if (runtime->write)
		runtime->write(runtime->opaque, reg, value);
}

static void
tx_isp_daynight_commit(const struct tx_isp_daynight_runtime *runtime)
{
	if (runtime->registers->has_commit)
		tx_isp_daynight_write(runtime, runtime->registers->commit,
				      runtime->registers->commit_value);
}

int tx_isp_daynight_stage(u32 *running_mode, u32 *pending,
			  u32 requested_mode, u32 *previous_mode)
{
	u32 previous;

	if (!running_mode || !pending)
		return -EINVAL;
	if (requested_mode > TX_ISP_NIGHT_MODE)
		return -ERANGE;

	previous = *running_mode;
	if (previous_mode)
		*previous_mode = previous;
	if (previous == requested_mode)
		return 0;

	*running_mode = requested_mode;
	/* Publish the mode before the frame ISR observes the pending flag. */
	wmb();
	*pending = TX_ISP_DAYNIGHT_SWITCH;
	return 1;
}

int tx_isp_daynight_apply(struct tx_isp_daynight_runtime *runtime)
{
	u32 mode;
	u32 pending;
	int notify_ret = 0;

	if (!runtime || !runtime->running_mode || !runtime->pending ||
	    !runtime->commit_pending || !runtime->registers ||
	    !runtime->write)
		return -EINVAL;

	mode = *runtime->running_mode;
	if (mode > TX_ISP_NIGHT_MODE)
		return -ERANGE;

	/*
	 * The frame after a normal switch closes the shadow transaction. Day
	 * mode restores the chroma fill here; night mode keeps its neutral UV.
	 */
	if (*runtime->commit_pending == 1) {
		if (mode == TX_ISP_DAY_MODE)
			tx_isp_daynight_write(runtime,
					      runtime->registers->fill,
					      runtime->registers->day_fill);
		tx_isp_daynight_commit(runtime);
		*runtime->commit_pending = 0;
	}

	pending = *runtime->pending;
	switch (pending) {
	case TX_ISP_DAYNIGHT_IDLE:
		return 0;

	case TX_ISP_DAYNIGHT_SWITCH:
		if (runtime->prepare)
			runtime->prepare(runtime->opaque, mode);
		if (mode == TX_ISP_NIGHT_MODE)
			tx_isp_daynight_write(runtime,
					      runtime->registers->fill,
					      runtime->registers->night_fill);
		tx_isp_daynight_commit(runtime);
		if (runtime->notify)
			notify_ret = runtime->notify(runtime->opaque, mode);
		*runtime->pending = TX_ISP_DAYNIGHT_IDLE;
		*runtime->commit_pending = 1;
		if (runtime->notify_result)
			*runtime->notify_result = notify_ret;
		return TX_ISP_DAYNIGHT_SWITCH;

	case TX_ISP_DAYNIGHT_FILL_DAY:
		tx_isp_daynight_write(runtime, runtime->registers->fill,
				      runtime->registers->day_fill);
		*runtime->pending = TX_ISP_DAYNIGHT_IDLE;
		return TX_ISP_DAYNIGHT_FILL_DAY;

	case TX_ISP_DAYNIGHT_FILL_NIGHT:
		tx_isp_daynight_write(runtime, runtime->registers->fill,
				      runtime->registers->night_fill);
		*runtime->pending = TX_ISP_DAYNIGHT_IDLE;
		return TX_ISP_DAYNIGHT_FILL_NIGHT;

	default:
		return -ERANGE;
	}
}
