/*
 * TX-ISP V4L2 Video Device Implementation
 * Handles V4L2 video devices for frame channels (/dev/video0, /dev/video1, etc.)
 *
 * Copyright 2024, <matteius@gmail.com>
 *
 * This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/module.h>
#include <linux/videodev2.h>
#include <media/v4l2-device.h>
#include <media/v4l2-dev.h>
#include <media/v4l2-fh.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-event.h>
#include <media/v4l2-ioctl.h>
#include <media/videobuf2-dma-contig.h>
#include "../include/tx_isp.h"
#include "../include/tx-isp-device.h"

/* V4L2 frame channel device structure */
struct tx_isp_v4l2_device {
    struct v4l2_device v4l2_dev;
    struct video_device *vdev;
    struct v4l2_format format;
    struct mutex lock;
    int channel_num;
    bool streaming;
    
    /* Buffer management */
    struct vb2_queue queue;
    struct list_head buf_list;
    spinlock_t buf_lock;
    
    /* Frame state */
    uint32_t sequence;
    wait_queue_head_t frame_wait;
    bool frame_ready;
};

/* Global V4L2 devices */
static struct tx_isp_v4l2_device *v4l2_devices[4] = {NULL};
static int num_v4l2_devices = 2; /* Default 2 channels like reference */

/* External ISP device reference */
extern struct tx_isp_dev *ourISPdev;

/* V4L2 buffer structure */
struct tx_isp_v4l2_buffer {
    struct vb2_buffer vb2_buf;
    struct list_head list;
    dma_addr_t dma_addr;
    void *vaddr;
    bool queued;
};

/* VIDIOC_S_FMT - Set format (CRITICAL for IMP_FrameSource_EnableChn) */
static int tx_isp_v4l2_s_fmt_vid_cap(struct file *file, void *priv,
                                     struct v4l2_format *f)
{
    struct tx_isp_v4l2_device *dev = video_drvdata(file);
    
    if (!dev) {
        pr_err("tx_isp_v4l2_s_fmt_vid_cap: Invalid device\n");
        return -EINVAL;
    }
    
    pr_info("*** VIDIOC_S_FMT Channel %d: %dx%d pixfmt=0x%x ***\n",
            dev->channel_num, 
            f->fmt.pix.width, f->fmt.pix.height, f->fmt.pix.pixelformat);
    
    /* Validate format */
    if (f->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) {
        pr_err("Channel %d: Invalid buffer type %d\n", dev->channel_num, f->type);
        return -EINVAL;
    }
    
    /* Validate dimensions */
    if (f->fmt.pix.width == 0 || f->fmt.pix.height == 0) {
        pr_err("Channel %d: Invalid dimensions %dx%d\n", 
               dev->channel_num, f->fmt.pix.width, f->fmt.pix.height);
        return -EINVAL;
    }
    
    /* Validate pixel format */
    if (f->fmt.pix.pixelformat != V4L2_PIX_FMT_NV12 && 
        f->fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV &&
        f->fmt.pix.pixelformat != 0x3231564e) { /* NV12 alternative format */
        pr_warn("Channel %d: Unsupported pixel format 0x%x, using NV12\n",
                dev->channel_num, f->fmt.pix.pixelformat);
        f->fmt.pix.pixelformat = V4L2_PIX_FMT_NV12;
    }
    
    /* Set format parameters based on channel */
    if (dev->channel_num == 0) {
        /* Main channel - full resolution */
        f->fmt.pix.width = min_t(u32, f->fmt.pix.width, 1920);
        f->fmt.pix.height = min_t(u32, f->fmt.pix.height, 1080);
    } else {
        /* Sub channel - smaller resolution */
        f->fmt.pix.width = min_t(u32, f->fmt.pix.width, 640);
        f->fmt.pix.height = min_t(u32, f->fmt.pix.height, 360);
    }
    
    /* Calculate stride and image size */
    f->fmt.pix.bytesperline = f->fmt.pix.width;
    if (f->fmt.pix.pixelformat == V4L2_PIX_FMT_YUYV) {
        f->fmt.pix.bytesperline *= 2;
        f->fmt.pix.sizeimage = f->fmt.pix.bytesperline * f->fmt.pix.height;
    } else {
        /* NV12 format */
        f->fmt.pix.sizeimage = f->fmt.pix.width * f->fmt.pix.height * 3 / 2;
    }
    
    f->fmt.pix.field = V4L2_FIELD_NONE;
    f->fmt.pix.colorspace = V4L2_COLORSPACE_REC709;
    
