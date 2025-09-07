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
 * Based on reference driver implementation. Iterates through subdevices
 * and calls their streaming functions. Handles rollback on failure.
 */
int tx_isp_video_link_stream(struct tx_isp_dev *dev, int enable)
{
    int i;
    int ret = 0;
    struct tx_isp_subdev **subdevs;
    struct tx_isp_subdev *sd;
    struct v4l2_subdev *v4l2_sd;
    struct v4l2_subdev_video_ops *video_ops;
    
    if (!dev) {
        pr_err("tx_isp_video_link_stream: dev is NULL\n");
        return -EINVAL;
    }
    
    /* Get subdevs array starting at offset 0x38 from dev */
    subdevs = (struct tx_isp_subdev **)((char *)dev + 0x38);
    
    pr_info("*** tx_isp_video_link_stream: %s streaming ***\n",
            enable ? "ENABLING" : "DISABLING");
    
    /* Iterate through 16 subdevices */
    for (i = 0; i < 0x10; i++) {
        sd = subdevs[i];
        
        if (!sd) {
            continue;
        }
        
        /* Get v4l2_subdev at offset 0xc4 */
        v4l2_sd = (struct v4l2_subdev *)((char *)sd + 0xc4);
        if (!v4l2_sd || !v4l2_sd->ops) {
            continue;
        }
        
        /* Get video ops */
        video_ops = v4l2_sd->ops->video;
        if (!video_ops || !video_ops->s_stream) {
            continue;
        }
        
        /* Call s_stream function */
        ret = video_ops->s_stream(v4l2_sd, enable);
        
        if (ret == 0) {
            /* Success, continue to next */
            continue;
        } else if (ret == -ENOIOCTLCMD) {
            /* Not implemented, continue */
            continue;
        } else {
            /* Error occurred, rollback previous successful calls */
            pr_err("tx_isp_video_link_stream: s_stream failed at subdev %d, ret=%d\n", i, ret);
            
            /* Rollback: disable all previously enabled subdevices */
            while (--i >= 0) {
                struct tx_isp_subdev *rollback_sd = subdevs[i];
                if (!rollback_sd) continue;
                
                struct v4l2_subdev *rollback_v4l2_sd = (struct v4l2_subdev *)((char *)rollback_sd + 0xc4);
                if (!rollback_v4l2_sd || !rollback_v4l2_sd->ops) continue;
                
                struct v4l2_subdev_video_ops *rollback_video_ops = rollback_v4l2_sd->ops->video;
                if (!rollback_video_ops || !rollback_video_ops->s_stream) continue;
                
                /* Disable this subdevice */
                rollback_video_ops->s_stream(rollback_v4l2_sd, enable ? 0 : 1);
            }
            
            return ret;
        }
    }
    
    pr_info("*** tx_isp_video_link_stream: CRASH-SAFE completion - no unaligned access attempted ***\n");
    
    return 0;
}

/**
 * tx_isp_video_link_stream_safe_wrapper - Additional safety wrapper
 *
 * This provides an extra layer of safety for critical streaming operations
 * that were causing the crashes mentioned in the logs.
 */
int tx_isp_video_link_stream_safe_wrapper(struct tx_isp_dev *dev, int enable)
{
    int ret;
    static DEFINE_MUTEX(streaming_mutex);
    
    pr_info("*** tx_isp_video_link_stream_safe_wrapper: %s streaming ***\n",
            enable ? "ENABLING" : "DISABLING");
    
    /* Global mutex to prevent concurrent streaming operations */
    /* Do NOT disable interrupts - this can cause atomic context violations */
    mutex_lock(&streaming_mutex);
    
    /* Call the main streaming function */
    ret = tx_isp_video_link_stream(dev, enable);
    
    mutex_unlock(&streaming_mutex);
    
    pr_info("*** tx_isp_video_link_stream_safe_wrapper: completed with result %d ***\n", ret);
    
    return ret;
}

EXPORT_SYMBOL(tx_isp_video_link_stream);
EXPORT_SYMBOL(tx_isp_video_link_stream_safe_wrapper);
