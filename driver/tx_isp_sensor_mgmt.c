// SPDX-License-Identifier: GPL-2.0+
/*
 * tx_isp_sensor_mgmt.c - ISP Sensor Management and Registration
 * 
 * This module implements the sensor registration mechanism that allows
 * external sensor modules (like gc2053.c) to register with the ISP core.
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include "../include/tx_isp.h"
#include "../include/tx-isp-common.h"
#include "../include/tx-isp-debug.h"

/* Global ISP core pointer for sensor registration - matches Binary Ninja */
static struct tx_isp_dev *g_ispcore = NULL;
EXPORT_SYMBOL(g_ispcore);

/* Registered sensor list */
static LIST_HEAD(sensor_list);
static DEFINE_MUTEX(sensor_list_mutex);

/* Structure to track registered sensors */
struct registered_sensor {
    struct list_head list;
    char name[32];
    struct tx_isp_subdev *sd;
    struct tx_isp_sensor *sensor;
    uint32_t chip_id;
    int i2c_addr;
};

/**
 * sensor_early_init - EXACT Binary Ninja reference implementation
 * This is the key function that sensor modules call to register with ISP core
 */
int sensor_early_init(struct tx_isp_dev *isp_dev)
{
    int32_t result = 0xffffffea; /* -EINVAL */
    
    ISP_INFO("*** sensor_early_init: called with isp_dev=%p ***\n", isp_dev);
    
    if (isp_dev != NULL) {
        result = 0;
        
        /* Binary Ninja: if (g_ispcore == 0) g_ispcore = arg1 */
        if (g_ispcore == NULL) {
            g_ispcore = isp_dev;
            ISP_INFO("*** sensor_early_init: g_ispcore set to %p ***\n", g_ispcore);
        }
    }
    
    ISP_INFO("*** sensor_early_init: result=%d ***\n", result);
    return result;
}
EXPORT_SYMBOL(sensor_early_init);

/**
 * tx_isp_register_sensor_subdev - Register a sensor with the ISP framework
 * This is called by sensor drivers (like gc2053.c) during their probe
 */
int tx_isp_register_sensor_subdev(struct tx_isp_subdev *sd, struct tx_isp_sensor *sensor)
{
    struct registered_sensor *reg_sensor;
    
    if (!sd || !sensor) {
        ISP_ERROR("tx_isp_register_sensor_subdev: Invalid parameters\n");
        return -EINVAL;
    }
    
    ISP_INFO("*** tx_isp_register_sensor_subdev: Registering sensor %s ***\n",
             sensor->info.name[0] ? sensor->info.name : "unknown");
    
    /* Allocate registration structure */
    reg_sensor = kzalloc(sizeof(*reg_sensor), GFP_KERNEL);
    if (!reg_sensor) {
        ISP_ERROR("Failed to allocate sensor registration structure\n");
        return -ENOMEM;
    }
    
    /* Fill registration info */
    INIT_LIST_HEAD(&reg_sensor->list);
    strncpy(reg_sensor->name, sensor->info.name, sizeof(reg_sensor->name) - 1);
    reg_sensor->sd = sd;
    reg_sensor->sensor = sensor;
    reg_sensor->chip_id = sensor->video.attr ? sensor->video.attr->chip_id : 0;
    reg_sensor->i2c_addr = sensor->info.i2c.addr;
    
    /* Add to sensor list */
    mutex_lock(&sensor_list_mutex);
    list_add_tail(&reg_sensor->list, &sensor_list);
    mutex_unlock(&sensor_list_mutex);
    
    ISP_INFO("*** tx_isp_register_sensor_subdev: Sensor %s registered successfully ***\n",
             reg_sensor->name);
    ISP_INFO("***   - Chip ID: 0x%x ***\n", reg_sensor->chip_id);
    ISP_INFO("***   - I2C Address: 0x%02x ***\n", reg_sensor->i2c_addr);
    
    /* If ISP core is available, notify it */
    if (g_ispcore) {
        ISP_INFO("*** tx_isp_register_sensor_subdev: ISP core available, setting up sensor link ***\n");
        
        /* Set the sensor in ISP core structure */
        g_ispcore->sensor_sd = sd;
        
        ISP_INFO("*** tx_isp_register_sensor_subdev: Sensor linked to ISP core successfully ***\n");
    } else {
        ISP_INFO("*** tx_isp_register_sensor_subdev: ISP core not ready yet, sensor will be linked later ***\n");
    }
    
    return 0;
}
EXPORT_SYMBOL(tx_isp_register_sensor_subdev);

/**
 * tx_isp_find_sensor_by_name - Find a registered sensor by name
 */
struct tx_isp_subdev *tx_isp_find_sensor_by_name(const char *name)
{
    struct registered_sensor *reg_sensor;
    struct tx_isp_subdev *result = NULL;
    
    if (!name) {
        return NULL;
    }
    
