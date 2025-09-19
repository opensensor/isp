#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/of.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/clk.h>
#include <linux/vmalloc.h>
#include <linux/completion.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/wait.h>
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

/* Binary Ninja reference global variables */
static struct tx_isp_fs_device *dump_fsd = NULL;  /* Global FS device pointer */
extern struct tx_isp_dev *ourISPdev;


/* Forward declarations */
static int frame_chan_event(void *data);


/* FS subdev core operations */
static int fs_core_ops_init(struct tx_isp_subdev *sd, int enable)
{
    pr_info("*** fs_core_ops_init: enable=%d ***\n", enable);
    return 0;
}

/* FS subdev sensor operations */
static int fs_sensor_ops_ioctl(struct tx_isp_subdev *sd, unsigned int cmd, void *arg)
{
    pr_info("*** fs_sensor_ops_ioctl: cmd=0x%x ***\n", cmd);
    return 0;
}

/* FS subdev core operations structure */
static struct tx_isp_subdev_core_ops fs_core_ops = {
    .init = fs_core_ops_init,
};

/* FS subdev sensor operations structure */
static struct tx_isp_subdev_sensor_ops fs_sensor_ops = {
    .ioctl = fs_sensor_ops_ioctl,
};

/* FS complete subdev operations structure */
static struct tx_isp_subdev_ops fs_subdev_ops = {
    .core = &fs_core_ops,
    .sensor = &fs_sensor_ops,
};

/* Frame source file operations - matching isp_framesource_fops */
static int fs_chardev_open(struct inode *inode, struct file *file)
{
    pr_info("*** FS device opened ***\n");
    return 0;
}

static int fs_chardev_release(struct inode *inode, struct file *file)
{
    pr_info("*** FS device released ***\n");
    return 0;
}

static long fs_chardev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    pr_info("*** FS IOCTL: cmd=0x%x arg=0x%lx ***\n", cmd, arg);
    return 0;
}

/* FS character device operations - isp_framesource_fops */
static const struct file_operations isp_framesource_fops = {
    .owner = THIS_MODULE,
    .open = fs_chardev_open,
    .release = fs_chardev_release,
    .unlocked_ioctl = fs_chardev_ioctl,
    .llseek = default_llseek,
};

/* Frame channel operations structure - fs_channel_ops */
static const struct file_operations fs_channel_ops = {
    .owner = THIS_MODULE,
    .open = fs_chardev_open,
    .release = fs_chardev_release,
    .unlocked_ioctl = fs_chardev_ioctl,
    .llseek = default_llseek,
};

/* Frame channel event handler - EXACT Binary Ninja implementation */
static int frame_chan_event(void *data)
{
    struct tx_isp_frame_channel *chan = (struct tx_isp_frame_channel *)data;
    
    if (!chan) {
        pr_err("frame_chan_event: NULL channel data\n");
        return -EINVAL;
    }
    
    pr_info("*** frame_chan_event: channel=%p, state=%d ***\n", chan, chan->state);
    
    /* Signal frame completion - use correct field name */
    complete(&chan->frame_done);
    
    /* Wake up any waiting processes - use correct field name */
    wake_up_interruptible(&chan->wait);
    
    return 0;
}

/* Frame channel initialization/deinitialization */
void tx_isp_frame_chan_deinit(struct tx_isp_frame_channel *chan)
{
    pr_info("Deinitializing frame channel\n");
    if (chan->active) {
        misc_deregister(&chan->misc);
    }
}