    /* Store the format */
    mutex_lock(&dev->lock);
    dev->format = *f;
    mutex_unlock(&dev->lock);
    
    pr_info("*** Channel %d: S_FMT SUCCESS - Format configured %dx%d ***\n",
            dev->channel_num, f->fmt.pix.width, f->fmt.pix.height);
    
    return 0;
}

/* VIDIOC_G_FMT - Get format */
static int tx_isp_v4l2_g_fmt_vid_cap(struct file *file, void *priv,
                                     struct v4l2_format *f)
{
    struct tx_isp_v4l2_device *dev = video_drvdata(file);
    
    if (!dev) {
        return -EINVAL;
    }
    
    pr_info("Channel %d: VIDIOC_G_FMT\n", dev->channel_num);
    
    mutex_lock(&dev->lock);
    *f = dev->format;
    mutex_unlock(&dev->lock);
    
    return 0;
}

/* VIDIOC_QUERYCAP - Query capabilities */
static int tx_isp_v4l2_querycap(struct file *file, void *priv,
                                struct v4l2_capability *cap)
{
    struct tx_isp_v4l2_device *dev = video_drvdata(file);
    
    if (!dev) {
        return -EINVAL;
    }
    
    strlcpy(cap->driver, "tx-isp", sizeof(cap->driver));
    snprintf(cap->card, sizeof(cap->card), "TX-ISP Channel %d", dev->channel_num);
    snprintf(cap->bus_info, sizeof(cap->bus_info), "platform:tx-isp");
    cap->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;
    cap->capabilities = cap->device_caps | V4L2_CAP_DEVICE_CAPS;
    
    return 0;
}

/* V4L2 ioctl operations */
static const struct v4l2_ioctl_ops tx_isp_v4l2_ioctl_ops = {
    .vidioc_querycap      = tx_isp_v4l2_querycap,
    .vidioc_g_fmt_vid_cap = tx_isp_v4l2_g_fmt_vid_cap,
    .vidioc_s_fmt_vid_cap = tx_isp_v4l2_s_fmt_vid_cap,
    .vidioc_try_fmt_vid_cap = tx_isp_v4l2_g_fmt_vid_cap, /* Same as g_fmt for now */
};

/* V4L2 file operations */
static int tx_isp_v4l2_open(struct file *file)
{
    struct tx_isp_v4l2_device *dev = video_drvdata(file);
    
    if (!dev) {
        pr_err("tx_isp_v4l2_open: Invalid device\n");
        return -ENODEV;
    }
    
    pr_info("*** V4L2 Channel %d opened ***\n", dev->channel_num);
    
    return v4l2_fh_open(file);
}

static int tx_isp_v4l2_release(struct file *file)
{
    struct tx_isp_v4l2_device *dev = video_drvdata(file);
    
    if (dev) {
        pr_info("*** V4L2 Channel %d closed ***\n", dev->channel_num);
        
        /* Stop streaming if active */
        if (dev->streaming) {
            dev->streaming = false;
            wake_up_interruptible(&dev->frame_wait);
        }
    }
    
    return v4l2_fh_release(file);
}

static const struct v4l2_file_operations tx_isp_v4l2_fops = {
    .owner          = THIS_MODULE,
    .open           = tx_isp_v4l2_open,
    .release        = tx_isp_v4l2_release,
    .unlocked_ioctl = video_ioctl2,
};

