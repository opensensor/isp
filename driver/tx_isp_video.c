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
int tx_isp_video_link_stream(struct tx_isp_dev *dev, int enable)
{
    int i;
    int ret = 0;
    struct tx_isp_subdev *sd;
    struct tx_isp_subdev_ops *subdev_ops;
    struct tx_isp_subdev_video_ops *video_ops;
    
    if (!dev) {
        pr_err("tx_isp_video_link_stream: dev is NULL\n");
        return -EINVAL;
    }
    
    pr_info("*** tx_isp_video_link_stream: %s streaming (MEMORY-SAFE) ***\n",
            enable ? "ENABLING" : "DISABLING");
    mcp_log_info("tx_isp_video_link_stream: entry", enable);
    
    /* SAFE: Use proper struct member access instead of offset 0x38 */
    /* Iterate through 16 subdevices as per Binary Ninja reference */
    for (i = 0; i < 16; i++) {
        sd = dev->subdevs[i];  /* SAFE: dev->subdevs instead of dev+0x38 */
        
        if (!sd) {
            continue;
        }
        
        /* SAFE: Use proper struct member access instead of offset 0xc4 */
        subdev_ops = sd->ops;
        if (!subdev_ops) {
            continue;
        }
        
        /* Get video ops from tx_isp_subdev_ops (not v4l2_subdev_ops) */
        video_ops = subdev_ops->video;
        if (!video_ops || !video_ops->s_stream) {
            continue;
        }
        
        /* Call s_stream function with proper tx_isp_subdev parameter */
        ret = video_ops->s_stream(sd, enable);
        mcp_log_info("tx_isp_video_link_stream: subdev s_stream call", ret);
        
        if (ret == 0) {
            /* Success, continue to next */
            continue;
        } else if (ret == -ENOIOCTLCMD) {
            /* Not implemented, continue */
            continue;
        } else {
            /* Error occurred, rollback previous successful calls */
            pr_err("tx_isp_video_link_stream: s_stream failed at subdev %d, ret=%d\n", i, ret);
            mcp_log_info("tx_isp_video_link_stream: rollback starting", i);
            
            /* Rollback: disable all previously enabled subdevices */
            while (--i >= 0) {
                struct tx_isp_subdev *rollback_sd = dev->subdevs[i];  /* SAFE: proper member access */
                if (!rollback_sd) continue;
                
                struct tx_isp_subdev_ops *rollback_ops = rollback_sd->ops;  /* SAFE: proper member access */
                if (!rollback_ops) continue;
                
                struct tx_isp_subdev_video_ops *rollback_video_ops = rollback_ops->video;
                if (!rollback_video_ops || !rollback_video_ops->s_stream) continue;
                
                /* Disable this subdevice */
                rollback_video_ops->s_stream(rollback_sd, enable ? 0 : 1);
            }
            
            mcp_log_info("tx_isp_video_link_stream: rollback complete", ret);
            return ret;
        }
    }
    
    pr_info("*** tx_isp_video_link_stream: MEMORY-SAFE completion - no unaligned access risk ***\n");
    mcp_log_info("tx_isp_video_link_stream: successful completion", 0);
    
    return 0;
}

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
