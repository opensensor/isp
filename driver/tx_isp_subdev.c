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


// Binary Ninja EXACT implementation 
int tx_isp_send_event_to_remote(struct v4l2_subdev *sd, unsigned int event, void *data)
{
    pr_info("*** tx_isp_send_event_to_remote: subdev=%p, event=0x%x ***\n", sd, event);
    
    if (sd != 0) {                              // arg1 != 0
        void *callback = *(void **)(sd + 0xc); // void* $a0 = *(arg1 + 0xc)
        pr_info("*** EVENT: subdev=%p, checking callback at offset +0xc ***\n", sd);
        pr_info("*** EVENT: callback_struct=%p (from subdev+0xc) ***\n", callback);
        
        if (callback != 0) {                    // $a0 != 0
            int (*handler)(void *, unsigned int, void *) = *(void **)(callback + 0x1c); // Get handler from callback+0x1c
            pr_info("*** EVENT: callback found, checking handler at offset +0x1c ***\n");
            pr_info("*** EVENT: event_callback=%p (from callback+0x1c) ***\n", handler);
            
            if (handler != 0) {                 // $t9_1 != 0
                pr_info("*** EVENT: Calling event callback %p for event 0x%x ***\n", handler, event);
                int ret = handler(callback, event, data); // Call handler (jump($t9_1))
                pr_info("*** EVENT: Callback returned %d (0x%x) ***\n", ret, ret);
                return ret;
            }
        }
    }
    
    pr_err("*** EVENT: No valid callback chain - subdev=%p ***\n", sd);
    return 0xfffffdfd; // -515 (binary: 0xfffffdfd)
}

/* Frame channel file operations */
static const struct file_operations fs_channel_ops = {
    .owner = THIS_MODULE,
    .open = frame_channel_open,
    .release = frame_channel_release,
    .unlocked_ioctl = frame_channel_unlocked_ioctl,
};

/* Frame channel initialization/deinitialization */
void tx_isp_frame_chan_deinit(struct tx_isp_frame_channel *chan)
{
    pr_info("Deinitializing frame channel\n");
    if (chan->active) {
        misc_deregister(&chan->misc);
    }
}



int tx_isp_setup_default_links(struct tx_isp_dev *dev) {
    int ret;

    pr_info("Setting up default links\n");

    // Link sensor output -> CSI input
    if (dev->sensor_sd && dev->csi_dev && dev->csi_dev->sd) {
        if (!dev->sensor_sd->ops || !dev->sensor_sd->ops->video ||
            !dev->sensor_sd->ops->video->link_setup) {
            pr_err("Sensor subdev missing required ops\n");
            return -EINVAL;
        }

        pr_info("Setting up sensor -> CSI link\n");
        ret = dev->sensor_sd->ops->video->link_setup(
            &dev->sensor_sd->outpads[0],
            &dev->csi_dev->sd->inpads[0],
            TX_ISP_LINKFLAG_ENABLED
        );
        if (ret && ret != -ENOIOCTLCMD) {
            pr_err("Failed to setup sensor->CSI link: %d\n", ret);
            return ret;
        }
    }

    // Link CSI output -> VIC input
    if (dev->csi_dev && dev->csi_dev->sd && dev->vic_dev && dev->vic_dev->sd) {
        if (!dev->csi_dev->sd->ops || !dev->csi_dev->sd->ops->video ||
            !dev->csi_dev->sd->ops->video->link_setup) {
            pr_err("CSI subdev missing required ops\n");
            return -EINVAL;
        }

        pr_info("Setting up CSI -> VIC link\n");
        ret = dev->csi_dev->sd->ops->video->link_setup(
            &dev->csi_dev->sd->outpads[0],
            &dev->vic_dev->sd->inpads[0],
            TX_ISP_LINKFLAG_ENABLED
        );
        if (ret && ret != -ENOIOCTLCMD) {
            pr_err("Failed to setup CSI->VIC link: %d\n", ret);
            return ret;
        }
    }

    // Link VIC outputs to DDR device if present
    if (dev->vic_dev && dev->vic_dev->sd && dev->ddr_dev && dev->ddr_dev->sd) {
        if (!dev->vic_dev->sd->ops || !dev->vic_dev->sd->ops->video ||
            !dev->vic_dev->sd->ops->video->link_setup) {
            pr_err("VIC subdev missing required ops\n");
            return -EINVAL;
        }

        pr_info("Setting up VIC -> DDR link\n");
        ret = dev->vic_dev->sd->ops->video->link_setup(
            &dev->vic_dev->sd->outpads[0],
            &dev->ddr_dev->sd->inpads[0],
            TX_ISP_LINKFLAG_ENABLED
        );
        if (ret && ret != -ENOIOCTLCMD) {
            pr_err("Failed to setup VIC->DDR link 0: %d\n", ret);
            return ret;
        }

        // Link second VIC output if present
        if (dev->vic_dev->sd->num_outpads > 1) {
            pr_info("Setting up second VIC -> DDR link\n");
            ret = dev->vic_dev->sd->ops->video->link_setup(
                &dev->vic_dev->sd->outpads[1],
                &dev->ddr_dev->sd->inpads[0],
                TX_ISP_LINKFLAG_ENABLED
            );
            if (ret && ret != -ENOIOCTLCMD) {
                pr_err("Failed to setup VIC->DDR link 1: %d\n", ret);
                return ret;
            }
        }
    }

    pr_info("All default links set up successfully\n");
    return 0;
}