/* Create V4L2 video device for a channel */
static int tx_isp_create_v4l2_device(int channel)
{
    struct tx_isp_v4l2_device *dev;
    struct video_device *vdev;
    int ret;
    
    pr_info("*** Creating V4L2 video device for channel %d ***\n", channel);
    
    /* Allocate device structure */
    dev = kzalloc(sizeof(struct tx_isp_v4l2_device), GFP_KERNEL);
    if (!dev) {
        pr_err("Failed to allocate V4L2 device for channel %d\n", channel);
        return -ENOMEM;
    }
    
    /* Initialize device structure */
    dev->channel_num = channel;
    dev->streaming = false;
    dev->sequence = 0;
    dev->frame_ready = false;
    mutex_init(&dev->lock);
    spin_lock_init(&dev->buf_lock);
    init_waitqueue_head(&dev->frame_wait);
    INIT_LIST_HEAD(&dev->buf_list);
    
    /* Set default format */
    dev->format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (channel == 0) {
        /* Main channel - HD */
        dev->format.fmt.pix.width = 1920;
        dev->format.fmt.pix.height = 1080;
    } else {
        /* Sub channel */
        dev->format.fmt.pix.width = 640;
        dev->format.fmt.pix.height = 360;
    }
    dev->format.fmt.pix.pixelformat = V4L2_PIX_FMT_NV12;
    dev->format.fmt.pix.field = V4L2_FIELD_NONE;
    dev->format.fmt.pix.bytesperline = dev->format.fmt.pix.width;
    dev->format.fmt.pix.sizeimage = dev->format.fmt.pix.width * 
                                   dev->format.fmt.pix.height * 3 / 2;
    dev->format.fmt.pix.colorspace = V4L2_COLORSPACE_REC709;
    
    /* Initialize V4L2 device */
    ret = v4l2_device_register(NULL, &dev->v4l2_dev);
    if (ret) {
        pr_err("Failed to register V4L2 device for channel %d: %d\n", channel, ret);
        goto err_free_dev;
    }
    
    /* Create video device */
    vdev = video_device_alloc();
    if (!vdev) {
        pr_err("Failed to allocate video device for channel %d\n", channel);
        ret = -ENOMEM;
        goto err_unreg_v4l2;
    }
    
    /* Configure video device */
    vdev->v4l2_dev = &dev->v4l2_dev;
    vdev->fops = &tx_isp_v4l2_fops;
    vdev->ioctl_ops = &tx_isp_v4l2_ioctl_ops;
    vdev->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;
    vdev->lock = &dev->lock;
    snprintf(vdev->name, sizeof(vdev->name), "tx-isp-ch%d", channel);
    video_set_drvdata(vdev, dev);
    
    /* Register video device - this creates /dev/videoX */
    ret = video_register_device(vdev, VFL_TYPE_GRABBER, -1);
    if (ret) {
        pr_err("Failed to register video device for channel %d: %d\n", channel, ret);
        goto err_free_vdev;
    }
    
    dev->vdev = vdev;
    v4l2_devices[channel] = dev;
    
    pr_info("*** V4L2 video device created: /dev/video%d for channel %d ***\n",
            vdev->num, channel);
    
    return 0;
    
err_free_vdev:
    video_device_release(vdev);
err_unreg_v4l2:
    v4l2_device_unregister(&dev->v4l2_dev);
err_free_dev:
    kfree(dev);
    return ret;
}

/* Destroy V4L2 video device */
static void tx_isp_destroy_v4l2_device(int channel)
{
    struct tx_isp_v4l2_device *dev = v4l2_devices[channel];
    
    if (!dev) {
        return;
    }
    
    pr_info("Destroying V4L2 video device for channel %d\n", channel);
    
    /* Unregister and free video device */
    if (dev->vdev) {
        video_unregister_device(dev->vdev);
        video_device_release(dev->vdev);
        dev->vdev = NULL;
    }
    
    /* Unregister V4L2 device */
    v4l2_device_unregister(&dev->v4l2_dev);
    
    /* Free device structure */
    kfree(dev);
    v4l2_devices[channel] = NULL;
    
    pr_info("V4L2 video device for channel %d destroyed\n", channel);
}

/* Initialize all V4L2 video devices */
int tx_isp_v4l2_init(void)
{
    int ret, i;
    
    pr_info("*** Initializing TX-ISP V4L2 video devices ***\n");
    
    for (i = 0; i < num_v4l2_devices; i++) {
        ret = tx_isp_create_v4l2_device(i);
        if (ret) {
            pr_err("Failed to create V4L2 device %d: %d\n", i, ret);
            goto cleanup;
        }
    }
    
    pr_info("*** TX-ISP V4L2 devices initialized successfully ***\n");
    return 0;
    
cleanup:
    /* Clean up already created devices */
    for (i = 0; i < num_v4l2_devices; i++) {
        tx_isp_destroy_v4l2_device(i);
    }
    return ret;
}

/* Cleanup all V4L2 video devices */
void tx_isp_v4l2_cleanup(void)
{
    int i;
    
    pr_info("Cleaning up TX-ISP V4L2 video devices\n");
    
    for (i = 0; i < num_v4l2_devices; i++) {
        tx_isp_destroy_v4l2_device(i);
    }
    
    pr_info("TX-ISP V4L2 cleanup complete\n");
}

/* Export functions */
EXPORT_SYMBOL(tx_isp_v4l2_init);
EXPORT_SYMBOL(tx_isp_v4l2_cleanup);
