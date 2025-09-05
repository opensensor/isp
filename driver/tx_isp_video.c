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
int tx_isp_video_link_stream(struct tx_isp_dev *dev)
{
    int ret = 0;
    int i;

    mcp_log_info("tx_isp_video_link_stream: *** MEMORY SAFE implementation ***");

    if (!dev) {
        mcp_log_error("tx_isp_video_link_stream: Invalid ISP device - null pointer");
        return -EINVAL;
    }

    mcp_log_info("tx_isp_video_link_stream: Enabling video streaming on subdevices");

    /* Initialize CSI hardware before streaming */
    if (dev->csi_dev) {
        struct tx_isp_subdev *csi_sd = NULL;
        
        /* Get the CSI subdevice */
        if (dev->csi_pdev) {
            csi_sd = platform_get_drvdata(dev->csi_pdev);
            if (csi_sd && csi_sd->ops && csi_sd->ops->core && csi_sd->ops->core->init) {
                mcp_log_info("tx_isp_video_link_stream: Initializing CSI hardware before streaming");
                
                /* Make sure the CSI device is properly linked to the subdevice */
                if (dev->csi_dev && !tx_isp_get_subdevdata(csi_sd)) {
                    tx_isp_set_subdevdata(csi_sd, dev->csi_dev);
                    mcp_log_info("tx_isp_video_link_stream: Linked CSI device to subdevice");
                }
                
                /* Also make sure the CSI device has the subdevice pointer */
                if (dev->csi_dev && dev->csi_dev->sd == NULL) {
                    dev->csi_dev->sd = csi_sd;
                    mcp_log_info("tx_isp_video_link_stream: Set CSI device subdev pointer");
                }
                
                /* Initialize the CSI hardware */
                ret = csi_sd->ops->core->init(csi_sd, 1);
                if (ret) {
                    mcp_log_error("tx_isp_video_link_stream: Failed to initialize CSI hardware: %d", ret);
                    return ret;
                }
            } else {
                mcp_log_error("tx_isp_video_link_stream: CSI subdev missing required ops");
                return -EINVAL;
            }
        } else {
            mcp_log_error("tx_isp_video_link_stream: CSI platform device is NULL");
            return -EINVAL;
        }
    } else {
        mcp_log_error("tx_isp_video_link_stream: CSI device is NULL");
        return -EINVAL;
    }

    /* Iterate through all subdevices using SAFE struct member access */
    for (i = 0; i < 16; i++) {
        struct tx_isp_subdev *subdev = dev->subdevs[i];
        
        if (!subdev) {
            continue;
        }
        
        mcp_log_info("tx_isp_video_link_stream: Checking subdev %p (index %d)", subdev, i);
        
        /* Validate subdevice structure before use */
        if (!subdev->ops) {
            mcp_log_info("tx_isp_video_link_stream: Subdev %p has no ops", subdev);
            continue;
        }
        
        /* Check if this subdevice has video operations */
        if (subdev->ops->video && subdev->ops->video->s_stream) {
            mcp_log_info("tx_isp_video_link_stream: Enabling stream on subdev %p", subdev);
            
            /* Enable streaming on this subdevice */
            ret = subdev->ops->video->s_stream(subdev, 1);
            
            /* Handle special error code - 0xfffffdfd is not a real error */
            if (ret && ret != 0xfffffdfd) {
                mcp_log_error("tx_isp_video_link_stream: Failed to enable stream on subdev %p: %d", subdev, ret);
                
                /* Disable streams on all previously enabled subdevices */
                int j;
                for (j = 0; j < i; j++) {
                    struct tx_isp_subdev *prev_subdev = dev->subdevs[j];
                    
                    if (!prev_subdev || !prev_subdev->ops) {
                        continue;
                    }
                    
                    if (prev_subdev->ops->video && prev_subdev->ops->video->s_stream) {
                        mcp_log_info("tx_isp_video_link_stream: Disabling stream on subdev %p due to error", prev_subdev);
                        prev_subdev->ops->video->s_stream(prev_subdev, 0);
                    }
                }
                
                return ret;
            }
            
            if (ret == 0) {
                mcp_log_info("tx_isp_video_link_stream: Successfully enabled stream on subdev %p", subdev);
            } else {
                mcp_log_info("tx_isp_video_link_stream: Stream enable returned special code 0x%x on subdev %p", ret, subdev);
            }
        } else {
            mcp_log_info("tx_isp_video_link_stream: Subdev %p has no video stream ops", subdev);
        }
    }

    mcp_log_info("tx_isp_video_link_stream: Video link stream enabled successfully");
    return 0;
}

EXPORT_SYMBOL(tx_isp_video_link_stream);