extern struct tx_isp_dev *ourISPdev;


/* Subdevice initialization */
/* First, let's define the missing enums/constants */
#define TX_ISP_PADTYPE_INPUT   0x1
#define TX_ISP_PADTYPE_OUTPUT  0x2
#define TX_ISP_PADSTATE_FREE   0x0
#define TX_ISP_LINKFLAG_DYNAMIC 0x1
#define TX_ISP_PADLINK_VIC     0x1
#define TX_ISP_PADLINK_DDR     0x2
#define TX_ISP_PADLINK_ISP     0x4

int tx_isp_subdev_init(struct platform_device *pdev, struct tx_isp_subdev *sd,
                      struct tx_isp_subdev_ops *ops)
{
    struct tx_isp_dev *dev = ourISPdev;
    int ret;

    pr_info("Starting subdev init");

    if (!sd || !ops || !pdev) {
        pr_err("Invalid parameters\n");
        return -EINVAL;
    }

    if (!dev) {
        pr_err("No ISP device data found\n");
        return -EINVAL;
    }

    /* Initialize subdev structure */
    pr_info("Initializing subdev structure\n");
    memset(sd, 0, sizeof(*sd));
    sd->ops = ops;

    /* Set pad counts based on device type */
    if (!strcmp(pdev->name, "tx-isp-vic")) {
        sd->num_inpads = 1;
        sd->num_outpads = 2;
    } else if (!strcmp(pdev->name, "tx-isp-csi")) {
        sd->num_inpads = 1;
        sd->num_outpads = 1;
    } else {
        sd->num_inpads = 1;
        sd->num_outpads = 1;
    }

    // Allocate input pads
    pr_info("Allocating input pads\n");
    if (sd->num_inpads) {
        sd->inpads = kzalloc(sizeof(struct tx_isp_subdev_pad) * sd->num_inpads, GFP_KERNEL);
        if (!sd->inpads) {
            pr_err("Failed to allocate input pads\n");
            return -ENOMEM;
        }

        for (int i = 0; i < sd->num_inpads; i++) {
            struct isp_channel *chn;

            // Initialize pad
            sd->inpads[i].sd = sd;
            sd->inpads[i].index = i;
            sd->inpads[i].type = TX_ISP_PADTYPE_INPUT;
            sd->inpads[i].state = TX_ISP_PADSTATE_FREE;
            sd->inpads[i].link.flag = TX_ISP_LINKFLAG_DYNAMIC;

            // Initialize channel
            chn = &dev->channels[i];
            chn->id = i;
            chn->enabled = false;
            chn->width = 0;
            chn->height = 0;

            // Store channel reference in pad
            sd->inpads[i].priv = chn;

            if (!strcmp(pdev->name, "tx-isp-vic"))
                sd->inpads[i].links_type = TX_ISP_PADLINK_VIC;
            else if (!strcmp(pdev->name, "tx-isp-csi"))
                sd->inpads[i].links_type = TX_ISP_PADLINK_VIC;
            else
                sd->inpads[i].links_type = TX_ISP_PADLINK_VIC | TX_ISP_PADLINK_ISP;
        }
    }

