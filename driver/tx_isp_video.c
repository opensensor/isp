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
 * tx_isp_video_link_stream - Enable video streaming across all linked subdevices
 * @dev: ISP device structure
 *
 * This function iterates through all subdevices in the ISP device and enables
 * video streaming on each one that has a video stream operation defined.
 * MEMORY SAFE implementation - uses proper struct member access and validation.
 *
 * Returns 0 on success, negative error code on failure
 */
int tx_isp_video_link_stream(struct tx_isp_dev *dev, int enable)
{
    int i, result;
    int enabled_count = 0;
    
    pr_info("*** tx_isp_video_link_stream: Enable streaming (SAFE STRUCT ACCESS implementation) ***\n");
    
    /* CRITICAL: Validate ISP device pointer with MIPS-safe bounds checking */
    if (!is_valid_kernel_pointer(dev)) {
        pr_err("tx_isp_video_link_stream: Invalid ISP device pointer %p\n", dev);
        return -EINVAL;
    }
    
    /* MEMORY SAFETY: Add memory barrier before accessing device structure */
    rmb();
    
    /* CRITICAL: Validate subdevs array pointer before accessing */
    if (!is_valid_kernel_pointer(dev->subdevs)) {
        pr_err("tx_isp_video_link_stream: ISP device has invalid subdevs array %p\n", dev->subdevs);
        return -EINVAL;
    }
    
    pr_info("tx_isp_video_link_stream: Processing %s request for %d subdevs\n",
            enable ? "ENABLE" : "DISABLE", 16);
    
    /* SAFE: Loop through subdevs array with bounds checking */
    for (i = 0; i < 16; i++) {
        struct tx_isp_subdev *subdev;
        
        /* CRITICAL: Safe subdev access with validation */
        subdev = dev->subdevs[i];
        if (!is_valid_kernel_pointer(subdev)) {
            /* Skip invalid subdev - this is normal for unused slots */
            continue;
        }
        
        /* MEMORY SAFETY: Add barrier before accessing subdev */
        rmb();
        
        /* Use safe helper function to call subdev */
        result = safe_subdev_call(subdev, enable, i);
        
        if (result == 0) {
            /* Success, continue to next subdev */
            enabled_count++;
            pr_info("tx_isp_video_link_stream: Stream %s on subdev %d: SUCCESS\n",
                    enable ? "ENABLED" : "DISABLED", i);
        } else if (result == -EINVAL) {
            /* Invalid subdev structure - skip but don't fail */
            pr_debug("tx_isp_video_link_stream: Subdev %d has invalid structure - skipping\n", i);
            continue;
        } else if (result != -515) { /* 0xfffffdfd = -515 */
            /* Real error occurred, need to cleanup */
            pr_err("tx_isp_video_link_stream: Stream %s FAILED on subdev %d: %d\n",
                   enable ? "enable" : "disable", i, result);
            
            /* SAFE: Cleanup previously enabled subdevs using safe helper */
            if (enable && enabled_count > 0) {
                pr_info("tx_isp_video_link_stream: Cleaning up %d previously enabled subdevs\n", enabled_count);
                
                int j;
                for (j = i - 1; j >= 0; j--) {
                    struct tx_isp_subdev *prev_subdev = dev->subdevs[j];
                    
                    if (is_valid_kernel_pointer(prev_subdev)) {
                        /* Use safe helper to disable */
                        int cleanup_result = safe_subdev_call(prev_subdev, 0, j);
                        if (cleanup_result == 0) {
                            pr_info("tx_isp_video_link_stream: Cleanup: disabled subdev %d\n", j);
                        } else {
                            pr_warn("tx_isp_video_link_stream: Cleanup failed for subdev %d: %d\n", j, cleanup_result);
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
    pr_info("tx_isp_video_link_stream: %s %d subdevs successfully\n",
            enable ? "ENABLED" : "DISABLED", enabled_count);
    
    /* MEMORY SAFETY: Final memory barrier */
    wmb();
    
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
