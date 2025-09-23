#ifndef __TX_ISP_SUBDEV_HELPERS_H__
#define __TX_ISP_SUBDEV_HELPERS_H__

#include "tx_isp.h"
#include "tx-isp-device.h"

/* Helper functions to find subdevices by name instead of hardcoded array indices */

/**
 * tx_isp_find_subdev_by_name - Find subdevice by platform device name
 * @isp_dev: Main ISP device
 * @name: Platform device name to search for
 * 
 * Returns: Pointer to subdev if found, NULL otherwise
 */
static inline struct tx_isp_subdev *tx_isp_find_subdev_by_name(struct tx_isp_dev *isp_dev, const char *name)
{
    int i;
    
    if (!isp_dev || !name) {
        return NULL;
    }
    
    for (i = 0; i < ISP_MAX_SUBDEVS; i++) {
        struct tx_isp_subdev *sd = isp_dev->subdevs[i];
        if (sd && sd->pdev && sd->pdev->name && strcmp(sd->pdev->name, name) == 0) {
            return sd;
        }
    }
    
    return NULL;
}

/**
 * tx_isp_get_csi_subdev - Get CSI subdevice (isp-w00)
 */
static inline struct tx_isp_subdev *tx_isp_get_csi_subdev(struct tx_isp_dev *isp_dev)
{
    return tx_isp_find_subdev_by_name(isp_dev, "isp-w00");
}

/**
 * tx_isp_get_vin_subdev - Get VIN subdevice (isp-w01)  
 */
static inline struct tx_isp_subdev *tx_isp_get_vin_subdev(struct tx_isp_dev *isp_dev)
{
    return tx_isp_find_subdev_by_name(isp_dev, "isp-w01");
}

/**
 * tx_isp_get_vic_subdev - Get VIC subdevice (isp-w02)
 */
static inline struct tx_isp_subdev *tx_isp_get_vic_subdev(struct tx_isp_dev *isp_dev)
{
    return tx_isp_find_subdev_by_name(isp_dev, "isp-w02");
}

/**
 * tx_isp_get_core_subdev - Get Core subdevice (isp-m0)
 */
static inline struct tx_isp_subdev *tx_isp_get_core_subdev(struct tx_isp_dev *isp_dev)
{
    return tx_isp_find_subdev_by_name(isp_dev, "isp-m0");
}

/**
 * tx_isp_get_fs_subdev - Get FS (Frame Source) subdevice (isp-fs)
 */
static inline struct tx_isp_subdev *tx_isp_get_fs_subdev(struct tx_isp_dev *isp_dev)
{
    return tx_isp_find_subdev_by_name(isp_dev, "isp-fs");
}

/* Duplicate removed - tx_isp_get_fs_subdev already defined above */

/**
 * tx_isp_find_sensor_subdev - Find first available sensor subdevice
 * Sensors are typically registered at indices 4+ but we should search dynamically
 */
static inline struct tx_isp_subdev *tx_isp_find_sensor_subdev(struct tx_isp_dev *isp_dev)
{
    int i;
    
    if (!isp_dev) {
        return NULL;
    }
    
    for (i = 0; i < ISP_MAX_SUBDEVS; i++) {
        struct tx_isp_subdev *sd = isp_dev->subdevs[i];
        if (sd && sd->ops && sd->ops->sensor) {
            return sd;
        }
    }
    
    return NULL;
}

/**
 * tx_isp_get_sensor_subdev - Get sensor subdevice (new helper function)
 * This replaces the old hardcoded index access
 * Note: Different from tx_isp_get_sensor() which returns struct tx_isp_sensor*
 */
static inline struct tx_isp_subdev *tx_isp_get_sensor_subdev(struct tx_isp_dev *isp_dev)
{
    return tx_isp_find_sensor_subdev(isp_dev);
}

/**
 * tx_isp_find_free_subdev_slot - Find first available slot in subdev array
 * @isp_dev: Main ISP device
 * 
 * Returns: Index of free slot, or -1 if array is full
 */
static inline int tx_isp_find_free_subdev_slot(struct tx_isp_dev *isp_dev)
{
    int i;
    
    if (!isp_dev) {
        return -1;
    }
    
    for (i = 0; i < ISP_MAX_SUBDEVS; i++) {
        if (isp_dev->subdevs[i] == NULL) {
            return i;
        }
    }
    
    return -1; /* Array is full */
}

/**
 * tx_isp_register_subdev_by_name - Register subdev in array by finding free slot
 * @isp_dev: Main ISP device
 * @sd: Subdevice to register
 * 
 * Returns: Index where subdev was registered, or -1 on failure
 */
static inline int tx_isp_register_subdev_by_name(struct tx_isp_dev *isp_dev, struct tx_isp_subdev *sd)
{
    int slot;
    
    if (!isp_dev || !sd) {
        return -1;
    }
    
    slot = tx_isp_find_free_subdev_slot(isp_dev);
    if (slot >= 0) {
        isp_dev->subdevs[slot] = sd;
        sd->isp = isp_dev;
    }
    
    return slot;
}

/**
 * tx_isp_unregister_subdev_by_name - Unregister subdev from array
 * @isp_dev: Main ISP device  
 * @name: Platform device name to unregister
 * 
 * Returns: 0 on success, -1 if not found
 */
static inline int tx_isp_unregister_subdev_by_name(struct tx_isp_dev *isp_dev, const char *name)
{
    int i;
    
    if (!isp_dev || !name) {
        return -1;
    }
    
    for (i = 0; i < ISP_MAX_SUBDEVS; i++) {
        struct tx_isp_subdev *sd = isp_dev->subdevs[i];
        if (sd && sd->pdev && sd->pdev->name && strcmp(sd->pdev->name, name) == 0) {
            isp_dev->subdevs[i] = NULL;
            sd->isp = NULL;
            return 0;
        }
    }
    
    return -1; /* Not found */
}

/**
 * tx_isp_count_registered_subdevs - Count number of registered subdevices
 */
static inline int tx_isp_count_registered_subdevs(struct tx_isp_dev *isp_dev)
{
    int i, count = 0;
    
    if (!isp_dev) {
        return 0;
    }
    
    for (i = 0; i < ISP_MAX_SUBDEVS; i++) {
        if (isp_dev->subdevs[i] != NULL) {
            count++;
        }
    }
    
    return count;
}

/**
 * tx_isp_debug_print_subdevs - Debug function to print all registered subdevices
 */
static inline void tx_isp_debug_print_subdevs(struct tx_isp_dev *isp_dev)
{
    int i;
    
    if (!isp_dev) {
        pr_info("tx_isp_debug_print_subdevs: isp_dev is NULL\n");
        return;
    }
    
    pr_info("=== ISP Subdevice Array Status ===\n");
    for (i = 0; i < ISP_MAX_SUBDEVS; i++) {
        struct tx_isp_subdev *sd = isp_dev->subdevs[i];
        if (sd) {
            const char *name = (sd->pdev && sd->pdev->name) ? sd->pdev->name : "unknown";
            pr_info("  [%d]: %s (sd=%p)\n", i, name, sd);
        } else {
            pr_info("  [%d]: (empty)\n", i);
        }
    }
    pr_info("=== End Subdevice Array ===\n");
}

#endif /* __TX_ISP_SUBDEV_HELPERS_H__ */