    /* Allocate output pads */
    pr_info("Allocating output pads\n");
    if (sd->num_outpads) {
        sd->outpads = kzalloc(sizeof(struct tx_isp_subdev_pad) * sd->num_outpads, GFP_KERNEL);
        if (!sd->outpads) {
            pr_err("Failed to allocate output pads\n");
            ret = -ENOMEM;
            goto err_free_in_channels;
        }

        /* Initialize each output pad */
        for (int i = 0; i < sd->num_outpads; i++) {
            struct isp_channel *chn;

            sd->outpads[i].sd = sd;
            sd->outpads[i].index = i;
            sd->outpads[i].type = TX_ISP_PADTYPE_OUTPUT;
            sd->outpads[i].state = TX_ISP_PADSTATE_FREE;
            sd->outpads[i].link.flag = TX_ISP_LINKFLAG_DYNAMIC;

            // Initialize channel
            chn = &dev->channels[i + sd->num_inpads];
            chn->id = i + sd->num_inpads;
            chn->enabled = false;
            chn->width = 0;
            chn->height = 0;

            // Store channel reference in pad
            sd->outpads[i].priv = chn;

            if (!strcmp(pdev->name, "tx-isp-vic"))
                sd->outpads[i].links_type = TX_ISP_PADLINK_DDR | TX_ISP_PADLINK_VIC;
            else if (!strcmp(pdev->name, "tx-isp-csi"))
                sd->outpads[i].links_type = TX_ISP_PADLINK_VIC;
            else
                sd->outpads[i].links_type = TX_ISP_PADLINK_VIC | TX_ISP_PADLINK_ISP;
        }
    }

    /* Map to appropriate register space */
    pr_info("Mapping to register space\n");
    if (dev->mem_region) {
        if (!strcmp(pdev->name, "tx-isp-vic")) {
            sd->base = dev->vic_regs;
        } else if (!strcmp(pdev->name, "tx-isp-csi")) {
            sd->base = ioremap(dev->csi_region->start,
                             resource_size(dev->csi_region));

            if (sd->base) {
                // Basic CSI initialization
                writel(1, sd->base + 0x40);
                wmb();
                udelay(100);

                writel(0x1, sd->base + 0x04);
                wmb();
                udelay(100);

                pr_info("CSI PHY initialized in probe\n");
            }
        } else {
            sd->base = ioremap(dev->mem_region->start,
                             resource_size(dev->mem_region));
        }
        pr_info("Using register base: %p\n", sd->base);
    }

    /* Store the subdev in device structure */
    if (!strcmp(pdev->name, "tx-isp-csi")) {
        if (!dev->csi_dev) {
            pr_err("CSI device not initialized\n");
            ret = -EINVAL;
            goto err_free_out_channels;
        }
        dev->csi_dev->sd = sd;
    } else if (!strcmp(pdev->name, "tx-isp-vin")) {
        dev->sensor_sd = sd;  // Store in sensor_sd for now
    } else if (!strcmp(pdev->name, "tx-isp-fs")) {
        dev->sensor_sd = sd;  // Store in sensor_sd for now
    } else {
        pr_info("Storing sensor subdev\n");
        dev->sensor_sd = sd;
    }

    platform_set_drvdata(pdev, dev);

