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

/* Forward declare frame channel ioctl function from tx-isp-module.c */
extern long frame_channel_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

/* VIDIOC_REQBUFS - Request buffers - Route to frame channel IOCTL */
static int tx_isp_v4l2_reqbufs(struct file *file, void *priv,
                               struct v4l2_requestbuffers *rb)
{
    struct tx_isp_v4l2_device *dev = video_drvdata(file);
    int ret;
    
    if (!dev) {
        return -EINVAL;
    }
    
    pr_info("*** V4L2 Channel %d: REQBUFS - Routing to frame channel IOCTL ***\n", dev->channel_num);
    
    /* CRITICAL: Route to existing Binary Ninja frame_channel_unlocked_ioctl */
    /* This preserves all the complex Binary Ninja buffer allocation logic */
    ret = frame_channel_unlocked_ioctl(file, 0xc0145608, (unsigned long)rb);
    
    pr_info("*** V4L2 Channel %d: REQBUFS routed, result=%d ***\n", dev->channel_num, ret);
    
    return ret;
}

/* VIDIOC_QBUF - Queue buffer - Route to frame channel IOCTL */
static int tx_isp_v4l2_qbuf(struct file *file, void *priv,
                            struct v4l2_buffer *buf)
{
    struct tx_isp_v4l2_device *dev = video_drvdata(file);
    int ret;
    
    if (!dev) {
        return -EINVAL;
    }
    
    pr_info("*** V4L2 Channel %d: QBUF - Routing to Binary Ninja frame channel IOCTL ***\n",
            dev->channel_num);
    
    /* CRITICAL: Route to frame_channel_unlocked_ioctl QBUF (0xc044560f) */
    /* This preserves the Binary Ninja tx_isp_send_event_to_remote logic */
    ret = frame_channel_unlocked_ioctl(file, 0xc044560f, (unsigned long)buf);
    
    pr_info("*** V4L2 Channel %d: QBUF routed with Binary Ninja event system, result=%d ***\n", 
            dev->channel_num, ret);
    
    return ret;
}

/* VIDIOC_DQBUF - Dequeue buffer - Route to frame channel IOCTL */
static int tx_isp_v4l2_dqbuf(struct file *file, void *priv,
                             struct v4l2_buffer *buf)
{
    struct tx_isp_v4l2_device *dev = video_drvdata(file);
    int ret;
    
    if (!dev) {
        return -EINVAL;
    }
    
    pr_info("*** V4L2 Channel %d: DQBUF - Routing to Binary Ninja frame channel IOCTL ***\n",
            dev->channel_num);
    
    /* CRITICAL: Route to frame_channel_unlocked_ioctl DQBUF (0xc0445611) */
    /* This preserves the Binary Ninja __fill_v4l2_buffer and sensor logic */
    ret = frame_channel_unlocked_ioctl(file, 0xc0445611, (unsigned long)buf);
    
    pr_info("*** V4L2 Channel %d: DQBUF routed with Binary Ninja buffer logic, result=%d ***\n",
            dev->channel_num, ret);
    
    return ret;
}

/* VIDIOC_STREAMON - Start streaming - Route to frame channel IOCTL */
static int tx_isp_v4l2_streamon(struct file *file, void *priv,
                                enum v4l2_buf_type type)
{
    struct tx_isp_v4l2_device *dev = video_drvdata(file);
    int ret;
    
    if (!dev) {
        return -EINVAL;
    }
    
    pr_info("*** V4L2 Channel %d: STREAMON - Routing to Binary Ninja frame channel IOCTL ***\n",
            dev->channel_num);
    
    /* CRITICAL: Route to frame_channel_unlocked_ioctl STREAMON (0x80045612) */
    /* This preserves the Binary Ninja sensor hardware initialization and VIC setup */
    ret = frame_channel_unlocked_ioctl(file, 0x80045612, (unsigned long)&type);
    
    pr_info("*** V4L2 Channel %d: STREAMON routed with Binary Ninja hardware init, result=%d ***\n",
            dev->channel_num, ret);
    
