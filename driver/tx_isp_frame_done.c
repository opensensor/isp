/*
 * TX ISP Frame Done Wakeup Implementation
 * 
 * This file implements the ISP frame done wakeup function
 * that is called by VIC when a frame is complete.
 */

#include <linux/kernel.h>
#include <linux/wait.h>
#include <linux/atomic.h>
#include "../include/tx-isp-debug.h"
#include "../include/tx_isp.h"

/* Frame done tracking variables */
atomic64_t frame_done_cnt = ATOMIC64_INIT(0);
EXPORT_SYMBOL(frame_done_cnt);
static int frame_done_cond = 0;
static DECLARE_WAIT_QUEUE_HEAD(frame_done_wait);

/**
 * isp_frame_done_wakeup - Notify ISP core that a frame is ready
 * This function is called by VIC when a frame is complete
 * Based on Binary Ninja decompilation
 */
void isp_frame_done_wakeup(void)
{
    /* Increment frame done counter */
    atomic64_inc(&frame_done_cnt);

    /* CRITICAL FIX: Frame counter now managed by frame_done_cnt only */
    extern struct tx_isp_dev *ourISPdev;
    if (ourISPdev) {
        pr_info("*** FRAME SYNC: Frame done count = %lld ***\n",
                atomic64_read(&frame_done_cnt));
    }

    /* Set condition flag */
    frame_done_cond = 1;

    /* Wake up any processes waiting for frame completion */
    wake_up(&frame_done_wait);

    pr_info("*** ISP FRAME DONE WAKEUP: Frame %lld ready for processing ***\n",
             atomic64_read(&frame_done_cnt));
}
EXPORT_SYMBOL(isp_frame_done_wakeup);

/**
 * isp_frame_done_wait - Wait for frame completion
 * This function allows processes to wait for frame done events
 */
int isp_frame_done_wait(int timeout_ms)
{
    int ret;
    
    /* Wait for frame done condition with timeout */
    ret = wait_event_timeout(frame_done_wait, 
                            frame_done_cond,
                            msecs_to_jiffies(timeout_ms));
    
    if (ret > 0) {
        /* Frame done occurred, clear condition */
        frame_done_cond = 0;
        return 0;
    } else if (ret == 0) {
        /* Timeout */
        return -ETIMEDOUT;
    }
    
    return ret;
}
EXPORT_SYMBOL(isp_frame_done_wait);

/**
 * isp_frame_done_get_count - Get current frame done count
 */
uint64_t isp_frame_done_get_count(void)
{
    return atomic64_read(&frame_done_cnt);
}
EXPORT_SYMBOL(isp_frame_done_get_count);
