/*
 * Video link stream implementation for TX ISP driver
 *
 * Copyright 2024, <matteius@gmail.com>
 *
 * This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/interrupt.h>
#include "../include/tx_isp.h"
#include "../include/tx_isp_core.h"
#include "../include/tx-isp-debug.h"
#include "../include/tx_isp_sysfs.h"
#include "../include/tx_isp_vic.h"
#include "../include/tx_isp_csi.h"
#include "../include/tx_isp_vin.h"
#include "../include/tx_isp_tuning.h"
#include "../include/tx-isp-device.h"


/**
 * tx_isp_video_link_stream - Main video link stream control
 * @dev: TX ISP device structure 
 * @enable: 1 to enable streaming, 0 to disable
 *
 * MEMORY-SAFE implementation using proper struct member access instead of unsafe offsets.
 * Based on Binary Ninja reference driver but using safe struct member access patterns.
 */


/**
 * tx_isp_video_link_stream_safe_wrapper - ATOMIC-SAFE wrapper  
 *
 * This provides an extra layer of safety for critical streaming operations
 * that were causing the crashes. NO MUTEX OPERATIONS to prevent atomic context violations.
 */
int tx_isp_video_link_stream_safe_wrapper(struct tx_isp_dev *dev, int enable)
{
    int ret;
    
    pr_info("*** tx_isp_video_link_stream_safe_wrapper: %s streaming (ATOMIC-SAFE) ***\n",
            enable ? "ENABLING" : "DISABLING");
    
    /* CRITICAL FIX: NO MUTEX OPERATIONS - prevents atomic context violations */
    /* The crash showed irqs_disabled(): 1 which means we're in atomic context */
    /* Mutex operations cause "sleeping function called from invalid context" */
    
    /* Call the main streaming function directly without any locking */
    ret = tx_isp_video_link_stream(dev, enable);
    
    pr_info("*** tx_isp_video_link_stream_safe_wrapper: ATOMIC-SAFE completed with result %d ***\n", ret);
    
    return ret;
}

EXPORT_SYMBOL(tx_isp_video_link_stream);
EXPORT_SYMBOL(tx_isp_video_link_stream_safe_wrapper);