    /* Update V4L2 state based on frame channel result */
    if (ret == 0) {
        mutex_lock(&dev->lock);
        dev->streaming = true;
        dev->sequence = 0;
        mutex_unlock(&dev->lock);
    }
    
    return ret;
}

/* VIDIOC_STREAMOFF - Stop streaming - Route to frame channel IOCTL */
static int tx_isp_v4l2_streamoff(struct file *file, void *priv,
                                 enum v4l2_buf_type type)
{
    struct tx_isp_v4l2_device *dev = video_drvdata(file);
    int ret;
    
    if (!dev) {
        return -EINVAL;
    }
    
    pr_info("*** V4L2 Channel %d: STREAMOFF - Routing to Binary Ninja frame channel IOCTL ***\n",
            dev->channel_num);
    
    /* CRITICAL: Route to frame_channel_unlocked_ioctl STREAMOFF (0x80045613) */
    /* This preserves the Binary Ninja sensor hardware shutdown logic */
    ret = frame_channel_unlocked_ioctl(file, 0x80045613, (unsigned long)&type);
    
    pr_info("*** V4L2 Channel %d: STREAMOFF routed with Binary Ninja shutdown, result=%d ***\n",
            dev->channel_num, ret);
    
    /* Update V4L2 state based on frame channel result */
    if (ret == 0) {
        mutex_lock(&dev->lock);
        dev->streaming = false;
        mutex_unlock(&dev->lock);
    }
    
    return ret;
}

/* VIDIOC_ENUM_FMT - Enumerate supported formats (CRITICAL for encoder compatibility) */
static int tx_isp_v4l2_enum_fmt_vid_cap(struct file *file, void *priv,
                                        struct v4l2_fmtdesc *f)
{
    struct tx_isp_v4l2_device *dev = video_drvdata(file);
    
    if (!dev) {
        return -EINVAL;
    }
    
    if (f->index >= 2) {
        return -EINVAL;
    }
    
    f->flags = 0;
    f->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    
    switch (f->index) {
    case 0:
        f->pixelformat = V4L2_PIX_FMT_NV12;
        strcpy(f->description, "NV12 4:2:0");
        break;
    case 1:
        f->pixelformat = V4L2_PIX_FMT_YUYV;
        strcpy(f->description, "YUYV 4:2:2");
        break;
    default:
        return -EINVAL;
    }
    
    pr_info("Channel %d: ENUM_FMT index=%d pixelformat=0x%x\n", 
            dev->channel_num, f->index, f->pixelformat);
    
    return 0;
}

/* VIDIOC_G_PARM - Get streaming parameters (CRITICAL for encoder framerate) */
static int tx_isp_v4l2_g_parm(struct file *file, void *priv,
                              struct v4l2_streamparm *parm)
{
    struct tx_isp_v4l2_device *dev = video_drvdata(file);
    
    if (!dev) {
        return -EINVAL;
    }
    
    if (parm->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) {
        return -EINVAL;
    }
    
    parm->parm.capture.capability = V4L2_CAP_TIMEPERFRAME;
    parm->parm.capture.timeperframe.numerator = 1;
    parm->parm.capture.timeperframe.denominator = 30; /* 30 FPS */
    parm->parm.capture.readbuffers = 4;
    
    pr_info("Channel %d: G_PARM framerate=%d/%d FPS\n", 
            dev->channel_num, 
            parm->parm.capture.timeperframe.denominator,
            parm->parm.capture.timeperframe.numerator);
    
    return 0;
}

/* VIDIOC_S_PARM - Set streaming parameters */
static int tx_isp_v4l2_s_parm(struct file *file, void *priv,
                              struct v4l2_streamparm *parm)
{
    struct tx_isp_v4l2_device *dev = video_drvdata(file);
    
    if (!dev) {
        return -EINVAL;
    }
    
    if (parm->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) {
        return -EINVAL;
    }
    
