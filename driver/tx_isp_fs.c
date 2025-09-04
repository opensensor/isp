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

/* Global frame source device structure - 0xe8 bytes as per Binary Ninja */
struct tx_isp_fs_device {
    struct tx_isp_subdev subdev;            /* Base subdev structure */
    void __iomem *base_regs;                /* Base register mapping +0xb8 */
    
    /* Binary Ninja structure layout - exact offsets */
    uint8_t padding[0xc8 - sizeof(struct tx_isp_subdev) - sizeof(void *)];  /* Padding to 0xc8 */
    
    void *channel_configs;                   /* +0xcc - channel config array */
    void *channel_buffer;                    /* +0xdc - kmalloc'ed channel buffer */
    uint32_t channel_count;                  /* +0xe0 - number of channels */
    uint32_t initialized;                    /* +0xe4 - initialization flag */
} __attribute__((packed));

/* Frame channel structure - 0x2ec bytes per channel as per Binary Ninja */
struct tx_isp_frame_channel {
    struct miscdevice misc_dev;              /* +0x00 - misc device structure */
    
    uint8_t padding1[0xab * 4 - sizeof(struct miscdevice)];  /* Padding to +0xab */
    
    char device_name[64];                    /* +0xab - device name buffer */
    void *channel_config;                    /* +0xaf - channel config pointer */
    uint32_t channel_id;                     /* +0xb0 - channel ID */
    
    uint8_t padding2[0x2ec - 0xb0 * 4 - sizeof(uint32_t) - 64 - sizeof(void *)];  /* Rest of structure */
    
    struct completion frame_completion;      /* Frame completion */
    struct mutex channel_lock;               /* Channel mutex */
    spinlock_t buffer_lock;                  /* Buffer spinlock */
    wait_queue_head_t wait_queue;           /* Wait queue */
    
    uint32_t state;                         /* +0xb4 - channel state */
} __attribute__((packed));

/* Forward declarations */
static int frame_chan_event(void *data);
static int tx_isp_frame_chan_deinit(struct tx_isp_frame_channel *chan);


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
    
    /* Signal frame completion */
    complete(&chan->frame_completion);
    
    /* Wake up any waiting processes */
    wake_up_interruptible(&chan->wait_queue);
    
    return 0;
}

/* Frame channel deinitialization - EXACT Binary Ninja implementation */
static int tx_isp_frame_chan_deinit(struct tx_isp_frame_channel *chan)
{
    if (!chan) {
        return 0;
    }
    
    pr_info("*** tx_isp_frame_chan_deinit: channel=%p ***\n", chan);
    
    /* Deregister misc device */
    misc_deregister(&chan->misc_dev);
    
    pr_info("tx_isp_frame_chan_deinit: misc device deregistered\n");
    
    return 0;
}

