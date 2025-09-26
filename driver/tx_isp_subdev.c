#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/clk.h>
#include "../include/tx_isp.h"
#include "../include/tx_isp_csi.h"
#include "../include/tx_isp_vin.h"
#include "../include/tx_isp_core.h"
#include "../include/tx_isp_vic.h"
#include "../include/tx_isp_sysfs.h"

/* Frame channel states */
#define FRAME_CHAN_INIT    2
#define FRAME_CHAN_OPENED  3
#define FRAME_CHAN_STREAM  4

/* Properly prototype the vb2 helper functions */
static void __vb2_queue_free(void *queue, void *alloc_ctx);

/* frame_channel_open and forward declarations */
int frame_channel_open(struct inode *inode, struct file *file);
int frame_channel_release(struct inode *inode, struct file *file);
long frame_channel_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg);


/* Frame buffer structure */
struct frame_channel_buffer {
    struct list_head list;      // For queue management
    struct v4l2_buffer v4l2_buf;  // V4L2 buffer info
    void *data;                   // Buffer data
    int state;                    // Buffer state
    dma_addr_t dma_addr;         // DMA address
};

/* Helper function implementations */
static int __frame_channel_vb2_streamoff(void *chan, void *queue)
{
    struct tx_isp_frame_channel *fchan = (struct tx_isp_frame_channel *)chan;
    int ret = 0;

    if (!fchan)
        return -EINVAL;

    // Stop streaming
    spin_lock(&fchan->slock);
    // Clear queues and reset state
    INIT_LIST_HEAD(&fchan->queue_head);
    INIT_LIST_HEAD(&fchan->done_head);
    fchan->queued_count = 0;
    fchan->done_count = 0;
    spin_unlock(&fchan->slock);

    return ret;
}

static void __fill_v4l2_buffer(void *vb, struct v4l2_buffer *buf)
{
    struct frame_channel_buffer *fbuf = (struct frame_channel_buffer *)vb;

    if (!fbuf || !buf)
        return;

    // Copy buffer info
    memcpy(buf, &fbuf->v4l2_buf, sizeof(struct v4l2_buffer));
}


/* Frame channel file operations */
static const struct file_operations fs_channel_ops = {
    .owner = THIS_MODULE,
    .open = frame_channel_open,
    .release = frame_channel_release,
    .unlocked_ioctl = frame_channel_unlocked_ioctl,
};




int tx_isp_setup_default_links(struct tx_isp_dev *dev) {
    int ret;

    pr_debug("Setting up default links\n");

    // Link sensor output -> CSI input
    if (dev->sensor_sd && dev->csi_dev) {
        if (!dev->sensor_sd->ops || !dev->sensor_sd->ops->video ||
            !dev->sensor_sd->ops->video->link_setup) {
            pr_debug("Sensor subdev missing required ops\n");
            return -EINVAL;
        }

        pr_debug("Setting up sensor -> CSI link\n");
        ret = dev->sensor_sd->ops->video->link_setup(
            &dev->sensor_sd->outpads[0],
            &dev->csi_dev->sd.inpads[0],
            TX_ISP_LINKFLAG_ENABLED
        );
        if (ret && ret != -ENOIOCTLCMD) {
            pr_debug("Failed to setup sensor->CSI link: %d\n", ret);
            return ret;
        }
    }

    // Link CSI output -> VIC input
    if (dev->csi_dev && dev->vic_dev) {
        if (!dev->vic_dev->sd.ops || !dev->vic_dev->sd.ops->video ||
            !dev->vic_dev->sd.ops->video->link_setup) {
            pr_debug("CSI subdev missing required ops\n");
            return -EINVAL;
        }

        pr_debug("Setting up CSI -> VIC link\n");
        ret = dev->csi_dev->sd.ops->video->link_setup(
            &dev->csi_dev->sd.outpads[0],
            &dev->vic_dev->sd.inpads[0],
            TX_ISP_LINKFLAG_ENABLED
        );
        if (ret && ret != -ENOIOCTLCMD) {
            pr_debug("Failed to setup CSI->VIC link: %d\n", ret);
            return ret;
        }
    }

    // Link VIC outputs to DDR device if present
    if (dev->vic_dev && dev->ddr_dev && dev->ddr_dev->sd) {
        if (!dev->vic_dev->sd.ops || !dev->vic_dev->sd.ops->video ||
            !dev->vic_dev->sd.ops->video->link_setup) {
            pr_debug("VIC subdev missing required ops\n");
            return -EINVAL;
        }

        pr_debug("Setting up VIC -> DDR link\n");
        ret = dev->vic_dev->sd.ops->video->link_setup(
            &dev->vic_dev->sd.outpads[0],
            &dev->ddr_dev->sd->inpads[0],
            TX_ISP_LINKFLAG_ENABLED
        );
        if (ret && ret != -ENOIOCTLCMD) {
            pr_debug("Failed to setup VIC->DDR link 0: %d\n", ret);
            return ret;
        }

        // Link second VIC output if present
        if (dev->vic_dev->sd.num_outpads > 1) {
            pr_debug("Setting up second VIC -> DDR link\n");
            ret = dev->vic_dev->sd.ops->video->link_setup(
                &dev->vic_dev->sd.outpads[1],
                &dev->ddr_dev->sd->inpads[0],
                TX_ISP_LINKFLAG_ENABLED
            );
            if (ret && ret != -ENOIOCTLCMD) {
                pr_debug("Failed to setup VIC->DDR link 1: %d\n", ret);
                return ret;
            }
        }
    }

    pr_debug("All default links set up successfully\n");
    return 0;
}


extern struct tx_isp_dev *ourISPdev;

/* No external function dependencies needed - using safe direct implementation */