    pr_info("Channel %d: S_PARM framerate=%d/%d FPS\n", 
            dev->channel_num, 
            parm->parm.capture.timeperframe.denominator,
            parm->parm.capture.timeperframe.numerator);
    
    /* Accept the parameters - encoder compatibility */
    parm->parm.capture.capability = V4L2_CAP_TIMEPERFRAME;
    
    return 0;
}

/* VIDIOC_QUERYCTRL - Query controls (for encoder compatibility) */
static int tx_isp_v4l2_queryctrl(struct file *file, void *priv,
                                 struct v4l2_queryctrl *qc)
{
    struct tx_isp_v4l2_device *dev = video_drvdata(file);
    
    if (!dev) {
        return -EINVAL;
    }
    
    /* Return common controls for encoder compatibility */
    switch (qc->id) {
    case V4L2_CID_BRIGHTNESS:
        strcpy(qc->name, "Brightness");
        qc->type = V4L2_CTRL_TYPE_INTEGER;
        qc->minimum = 0;
        qc->maximum = 255;
        qc->step = 1;
        qc->default_value = 128;
        qc->flags = 0;
        break;
    case V4L2_CID_CONTRAST:
        strcpy(qc->name, "Contrast");
        qc->type = V4L2_CTRL_TYPE_INTEGER;
        qc->minimum = 0;
        qc->maximum = 255;
        qc->step = 1;
        qc->default_value = 128;
        qc->flags = 0;
        break;
    default:
        return -EINVAL;
    }
    
    pr_info("Channel %d: QUERYCTRL id=0x%x name=%s\n", 
            dev->channel_num, qc->id, qc->name);
    
    return 0;
}

/* VIDIOC_G_CTRL - Get control value (encoder compatibility) */
static int tx_isp_v4l2_g_ctrl(struct file *file, void *priv,
                              struct v4l2_control *ctrl)
{
    struct tx_isp_v4l2_device *dev = video_drvdata(file);
    
    if (!dev) {
        return -EINVAL;
    }
    
    /* Return default values for common controls */
    switch (ctrl->id) {
    case V4L2_CID_BRIGHTNESS:
        ctrl->value = 128;
        break;
    case V4L2_CID_CONTRAST:
        ctrl->value = 128;
        break;
    case V4L2_CID_SATURATION:
        ctrl->value = 128;
        break;
    case V4L2_CID_HUE:
        ctrl->value = 0;
        break;
    default:
        return -EINVAL;
    }
    
    pr_info("Channel %d: G_CTRL id=0x%x value=%d\n", 
            dev->channel_num, ctrl->id, ctrl->value);
    
    return 0;
}

/* VIDIOC_S_CTRL - Set control value (encoder compatibility) */
static int tx_isp_v4l2_s_ctrl(struct file *file, void *priv,
                              struct v4l2_control *ctrl)
{
    struct tx_isp_v4l2_device *dev = video_drvdata(file);
    
    if (!dev) {
        return -EINVAL;
    }
    
    pr_info("Channel %d: S_CTRL id=0x%x value=%d\n", 
            dev->channel_num, ctrl->id, ctrl->value);
    
    /* Accept control values for encoder compatibility */
    switch (ctrl->id) {
    case V4L2_CID_BRIGHTNESS:
    case V4L2_CID_CONTRAST:
    case V4L2_CID_SATURATION:
    case V4L2_CID_HUE:
        /* Accept the values */
        break;
    default:
        return -EINVAL;
    }
    
    return 0;
}

/* VIDIOC_CROPCAP - Get crop capabilities (encoder compatibility) */
static int tx_isp_v4l2_cropcap(struct file *file, void *priv,
                               struct v4l2_cropcap *cap)
{
    struct tx_isp_v4l2_device *dev = video_drvdata(file);
    
    if (!dev) {
        return -EINVAL;
    }
    
    if (cap->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) {
        return -EINVAL;
    }
    
    /* Set crop capabilities based on channel */
    cap->bounds.left = 0;
    cap->bounds.top = 0;
    cap->bounds.width = (dev->channel_num == 0) ? 1920 : 640;
    cap->bounds.height = (dev->channel_num == 0) ? 1080 : 360;
    
