#ifndef TX_ISP_DAYNIGHT_H
#define TX_ISP_DAYNIGHT_H

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdbool.h>
#include <stdint.h>
typedef uint32_t u32;
#endif

enum tx_isp_daynight_mode {
	TX_ISP_DAY_MODE = 0,
	TX_ISP_NIGHT_MODE = 1,
};

enum tx_isp_daynight_state {
	TX_ISP_DAY_STATE = 0,
	TX_ISP_NIGHT_STATE = 1,
	TX_ISP_CUSTOM_DAY_STATE = 2,
	TX_ISP_CUSTOM_NIGHT_STATE = 3,
};

#define TX_ISP_DAYNIGHT_CUSTOM_FLAG 2U

enum tx_isp_daynight_pending {
	TX_ISP_DAYNIGHT_IDLE = 0,
	TX_ISP_DAYNIGHT_SWITCH = 1,
	TX_ISP_DAYNIGHT_FILL_DAY = 2,
	TX_ISP_DAYNIGHT_FILL_NIGHT = 3,
};

struct tx_isp_daynight_registers {
	u32 fill;
	u32 commit;
	u32 day_fill;
	u32 night_fill;
	u32 commit_value;
	bool has_commit;
};

typedef void (*tx_isp_daynight_write_fn)(void *opaque, u32 reg, u32 value);
typedef void (*tx_isp_daynight_prepare_fn)(void *opaque, u32 mode);
typedef int (*tx_isp_daynight_notify_fn)(void *opaque, u32 mode);

struct tx_isp_daynight_runtime {
	u32 *running_mode;
	u32 *pending;
	u32 *commit_pending;
	const struct tx_isp_daynight_registers *registers;
	tx_isp_daynight_write_fn write;
	tx_isp_daynight_prepare_fn prepare;
	tx_isp_daynight_notify_fn notify;
	void *opaque;
	int *notify_result;
};

/*
 * Stage a mode change for the next frame boundary.
 *
 * Returns 1 when a transition was queued, 0 when the requested mode was
 * already active, or a negative errno.
 */
int tx_isp_daynight_stage(u32 *running_mode, u32 *pending,
			  u32 requested_mode, u32 *previous_mode);

/*
 * Stage the OEM custom-bank CSC fill transition. Enabling custom mode always
 * restores live chroma; disabling it restores the fill for the underlying
 * day/night state. The extended running state remains 2 (custom-day) or
 * 3 (custom-night) until a normal running-mode request replaces it.
 */
int tx_isp_daynight_stage_custom(u32 *running_state, u32 *pending,
				 u32 enabled);

/*
 * Apply one frame of the shared day/night transition state machine.
 *
 * The return value is the pending state that was handled, zero when idle, or
 * a negative errno. SoC adapters supply register addresses and optional
 * prepare/notify hooks while the transition ordering remains common.
 */
int tx_isp_daynight_apply(struct tx_isp_daynight_runtime *runtime);

#endif /* TX_ISP_DAYNIGHT_H */