    mutex_lock(&sensor_list_mutex);
    list_for_each_entry(reg_sensor, &sensor_list, list) {
        if (strcmp(reg_sensor->name, name) == 0) {
            result = reg_sensor->sd;
            break;
        }
    }
    mutex_unlock(&sensor_list_mutex);
    
    if (result) {
        ISP_INFO("tx_isp_find_sensor_by_name: Found sensor %s\n", name);
    } else {
        ISP_INFO("tx_isp_find_sensor_by_name: Sensor %s not found\n", name);
    }
    
    return result;
}
EXPORT_SYMBOL(tx_isp_find_sensor_by_name);

/**
 * tx_isp_find_sensor_by_chip_id - Find a registered sensor by chip ID
 */
struct tx_isp_subdev *tx_isp_find_sensor_by_chip_id(uint32_t chip_id)
{
    struct registered_sensor *reg_sensor;
    struct tx_isp_subdev *result = NULL;
    
    mutex_lock(&sensor_list_mutex);
    list_for_each_entry(reg_sensor, &sensor_list, list) {
        if (reg_sensor->chip_id == chip_id) {
            result = reg_sensor->sd;
            break;
        }
    }
    mutex_unlock(&sensor_list_mutex);
    
    if (result) {
        ISP_INFO("tx_isp_find_sensor_by_chip_id: Found sensor with chip ID 0x%x\n", chip_id);
    } else {
        ISP_INFO("tx_isp_find_sensor_by_chip_id: No sensor found with chip ID 0x%x\n", chip_id);
    }
    
    return result;
}
EXPORT_SYMBOL(tx_isp_find_sensor_by_chip_id);

/**
 * tx_isp_get_default_sensor - Get the first registered sensor (default)
 */
struct tx_isp_subdev *tx_isp_get_default_sensor(void)
{
    struct registered_sensor *reg_sensor;
    struct tx_isp_subdev *result = NULL;
    
    mutex_lock(&sensor_list_mutex);
    if (!list_empty(&sensor_list)) {
        reg_sensor = list_first_entry(&sensor_list, struct registered_sensor, list);
        result = reg_sensor->sd;
        ISP_INFO("tx_isp_get_default_sensor: Using default sensor %s\n", reg_sensor->name);
    }
    mutex_unlock(&sensor_list_mutex);
    
    if (!result) {
        ISP_INFO("tx_isp_get_default_sensor: No sensors registered yet\n");
    }
    
    return result;
}
EXPORT_SYMBOL(tx_isp_get_default_sensor);

/**
 * tx_isp_list_registered_sensors - List all registered sensors (for debugging)
 */
void tx_isp_list_registered_sensors(void)
{
    struct registered_sensor *reg_sensor;
    int count = 0;
    
    ISP_INFO("*** tx_isp_list_registered_sensors: Listing all registered sensors ***\n");
    
    mutex_lock(&sensor_list_mutex);
    list_for_each_entry(reg_sensor, &sensor_list, list) {
        count++;
        ISP_INFO("*** Sensor %d: %s (chip_id=0x%x, i2c_addr=0x%02x, sd=%p) ***\n",
                 count, reg_sensor->name, reg_sensor->chip_id, 
                 reg_sensor->i2c_addr, reg_sensor->sd);
    }
    mutex_unlock(&sensor_list_mutex);
    
    if (count == 0) {
        ISP_INFO("*** tx_isp_list_registered_sensors: No sensors registered yet ***\n");
    } else {
        ISP_INFO("*** tx_isp_list_registered_sensors: Total %d sensors registered ***\n", count);
    }
}
EXPORT_SYMBOL(tx_isp_list_registered_sensors);

/**
 * tx_isp_get_sensor_ops - Get sensor operations from registered sensor
 * This is what the ISP wrapper should call instead of accessing NULL pointers
 */
struct tx_isp_subdev_ops *tx_isp_get_sensor_ops(const char *sensor_name)
{
    struct tx_isp_subdev *sensor_sd;
    
    if (!sensor_name) {
        /* Try to get default sensor */
        sensor_sd = tx_isp_get_default_sensor();
    } else {
        /* Find specific sensor */
        sensor_sd = tx_isp_find_sensor_by_name(sensor_name);
    }
    
    if (sensor_sd && sensor_sd->ops) {
        ISP_INFO("tx_isp_get_sensor_ops: Found sensor ops for %s\n", sensor_name ? sensor_name : "default");
        return sensor_sd->ops;
    }
    
    ISP_ERROR("tx_isp_get_sensor_ops: No sensor ops found for %s\n", sensor_name ? sensor_name : "default");
    return NULL;
}
EXPORT_SYMBOL(tx_isp_get_sensor_ops);

/**
 * tx_isp_call_sensor_init - Call sensor init function safely
 */
