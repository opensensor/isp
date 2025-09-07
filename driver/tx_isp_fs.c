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

/* Frame source channel configuration structure - 0x24 bytes per channel */
struct tx_isp_fs_channel_config {
    uint32_t field_00;                      /* 0x00: Configuration field */
    uint32_t field_04;                      /* 0x04: Pad ID field (referenced at +4) */
    uint32_t channel_enabled;               /* 0x08: Channel enable flag (checked at +5) */
    uint32_t field_0c[4];                   /* 0x0c-0x1b: Reserved fields */
    void (*event_callback)(void *data);     /* 0x1c: Event callback function pointer */
    uint32_t field_20;                      /* 0x20: Final field */
} __attribute__((packed));

/* Frame source device structure - 0xe8 bytes to match Binary Ninja */
struct tx_isp_fs_device {
    struct tx_isp_subdev subdev;                    /* 0x00: Base subdev structure */
    
    /* Padding to reach proper offsets based on Binary Ninja analysis */
    uint8_t padding_to_cc[0xcc - sizeof(struct tx_isp_subdev)]; /* Pad to 0xcc */
    
    struct tx_isp_fs_channel_config *channel_configs;   /* 0xcc: Channel config array */
    uint32_t channel_count_field;                       /* 0xd0: Channel count field (read at +0xc8) */
    void *self_pointer;                                 /* 0xd4: Self-pointer (set at +0xd4) */
    uint32_t field_d8;                                  /* 0xd8: Reserved field */
    struct tx_isp_frame_channel *channel_buffer;        /* 0xdc: Channel buffer pointer */
    uint32_t channel_count;                             /* 0xe0: Active channel count */
    uint32_t initialized;                               /* 0xe4: Initialization flag */
} __attribute__((packed));

/* Verify struct size matches Binary Ninja expectation */
static_assert(sizeof(struct tx_isp_fs_device) == 0xe8, "tx_isp_fs_device size must be 0xe8 bytes");
static_assert(sizeof(struct tx_isp_fs_channel_config) == 0x24, "tx_isp_fs_channel_config size must be 0x24 bytes");


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