/* tx_isp_fs_probe - EXACT Binary Ninja reference implementation */
int tx_isp_fs_probe(struct platform_device *pdev)
{
    struct tx_isp_fs_device *fs_dev;
    struct tx_isp_platform_data *pdata;
    struct tx_isp_frame_channel *channels_buffer = NULL;
    struct tx_isp_frame_channel *current_channel;
    void *channel_config_ptr;
    uint32_t channel_count;
    int ret;
    int i;

    /* Binary Ninja: private_kmalloc(0xe8, 0xd0) */
    fs_dev = private_kmalloc(sizeof(struct tx_isp_fs_device), GFP_KERNEL);
    if (!fs_dev) {
        /* Binary Ninja: isp_printf(2, "Err [VIC_INT] : control limit err!!!\n", $a2) */
        isp_printf(2, "Err [VIC_INT] : control limit err!!!\n", sizeof(struct tx_isp_fs_device));
        return -EFAULT;  /* Binary Ninja returns 0xfffffff4 */
    }
    
    /* Binary Ninja: memset($v0, 0, 0xe8) */
    memset(fs_dev, 0, sizeof(struct tx_isp_fs_device));

    /* Binary Ninja: void* $s2_1 = arg1[0x16] */
    pdata = pdev->dev.platform_data;

    /* Binary Ninja: tx_isp_subdev_init(arg1, $v0, &fs_subdev_ops) */
    ret = tx_isp_subdev_init(pdev, &fs_dev->subdev, &fs_subdev_ops);
    if (ret != 0) {
        /* Binary Ninja: isp_printf(2, "Err [VIC_INT] : image syfifo ovf !!!\n", zx.d(*($s2_1 + 2))) */
        if (pdata) {
            isp_printf(2, "Err [VIC_INT] : image syfifo ovf !!!\n", pdata->sensor_type);
        } else {
            isp_printf(2, "Err [VIC_INT] : image syfifo ovf !!!\n", 0);
        }
        /* Binary Ninja: private_kfree($v0) */
        private_kfree(fs_dev);
        return -EFAULT;  /* Binary Ninja returns 0xfffffff4 */
    }

    /* Binary Ninja: uint32_t $a0_2 = zx.d(*($v0 + 0xc8)) */
    channel_count = fs_dev->channel_count;  /* Get channel count from offset 0xc8 */

    /* Binary Ninja: *($v0 + 0xe0) = $a0_2 */
    fs_dev->initialized = channel_count;  /* Store channel count at offset 0xe0 */

    /* Binary Ninja: if ($a0_2 == 0) goto label_1c670 */
    if (channel_count == 0) {
        goto setup_complete;
    }
    
    /* SAFE: Use proper struct size instead of fixed 0x2ec */
    channels_buffer = kzalloc(channel_count * sizeof(struct tx_isp_frame_channel), GFP_KERNEL);
    if (!channels_buffer) {
        pr_err("Failed to allocate channels buffer\n");
        ret = -ENOMEM;
        goto error_cleanup;
    }
    
    /* SAFE: Direct struct member access */
    fs_dev->channel_buffer = channels_buffer;
    
    /* Binary Ninja: Channel initialization loop */
    pr_info("tx_isp_fs_probe: initializing %d frame channels\n", channel_count);
    
    for (i = 0; i < channel_count; i++) {
        /* SAFE: Use proper array indexing instead of offset calculation */
        current_channel = &channels_buffer[i];
        
        /* SAFE: Use proper array indexing for channel configs */
        channel_config_ptr = (char *)fs_dev->channel_configs + (i * 0x24);
        
        /* SAFE: Simple null check - remove unsafe pointer range checks */
        if (!current_channel || !channel_config_ptr) {
            ret = -EINVAL;
            goto error_cleanup_loop;
        }
        
        /* Set pad info based on channel config */
        current_channel->pad_id = i;
        
        /* Binary Ninja: if (zx.d(*($s6_1 + 5)) != 0) */
        if (*(uint32_t *)((char *)channel_config_ptr + 5) != 0) {
            /* Binary Ninja: sprintf(&$s0_2[0xab], "Err [VIC_INT] : mipi fid asfifo ovf!!!\n") */
            snprintf(current_channel->name, sizeof(current_channel->name), 
                     "/dev/framechan%d", i);
            
            /* Binary Ninja: *$s0_2 = 0xff */
            current_channel->misc.minor = MISC_DYNAMIC_MINOR;
            /* Binary Ninja: $s0_2[2] = &fs_channel_ops */
            current_channel->misc.fops = &fs_channel_ops;
            /* Binary Ninja: $s0_2[1] = &$s0_2[0xab] */
            current_channel->misc.name = current_channel->name;
            
            /* Binary Ninja: if (private_misc_register($s0_2) s< 0) */
            ret = misc_register(&current_channel->misc);
            if (ret < 0) {
                /* Binary Ninja: isp_printf(2, "Err [VIC_INT] : mipi ch0 hcomp err !!!\n", $s0_2[0xb0]) */
                pr_err("Err [VIC_INT] : mipi ch0 hcomp err !!!\n");
                /* Binary Ninja: result = 0xfffffffe */
                ret = -2;
                goto error_cleanup_loop;
            }
            
            /* Binary Ninja: Initialize completion and synchronization objects */
            /* Binary Ninja: private_init_completion(&$s0_2[0xb5]) */
            init_completion(&current_channel->frame_done);
            /* Binary Ninja: private_spin_lock_init(&$s0_2[0x89]) */
            spin_lock_init(&current_channel->slock);
            /* Binary Ninja: private_raw_mutex_init(&$s0_2[0xa], ...) */
            mutex_init(&current_channel->mlock);
            /* Binary Ninja: private_init_waitqueue_head(&$s0_2[0x8a]) */
            init_waitqueue_head(&current_channel->wait);
            
            /* Binary Ninja: Set up event callback */
            /* Binary Ninja: *($s6_1 + 0x1c) = frame_chan_event */
            *(void **)((char *)channel_config_ptr + 0x1c) = frame_chan_event;
            
            /* Binary Ninja: $s0_2[0xb4] = 1 */
            current_channel->state = 1;  /* Active state */
            
            pr_info("tx_isp_fs_probe: initialized frame channel %d: %s\n",
                    i, current_channel->name);
        } else {
            /* Binary Ninja: $s0_2[0xb4] = 0 */
            current_channel->state = 0;  /* Inactive state */
            pr_info("tx_isp_fs_probe: channel %d inactive\n", i);
        }
    }
    
    goto setup_complete;
    
error_cleanup_loop:
    /* SAFE: Error cleanup - deinitialize created channels */
    for (i = i - 1; i >= 0; i--) {
        current_channel = &channels_buffer[i];
        tx_isp_frame_chan_deinit(current_channel);
    }

error_cleanup:
    if (channels_buffer) {
        kfree(channels_buffer);
    }
    kfree(fs_dev);
    return ret;

setup_complete:
    /* Binary Ninja: *($v0 + 0xe4) = 1 */
    fs_dev->initialized = 1;

    /* Binary Ninja: private_platform_set_drvdata(arg1, $v0) */
    private_platform_set_drvdata(pdev, fs_dev);

    /* Binary Ninja: *($v0 + 0x34) = &isp_framesource_fops */
    tx_isp_set_subdev_nodeops(&fs_dev->subdev, &isp_framesource_fops);

    /* Binary Ninja: *($v0 + 0xd4) = $v0 */
    fs_dev->self_ptr = fs_dev;  /* Self-pointer for validation */

    /* Binary Ninja: dump_fsd = $v0 */
    dump_fsd = fs_dev;

    /* CRITICAL FIX: Link this properly initialized FS device to the global ISP device */
    if (ourISPdev) {
        ourISPdev->fs_dev = fs_dev;
        pr_info("*** FS PROBE: CRITICAL - Linked FS device to ourISPdev->fs_dev: %p ***\n", ourISPdev->fs_dev);
    }

    return 0;
}

