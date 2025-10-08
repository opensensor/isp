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
 * tx_isp_get_csi_subdev - Get CSI subdevice (isp-w01)
 */
static inline struct tx_isp_subdev *tx_isp_get_csi_subdev(struct tx_isp_dev *isp_dev)
{
    return tx_isp_find_subdev_by_name(isp_dev, "isp-w01");
}

/**
 * tx_isp_get_vin_subdev - Get VIN subdevice (isp-w00)
 */
static inline struct tx_isp_subdev *tx_isp_get_vin_subdev(struct tx_isp_dev *isp_dev)
{
    return tx_isp_find_subdev_by_name(isp_dev, "isp-w00");
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
 * tx_isp_find_sensor_subdev - Find first available REAL sensor subdevice
 * CRITICAL FIX: Search only in sensor slots (5+) and exclude ISP core devices
 * Subdev layout: 0=CSI(isp-w01), 1=VIC(isp-w02), 2=VIN(isp-w00), 3=Core(isp-m0), 4=fs(isp-fs), 5+=REAL_SENSORS
 */
static inline struct tx_isp_subdev *tx_isp_find_sensor_subdev(struct tx_isp_dev *isp_dev)
{
    int i;

    if (!isp_dev) {
        return NULL;
    }

    /* CRITICAL FIX: Search ALL slots (0 to ISP_MAX_SUBDEVS) to find sensor */
    /* The sensor can be at any index depending on registration order */
    for (i = 0; i < ISP_MAX_SUBDEVS; i++) {
        struct tx_isp_subdev *sd = isp_dev->subdevs[i];
        if (sd && sd->ops && sd->ops->sensor) {
            /* Additional validation: make sure this is NOT an ISP core device */
            if (sd->pdev && sd->pdev->name) {
                /* Exclude all known ISP core device names */
                if (strcmp(sd->pdev->name, "isp-m0") != 0 &&
                    strcmp(sd->pdev->name, "isp-w00") != 0 &&
                    strcmp(sd->pdev->name, "isp-w01") != 0 &&
                    strcmp(sd->pdev->name, "isp-w02") != 0 &&
                    strcmp(sd->pdev->name, "isp-fs") != 0) {
                    /* This looks like a real sensor device */
                    return sd;
                }
            }
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
 * tx_isp_register_subdev_by_name - Register subdev in array at FIXED slot based on device name
 * @isp_dev: Main ISP device
 * @sd: Subdevice to register
 *
 * CRITICAL: Subdev layout MUST be: 0=CSI, 1=VIC, 2=VIN, 3=Core, 4=fs, 5+=REAL_SENSORS
 * This matches the reference driver's expected layout.
 *
 * Returns: Index where subdev was registered, or -1 on failure
 */
static inline int tx_isp_register_subdev_by_name(struct tx_isp_dev *isp_dev, struct tx_isp_subdev *sd)
{
    int slot = -1;
    int i;
    const char *dev_name;

    if (!isp_dev || !sd) {
        return -1;
    }

    /* CRITICAL FIX: Check if this subdev is already registered to prevent duplicates */
    for (i = 0; i < ISP_MAX_SUBDEVS; i++) {
        if (isp_dev->subdevs[i] == sd) {
            /* Already registered - return existing slot */
            pr_info("*** tx_isp_register_subdev_by_name: DUPLICATE DETECTED - sd=%p already at slot %d ***\n", sd, i);
            return i;
        }
    }

    /* Get device name from platform device */
    dev_name = (sd->pdev && sd->pdev->name) ? sd->pdev->name : NULL;

    if (!dev_name) {
        pr_err("*** tx_isp_register_subdev_by_name: No device name - cannot determine slot ***\n");
        return -1;
    }

    /* CRITICAL: Assign FIXED slots based on device name to match reference driver layout */
    /* Layout: 0=CSI(isp-w01), 1=VIC(isp-w02), 2=VIN(isp-w00), 3=Core(isp-m0), 4=fs(isp-fs), 5+=SENSORS */

    if (strcmp(dev_name, "isp-w01") == 0) {
        /* CSI device - MUST be at index 0 */
        slot = 0;
        pr_info("*** tx_isp_register_subdev_by_name: CSI device '%s' assigned to FIXED slot %d ***\n", dev_name, slot);
    } else if (strcmp(dev_name, "isp-w02") == 0) {
        /* VIC device - MUST be at index 1 */
        slot = 1;
        pr_info("*** tx_isp_register_subdev_by_name: VIC device '%s' assigned to FIXED slot %d ***\n", dev_name, slot);
    } else if (strcmp(dev_name, "isp-w00") == 0) {
        /* VIN device - MUST be at index 2 */
        slot = 2;
        pr_info("*** tx_isp_register_subdev_by_name: VIN device '%s' assigned to FIXED slot %d ***\n", dev_name, slot);
    } else if (strcmp(dev_name, "isp-m0") == 0 || strcmp(dev_name, "tx-isp") == 0) {
        /* Core device - MUST be at index 3 */
        slot = 3;
        pr_info("*** tx_isp_register_subdev_by_name: Core device '%s' assigned to FIXED slot %d ***\n", dev_name, slot);
    } else if (strcmp(dev_name, "isp-fs") == 0) {
        /* FS device - MUST be at index 4 */
        slot = 4;
        pr_info("*** tx_isp_register_subdev_by_name: FS device '%s' assigned to FIXED slot %d ***\n", dev_name, slot);
    } else {
        /* Sensor or other device - find first free slot starting from index 5 */
        for (i = 5; i < ISP_MAX_SUBDEVS; i++) {
            if (isp_dev->subdevs[i] == NULL) {
                slot = i;
                break;
            }
        }
        if (slot >= 0) {
            pr_info("*** tx_isp_register_subdev_by_name: Sensor/other device '%s' assigned to slot %d ***\n", dev_name, slot);
        } else {
            pr_err("*** tx_isp_register_subdev_by_name: No free sensor slots for device '%s' ***\n", dev_name);
            return -1;
        }
    }

    /* Check if slot is already occupied */
    if (isp_dev->subdevs[slot] != NULL) {
        pr_err("*** tx_isp_register_subdev_by_name: CRITICAL ERROR - slot %d already occupied by %p, cannot register '%s' ***\n",
               slot, isp_dev->subdevs[slot], dev_name);
        return -1;
    }

    /* Register at the determined slot */
    isp_dev->subdevs[slot] = sd;
    sd->isp = isp_dev;

    pr_info("*** tx_isp_register_subdev_by_name: Successfully registered '%s' at slot %d ***\n", dev_name, slot);

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