/* tx_isp_fs_probe - Memory-safe implementation following Binary Ninja reference */
int tx_isp_fs_probe(struct platform_device *pdev)
{
    struct tx_isp_fs_device *fs_dev;
    struct tx_isp_frame_channel *channels_buffer = NULL;
    struct tx_isp_frame_channel *current_channel;
    struct tx_isp_fs_channel_config *channel_config;
    uint32_t channel_count;
    int ret;
    int i;
    
    /* MCP log entry point */
    pr_info("*** MCP LOG: tx_isp_fs_probe entry ***\n");
    
    /* Allocate main FS device structure - must be 0xe8 bytes */
    fs_dev = kzalloc(sizeof(struct tx_isp_fs_device), GFP_KERNEL);
    if (!fs_dev) {
        pr_err("Err [VIC_INT] : control limit err!!!\n");
        /* MCP log allocation failure */
        pr_info("*** MCP LOG: tx_isp_fs_probe allocation failed ***\n");
        return -12;
    }
    
    /* MCP log successful allocation */
    pr_info("*** MCP LOG: tx_isp_fs_probe allocated fs_dev=%p, size=0x%lx ***\n", 
            fs_dev, sizeof(struct tx_isp_fs_device));
    
    /* Initialize subdev - following reference driver pattern */
    ret = tx_isp_subdev_init(pdev, &fs_dev->subdev, &fs_subdev_ops);
    if (ret != 0) {
        pr_err("Err [VIC_INT] : image syfifo ovf !!!\n");
        /* MCP log subdev init failure */
        pr_info("*** MCP LOG: tx_isp_fs_probe subdev_init failed, ret=%d ***\n", ret);
        kfree(fs_dev);
        return -12;
    }
    
    /* MCP log successful subdev init */
    pr_info("*** MCP LOG: tx_isp_fs_probe subdev initialized ***\n");
    
    /* Memory-safe channel count access - use proper struct member */
    channel_count = fs_dev->channel_count_field;
    fs_dev->channel_count = channel_count;
    
    /* MCP log channel count */
    pr_info("*** MCP LOG: tx_isp_fs_probe channel_count=%d ***\n", channel_count);
    
    /* Handle zero channels case */
    if (channel_count == 0) {
        goto setup_complete;
    }
    
    /* Allocate channel buffer - memory-safe allocation */
    channels_buffer = kzalloc(channel_count * sizeof(struct tx_isp_frame_channel), GFP_KERNEL);
    if (!channels_buffer) {
        pr_err("Failed to allocate channels buffer\n");
        /* MCP log channel buffer allocation failure */
        pr_info("*** MCP LOG: tx_isp_fs_probe channel buffer allocation failed ***\n");
        ret = -ENOMEM;
        goto error_cleanup;
    }
    
    /* Memory-safe assignment */
    fs_dev->channel_buffer = channels_buffer;
    
    /* MCP log channel buffer allocation */
    pr_info("*** MCP LOG: tx_isp_fs_probe allocated channel buffer=%p, count=%d ***\n", 
            channels_buffer, channel_count);
    
    /* Initialize channels using memory-safe array access */
    for (i = 0; i < channel_count; i++) {
        /* Memory-safe channel access - use proper array indexing */
        current_channel = &channels_buffer[i];
        
        /* Memory-safe channel config access */
        if (fs_dev->channel_configs) {
            channel_config = &fs_dev->channel_configs[i];
        } else {
            /* Handle missing channel config gracefully */
            pr_warn("tx_isp_fs_probe: missing channel configs, channel %d inactive\n", i);
            current_channel->state = 0;
            continue;
        }
        
        /* Validate pointers before use */
        if (!current_channel || !channel_config) {
            pr_err("tx_isp_fs_probe: invalid channel pointers at index %d\n", i);
            ret = -EINVAL;
            goto error_cleanup_loop;
        }
        
        /* Memory-safe pad ID assignment */
        current_channel->pad_id = i;
        
        /* MCP log channel initialization start */
        pr_info("*** MCP LOG: tx_isp_fs_probe initializing channel %d ***\n", i);
        
        /* Memory-safe channel enable check */
        if (channel_config->channel_enabled != 0) {
            /* Create channel device name safely */
            snprintf(current_channel->name, sizeof(current_channel->name), 
                     "/dev/framechan%d", i);
            
            /* Configure misc device safely */
            current_channel->misc.minor = MISC_DYNAMIC_MINOR;
            current_channel->misc.fops = &fs_channel_ops;
            current_channel->misc.name = current_channel->name;
            
            /* Register misc device */
            ret = misc_register(&current_channel->misc);
            if (ret < 0) {
                pr_err("Err [VIC_INT] : mipi ch0 hcomp err !!!\n");
                /* MCP log misc register failure */
                pr_info("*** MCP LOG: tx_isp_fs_probe misc_register failed for channel %d, ret=%d ***\n", i, ret);
                ret = -2;
                goto error_cleanup_loop;
            }
            
            /* Initialize synchronization objects safely */
            init_completion(&current_channel->frame_done);
            spin_lock_init(&current_channel->slock);
            mutex_init(&current_channel->mlock);
            init_waitqueue_head(&current_channel->wait);
            
            /* Memory-safe event callback assignment */
            channel_config->event_callback = frame_chan_event;
            
            /* Set channel as active */
            current_channel->state = 1;
            current_channel->active = 1;
            
            /* MCP log successful channel initialization */
            pr_info("*** MCP LOG: tx_isp_fs_probe initialized channel %d: %s ***\n", 
                    i, current_channel->name);
        } else {
            /* Channel inactive */
            current_channel->state = 0;
            current_channel->active = 0;
            /* MCP log inactive channel */
            pr_info("*** MCP LOG: tx_isp_fs_probe channel %d inactive ***\n", i);
        }
    }
    
    goto setup_complete;
    
error_cleanup_loop:
    /* Memory-safe error cleanup */
    for (i = i - 1; i >= 0; i--) {
        current_channel = &channels_buffer[i];
        if (current_channel->active) {
            tx_isp_frame_chan_deinit(current_channel);
        }
    }

error_cleanup:
    if (channels_buffer) {
        kfree(channels_buffer);
    }
    kfree(fs_dev);
    /* MCP log error cleanup */
    pr_info("*** MCP LOG: tx_isp_fs_probe error cleanup completed, ret=%d ***\n", ret);
    return ret;

setup_complete:
    /* Memory-safe final setup */
    fs_dev->initialized = 1;
    
    platform_set_drvdata(pdev, fs_dev);
    
    /* Memory-safe file operations assignment */
    tx_isp_set_subdev_nodeops(&fs_dev->subdev, (struct file_operations *)&isp_framesource_fops);
    
    /* Memory-safe self-pointer assignment */
    fs_dev->self_pointer = fs_dev;
    
    /* MCP log successful completion */
    pr_info("*** MCP LOG: tx_isp_fs_probe completed successfully, channels=%d ***\n", channel_count);
    pr_info("*** FS PROBE COMPLETE - /proc/jz/isp/isp-fs SHOULD NOW BE AVAILABLE ***\n");
    
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