/* FS remove function */
int tx_isp_fs_remove(struct platform_device *pdev)
{
    struct tx_isp_fs_device *fs_dev = platform_get_drvdata(pdev);
    struct tx_isp_frame_channel *channels_buffer;
    struct tx_isp_frame_channel *current_channel;
    int i;
    
    pr_info("*** tx_isp_fs_remove ***\n");
    
    if (!fs_dev) {
        return 0;
    }
    
    /* SAFE: Clean up frame channels with proper array indexing */
    channels_buffer = (struct tx_isp_frame_channel *)fs_dev->channel_buffer;
    if (channels_buffer) {
        for (i = 0; i < fs_dev->channel_count; i++) {
            current_channel = &channels_buffer[i];
            if (current_channel->state) {
                tx_isp_frame_chan_deinit(current_channel);
            }
        }
        kfree(channels_buffer);
    }
    
    /* Clean up subdev */
    tx_isp_subdev_deinit(&fs_dev->subdev);
    
    kfree(fs_dev);
    
    pr_info("FS device removed\n");
    return 0;
}



/* FS platform driver structure */
static struct platform_driver tx_isp_fs_platform_driver = {
    .probe = tx_isp_fs_probe,
    .remove = tx_isp_fs_remove,
    .driver = {
        .name = "tx-isp-fs",
        .owner = THIS_MODULE,
    },
};


/* FS platform init/exit functions */
int __init tx_isp_fs_platform_init(void)
{
    int ret;
    
    pr_info("*** TX ISP FS PLATFORM DRIVER REGISTRATION ***\n");
    
    ret = platform_driver_register(&tx_isp_fs_platform_driver);
    if (ret) {
        pr_err("Failed to register FS platform driver: %d\n", ret);
        return ret;
    }
    
    pr_info("FS platform driver registered successfully\n");
    return 0;
}

void __exit tx_isp_fs_platform_exit(void)
{
    pr_info("*** TX ISP FS PLATFORM DRIVER UNREGISTRATION ***\n");
    platform_driver_unregister(&tx_isp_fs_platform_driver);
    pr_info("FS platform driver unregistered\n");
}

/* Export symbols */
EXPORT_SYMBOL(tx_isp_fs_probe);
EXPORT_SYMBOL(tx_isp_fs_platform_init);
EXPORT_SYMBOL(tx_isp_fs_platform_exit);