int tx_isp_call_sensor_init(const char *sensor_name, int enable)
{
    struct tx_isp_subdev_ops *ops = tx_isp_get_sensor_ops(sensor_name);
    
    if (ops && ops->core && ops->core->init) {
        ISP_INFO("tx_isp_call_sensor_init: Calling sensor init for %s, enable=%d\n",
                 sensor_name ? sensor_name : "default", enable);
        
        /* Find the sensor subdev */
        struct tx_isp_subdev *sensor_sd = sensor_name ? 
            tx_isp_find_sensor_by_name(sensor_name) : tx_isp_get_default_sensor();
        
        if (sensor_sd) {
            return ops->core->init(sensor_sd, enable);
        }
    }
    
    ISP_ERROR("*** tx_isp_call_sensor_init: NO REAL SENSOR DRIVER INIT FUNCTION AVAILABLE! ***\n");
    return -ENODEV;
}
EXPORT_SYMBOL(tx_isp_call_sensor_init);

/**
 * tx_isp_call_sensor_g_chip_ident - Call sensor g_chip_ident function safely
 */
int tx_isp_call_sensor_g_chip_ident(const char *sensor_name, struct tx_isp_chip_ident *chip)
{
    struct tx_isp_subdev_ops *ops = tx_isp_get_sensor_ops(sensor_name);
    
    if (ops && ops->core && ops->core->g_chip_ident) {
        ISP_INFO("tx_isp_call_sensor_g_chip_ident: Calling sensor g_chip_ident for %s\n",
                 sensor_name ? sensor_name : "default");
        
        /* Find the sensor subdev */
        struct tx_isp_subdev *sensor_sd = sensor_name ? 
            tx_isp_find_sensor_by_name(sensor_name) : tx_isp_get_default_sensor();
        
        if (sensor_sd) {
            return ops->core->g_chip_ident(sensor_sd, chip);
        }
    }
    
    ISP_ERROR("*** tx_isp_call_sensor_g_chip_ident: NO REAL SENSOR DRIVER G_CHIP_IDENT FUNCTION AVAILABLE! ***\n");
    return -ENODEV;
}
EXPORT_SYMBOL(tx_isp_call_sensor_g_chip_ident);

/**
 * tx_isp_call_sensor_s_stream - Call sensor s_stream function safely
 */
int tx_isp_call_sensor_s_stream(const char *sensor_name, int enable)
{
    struct tx_isp_subdev_ops *ops = tx_isp_get_sensor_ops(sensor_name);
    
    if (ops && ops->video && ops->video->s_stream) {
        ISP_INFO("tx_isp_call_sensor_s_stream: Calling sensor s_stream for %s, enable=%d\n",
                 sensor_name ? sensor_name : "default", enable);
        ISP_INFO("*** tx_isp_call_sensor_s_stream: THIS SHOULD WRITE 0x3e=0x91 ***\n");
        
        /* Find the sensor subdev */
        struct tx_isp_subdev *sensor_sd = sensor_name ? 
            tx_isp_find_sensor_by_name(sensor_name) : tx_isp_get_default_sensor();
        
        if (sensor_sd) {
            int ret = ops->video->s_stream(sensor_sd, enable);
            if (ret == 0) {
                ISP_INFO("*** tx_isp_call_sensor_s_stream: SUCCESS - 0x3e=0x91 should now be written! ***\n");
            } else {
                ISP_ERROR("*** tx_isp_call_sensor_s_stream: FAILED: %d ***\n", ret);
            }
            return ret;
        }
    }
    
    ISP_ERROR("*** tx_isp_call_sensor_s_stream: NO REAL SENSOR DRIVER S_STREAM FUNCTION AVAILABLE! ***\n");
    ISP_ERROR("*** tx_isp_call_sensor_s_stream: THIS IS WHY 0x3e=0x91 IS NOT BEING WRITTEN! ***\n");
    return -ENODEV;
}
EXPORT_SYMBOL(tx_isp_call_sensor_s_stream);

/**
 * Module init/exit functions
 */
static int __init tx_isp_sensor_mgmt_init(void)
{
    ISP_INFO("*** TX ISP Sensor Management Module Initialized ***\n");
    return 0;
}

static void __exit tx_isp_sensor_mgmt_exit(void)
{
    struct registered_sensor *reg_sensor, *tmp;
    
    /* Clean up sensor list */
    mutex_lock(&sensor_list_mutex);
    list_for_each_entry_safe(reg_sensor, tmp, &sensor_list, list) {
        list_del(&reg_sensor->list);
        kfree(reg_sensor);
    }
    mutex_unlock(&sensor_list_mutex);
    
    ISP_INFO("*** TX ISP Sensor Management Module Exited ***\n");
}

module_init(tx_isp_sensor_mgmt_init);
module_exit(tx_isp_sensor_mgmt_exit);

MODULE_DESCRIPTION("TX ISP Sensor Management and Registration");
MODULE_LICENSE("GPL");
