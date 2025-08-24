/*
* Video Class definitions of Tomahawk series SoC.
 *
 * Copyright 2017, <xianghui.shen@ingenic.com>
 *
 * This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/of.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include "../include/tx_isp.h"
#include "../include/tx_isp_core.h"
#include "../include/tx-isp-debug.h"
#include "../include/tx_isp_sysfs.h"
#include "../include/tx_isp_vic.h"
#include "../include/tx_isp_csi.h"
#include "../include/tx_isp_vin.h"
#include "../include/tx_isp_tuning.h"
#include "../include/tx-isp-device.h"
#include "../include/tx-libimp.h"

// Simple global device instance
struct tx_isp_dev *ourISPdev = NULL;

// Basic IOCTL handler matching reference behavior
static long tx_isp_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct tx_isp_dev *isp_dev = ourISPdev;
    void __user *argp = (void __user *)arg;
    int ret = 0;

    if (!isp_dev) {
        pr_err("ISP device not initialized\n");
        return -ENODEV;
    }

    pr_info("ISP IOCTL: cmd=0x%x arg=0x%lx\n", cmd, arg);

    switch (cmd) {
    case 0x40045626: {  // VIDIOC_GET_SENSOR_INFO - Simple success response
        int __user *result = (int __user *)arg;
        if (put_user(1, result)) {
            pr_err("Failed to update sensor result\n");
            return -EFAULT;
        }
        pr_info("Sensor info request: returning success (1)\n");
        return 0;
    }
    default:
        pr_info("Unhandled ioctl cmd: 0x%x\n", cmd);
        return -ENOTTY;
    }

    return ret;
}

// Simple open handler following reference pattern
int tx_isp_open(struct inode *inode, struct file *file)
{
    struct tx_isp_dev *isp = ourISPdev;
    int ret = 0;

    if (!isp) {
        pr_err("ISP device not initialized\n");
        return -ENODEV;
    }

    /* Check if already opened */
    if (isp->refcnt) {
        isp->refcnt++;
        file->private_data = isp;
        pr_info("ISP opened (refcnt=%d)\n", isp->refcnt);
        return 0;
    }

    /* Mark as open */
    isp->refcnt = 1;
    isp->is_open = true;
    file->private_data = isp;

    pr_info("ISP opened successfully\n");
    return ret;
}

// Simple release handler
static int tx_isp_release(struct inode *inode, struct file *file)
{
    struct tx_isp_dev *isp = file->private_data;

    if (!isp)
        return 0;

    /* Handle refcount */
    if (isp->refcnt > 0) {
        isp->refcnt--;
        if (isp->refcnt == 0) {
            isp->is_open = false;
        }
    }

    pr_info("ISP released (refcnt=%d)\n", isp->refcnt);
    return 0;
}

/* Character device operations */
static const struct file_operations tx_isp_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = tx_isp_unlocked_ioctl,
    .open = tx_isp_open,
    .release = tx_isp_release,
};

// Simple platform device - just the basics needed
static struct platform_device tx_isp_platform_device = {
    .name = "tx-isp",
    .id = -1,
};

// Simple platform driver - minimal implementation
static int tx_isp_platform_probe(struct platform_device *pdev)
{
    pr_info("tx_isp_platform_probe called\n");
    return 0;
}

static int tx_isp_platform_remove(struct platform_device *pdev)
{
    pr_info("tx_isp_platform_remove called\n");
    return 0;
}

static struct platform_driver tx_isp_driver = {
    .probe = tx_isp_platform_probe,
    .remove = tx_isp_platform_remove,
    .driver = {
        .name = "tx-isp",
        .owner = THIS_MODULE,
    },
};

// Misc device for creating /dev/tx-isp
static struct miscdevice tx_isp_miscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "tx-isp",
    .fops = &tx_isp_fops,
};

// Stub for private_driver_get_interface - simulate reference behavior
static int private_driver_get_interface(void)
{
    pr_info("private_driver_get_interface: checking GPIO mode\n");
    // Return 0 to indicate success (GPIO mode supported)
    return 0;
}

// Main initialization function following reference pattern exactly
static int tx_isp_init(void)
{
    int ret;
    int gpio_mode_check;

    pr_info("TX ISP driver initializing...\n");

    /* Step 1: Check driver interface (matches reference) */
    gpio_mode_check = private_driver_get_interface();
    if (gpio_mode_check != 0) {
        pr_err("VIC_CTRL : %08x\n", gpio_mode_check);
        return gpio_mode_check;
    }

    /* Allocate ISP device structure */
    ourISPdev = kzalloc(sizeof(struct tx_isp_dev), GFP_KERNEL);
    if (!ourISPdev) {
        pr_err("Failed to allocate ISP device\n");
        return -ENOMEM;
    }

    /* Initialize device structure */
    spin_lock_init(&ourISPdev->lock);
    ourISPdev->refcnt = 0;
    ourISPdev->is_open = false;

    /* Step 2: Register platform device (matches reference) */
    ret = platform_device_register(&tx_isp_platform_device);
    if (ret != 0) {
        pr_err("not support the gpio mode!\n");
        goto err_free_dev;
    }

    /* Step 3: Register platform driver (matches reference) */
    ret = platform_driver_register(&tx_isp_driver);
    if (ret != 0) {
        pr_err("Failed to register platform driver: %d\n", ret);
        platform_device_unregister(&tx_isp_platform_device);
        goto err_free_dev;
    }

    /* Step 4: Register misc device to create /dev/tx-isp */
    ret = misc_register(&tx_isp_miscdev);
    if (ret != 0) {
        pr_err("Failed to register misc device: %d\n", ret);
        platform_driver_unregister(&tx_isp_driver);
        platform_device_unregister(&tx_isp_platform_device);
        goto err_free_dev;
    }

    /* Initialize proc entries */
    ret = tx_isp_proc_init(ourISPdev);
    if (ret) {
        pr_err("Failed to create proc entries: %d\n", ret);
        misc_deregister(&tx_isp_miscdev);
        platform_driver_unregister(&tx_isp_driver);
        platform_device_unregister(&tx_isp_platform_device);
        goto err_free_dev;
    }

    pr_info("TX ISP driver initialized successfully\n");
    pr_info("Device nodes created:\n");
    pr_info("  /dev/tx-isp (major=10, minor=dynamic)\n");
    pr_info("  /proc/jz/isp/isp-w02\n");
    
    return 0;

err_free_dev:
    kfree(ourISPdev);
    ourISPdev = NULL;
    return ret;
}

static void tx_isp_exit(void)
{
    pr_info("TX ISP driver exiting...\n");

    if (ourISPdev) {
        /* Clean up proc entries */
        tx_isp_proc_exit(ourISPdev);
        
        /* Unregister misc device */
        misc_deregister(&tx_isp_miscdev);
        
        /* Unregister platform components */
        platform_driver_unregister(&tx_isp_driver);
        platform_device_unregister(&tx_isp_platform_device);
        
        /* Free device structure */
        kfree(ourISPdev);
        ourISPdev = NULL;
    }

    pr_info("TX ISP driver removed\n");
}

module_init(tx_isp_init);
module_exit(tx_isp_exit);

MODULE_AUTHOR("Matt Davis <matteius@gmail.com>");
MODULE_DESCRIPTION("TX-ISP Camera Driver");
MODULE_LICENSE("GPL");