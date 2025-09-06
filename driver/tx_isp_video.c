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
 * tx_isp_video_link_stream - Enable video streaming across all linked subdevices
 * @dev: ISP device structure
 *
 * This function iterates through all subdevices in the ISP device and enables
 * video streaming on each one that has a video stream operation defined.
 * MEMORY SAFE implementation - uses proper struct member access instead of offsets.
 *
 * Returns 0 on success, negative error code on failure
 */
int tx_isp_video_link_stream(struct tx_isp_dev *dev, int enable)
{
    int i;
    unsigned long flags;
    
    pr_info("*** tx_isp_video_link_stream: Enable streaming (SAFE STRUCT ACCESS implementation) ***\n");
    
    /* CRITICAL: Validate ISP device pointer with MIPS-safe bounds checking */
    if (!dev || (unsigned long)dev < 0x80000000 || (unsigned long)dev >= 0xfffff001) {
        pr_err("tx_isp_video_link_stream: Invalid ISP device pointer %p\n", dev);
        return -EINVAL;
    }
    
    /* MEMORY SAFETY: Add memory barrier before accessing device structure */
    rmb();
    
    /* CRITICAL: Validate subdevs array pointer before accessing */
    if (!dev->subdevs) {
        pr_err("tx_isp_video_link_stream: ISP device has no subdevs array\n");
        return -EINVAL;
    }
    
    pr_info("tx_isp_video_link_stream: Processing %s request for %d subdevs\n",
            enable ? "ENABLE" : "DISABLE", 16);
    
    /* SAFE: Loop through subdevs array with bounds checking */
    for (i = 0; i < 16; i++) {
        struct tx_isp_subdev *subdev;
        struct tx_isp_subdev_ops *ops;
        struct tx_isp_subdev_video_ops *video_ops;
        int (*s_stream_func)(struct tx_isp_subdev *, int);
        int result;
        
        /* CRITICAL: Safe subdev access with validation */
        subdev = dev->subdevs[i];
        if (!subdev || (unsigned long)subdev < 0x80000000 || (unsigned long)subdev >= 0xfffff001) {
            /* Skip invalid subdev - this is normal for unused slots */
            continue;
        }
        
        /* MEMORY SAFETY: Add barrier before accessing subdev structure */
        rmb();
        
        /* CRITICAL: Safe ops structure access with validation */
        ops = subdev->ops;
        if (!ops || (unsigned long)ops < 0x80000000 || (unsigned long)ops >= 0xfffff001) {
            pr_debug("tx_isp_video_link_stream: Subdev %d has no ops structure\n", i);
            continue;
        }
        
        /* MEMORY SAFETY: Add barrier before accessing ops structure */
        rmb();
        
        /* CRITICAL: Safe video ops access with validation */
        video_ops = ops->video;
        if (!video_ops || (unsigned long)video_ops < 0x80000000 || (unsigned long)video_ops >= 0xfffff001) {
            pr_debug("tx_isp_video_link_stream: Subdev %d has no video ops\n", i);
            continue;
        }
        
        /* MEMORY SAFETY: Add barrier before accessing video_ops structure */
        rmb();
        
        /* CRITICAL: Safe s_stream function pointer access with validation */
        s_stream_func = video_ops->s_stream;
        if (!s_stream_func || (unsigned long)s_stream_func < 0x80000000 || (unsigned long)s_stream_func >= 0xfffff001) {
            pr_debug("tx_isp_video_link_stream: Subdev %d has no s_stream function\n", i);
            continue;
        }
        
        pr_info("tx_isp_video_link_stream: Calling s_stream on subdev %d (func=%p, enable=%d)\n",
                i, s_stream_func, enable);
        
        /* CRITICAL: Call s_stream function with proper error handling */
        result = s_stream_func(subdev, enable);
        
        if (result == 0) {
            /* Success, continue to next subdev */
            pr_info("tx_isp_video_link_stream: Stream %s on subdev %d: SUCCESS\n",
                    enable ? "ENABLED" : "DISABLED", i);
        } else if (result != -515) { /* 0xfffffdfd = -515 */
            /* Real error occurred, need to cleanup */
            pr_err("tx_isp_video_link_stream: Stream %s FAILED on subdev %d: %d\n",
                   enable ? "enable" : "disable", i, result);
            
            /* SAFE: Cleanup previously enabled subdevs */
            if (enable) { /* Only cleanup if we were enabling */
                int j;
                for (j = i - 1; j >= 0; j--) {
                    struct tx_isp_subdev *prev_subdev = dev->subdevs[j];
                    
                    if (prev_subdev &&
                        (unsigned long)prev_subdev >= 0x80000000 &&
                        (unsigned long)prev_subdev < 0xfffff001) {
                        
                        struct tx_isp_subdev_ops *prev_ops = prev_subdev->ops;
                        if (prev_ops &&
                            (unsigned long)prev_ops >= 0x80000000 &&
                            (unsigned long)prev_ops < 0xfffff001) {
                            
                            struct tx_isp_subdev_video_ops *prev_video_ops = prev_ops->video;
                            if (prev_video_ops &&
                                (unsigned long)prev_video_ops >= 0x80000000 &&
                                (unsigned long)prev_video_ops < 0xfffff001) {
                                
                                int (*prev_s_stream_func)(struct tx_isp_subdev *, int) = prev_video_ops->s_stream;
                                if (prev_s_stream_func &&
                                    (unsigned long)prev_s_stream_func >= 0x80000000 &&
                                    (unsigned long)prev_s_stream_func < 0xfffff001) {
                                    
                                    /* Disable this subdev */
                                    prev_s_stream_func(prev_subdev, 0);
                                    pr_info("tx_isp_video_link_stream: Cleanup: disabled subdev %d\n", j);
                                }
                            }
                        }
                    }
                }
            }
            
            return result;
        } else {
            /* Special case: -515 (0xfffffdfd) is not a real error */
            pr_info("tx_isp_video_link_stream: Stream %s on subdev %d: special code -515 (ignored)\n",
                    enable ? "enabled" : "disabled", i);
        }
    }
    
    pr_info("*** tx_isp_video_link_stream: SAFE implementation completed successfully ***\n");
    return 0;
}

EXPORT_SYMBOL(tx_isp_video_link_stream);