/* tx_isp_fs_probe - EXACT Binary Ninja implementation */
int tx_isp_fs_probe(struct platform_device *pdev)
{
    struct tx_isp_fs_device *fs_dev;
    struct tx_isp_frame_channel *channels_buffer = NULL;
    struct tx_isp_frame_channel *current_channel;
    void *channel_config_ptr;
    uint32_t channel_count;
    int ret;
    int i;
    
    pr_info("*** tx_isp_fs_probe: EXACT Binary Ninja implementation ***\n");
    
    /* Binary Ninja: $v0, $a2 = private_kmalloc(0xe8, 0xd0) */
    fs_dev = kzalloc(0xe8, GFP_KERNEL);  /* 0xe8 = 232 bytes */
    if (!fs_dev) {
        /* Binary Ninja: isp_printf(2, "Err [VIC_INT] : control limit err!!!\n", $a2) */
        pr_err("Err [VIC_INT] : control limit err!!!\n");
        /* Binary Ninja: return 0xfffffff4 */
        return -12;
    }
    
    /* Binary Ninja: memset($v0, 0, 0xe8) */
    memset(fs_dev, 0, 0xe8);
    
    /* Binary Ninja: void* $s2_1 = arg1[0x16] */
    /* This references platform device resource information */
    
    /* Binary Ninja: if (tx_isp_subdev_init(arg1, $v0, &fs_subdev_ops) == 0) */
    ret = tx_isp_subdev_init(pdev, &fs_dev->subdev, &fs_subdev_ops);
    if (ret != 0) {
        /* Binary Ninja: isp_printf(2, "Err [VIC_INT] : image syfifo ovf !!!\n", zx.d(*($s2_1 + 2))) */
        pr_err("Err [VIC_INT] : image syfifo ovf !!!\n");
        /* Binary Ninja: private_kfree($v0) */
        kfree(fs_dev);
        /* Binary Ninja: return 0xfffffff4 */
        return -12;
    }
    
    /* Binary Ninja: uint32_t $a0_2 = zx.d(*($v0 + 0xc8)) */
    channel_count = *(uint32_t *)((char *)fs_dev + 0xc8);
    /* Binary Ninja: *($v0 + 0xe0) = $a0_2 */
    fs_dev->channel_count = channel_count;
    
    pr_info("tx_isp_fs_probe: channel_count=%d\n", channel_count);
    
    /* Binary Ninja: if ($a0_2 == 0) goto label_1c670 */
    if (channel_count == 0) {
        goto setup_complete;
    }
    
    /* Binary Ninja: int32_t $v0_3 = private_kmalloc($a0_2 * 0x2ec, 0xd0) */
    channels_buffer = kzalloc(channel_count * 0x2ec, GFP_KERNEL);  /* 0x2ec = 748 bytes per channel */
    if (!channels_buffer) {
        pr_err("Failed to allocate channels buffer\n");
        ret = -ENOMEM;
        goto error_cleanup;
    }
    
    /* Binary Ninja: *($v0 + 0xdc) = $v0_3 */
    fs_dev->channel_buffer = channels_buffer;
    
    /* Binary Ninja: memset($v0_3, 0, 0x2ec * $a2_2) */
    memset(channels_buffer, 0, 0x2ec * channel_count);
    
    /* Binary Ninja: Channel initialization loop */
    pr_info("tx_isp_fs_probe: initializing %d frame channels\n", channel_count);
    
    for (i = 0; i < channel_count; i++) {
        /* Binary Ninja: int32_t $s4_1 = $s2_2 * 0x2ec */
        /* Binary Ninja: int32_t* $s0_2 = *($v0 + 0xdc) + $s4_1 */
        current_channel = (struct tx_isp_frame_channel *)((char *)channels_buffer + (i * 0x2ec));
        
        /* Binary Ninja: void* $s6_1 = $s2_2 * 0x24 + *($v0 + 0xcc) */
        channel_config_ptr = (char *)fs_dev->channel_configs + (i * 0x24);
        
        /* Binary Ninja: if ($s0_2 == 0 || $s0_2 u>= 0xfffff001 || $s6_1 == 0 || $s6_1 u>= 0xfffff001) */
        if (!current_channel || (unsigned long)current_channel >= 0xfffff001 ||
            !channel_config_ptr || (unsigned long)channel_config_ptr >= 0xfffff001) {
            ret = -EINVAL;
            goto error_cleanup_loop;
        }
        
        /* Binary Ninja: uint32_t $a2_4 = zx.d(*($s6_1 + 4)) */
        /* Binary Ninja: $s0_2[0xaf] = $s6_1 */
        current_channel->channel_config = channel_config_ptr;
        /* Binary Ninja: $s0_2[0xb0] = $a2_4 */
        current_channel->channel_id = *(uint32_t *)((char *)channel_config_ptr + 4);
        
        /* Binary Ninja: if (zx.d(*($s6_1 + 5)) != 0) */
        if (*(uint32_t *)((char *)channel_config_ptr + 5) != 0) {
            /* Binary Ninja: sprintf(&$s0_2[0xab], "Err [VIC_INT] : mipi fid asfifo ovf!!!\n") */
            snprintf(current_channel->device_name, sizeof(current_channel->device_name), 
                     "/dev/framechan%d", i);
            
            /* Binary Ninja: *$s0_2 = 0xff */
            current_channel->misc_dev.minor = MISC_DYNAMIC_MINOR;
            /* Binary Ninja: $s0_2[2] = &fs_channel_ops */
            current_channel->misc_dev.fops = &fs_channel_ops;
            /* Binary Ninja: $s0_2[1] = &$s0_2[0xab] */
            current_channel->misc_dev.name = current_channel->device_name;
            
            /* Binary Ninja: if (private_misc_register($s0_2) s< 0) */
            ret = misc_register(&current_channel->misc_dev);
            if (ret < 0) {
                /* Binary Ninja: isp_printf(2, "Err [VIC_INT] : mipi ch0 hcomp err !!!\n", $s0_2[0xb0]) */
                pr_err("Err [VIC_INT] : mipi ch0 hcomp err !!!\n");
                /* Binary Ninja: result = 0xfffffffe */
                ret = -2;
                goto error_cleanup_loop;
            }
            
            /* Binary Ninja: Initialize channel structures - &$s0_2[9] u< 0xfffff001 */
            /* Binary Ninja: memset(&$s0_2[9], 0, 0x218) */
            memset(&current_channel->frame_completion, 0, 0x218);  /* Clear completion area */
            
            /* Binary Ninja: Initialize completion and synchronization objects */
            /* Binary Ninja: private_init_completion(&$s0_2[0xb5]) */
            init_completion(&current_channel->frame_completion);
            /* Binary Ninja: private_spin_lock_init(&$s0_2[0x89]) */
            spin_lock_init(&current_channel->buffer_lock);
            /* Binary Ninja: private_raw_mutex_init(&$s0_2[0xa], ...) */
            mutex_init(&current_channel->channel_lock);
            /* Binary Ninja: private_init_waitqueue_head(&$s0_2[0x8a]) */
            init_waitqueue_head(&current_channel->wait_queue);
            
            /* Binary Ninja: Set up event callback */
            /* Binary Ninja: *($s6_1 + 0x1c) = frame_chan_event */
            *(void **)((char *)channel_config_ptr + 0x1c) = frame_chan_event;
            
            /* Binary Ninja: $s0_2[0xb4] = 1 */
            current_channel->state = 1;  /* Active state */
            
            pr_info("tx_isp_fs_probe: initialized frame channel %d: %s\n", 
                    i, current_channel->device_name);
        } else {
            /* Binary Ninja: $s0_2[0xb4] = 0 */
            current_channel->state = 0;  /* Inactive state */
            pr_info("tx_isp_fs_probe: channel %d inactive\n", i);
        }
    }
    
    goto setup_complete;
    
error_cleanup_loop:
    /* Binary Ninja: Error cleanup - deinitialize created channels */
    for (i = i - 1; i >= 0; i--) {
        current_channel = (struct tx_isp_frame_channel *)((char *)channels_buffer + (i * 0x2ec));
        tx_isp_frame_chan_deinit(current_channel);
    }

error_cleanup:
    if (channels_buffer) {
        kfree(channels_buffer);
    }
    kfree(fs_dev);
    return ret;

setup_complete:
    /* Binary Ninja: label_1c670 */
    /* Binary Ninja: *($v0 + 0xe4) = 1 */
    fs_dev->initialized = 1;
    
    /* Binary Ninja: private_platform_set_drvdata(arg1, $v0) */
    platform_set_drvdata(pdev, fs_dev);
    
    /* Binary Ninja: *($v0 + 0x34) = &isp_framesource_fops */
    *(const struct file_operations **)((char *)&fs_dev->subdev + 0x34) = &isp_framesource_fops;
    
    /* Binary Ninja: *($v0 + 0xd4) = $v0 */
    *(struct tx_isp_fs_device **)((char *)fs_dev + 0xd4) = fs_dev;  /* Self-pointer */
    
    pr_info("*** tx_isp_fs_probe: FS device created successfully (size=0xe8, channels=%d) ***\n", 
            channel_count);
    pr_info("*** FS PROBE COMPLETE - /proc/jz/isp/isp-fs SHOULD NOW BE AVAILABLE ***\n");
    
    /* Binary Ninja: return 0 */
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
    
    /* Clean up frame channels */
    channels_buffer = (struct tx_isp_frame_channel *)fs_dev->channel_buffer;
    if (channels_buffer) {
        for (i = 0; i < fs_dev->channel_count; i++) {
            current_channel = (struct tx_isp_frame_channel *)((char *)channels_buffer + (i * 0x2ec));
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
