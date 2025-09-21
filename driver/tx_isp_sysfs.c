#include <linux/module.h>
#include <linux/device.h>
#include <linux/slab.h>
#include "include/tx_isp_sysfs.h"
#include "../include/tx_isp.h"
#include "../include/tx_isp_core_device.h"

/* External system register functions from reference driver */
extern void system_reg_write(u32 reg, u32 value);
extern uint32_t system_reg_read(u32 reg);

/* Helper function to show sensor info */
static ssize_t sensor_info_show(struct device *dev,
                               struct device_attribute *attr, char *buf)
{
    struct tx_isp_dev *isp = dev_get_drvdata(dev);
    if (!isp)
        return -EINVAL;

    return sprintf(buf, "Name: %s\nResolution: %dx%d\nInterface: %d\n",
                  isp->sensor_name,
                  isp->sensor_width,
                  isp->sensor_height,
                  isp->sensor_interface_type);
}
static DEVICE_ATTR_RO(sensor_info);

/* Helper function to show/store WDR mode */
static ssize_t wdr_mode_show(struct device *dev,
                            struct device_attribute *attr, char *buf)
{
    struct tx_isp_dev *isp = dev_get_drvdata(dev);
    if (!isp)
        return -EINVAL;

    if (isp->core_dev) {
        return sprintf(buf, "%d\n", isp->core_dev->wdr_mode);
    } else {
        return sprintf(buf, "0\n");  /* Default WDR mode */
    }
}

static ssize_t wdr_mode_store(struct device *dev,
                             struct device_attribute *attr,
                             const char *buf, size_t count)
{
    struct tx_isp_dev *isp = dev_get_drvdata(dev);
    unsigned long mode;
    int ret;

    if (!isp)
        return -EINVAL;

    ret = kstrtoul(buf, 0, &mode);
    if (ret)
        return ret;

    /* Update WDR mode in core device */
    if (isp->core_dev) {
        isp->core_dev->wdr_mode = mode;
    } else {
        pr_err("wdr_mode_store: No core device available\n");
        return -ENODEV;
    }
    
    /* Update hardware if needed */
    if (isp->streaming_enabled) {
        u32 ctrl = system_reg_read(ISP_CTRL);  /* Use system_reg_read from reference driver */
        if (mode)
            ctrl |= ISP_CTRL_WDR_EN;
        else
            ctrl &= ~ISP_CTRL_WDR_EN;
        system_reg_write(ISP_CTRL, ctrl);  /* Use system_reg_write from reference driver */
    }

    return count;
}
static DEVICE_ATTR_RW(wdr_mode);

/* Helper function to show/store streaming state */
static ssize_t streaming_show(struct device *dev,
                            struct device_attribute *attr, char *buf)
{
    struct tx_isp_dev *isp = dev_get_drvdata(dev);
    if (!isp)
        return -EINVAL;

    return sprintf(buf, "%d\n", isp->streaming_enabled ? 1 : 0);
}

static ssize_t streaming_store(struct device *dev,
                             struct device_attribute *attr,
                             const char *buf, size_t count)
{
    struct tx_isp_dev *isp = dev_get_drvdata(dev);
    bool enable;
    int ret;

    if (!isp)
        return -EINVAL;

    ret = kstrtobool(buf, &enable);
    if (ret)
        return ret;

    if (enable == isp->streaming_enabled)
        return count;

    if (enable) {
        /* Start streaming */
        isp_write32(ISP_CTRL, ISP_CTRL_EN);
        isp->streaming_enabled = true;
    } else {
        /* Stop streaming */
        isp_write32(ISP_CTRL, 0);
        isp->streaming_enabled = false;
    }

    return count;
}
static DEVICE_ATTR_RW(streaming);

/* Helper function to show statistics */
static ssize_t statistics_show(struct device *dev,
                             struct device_attribute *attr, char *buf)
{
    struct tx_isp_dev *isp = dev_get_drvdata(dev);
    unsigned long flags;
    int len = 0;

    if (!isp)
        return -EINVAL;

    spin_lock_irqsave(&isp->ae_lock, flags);
    extern atomic64_t frame_done_cnt;
    len += sprintf(buf + len, "Frame Count: %llu\n", (unsigned long long)atomic64_read(&frame_done_cnt));
    if (isp->ae_valid) {
        len += sprintf(buf + len, "AE Enabled: %d\n", isp->ae_algo_enabled);
        /* Add more statistics as needed */
    }
    spin_unlock_irqrestore(&isp->ae_lock, flags);

    return len;
}
static DEVICE_ATTR_RO(statistics);

/* Array of attributes */
static struct attribute *tx_isp_attrs[] = {
    &dev_attr_sensor_info.attr,
    &dev_attr_wdr_mode.attr,
    &dev_attr_streaming.attr,
    &dev_attr_statistics.attr,
    NULL
};

/* Attribute group */
static const struct attribute_group tx_isp_attr_group = {
    .attrs = tx_isp_attrs,
};

/* Initialize sysfs interface */
int tx_isp_sysfs_init(struct tx_isp_dev *isp)
{
    int ret;

    if (!isp || !isp->dev)
        return -EINVAL;

    /* Create sysfs group */
    ret = sysfs_create_group(&isp->dev->kobj, &tx_isp_attr_group);
    if (ret) {
        ISP_ERROR("Failed to create sysfs group\n");
        return ret;
    }

    return 0;
}

/* Cleanup sysfs interface */
void tx_isp_sysfs_exit(struct tx_isp_dev *isp)
{
    if (!isp || !isp->dev)
        return;

    sysfs_remove_group(&isp->dev->kobj, &tx_isp_attr_group);
}
