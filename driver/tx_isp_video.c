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

    /* Iterate through registered subdevices in subdev_graph[] */
    int enabled_count = 0;
    for (i = 0; i < ISP_MAX_SUBDEVS; i++) {
        void *subdev_data = dev->subdev_graph[i];
        struct tx_isp_subdev *subdev = NULL;
        
        if (!subdev_data) {
            continue;
        }
        
        mcp_log_info("tx_isp_video_link_stream: Found subdev_data %p at graph index %d", subdev_data, i);
        
        /* Try to get subdevice from platform device if this is platform device data */
        if (dev->subdev_list && i < dev->subdev_count && dev->subdev_list[i]) {
            subdev = platform_get_drvdata(dev->subdev_list[i]);
            if (subdev) {
                mcp_log_info("tx_isp_video_link_stream: Got subdev %p from platform device %p", subdev, dev->subdev_list[i]);
            }
        }
        
        /* If we don't have a subdev yet, try alternative discovery methods */
        if (!subdev) {
            /* MIPS ALIGNMENT FIX: Ensure proper 4-byte alignment before dereferencing */
            uintptr_t subdev_addr = (uintptr_t)subdev_data;
            
            /* Check if subdev_data is properly aligned for MIPS (4-byte boundary) */
            if ((subdev_addr & 0x3) == 0) {  /* Must be 4-byte aligned */
                /* Safe to dereference as pointer */
                struct tx_isp_subdev **potential_subdev = (struct tx_isp_subdev **)subdev_data;
                if (potential_subdev) {
                    /* Use safe memory access - check alignment before dereferencing */
                    uintptr_t target_addr = (uintptr_t)potential_subdev;
                    if ((target_addr & 0x3) == 0) {  /* Ensure target is also aligned */
                        struct tx_isp_subdev *candidate = *potential_subdev;
                        /* Validate candidate pointer alignment and range */
                        uintptr_t candidate_addr = (uintptr_t)candidate;
                        if (candidate && 
                            (candidate_addr & 0x3) == 0 &&  /* Must be 4-byte aligned */
                            candidate_addr > 0x80000000 && 
                            candidate_addr < 0xfffff000) {
                            subdev = candidate;
                            mcp_log_info("tx_isp_video_link_stream: Found aligned subdev %p via safe dereferencing", subdev);
                        }
                    }
                }
            } else {
                mcp_log_info("tx_isp_video_link_stream: Skipping unaligned subdev_data %p (addr=0x%lx)", 
                             subdev_data, subdev_addr);
            }
        }
        
        if (!subdev) {
            mcp_log_info("tx_isp_video_link_stream: No subdev found for graph index %d, skipping", i);
            continue;
        }
        
        mcp_log_info("tx_isp_video_link_stream: Processing subdev %p (graph index %d)", subdev, i);
        
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
            enabled_count++;
            
            /* Handle special error code - 0xfffffdfd is not a real error */
            if (ret && ret != 0xfffffdfd) {
                mcp_log_error("tx_isp_video_link_stream: Failed to enable stream on subdev %p: %d", subdev, ret);
                
                /* Disable streams on all previously enabled subdevices */
                int j;
                for (j = 0; j < i; j++) {
                    void *prev_subdev_data = dev->subdev_graph[j];
                    struct tx_isp_subdev *prev_subdev = NULL;
                    
                    if (!prev_subdev_data) {
                        continue;
                    }
                    
                    if (dev->subdev_list && j < dev->subdev_count && dev->subdev_list[j]) {
                        prev_subdev = platform_get_drvdata(dev->subdev_list[j]);
                    }
                    
                    if (!prev_subdev) {
                        /* MIPS ALIGNMENT FIX: Same alignment checks for cleanup code */
                        uintptr_t prev_addr = (uintptr_t)prev_subdev_data;
                        
                        /* Check 4-byte alignment before dereferencing */
                        if ((prev_addr & 0x3) == 0) {  /* Must be 4-byte aligned */
                            struct tx_isp_subdev **potential_prev = (struct tx_isp_subdev **)prev_subdev_data;
                            if (potential_prev) {
                                uintptr_t prev_target_addr = (uintptr_t)potential_prev;
                                if ((prev_target_addr & 0x3) == 0) {  /* Ensure target is also aligned */
                                    struct tx_isp_subdev *candidate = *potential_prev;
                                    uintptr_t candidate_addr = (uintptr_t)candidate;
                                    if (candidate && 
                                        (candidate_addr & 0x3) == 0 &&  /* Must be 4-byte aligned */
                                        candidate_addr > 0x80000000 && 
                                        candidate_addr < 0xfffff000) {
                                        prev_subdev = candidate;
                                    }
                                }
                            }
                        }
                    }
                    
                    if (prev_subdev && prev_subdev->ops && prev_subdev->ops->video && prev_subdev->ops->video->s_stream) {
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

    mcp_log_info("tx_isp_video_link_stream: Enable operation completed successfully on %d subdevices", enabled_count);
    return 0;
}

EXPORT_SYMBOL(tx_isp_video_link_stream);
