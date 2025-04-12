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
 * It follows the OEM implementation pattern by checking each subdevice in order.
 *
 * Returns 0 on success, negative error code on failure
 */
int tx_isp_video_link_stream(struct tx_isp_dev *dev)
{
    int ret = 0;
    int i;

    pr_info("Enabling video link stream\n");

    if (!dev) {
        pr_err("Invalid ISP device\n");
        return -EINVAL;
    }

    /* Initialize CSI hardware before streaming */
    if (dev->csi_dev) {
        struct tx_isp_subdev *csi_sd = NULL;
        
        /* Get the CSI subdevice */
        if (dev->csi_pdev) {
            csi_sd = platform_get_drvdata(dev->csi_pdev);
            if (csi_sd && csi_sd->ops && csi_sd->ops->core && csi_sd->ops->core->init) {
                pr_info("Initializing CSI hardware before streaming\n");
                
                /* Make sure the CSI device is properly linked to the subdevice */
                if (dev->csi_dev && !tx_isp_get_subdevdata(csi_sd)) {
                    tx_isp_set_subdevdata(csi_sd, dev->csi_dev);
                }
                
                /* Also make sure the CSI device has the subdevice pointer */
                if (dev->csi_dev && dev->csi_dev->sd == NULL) {
                    dev->csi_dev->sd = csi_sd;
                }
                
                /* Initialize the CSI hardware */
                ret = csi_sd->ops->core->init(csi_sd, 1);
                if (ret) {
                    pr_err("Failed to initialize CSI hardware: %d\n", ret);
                    return ret;
                }
            } else {
                pr_err("ISP device is NULL\n");
                return -EINVAL;
            }
        } else {
            pr_err("CSI platform device is NULL\n");
            return -EINVAL;
        }
    } else {
        pr_err("ISP device is NULL\n");
        return -EINVAL;
    }

    /* Iterate through all modules in the module_graph (up to 16 as in OEM code) */
    for (i = 0; i < 0x10; i++) {
        void *module = dev->module_graph[i];
        struct tx_isp_subdev *subdev = NULL;
        
        if (!module) {
            continue;
        }
        
        /* Get the subdevice from the module - handle alignment carefully */
        /* The module structure has the subdev at offset 0xc4 */
        void *subdev_ptr;
        /* Use memcpy to avoid unaligned access */
        memcpy(&subdev_ptr, ((char *)module) + 0xc4, sizeof(void *));
        subdev = (struct tx_isp_subdev *)subdev_ptr;
        
        if (!subdev) {
            continue;
        }
        
        pr_info("Checking subdev %p (index %d)\n", subdev, i);
        
        /* Check if this subdevice has video operations */
        if (subdev->ops && subdev->ops->video && subdev->ops->video->s_stream) {
            pr_info("Enabling stream on subdev %p\n", subdev);
            
            /* Enable streaming on this subdevice */
            ret = subdev->ops->video->s_stream(subdev, 1);
            
            /* Handle special error code */
            if (ret && ret != 0xfffffdfd) {
                pr_err("Failed to enable stream on subdev %p: %d\n", subdev, ret);
                
                /* Disable streams on all previously enabled subdevices */
                int j;
                for (j = 0; j < i; j++) {
                    void *prev_module = dev->module_graph[j];
                    struct tx_isp_subdev *prev_subdev = NULL;
                    
                    if (!prev_module) {
                        continue;
                    }
                    
                    /* Use memcpy to avoid unaligned access */
                    void *prev_subdev_ptr;
                    memcpy(&prev_subdev_ptr, ((char *)prev_module) + 0xc4, sizeof(void *));
                    prev_subdev = (struct tx_isp_subdev *)prev_subdev_ptr;
                    
                    if (prev_subdev && prev_subdev->ops && prev_subdev->ops->video && 
                        prev_subdev->ops->video->s_stream) {
                        prev_subdev->ops->video->s_stream(prev_subdev, 0);
                    }
                }
                
                return ret;
            }
        }
    }

    pr_info("Video link stream enabled\n");
    return 0;
}

EXPORT_SYMBOL(tx_isp_video_link_stream);
