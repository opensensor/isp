/*
 * TX ISP Frame Done Wakeup Implementation
 *
 * OEM EXACT: isp_frame_done_wakeup at 0x8a10.
 * Increments 64-bit frame counter, sets condition flag, wakes waitqueue.
 * Called from ISR via ISP_TUNING_EVENT_FRAME (0x4000002) on every frame.
 * libimp waits on this via ioctl 0x8000162 (isp_frame_done_wait).
 */

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/jiffies.h>
#include <linux/atomic.h>
#include <linux/errno.h>
#include <linux/types.h>

/* Frame done tracking variables — OEM globals at 0xa2a30/0xa2a38 */
static atomic64_t frame_done_cnt = ATOMIC64_INIT(0);
static int frame_done_cond = 0;
static DECLARE_WAIT_QUEUE_HEAD(frame_done_wait);

/**
 * isp_frame_done_wakeup - OEM EXACT at 0x8a10
 * Increment counter, set condition, wake waitqueue. Nothing else.
 */
void isp_frame_done_wakeup(void)
{
    atomic64_inc(&frame_done_cnt);
    frame_done_cond = 1;
    wake_up(&frame_done_wait);
}
EXPORT_SYMBOL(isp_frame_done_wakeup);

/**
 * isp_frame_done_wait - OEM EXACT at 0x79dc
 * Wait for frame done with timeout, return counter values.
 */
int isp_frame_done_wait(int timeout_ms)
{
    int ret;

    ret = wait_event_timeout(frame_done_wait,
                            frame_done_cond,
                            msecs_to_jiffies(timeout_ms));

    if (ret > 0) {
        frame_done_cond = 0;
        return 0;
    } else if (ret == 0) {
        return -ETIMEDOUT;
    }

    return ret;
}
EXPORT_SYMBOL(isp_frame_done_wait);

/* OEM EXACT: Extended wait returning counters (used by ioctl 0x8000162) */
int isp_frame_done_wait_ex(int timeout_ms, u32 out[2])
{
    int ret;
    u64 cnt;

    if (!out)
        return -EINVAL;

    ret = wait_event_timeout(frame_done_wait, frame_done_cond, msecs_to_jiffies(timeout_ms));
    frame_done_cond = 0;

    cnt = atomic64_read(&frame_done_cnt);
    out[0] = (u32)cnt;
    out[1] = (u32)(cnt >> 32);

    return (ret > 0) ? 0 : (ret == 0 ? -ETIMEDOUT : ret);
}
EXPORT_SYMBOL_GPL(isp_frame_done_wait_ex);

/**
 * isp_frame_done_get_count - Get current frame done count
 */
uint64_t isp_frame_done_get_count(void)
{
    return atomic64_read(&frame_done_cnt);
}
EXPORT_SYMBOL(isp_frame_done_get_count);