    pr_info("Subdev %s initialized successfully\n", pdev->name);
    return 0;

err_free_out_channels:
    if (sd->outpads) {
        kfree(sd->outpads);
    }

err_free_in_channels:
    if (sd->inpads) {
        kfree(sd->inpads);
    }
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

/* Frame source probe function from decompiled code */
int tx_isp_fs_probe(struct platform_device *pdev)
{
    struct tx_isp_subdev *sd = NULL;
    struct tx_isp_frame_channel *frame_chans = NULL;
    int32_t ret = 0;
    int i;

    pr_info("FS probe called\n");

    sd = kmalloc(sizeof(struct tx_isp_subdev), GFP_KERNEL);
    if (sd == NULL) {
        ISP_ERROR("Failed to allocate fs device\n");
        return -ENOMEM;
    }

    memset(sd, 0, sizeof(struct tx_isp_subdev));

    ret = tx_isp_subdev_init(pdev, sd, &fs_subdev_ops);
    if (ret < 0) {
        ISP_ERROR("tx_isp_subdev_init failed!\n");
        kfree(sd);
        return ret;
    }

    if (sd->num_outpads > 0) {
        frame_chans = kmalloc(sd->num_outpads * sizeof(struct tx_isp_frame_channel),
                                    GFP_KERNEL);
        if (!frame_chans) {
            ret = -ENOMEM;
            goto err_free_sd;
        }
        memset(frame_chans, 0, sd->num_outpads * sizeof(struct tx_isp_frame_channel));

        for (i = 0; i < sd->num_outpads; i++) {
            struct tx_isp_frame_channel *chan = &frame_chans[i];
            struct tx_isp_subdev_pad *pad = &sd->outpads[i];

            chan->pad = pad;
            chan->pad_id = i;

            if (pad->type) {
                snprintf(chan->name, sizeof(chan->name), "framechan%d", i);
                chan->misc.minor = 0xFF;
                chan->misc.fops = &fs_channel_ops;
                chan->misc.name = chan->name;

                if (misc_register(&chan->misc) < 0) {
                    ISP_ERROR("Failed to register framechan%d!\n", i);
                    ret = -ENODEV;
                    goto err_free_chans;
                }

                chan->state = FRAME_CHAN_INIT;
                spin_lock_init(&chan->slock);
                private_raw_mutex_init(&chan->mlock, "chan->mlock", NULL);
                init_completion(&chan->frame_done);
                INIT_LIST_HEAD(&chan->queue_head);
                INIT_LIST_HEAD(&chan->done_head);
                chan->queued_count = 0;
                chan->done_count = 0;
                init_waitqueue_head(&chan->wait);
                chan->active = 1;
            } else {
                chan->active = 0;
            }
        }

        sd->frame_chans = frame_chans;
        sd->num_channels = sd->num_outpads;
    }

    platform_set_drvdata(pdev, sd);
    sd->vin_state = 1;

    return 0;

err_free_chans:
    for (i--; i >= 0; i--) {
        struct tx_isp_frame_channel *chan = &frame_chans[i];
        if (chan->active) {
            misc_deregister(&chan->misc);
        }
    }
    kfree(frame_chans);
err_free_sd:
    tx_isp_subdev_deinit(sd);
    kfree(sd);
    return ret;
}
EXPORT_SYMBOL(tx_isp_fs_probe);

/* Frame source remove function from decompiled code */
int tx_isp_fs_remove(struct platform_device *pdev)
{
    struct tx_isp_subdev *sd = platform_get_drvdata(pdev);

    if (!sd)
        return -EINVAL;

    kfree(sd->fs_wdr_shadow);
    tx_isp_subdev_deinit(sd);
    kfree(sd);

    return 0;
}
EXPORT_SYMBOL(tx_isp_fs_remove);

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

static struct platform_driver tx_isp_fs_driver = {
    .probe = tx_isp_fs_probe,
    .remove = tx_isp_fs_remove,
    .driver = {
        .name = "tx-isp-fs",
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
        .name = "tx-isp-vic",
        .owner = THIS_MODULE,
    },
};