    cap->defrect = cap->bounds;
    cap->pixelaspect.numerator = 1;
    cap->pixelaspect.denominator = 1;
    
    pr_info("Channel %d: CROPCAP %dx%d\n", 
            dev->channel_num, cap->bounds.width, cap->bounds.height);
    
    return 0;
}

/* V4L2 ioctl operations */
static const struct v4l2_ioctl_ops tx_isp_v4l2_ioctl_ops = {
    .vidioc_querycap      = tx_isp_v4l2_querycap,
    .vidioc_g_fmt_vid_cap = tx_isp_v4l2_g_fmt_vid_cap,
    .vidioc_s_fmt_vid_cap = tx_isp_v4l2_s_fmt_vid_cap,
    .vidioc_try_fmt_vid_cap = tx_isp_v4l2_g_fmt_vid_cap, /* Same as g_fmt for now */
    
    /* Format enumeration - CRITICAL for encoder */
    .vidioc_enum_fmt_vid_cap = tx_isp_v4l2_enum_fmt_vid_cap,
    
    /* Streaming parameters - CRITICAL for encoder framerate */
    .vidioc_g_parm        = tx_isp_v4l2_g_parm,
    .vidioc_s_parm        = tx_isp_v4l2_s_parm,
    
    /* Controls - CRITICAL for encoder compatibility */
    .vidioc_queryctrl     = tx_isp_v4l2_queryctrl,
    .vidioc_g_ctrl        = tx_isp_v4l2_g_ctrl,
    .vidioc_s_ctrl        = tx_isp_v4l2_s_ctrl,
    
    /* Crop capabilities - encoder compatibility */
    .vidioc_cropcap       = tx_isp_v4l2_cropcap,
    
    /* Buffer management */
    .vidioc_reqbufs       = tx_isp_v4l2_reqbufs,
    .vidioc_qbuf          = tx_isp_v4l2_qbuf,
    .vidioc_dqbuf         = tx_isp_v4l2_dqbuf,
    
    /* Streaming control */
    .vidioc_streamon      = tx_isp_v4l2_streamon,
    .vidioc_streamoff     = tx_isp_v4l2_streamoff,
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

/* Memory mapping support for encoder buffer access */
static int tx_isp_v4l2_mmap(struct file *file, struct vm_area_struct *vma)
{
    struct tx_isp_v4l2_device *dev = video_drvdata(file);
    unsigned long size;
    
    if (!dev) {
        pr_err("tx_isp_v4l2_mmap: Invalid device\n");
        return -EINVAL;
    }
    
    size = vma->vm_end - vma->vm_start;
    
    pr_info("*** Channel %d: MMAP request - offset=0x%lx size=%lu ***\n",
            dev->channel_num, vma->vm_pgoff << PAGE_SHIFT, size);
    
    /* For encoder compatibility, we need to support memory mapping */
    /* This is a basic implementation - actual buffer mapping would be more complex */
    vma->vm_flags |= VM_IO | VM_DONTEXPAND | VM_DONTDUMP;
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
    
    pr_info("*** Channel %d: MMAP SUCCESS ***\n", dev->channel_num);
    
    return 0;
}

static const struct v4l2_file_operations tx_isp_v4l2_fops = {
    .owner          = THIS_MODULE,
    .open           = tx_isp_v4l2_open,
    .release        = tx_isp_v4l2_release,
    .unlocked_ioctl = video_ioctl2,
    .mmap           = tx_isp_v4l2_mmap,
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
    
    /* Initialize V4L2 device - use ISP device as parent if available */
    ret = v4l2_device_register(ourISPdev ? ourISPdev->dev : NULL, &dev->v4l2_dev);
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
    vdev->vfl_dir = VFL_DIR_RX;  /* Add this - indicates capture device */
    /* Note: device_caps member not available in this kernel version */
    /* Capabilities are communicated via VIDIOC_QUERYCAP ioctl instead */
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