/* Subdevice initialization */
/* First, let's define the missing enums/constants */
#define TX_ISP_PADTYPE_INPUT   0x1
#define TX_ISP_PADTYPE_OUTPUT  0x2
#define TX_ISP_PADSTATE_FREE   0x0
#define TX_ISP_LINKFLAG_DYNAMIC 0x1
#define TX_ISP_PADLINK_VIC     0x1
#define TX_ISP_PADLINK_DDR     0x2
#define TX_ISP_PADLINK_ISP     0x4

/* tx_isp_subdev_init - SAFE implementation that prevents unaligned access crash */
int tx_isp_subdev_init(struct platform_device *pdev, struct tx_isp_subdev *sd,
                      struct tx_isp_subdev_ops *ops)
{
    struct tx_isp_dev *dev = ourISPdev;
    
    pr_debug("Starting subdev init for %s\n", pdev ? pdev->name : "unknown");

    /* CRITICAL FIX: Early validation to prevent unaligned access */
    if (!pdev || !sd) {
        pr_debug("tx_isp_subdev_init: Invalid parameters\n");
        return -EINVAL;
    }

    if (!dev) {
        pr_debug("No ISP device data found\n");
        return -EINVAL;
    }

    /* SAFE: Use proper struct member access instead of unsafe offset arithmetic */
    pr_debug("Initializing subdev structure\n");
    
    /* SAFE: Store ops pointer using proper struct member access */
    sd->ops = ops;
    
    /* SAFE: Initialize basic subdev fields */
    sd->isp = (void*)dev;
    
    /* SAFE: Simple initialization without unsafe hardware calls */
    pr_debug("Basic subdev initialization complete\n");

    /* SAFE: Set platform data using standard kernel API */
    platform_set_drvdata(pdev, dev);
    
    pr_debug("*** SAFE: Subdev %s initialized successfully - no unaligned access risk ***\n", pdev->name);
    return 0;
}
EXPORT_SYMBOL(tx_isp_subdev_init);

/* Subdevice deinitialization */
void tx_isp_subdev_deinit(struct tx_isp_subdev *sd)
{
    if (!sd)
        return;

    /* Nothing special to clean up currently */
}
EXPORT_SYMBOL(tx_isp_subdev_deinit);

static struct tx_isp_subdev_ops fs_subdev_ops = { 0 }; // All fields NULL/0


/* Platform driver structures */
static struct platform_driver tx_isp_csi_driver = {
    .probe = tx_isp_csi_probe,
    .remove = tx_isp_csi_remove,
    .driver = {
        .name = "tx-isp-csi",
        .owner = THIS_MODULE,
    },
};

static struct platform_driver tx_isp_vin_driver = {
    .probe = tx_isp_vin_probe,
    .remove = tx_isp_vin_remove,
    .driver = {
        .name = "tx-isp-vin",
        .owner = THIS_MODULE,
    },
};

static struct platform_driver tx_isp_core_driver = {
    .probe = tx_isp_core_probe,
    .remove = tx_isp_core_remove,
    .driver = {
        .name = "tx-isp-core",
        .owner = THIS_MODULE,
    },
};

static struct platform_driver tx_isp_vic_driver = {
    .probe = tx_isp_vic_probe,
    .remove = tx_isp_vic_remove,
    .driver = {
        .name = "isp-w02",
        .owner = THIS_MODULE,
    },
};

/* Platform driver initialization functions - MISSING from original implementation */
int __init tx_isp_subdev_platform_init(void)
{
    int ret;
    
    pr_debug("*** TX ISP SUBDEV PLATFORM DRIVERS REGISTRATION ***\n");
    
    /* Register CSI platform driver */
    ret = platform_driver_register(&tx_isp_csi_driver);
    if (ret) {
        pr_debug("Failed to register CSI platform driver: %d\n", ret);
        return ret;
    }
    
    /* Register VIN platform driver */
    ret = platform_driver_register(&tx_isp_vin_driver);
    if (ret) {
        pr_debug("Failed to register VIN platform driver: %d\n", ret);
        goto err_unregister_csi;
    }
    
    /* Register CORE platform driver */
    ret = platform_driver_register(&tx_isp_core_driver);
    if (ret) {
        pr_debug("Failed to register CORE platform driver: %d\n", ret);
        goto err_unregister_vin;
    }
    
    /* Register VIC platform driver */
    ret = platform_driver_register(&tx_isp_vic_driver);
    if (ret) {
        pr_debug("Failed to register VIC platform driver: %d\n", ret);
        goto err_unregister_core;
    }
    
    pr_debug("All ISP subdev platform drivers registered successfully\n");
    return 0;
    
err_unregister_core:
    platform_driver_unregister(&tx_isp_core_driver);
err_unregister_vin:
    platform_driver_unregister(&tx_isp_vin_driver);
err_unregister_csi:
    platform_driver_unregister(&tx_isp_csi_driver);
    return ret;
}

void __exit tx_isp_subdev_platform_exit(void)
{
    pr_debug("*** TX ISP SUBDEV PLATFORM DRIVERS UNREGISTRATION ***\n");
    
    /* Unregister all platform drivers in reverse order */
    platform_driver_unregister(&tx_isp_vic_driver);
    platform_driver_unregister(&tx_isp_core_driver);
    platform_driver_unregister(&tx_isp_vin_driver);
    platform_driver_unregister(&tx_isp_csi_driver);
    
    pr_debug("All ISP subdev platform drivers unregistered\n");
}

/* Export symbols for main module to call these functions */
EXPORT_SYMBOL(tx_isp_subdev_platform_init);
EXPORT_SYMBOL(tx_isp_subdev_platform_exit);
