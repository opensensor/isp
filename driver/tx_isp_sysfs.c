#include <linux/module.h>
#include <linux/device.h>
#include <linux/slab.h>
#include "include/tx_isp_sysfs.h"
#include "../include/tx_isp.h"

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

    return sprintf(buf, "%d\n", isp->wdr_mode);
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

    /* Update WDR mode */
    isp->wdr_mode = mode;
    
    /* Update hardware if needed */
    if (isp->streaming_enabled) {
        u32 ctrl = isp_read32(ISP_CTRL);
        if (mode)
            ctrl |= ISP_CTRL_WDR_EN;
        else
            ctrl &= ~ISP_CTRL_WDR_EN;
        isp_write32(ISP_CTRL, ctrl);
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
    len += sprintf(buf + len, "Frame Count: %u\n", isp->frame_count);
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
