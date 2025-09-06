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
    
    mcp_log_info("tx_isp_video_link_stream: EXACT Binary Ninja implementation");
    
    if (!dev) {
        mcp_log_error("tx_isp_video_link_stream: Invalid ISP device - null pointer");
        return -EINVAL;
    }
    
    /* EXACT Binary Ninja implementation: Loop through subdevs array at offset 0x38 */
    for (i = 0; i < 16; i++) {
        struct tx_isp_subdev *subdev = dev->subdevs[i];
        
        if (subdev != NULL) {
            /* Access ops structure at offset 0xc4 in subdev */
            struct tx_isp_subdev_ops *ops = subdev->ops;
            
            if (ops != NULL) {
                /* Access video ops structure at offset +4 in ops */
                struct tx_isp_subdev_video_ops *video_ops = ops->video;
                
                if (video_ops != NULL) {
                    /* Access s_stream function at offset +4 in video_ops */
                    int (*s_stream_func)(struct tx_isp_subdev *, int) = video_ops->s_stream;
                    
                    if (s_stream_func != NULL) {
                        /* Call s_stream function */
                        int result = s_stream_func(subdev, enable);
                        
                        if (result == 0) {
                            /* Success, continue to next subdev */
                            mcp_log_info("tx_isp_video_link_stream: Stream %s on subdev %d: success",
                                        enable ? "enabled" : "disabled", i);
                        } else if (result != 0xfffffdfd) {
                            /* Real error occurred, need to cleanup */
                            mcp_log_error("tx_isp_video_link_stream: Stream %s failed on subdev %d: %d",
                                         enable ? "enable" : "disable", i, result);
                            
                            /* Binary Ninja cleanup: disable all previously enabled subdevs */
                            int j;
                            for (j = i - 1; j >= 0; j--) {
                                struct tx_isp_subdev *prev_subdev = dev->subdevs[j];
                                
                                if (prev_subdev != NULL) {
                                    struct tx_isp_subdev_ops *prev_ops = prev_subdev->ops;
                                    
                                    if (prev_ops != NULL) {
                                        struct tx_isp_subdev_video_ops *prev_video_ops = prev_ops->video;
                                        
                                        if (prev_video_ops != NULL) {
                                            int (*prev_s_stream_func)(struct tx_isp_subdev *, int) = prev_video_ops->s_stream;
                                            
                                            if (prev_s_stream_func != NULL) {
                                                /* Disable this subdev - use opposite of original enable flag */
                                                prev_s_stream_func(prev_subdev, enable ? 0 : 1);
                                                mcp_log_info("tx_isp_video_link_stream: Cleanup: disabled subdev %d", j);
                                            }
                                        }
                                    }
                                }
                            }
                            
                            return result;
                        } else {
                            /* Special case: 0xfffffdfd is not a real error */
                            mcp_log_info("tx_isp_video_link_stream: Stream %s on subdev %d: special code 0xfffffdfd",
                                        enable ? "enabled" : "disabled", i);
                        }
                    }
                }
            }
        }
    }
    
    mcp_log_info("tx_isp_video_link_stream: Binary Ninja implementation completed successfully");
    return 0;
}

EXPORT_SYMBOL(tx_isp_video_link_stream);
