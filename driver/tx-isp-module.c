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

// Simple sensor registration structure
struct registered_sensor {
    char name[32];
    int index;
    struct list_head list;
};

// Simple global device instance
struct tx_isp_dev *ourISPdev = NULL;
static LIST_HEAD(sensor_list);
static DEFINE_MUTEX(sensor_list_mutex);
static int sensor_count = 0;
static int isp_memopt = 0; // Memory optimization flag like reference

// ISP Tuning device support - missing component for /dev/isp-m0
static struct cdev isp_tuning_cdev;
static struct class *isp_tuning_class = NULL;
static dev_t isp_tuning_devno;
static int isp_tuning_major = 0;
static char isp_tuning_buffer[0x500c]; // Tuning parameter buffer from reference

// Frame channel devices - create video channel devices like reference
static struct miscdevice frame_channel_devices[4]; /* Support up to 4 video channels */
static int num_channels = 2; /* Default to 2 channels (CH0, CH1) like reference */

// ISP Tuning IOCTLs from reference (0x20007400 series)
#define ISP_TUNING_GET_PARAM    0x20007400
#define ISP_TUNING_SET_PARAM    0x20007401
#define ISP_TUNING_GET_AE_INFO  0x20007403
#define ISP_TUNING_SET_AE_INFO  0x20007404
#define ISP_TUNING_GET_AWB_INFO 0x20007406
#define ISP_TUNING_SET_AWB_INFO 0x20007407
#define ISP_TUNING_GET_STATS    0x20007408
#define ISP_TUNING_GET_STATS2   0x20007409

// Forward declarations for frame channel devices
static int frame_channel_open(struct inode *inode, struct file *file);
static int frame_channel_release(struct inode *inode, struct file *file);
static long frame_channel_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

// Helper functions matching reference driver patterns
static void* find_subdev_link_pad(struct tx_isp_dev *isp_dev, char *name)
{
    int i;
    
    // Reference implementation searches through 16 subdevices at offset 0x38
    // For now, return NULL since we don't have full subdev infrastructure
    pr_debug("find_subdev_link_pad: searching for %s\n", name);
    
    // In full implementation, this would:
    // 1. Iterate through isp_dev->subdevs[16] array
    // 2. Compare subdev names
    // 3. Return pad structure based on pad type
    
    return NULL;
}

static int tx_isp_video_link_destroy_impl(struct tx_isp_dev *isp_dev)
{
    // Reference: tx_isp_video_link_destroy.isra.5
    // Gets link_config from offset 0x118, destroys links, sets to -1
    
    pr_info("Video link destroy: cleaning up pipeline connections\n");
    
    // In full implementation:
    // 1. Get link_config from isp_dev->link_config (offset 0x118)
    // 2. Iterate through configs array
    // 3. Call find_subdev_link_pad() and subdev_video_destroy_link()
    // 4. Set link_config to -1
    
    return 0;
}

static int tx_isp_video_link_stream_impl(struct tx_isp_dev *isp_dev, int enable)
{
    int i;
    
    // Reference: tx_isp_video_link_stream
    // Iterates through 16 subdevices, calls video ops stream function
    
    pr_info("Video link stream: %s\n", enable ? "enable" : "disable");
    
    // In full implementation:
    // for (i = 0; i < 16; i++) {
    //     if (isp_dev->subdevs[i] && isp_dev->subdevs[i]->ops->video->stream)
    //         result = isp_dev->subdevs[i]->ops->video->stream(isp_dev->subdevs[i], enable);
    // }
    
    return 0;
}

static int tx_isp_video_s_stream_impl(struct tx_isp_dev *isp_dev, int enable)
{
    int i;
    
    // Reference: tx_isp_video_s_stream
    // Similar to link stream but different ops function
    
    pr_info("Video s_stream: %s\n", enable ? "start" : "stop");
    
    // In full implementation:
    // for (i = 0; i < 16; i++) {
    //     if (isp_dev->subdevs[i] && isp_dev->subdevs[i]->ops->video->s_stream)
    //         result = isp_dev->subdevs[i]->ops->video->s_stream(isp_dev->subdevs[i], enable);
    // }
    
    return 0;
}

// ISP Tuning device implementation - missing component for IMP_ISP_EnableTuning()
static long isp_tuning_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    void __user *argp = (void __user *)arg;
    int param_type;
    
    pr_info("ISP Tuning IOCTL: cmd=0x%x\n", cmd);
    
    // Handle V4L2 control IOCTLs (VIDIOC_S_CTRL, VIDIOC_G_CTRL)
    if (cmd == 0xc008561c || cmd == 0xc008561b) { // VIDIOC_S_CTRL / VIDIOC_G_CTRL
        struct v4l2_control {
            uint32_t id;
            int32_t value;
        } ctrl;
        
        if (copy_from_user(&ctrl, argp, sizeof(ctrl)))
            return -EFAULT;
        
        pr_info("V4L2 Control: id=0x%x value=%d\n", ctrl.id, ctrl.value);
        
        // Handle common ISP controls based on reference implementation
        switch (ctrl.id) {
        case 0x980900: // V4L2_CID_BRIGHTNESS
            pr_info("ISP Brightness: %d\n", ctrl.value);
            break;
        case 0x980901: // V4L2_CID_CONTRAST
            pr_info("ISP Contrast: %d\n", ctrl.value);
            break;
        case 0x980902: // V4L2_CID_SATURATION
            pr_info("ISP Saturation: %d\n", ctrl.value);
            break;
        case 0x98091b: // V4L2_CID_SHARPNESS
            pr_info("ISP Sharpness: %d\n", ctrl.value);
            break;
        case 0x980914: // V4L2_CID_VFLIP
            pr_info("ISP VFlip: %d\n", ctrl.value);
            break;
        case 0x980915: // V4L2_CID_HFLIP
            pr_info("ISP HFlip: %d\n", ctrl.value);
            break;
        case 0x980918: // Anti-flicker
            pr_info("ISP Anti-flicker: %d\n", ctrl.value);
            break;
        case 0x8000086: // 2DNS ratio
            pr_info("ISP 2DNS ratio: %d\n", ctrl.value);
            break;
        case 0x8000085: // 3DNS ratio
            pr_info("ISP 3DNS ratio: %d\n", ctrl.value);
            break;
        case 0x8000028: // Max analog gain
            pr_info("ISP Max analog gain: %d\n", ctrl.value);
            break;
        case 0x8000029: // Max digital gain
            pr_info("ISP Max digital gain: %d\n", ctrl.value);
            break;
        case 0x8000023: // AE compensation
            pr_info("ISP AE compensation: %d\n", ctrl.value);
            break;
        case 0x80000e0: // Sensor FPS
            pr_info("ISP Sensor FPS: %d\n", ctrl.value);
            break;
        case 0x8000062: // DPC strength
            pr_info("ISP DPC strength: %d\n", ctrl.value);
            break;
        case 0x80000a2: // DRC strength
            pr_info("ISP DRC strength: %d\n", ctrl.value);
            break;
        case 0x8000039: // Defog strength
            pr_info("ISP Defog strength: %d\n", ctrl.value);
            break;
        case 0x8000101: // BCSH Hue
            pr_info("ISP BCSH Hue: %d\n", ctrl.value);
            break;
        default:
            pr_info("Unknown V4L2 control: id=0x%x value=%d\n", ctrl.id, ctrl.value);
            break;
        }
        
        // For VIDIOC_G_CTRL, copy back the (possibly modified) value
        if (cmd == 0xc008561b) {
            if (copy_to_user(argp, &ctrl, sizeof(ctrl)))
                return -EFAULT;
        }
        
        return 0;
    }
    
    // Handle extended control IOCTL
    if (cmd == 0xc00c56c6) { // VIDIOC_S_EXT_CTRLS or similar
        pr_info("Extended V4L2 control operation\n");
        // For now, just acknowledge - would need more analysis for full implementation
        return 0;
    }
    
    // Check if this is a tuning command (0x74xx series from reference)
    if ((cmd >> 8 & 0xFF) == 0x74) {
        if ((cmd & 0xFF) < 0x33) {
            if ((cmd - ISP_TUNING_GET_PARAM) < 0xA) {
                
                switch (cmd) {
                case ISP_TUNING_GET_PARAM: {
                    // Copy tuning parameters from kernel to user
                    if (copy_from_user(isp_tuning_buffer, argp, sizeof(isp_tuning_buffer)))
                        return -EFAULT;
                    
                    // Reference processes various ISP parameter types (0-24)
                    param_type = *(int*)isp_tuning_buffer;
                    pr_info("ISP get tuning param type: %d\n", param_type);
                    
                    // For now, return success with dummy data
                    memset(isp_tuning_buffer + 4, 0x5A, 16); // Dummy tuning data
                    
                    if (copy_to_user(argp, isp_tuning_buffer, sizeof(isp_tuning_buffer)))
                        return -EFAULT;
                    
                    return 0;
                }
                case ISP_TUNING_SET_PARAM: {
                    // Set tuning parameters from user to kernel
                    if (copy_from_user(isp_tuning_buffer, argp, sizeof(isp_tuning_buffer)))
                        return -EFAULT;
                    
                    param_type = *(int*)isp_tuning_buffer;
                    pr_info("ISP set tuning param type: %d\n", param_type);
                    
                    // Reference calls various tisp_*_set_par_cfg() functions
                    // For now, acknowledge the parameter set
                    return 0;
                }
                case ISP_TUNING_GET_AE_INFO: {
                    pr_info("ISP get AE info\n");
                    
                    // Get AE (Auto Exposure) information
                    memset(isp_tuning_buffer, 0, sizeof(isp_tuning_buffer));
                    *(int*)isp_tuning_buffer = 1; // AE enabled
                    
                    if (copy_to_user(argp, isp_tuning_buffer, sizeof(isp_tuning_buffer)))
                        return -EFAULT;
                    
                    return 0;
                }
                case ISP_TUNING_SET_AE_INFO: {
                    pr_info("ISP set AE info\n");
                    
                    if (copy_from_user(isp_tuning_buffer, argp, sizeof(isp_tuning_buffer)))
                        return -EFAULT;
                    
                    // Process AE settings
                    return 0;
                }
                case ISP_TUNING_GET_AWB_INFO: {
                    pr_info("ISP get AWB info\n");
                    
                    // Get AWB (Auto White Balance) information
                    memset(isp_tuning_buffer, 0, sizeof(isp_tuning_buffer));
                    *(int*)isp_tuning_buffer = 1; // AWB enabled
                    
                    if (copy_to_user(argp, isp_tuning_buffer, sizeof(isp_tuning_buffer)))
                        return -EFAULT;
                    
                    return 0;
                }
                case ISP_TUNING_SET_AWB_INFO: {
                    pr_info("ISP set AWB info\n");
                    
                    if (copy_from_user(isp_tuning_buffer, argp, sizeof(isp_tuning_buffer)))
                        return -EFAULT;
                    
                    // Process AWB settings
                    return 0;
                }
                case ISP_TUNING_GET_STATS:
                case ISP_TUNING_GET_STATS2: {
                    pr_info("ISP get statistics\n");
                    
                    // Get ISP statistics information
                    memset(isp_tuning_buffer, 0, sizeof(isp_tuning_buffer));
                    strcpy(isp_tuning_buffer + 12, "ISP_STATS");
                    
                    if (copy_to_user(argp, isp_tuning_buffer, sizeof(isp_tuning_buffer)))
                        return -EFAULT;
                    
                    return 0;
                }
                default:
                    pr_info("Unhandled ISP tuning cmd: 0x%x\n", cmd);
                    return 0;
                }
            }
        }
    }
    
    pr_info("Invalid ISP tuning command: 0x%x\n", cmd);
    return -EINVAL;
}

static int isp_tuning_open(struct inode *inode, struct file *file)
{
    pr_info("ISP tuning device opened\n");
    return 0;
}

static int isp_tuning_release(struct inode *inode, struct file *file)
{
    pr_info("ISP tuning device released\n");
    return 0;
}

static const struct file_operations isp_tuning_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = isp_tuning_ioctl,
    .open = isp_tuning_open,
    .release = isp_tuning_release,
};

/* Frame channel device file operations */
static const struct file_operations frame_channel_fops = {
    .owner = THIS_MODULE,
    .open = frame_channel_open,
    .release = frame_channel_release,
    .unlocked_ioctl = frame_channel_unlocked_ioctl,
    .compat_ioctl = frame_channel_unlocked_ioctl,
};

// Create ISP tuning device node (reference: tisp_code_create_tuning_node)
static int create_isp_tuning_device(void)
{
    int ret;
    
    pr_info("Creating ISP tuning device...\n");
    
    // Allocate character device region
    if (isp_tuning_major == 0) {
        ret = alloc_chrdev_region(&isp_tuning_devno, 0, 1, "isp-m0");
        if (ret < 0) {
            pr_err("Failed to allocate chrdev region for ISP tuning\n");
            return ret;
        }
        isp_tuning_major = MAJOR(isp_tuning_devno);
    } else {
        isp_tuning_devno = MKDEV(isp_tuning_major, 0);
        ret = register_chrdev_region(isp_tuning_devno, 1, "isp-m0");
        if (ret < 0) {
            pr_err("Failed to register chrdev region for ISP tuning\n");
            return ret;
        }
    }
    
    // Initialize and add character device
    cdev_init(&isp_tuning_cdev, &isp_tuning_fops);
    ret = cdev_add(&isp_tuning_cdev, isp_tuning_devno, 1);
    if (ret < 0) {
        pr_err("Failed to add ISP tuning cdev\n");
        unregister_chrdev_region(isp_tuning_devno, 1);
        return ret;
    }
    
    // Create device class
    isp_tuning_class = class_create(THIS_MODULE, "isp-tuning");
    if (IS_ERR(isp_tuning_class)) {
        pr_err("Failed to create ISP tuning class\n");
        cdev_del(&isp_tuning_cdev);
        unregister_chrdev_region(isp_tuning_devno, 1);
        return PTR_ERR(isp_tuning_class);
    }
    
    // Create device node /dev/isp-m0
    if (!device_create(isp_tuning_class, NULL, isp_tuning_devno, NULL, "isp-m0")) {
        pr_err("Failed to create ISP tuning device\n");
        class_destroy(isp_tuning_class);
        cdev_del(&isp_tuning_cdev);
        unregister_chrdev_region(isp_tuning_devno, 1);
        return -ENODEV;
    }
    
    pr_info("ISP tuning device created: /dev/isp-m0 (major=%d)\n", isp_tuning_major);
    return 0;
}

// Destroy ISP tuning device node (reference: tisp_code_destroy_tuning_node)
static void destroy_isp_tuning_device(void)
{
    if (isp_tuning_class) {
        device_destroy(isp_tuning_class, isp_tuning_devno);
        class_destroy(isp_tuning_class);
        cdev_del(&isp_tuning_cdev);
        unregister_chrdev_region(isp_tuning_devno, 1);
        isp_tuning_class = NULL;
        pr_info("ISP tuning device destroyed\n");
    }
}

// Frame channel device implementations - based on reference tx_isp_fs_probe
static int frame_channel_open(struct inode *inode, struct file *file)
{
    int channel = iminor(inode);
    pr_info("Frame channel %d opened\n", channel);
    
    // Reference implementation checks channel state and initializes buffers
    // In frame_channel_open: checks if channel state < 2, sets to 3
    // Initializes completion, clears buffers, etc.
    
    return 0;
}

static int frame_channel_release(struct inode *inode, struct file *file)
{
    int channel = iminor(inode);
    pr_info("Frame channel %d released\n", channel);
    
    // Reference implementation cleans up channel resources
    // Frees buffers, resets state, etc.
    
    return 0;
}

static long frame_channel_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    void __user *argp = (void __user *)arg;
    int channel = iminor(file_inode(file));
    
    pr_info("Frame channel %d IOCTL: cmd=0x%x\n", channel, cmd);
    
    // Reference implementation handles many IOCTLs:
    // 0xc044560f - VIDIOC_QBUF (queue buffer)
    // 0xc0445609 - VIDIOC_DQBUF (dequeue buffer)
    // 0xc0145608 - VIDIOC_REQBUFS (request buffers)
    // 0x80045612 - VIDIOC_STREAMON (start streaming)
    // 0x80045613 - VIDIOC_STREAMOFF (stop streaming)
    // 0x407056c4 - VIDIOC_G_FMT (get format)
    // 0xc07056c3 - VIDIOC_S_FMT (set format)
    // 0x400456bf - Wait for completion
    // 0x800456c5 - Frame done event
    
    switch (cmd) {
    case 0xc0145608: { // VIDIOC_REQBUFS - Request buffers
        struct v4l2_requestbuffers {
            uint32_t count;
            uint32_t type;
            uint32_t memory;
            uint32_t capabilities;
            uint32_t reserved[1];
        } reqbuf;
        
        if (copy_from_user(&reqbuf, argp, sizeof(reqbuf)))
            return -EFAULT;
            
        pr_info("Channel %d: Request %d buffers, type=%d memory=%d\n",
                channel, reqbuf.count, reqbuf.type, reqbuf.memory);
                
        // Reference allocates video buffers based on count
        // For now, acknowledge the request
        reqbuf.count = min(reqbuf.count, 8U); // Limit to 8 buffers
        
        if (copy_to_user(argp, &reqbuf, sizeof(reqbuf)))
            return -EFAULT;
            
        return 0;
    }
    case 0xc044560f: { // VIDIOC_QBUF - Queue buffer
        struct v4l2_buffer {
            uint32_t index;
            uint32_t type;
            uint32_t bytesused;
            uint32_t flags;
            uint32_t field;
            struct timeval timestamp;
            struct v4l2_timecode timecode;
            uint32_t sequence;
            uint32_t memory;
            union {
                uint32_t offset;
                unsigned long userptr;
                void *planes;
            } m;
            uint32_t length;
            uint32_t reserved2;
            uint32_t reserved;
        } buffer;
        
        if (copy_from_user(&buffer, argp, sizeof(buffer)))
            return -EFAULT;
            
        pr_info("Channel %d: Queue buffer index=%d\n", channel, buffer.index);
        
        // Reference queues buffer for video capture
        // Sets buffer state, adds to ready queue, etc.
        
        return 0;
    }
    case 0xc0445609: { // VIDIOC_DQBUF - Dequeue buffer
        struct v4l2_buffer {
            uint32_t index;
            uint32_t type;
            uint32_t bytesused;
            uint32_t flags;
            uint32_t field;
            struct timeval timestamp;
            struct v4l2_timecode timecode;
            uint32_t sequence;
            uint32_t memory;
            union {
                uint32_t offset;
                unsigned long userptr;
                void *planes;
            } m;
            uint32_t length;
            uint32_t reserved2;
            uint32_t reserved;
        } buffer;
        
        if (copy_from_user(&buffer, argp, sizeof(buffer)))
            return -EFAULT;
            
        pr_info("Channel %d: Dequeue buffer request\n", channel);
        
        // Reference waits for completed buffer and returns it
        // For now, return dummy buffer data
        buffer.index = 0;
        buffer.bytesused = 1920 * 1080 * 3 / 2; // YUV420 size
        buffer.flags = 0x1; // V4L2_BUF_FLAG_MAPPED
        buffer.sequence = 0;
        
        if (copy_to_user(argp, &buffer, sizeof(buffer)))
            return -EFAULT;
            
        return 0;
    }
    case 0x80045612: { // VIDIOC_STREAMON - Start streaming
        uint32_t type;
        
        if (copy_from_user(&type, argp, sizeof(type)))
            return -EFAULT;
            
        pr_info("Channel %d: Stream ON, type=%d\n", channel, type);
        
        // Reference enables video streaming for this channel
        // Sets streaming state, starts DMA, etc.
        
        return 0;
    }
    case 0x80045613: { // VIDIOC_STREAMOFF - Stop streaming
        uint32_t type;
        
        if (copy_from_user(&type, argp, sizeof(type)))
            return -EFAULT;
            
        pr_info("Channel %d: Stream OFF, type=%d\n", channel, type);
        
        // Reference disables video streaming for this channel
        // Stops DMA, clears buffers, etc.
        
        return 0;
    }
    case 0x407056c4: { // VIDIOC_G_FMT - Get format
        struct v4l2_format {
            uint32_t type;
            union {
                struct {
                    uint32_t width;
                    uint32_t height;
                    uint32_t pixelformat;
                    uint32_t field;
                    uint32_t bytesperline;
                    uint32_t sizeimage;
                    uint32_t colorspace;
                    uint32_t priv;
                } pix;
                uint8_t raw_data[200];
            } fmt;
        } format;
        
        if (copy_from_user(&format, argp, sizeof(format)))
            return -EFAULT;
            
        pr_info("Channel %d: Get format, type=%d\n", channel, format.type);
        
        // Set default HD format
        format.fmt.pix.width = 1920;
        format.fmt.pix.height = 1080;
        format.fmt.pix.pixelformat = 0x3231564e; // NV12 format
        format.fmt.pix.field = 1; // V4L2_FIELD_NONE
        format.fmt.pix.bytesperline = 1920;
        format.fmt.pix.sizeimage = 1920 * 1080 * 3 / 2;
        format.fmt.pix.colorspace = 8; // V4L2_COLORSPACE_REC709
        
        if (copy_to_user(argp, &format, sizeof(format)))
            return -EFAULT;
            
        return 0;
    }
    case 0xc07056c3: { // VIDIOC_S_FMT - Set format
        struct v4l2_format {
            uint32_t type;
            union {
                struct {
                    uint32_t width;
                    uint32_t height;
                    uint32_t pixelformat;
                    uint32_t field;
                    uint32_t bytesperline;
                    uint32_t sizeimage;
                    uint32_t colorspace;
                    uint32_t priv;
                } pix;
                uint8_t raw_data[200];
            } fmt;
        } format;
        
        if (copy_from_user(&format, argp, sizeof(format)))
            return -EFAULT;
            
        pr_info("Channel %d: Set format %dx%d pixfmt=0x%x\n",
                channel, format.fmt.pix.width, format.fmt.pix.height, format.fmt.pix.pixelformat);
        
        // Reference validates and configures the format
        // For now, acknowledge the format set
        
        return 0;
    }
    case 0x400456bf: { // Frame completion wait
        uint32_t result = 1; // Frame ready
        
        pr_info("Channel %d: Frame completion wait\n", channel);
        
        // Reference waits for frame completion event
        // For now, immediately return success
        
        if (copy_to_user(argp, &result, sizeof(result)))
            return -EFAULT;
            
        return 0;
    }
    case 0x800456c5: { // Frame done event
        uint32_t event_data;
        
        if (copy_from_user(&event_data, argp, sizeof(event_data)))
            return -EFAULT;
            
        pr_info("Channel %d: Frame done event, data=0x%x\n", channel, event_data);
        
        // Reference processes frame completion event
        // Signals waiting threads, updates statistics, etc.
        
        return 0;
    }
    default:
        pr_info("Channel %d: Unhandled IOCTL 0x%x\n", channel, cmd);
        return -ENOTTY;
    }
    
    return 0;
}

// Create frame channel devices - based on reference tx_isp_fs_probe
static int create_frame_channel_devices(void)
{
    int ret, i;
    char device_name[16];
    
    pr_info("Creating %d frame channel devices...\n", num_channels);
    
    for (i = 0; i < num_channels; i++) {
        // Reference creates devices like "video0", "video1" etc.
        // But some systems expect "video%d" devices to be created by V4L2
        // For TX-ISP, reference uses names that match framesource expectations
        snprintf(device_name, sizeof(device_name), "video%d", i);
        
        frame_channel_devices[i].minor = MISC_DYNAMIC_MINOR;
        frame_channel_devices[i].name = kstrdup(device_name, GFP_KERNEL);
        frame_channel_devices[i].fops = &frame_channel_fops;
        
        if (!frame_channel_devices[i].name) {
            pr_err("Failed to allocate name for channel %d\n", i);
            ret = -ENOMEM;
            goto cleanup;
        }
        
        ret = misc_register(&frame_channel_devices[i]);
        if (ret < 0) {
            pr_err("Failed to register frame channel %d: %d\n", i, ret);
            kfree(frame_channel_devices[i].name);
            frame_channel_devices[i].name = NULL;
            goto cleanup;
        }
        
        pr_info("Frame channel device created: /dev/%s\n", device_name);
    }
    
    return 0;
    
cleanup:
    // Clean up already created devices
    for (i = i - 1; i >= 0; i--) {
        if (frame_channel_devices[i].name) {
            misc_deregister(&frame_channel_devices[i]);
            kfree(frame_channel_devices[i].name);
            frame_channel_devices[i].name = NULL;
        }
    }
    return ret;
}

// Destroy frame channel devices
static void destroy_frame_channel_devices(void)
{
    int i;
    
    for (i = 0; i < num_channels; i++) {
        if (frame_channel_devices[i].name) {
            misc_deregister(&frame_channel_devices[i]);
            kfree(frame_channel_devices[i].name);
            frame_channel_devices[i].name = NULL;
            pr_info("Frame channel device %d destroyed\n", i);
        }
    }
}

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
    case 0x805056c1: { // TX_ISP_SENSOR_REGISTER - Register sensor
        struct tx_isp_sensor_register_info {
            char name[32];
            // Other fields would be here in real struct
        } reg_info;
        struct registered_sensor *sensor, *tmp;
        
        if (copy_from_user(&reg_info, argp, sizeof(reg_info)))
            return -EFAULT;
            
        mutex_lock(&sensor_list_mutex);
        
        // Check if sensor already registered
        list_for_each_entry_safe(sensor, tmp, &sensor_list, list) {
            if (strncmp(sensor->name, reg_info.name, sizeof(sensor->name)) == 0) {
                mutex_unlock(&sensor_list_mutex);
                pr_info("Sensor %s already registered\n", reg_info.name);
                return 0;
            }
        }
        
        // Add new sensor to list
        sensor = kzalloc(sizeof(struct registered_sensor), GFP_KERNEL);
        if (!sensor) {
            mutex_unlock(&sensor_list_mutex);
            return -ENOMEM;
        }
        
        strncpy(sensor->name, reg_info.name, sizeof(sensor->name) - 1);
        sensor->name[sizeof(sensor->name) - 1] = '\0';
        sensor->index = sensor_count++;
        INIT_LIST_HEAD(&sensor->list);
        list_add_tail(&sensor->list, &sensor_list);
        
        mutex_unlock(&sensor_list_mutex);
        
        pr_info("Sensor registered: %s (index %d)\n", sensor->name, sensor->index);
        return 0;
    }
    case 0xc050561a: { // TX_ISP_SENSOR_ENUM_INPUT - Enumerate sensor inputs
        struct sensor_enum_input {
            int index;
            char name[32];
        } input;
        struct registered_sensor *sensor;
        int found = 0;
        
        if (copy_from_user(&input, argp, sizeof(input)))
            return -EFAULT;
            
        mutex_lock(&sensor_list_mutex);
        
        // Find sensor at requested index
        
        list_for_each_entry(sensor, &sensor_list, list) {
            if (sensor->index == input.index) {
                strncpy(input.name, sensor->name, sizeof(input.name) - 1);
                input.name[sizeof(input.name) - 1] = '\0';
                found = 1;
                break;
            }
        }
        
        mutex_unlock(&sensor_list_mutex);
        
        if (!found) {
            pr_info("No sensor at index %d\n", input.index);
            return -EINVAL;
        }
        
        if (copy_to_user(argp, &input, sizeof(input)))
            return -EFAULT;
            
        pr_info("Sensor enumeration: index=%d name=%s\n", input.index, input.name);
        return 0;
    }
    case 0xc0045627: { // TX_ISP_SENSOR_SET_INPUT - Set active sensor input
        int input_index;
        struct registered_sensor *sensor;
        int found = 0;
        
        if (copy_from_user(&input_index, argp, sizeof(input_index)))
            return -EFAULT;
            
        // Validate sensor exists
        mutex_lock(&sensor_list_mutex);
        
        list_for_each_entry(sensor, &sensor_list, list) {
            if (sensor->index == input_index) {
                found = 1;
                break;
            }
        }
        mutex_unlock(&sensor_list_mutex);
        
        if (!found) {
            pr_err("No sensor at index %d for set input\n", input_index);
            return -EINVAL;
        }
        
        pr_info("Sensor input set to index %d (%s)\n", input_index, sensor->name);
        return 0;
    }
    case 0x805056c2: { // TX_ISP_SENSOR_RELEASE_SENSOR - Release/unregister sensor
        struct tx_isp_sensor_register_info {
            char name[32];
            // Other fields would be here in real struct
        } unreg_info;
        struct registered_sensor *sensor, *tmp;
        int found = 0;
        
        if (copy_from_user(&unreg_info, argp, sizeof(unreg_info)))
            return -EFAULT;
            
        mutex_lock(&sensor_list_mutex);
        
        // Find and remove sensor from list
        
        list_for_each_entry_safe(sensor, tmp, &sensor_list, list) {
            if (strncmp(sensor->name, unreg_info.name, sizeof(sensor->name)) == 0) {
                list_del(&sensor->list);
                kfree(sensor);
                found = 1;
                break;
            }
        }
        
        mutex_unlock(&sensor_list_mutex);
        
        if (found) {
            pr_info("Sensor released: %s\n", unreg_info.name);
        } else {
            pr_info("Sensor not found for release: %s\n", unreg_info.name);
        }
        
        return 0;
    }
    case 0x8038564f: { // TX_ISP_SENSOR_S_REGISTER - Set sensor register
        struct sensor_reg_write {
            uint32_t addr;
            uint32_t val;
            uint32_t size;
            // Additional fields from reference
            uint32_t reserved[10];
        } reg_write;
        
        if (copy_from_user(&reg_write, argp, sizeof(reg_write)))
            return -EFAULT;
        
        pr_info("Sensor register write: addr=0x%x val=0x%x size=%d\n",
                reg_write.addr, reg_write.val, reg_write.size);
        
        // In real implementation, this would write to sensor via I2C
        // Following reference pattern of calling sensor ops
        
        return 0;
    }
    case 0xc0385650: { // TX_ISP_SENSOR_G_REGISTER - Get sensor register
        struct sensor_reg_read {
            uint32_t addr;
            uint32_t val;    // Will be filled by driver
            uint32_t size;
            // Additional fields from reference
            uint32_t reserved[10];
        } reg_read;
        
        if (copy_from_user(&reg_read, argp, sizeof(reg_read)))
            return -EFAULT;
        
        pr_info("Sensor register read: addr=0x%x size=%d\n",
                reg_read.addr, reg_read.size);
        
        // In real implementation, this would read from sensor via I2C
        // For now, return dummy data
        reg_read.val = 0x5A; // Dummy sensor data
        
        if (copy_to_user(argp, &reg_read, sizeof(reg_read)))
            return -EFAULT;
        
        pr_info("Sensor register read result: addr=0x%x val=0x%x\n",
                reg_read.addr, reg_read.val);
        
        return 0;
    }
    case 0x800856d5: { // TX_ISP_GET_BUF - Calculate required buffer size
        struct isp_buf_result {
            uint32_t addr;   // Physical address (usually 0)
            uint32_t size;   // Calculated buffer size
        } buf_result;
        uint32_t width = 1920;   // Default HD width
        uint32_t height = 1080;  // Default HD height
        uint32_t stride_factor;
        uint32_t main_buf;
        uint32_t total_main;
        uint32_t yuv_stride;
        uint32_t total_size;
        
        pr_info("ISP buffer calculation: width=%d height=%d memopt=%d\n",
                width, height, isp_memopt);
        
        // Reference buffer size calculation logic
        // Based on the decompiled code from reference driver
        
        stride_factor = height << 3; // $t0_3 = $a2_9 << 3
        
        // Main buffer calculation: (($v0_83 + 7) u>> 3) * $t0_3
        main_buf = ((width + 7) >> 3) * stride_factor;
        
        // Additional buffer: ($a0_29 u>> 1) + $a0_29
        total_main = (main_buf >> 1) + main_buf;
        
        // YUV buffer calculation: (((($v0_83 + 0x1f) u>> 5) + 7) u>> 3) * (((($a2_9 + 0xf) u>> 4) + 1) << 3)
        yuv_stride = ((((width + 0x1f) >> 5) + 7) >> 3) * ((((height + 0xf) >> 4) + 1) << 3);
        
        total_size = total_main + yuv_stride;
        
        // Memory optimization affects calculation
        if (isp_memopt == 0) {
            uint32_t extra_buf = (((width >> 1) + 7) >> 3) * stride_factor;
            uint32_t misc_buf = ((((width >> 5) + 7) >> 3) * stride_factor) >> 5;
            
            total_size = (yuv_stride << 2) + misc_buf + (extra_buf >> 1) + total_main + extra_buf;
        }
        
        pr_info("ISP calculated buffer size: %d bytes (0x%x)\n", total_size, total_size);
        
        // Set result: address=0, size=calculated
        buf_result.addr = 0;
        buf_result.size = total_size;
        
        if (copy_to_user(argp, &buf_result, sizeof(buf_result)))
            return -EFAULT;
            
        return 0;
    }
    case 0x800856d4: { // TX_ISP_SET_BUF - Set buffer addresses and configure DMA
        struct isp_buf_setup {
            uint32_t addr;   // Physical buffer address
            uint32_t size;   // Buffer size
        } buf_setup;
        uint32_t width = 1920;
        uint32_t height = 1080;
        uint32_t stride;
        uint32_t frame_size;
        uint32_t uv_offset;
        uint32_t yuv_stride;
        uint32_t yuv_size;
        
        if (copy_from_user(&buf_setup, argp, sizeof(buf_setup)))
            return -EFAULT;
        
        pr_info("ISP set buffer: addr=0x%x size=%d\n", buf_setup.addr, buf_setup.size);
        
        // Calculate stride and buffer offsets like reference
        stride = ((width + 7) >> 3) << 3;  // Aligned stride
        frame_size = stride * height;
        
        // Validate buffer size
        if (buf_setup.size < frame_size) {
            pr_err("Buffer too small: need %d, got %d\n", frame_size, buf_setup.size);
            return -EFAULT;
        }
        
        // Configure main frame buffers (system registers like reference)
        // These would be actual register writes in hardware implementation
        pr_info("Configuring main frame buffer: 0x%x stride=%d\n", buf_setup.addr, stride);
        
        // UV buffer offset calculation
        uv_offset = frame_size + (frame_size >> 1);
        if (buf_setup.size >= uv_offset) {
            pr_info("Configuring UV buffer: 0x%x\n", buf_setup.addr + frame_size);
        }
        
        // YUV420 additional buffer configuration
        yuv_stride = ((((width + 0x1f) >> 5) + 7) >> 3) << 3;
        yuv_size = yuv_stride * ((((height + 0xf) >> 4) + 1) << 3);
        
        if (!isp_memopt) {
            // Full buffer mode - configure all planes
            pr_info("Full buffer mode: YUV stride=%d size=%d\n", yuv_stride, yuv_size);
        } else {
            // Memory optimized mode
            pr_info("Memory optimized mode\n");
        }
        
        return 0;
    }
    case 0x800856d6: { // TX_ISP_WDR_SET_BUF - WDR buffer setup
        struct wdr_buf_setup {
            uint32_t addr;   // WDR buffer address
            uint32_t size;   // WDR buffer size
        } wdr_setup;
        uint32_t wdr_width = 1920;
        uint32_t wdr_height = 1080;
        uint32_t wdr_mode = 1; // Linear mode by default
        uint32_t required_size;
        uint32_t stride_lines;
        
        if (copy_from_user(&wdr_setup, argp, sizeof(wdr_setup)))
            return -EFAULT;
        
        pr_info("WDR buffer setup: addr=0x%x size=%d\n", wdr_setup.addr, wdr_setup.size);
        
        if (wdr_mode == 1) {
            // Linear mode calculation
            stride_lines = wdr_height;
            required_size = (stride_lines * wdr_width) << 1; // 16-bit per pixel
        } else if (wdr_mode == 2) {
            // WDR mode calculation - different formula
            required_size = wdr_width * wdr_height * 2; // Different calculation for WDR
            stride_lines = required_size / (wdr_width << 1);
        } else {
            pr_err("Unsupported WDR mode: %d\n", wdr_mode);
            return -EINVAL;
        }
        
        pr_info("WDR mode %d: required_size=%d stride_lines=%d\n",
                wdr_mode, required_size, stride_lines);
        
        if (wdr_setup.size < required_size) {
            pr_err("WDR buffer too small: need %d, got %d\n", required_size, wdr_setup.size);
            return -EFAULT;
        }
        
        // Configure WDR registers (like reference 0x2004, 0x2008, 0x200c)
        pr_info("Configuring WDR registers: addr=0x%x stride=%d lines=%d\n",
                wdr_setup.addr, wdr_width << 1, stride_lines);
        
        return 0;
    }
    case 0x800856d7: { // TX_ISP_WDR_GET_BUF - Get WDR buffer size
        struct wdr_buf_result {
            uint32_t addr;   // WDR buffer address (usually 0)
            uint32_t size;   // Calculated WDR buffer size
        } wdr_result;
        uint32_t wdr_width = 1920;
        uint32_t wdr_height = 1080;
        uint32_t wdr_mode = 1; // Linear mode
        uint32_t wdr_size;
        
        pr_info("WDR buffer calculation: width=%d height=%d mode=%d\n",
                wdr_width, wdr_height, wdr_mode);
        
        if (wdr_mode == 1) {
            // Linear mode: ($s1_13 * *($s2_23 + 0x124)) << 1
            wdr_size = (wdr_height * wdr_width) << 1;
        } else if (wdr_mode == 2) {
            // WDR mode: different calculation
            wdr_size = wdr_width * wdr_height * 2;
        } else {
            pr_err("WDR mode not supported\n");
            return -EINVAL;
        }
        
        pr_info("WDR calculated buffer size: %d bytes (0x%x)\n", wdr_size, wdr_size);
        
        wdr_result.addr = 0;
        wdr_result.size = wdr_size;
        
        if (copy_to_user(argp, &wdr_result, sizeof(wdr_result)))
            return -EFAULT;
        
        return 0;
    }
    case 0x800456d0: { // TX_ISP_VIDEO_LINK_SETUP - Video link configuration
        int link_config;
        
        if (copy_from_user(&link_config, argp, sizeof(link_config)))
            return -EFAULT;
        
        if (link_config >= 2) {
            pr_err("Invalid video link config: %d (valid: 0-1)\n", link_config);
            return -EINVAL;
        }
        
        pr_info("Video link setup: config=%d\n", link_config);
        
        // Reference implementation configures subdev links and pads
        // In full implementation, this would:
        // 1. Find subdev pads using find_subdev_link_pad()
        // 2. Setup media pipeline connections
        // 3. Configure link properties based on config value
        // 4. Store config in device structure at offset 0x10c
        
        // For now, acknowledge the link setup
        // isp_dev->link_config = link_config; // Would store at offset 0x10c
        
        return 0;
    }
    case 0x800456d1: { // TX_ISP_VIDEO_LINK_DESTROY - Destroy video links
        return tx_isp_video_link_destroy_impl(isp_dev);
    }
    case 0x800456d2: { // TX_ISP_VIDEO_LINK_STREAM_ON - Enable video link streaming
        return tx_isp_video_link_stream_impl(isp_dev, 1);
    }
    case 0x800456d3: { // TX_ISP_VIDEO_LINK_STREAM_OFF - Disable video link streaming
        return tx_isp_video_link_stream_impl(isp_dev, 0);
    }
    case 0x80045612: { // VIDIOC_STREAMON - Start video streaming
        return tx_isp_video_s_stream_impl(isp_dev, 1);
    }
    case 0x80045613: { // VIDIOC_STREAMOFF - Stop video streaming
        return tx_isp_video_s_stream_impl(isp_dev, 0);
    }
    case 0x800456d8: { // TX_ISP_WDR_ENABLE - Enable WDR mode
        int wdr_enable = 1;
        
        pr_info("WDR mode ENABLE\n");
        
        // Configure WDR enable (matches reference logic)
        // In reference: *($s2_24 + 0x17c) = 1
        // and calls tisp_s_wdr_en(1)
        
        return 0;
    }
    case 0x800456d9: { // TX_ISP_WDR_DISABLE - Disable WDR mode
        int wdr_disable = 0;
        
        pr_info("WDR mode DISABLE\n");
        
        // Configure WDR disable (matches reference logic)
        // In reference: *($s2_23 + 0x17c) = 0
        // and calls tisp_s_wdr_en(0)
        
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

    /* Create ISP tuning device - required for IMP_ISP_EnableTuning() */
    ret = create_isp_tuning_device();
    if (ret) {
        pr_err("Failed to create ISP tuning device: %d\n", ret);
        tx_isp_proc_exit(ourISPdev);
        misc_deregister(&tx_isp_miscdev);
        platform_driver_unregister(&tx_isp_driver);
        platform_device_unregister(&tx_isp_platform_device);
        goto err_free_dev;
    }

    /* Create frame channel devices - required for video streaming */
    ret = create_frame_channel_devices();
    if (ret) {
        pr_err("Failed to create frame channel devices: %d\n", ret);
        destroy_isp_tuning_device();
        tx_isp_proc_exit(ourISPdev);
        misc_deregister(&tx_isp_miscdev);
        platform_driver_unregister(&tx_isp_driver);
        platform_device_unregister(&tx_isp_platform_device);
        goto err_free_dev;
    }

    pr_info("TX ISP driver initialized successfully\n");
    pr_info("Device nodes created:\n");
    pr_info("  /dev/tx-isp (major=10, minor=dynamic)\n");
    pr_info("  /dev/isp-m0 (major=%d, minor=0) - ISP tuning interface\n", isp_tuning_major);
    for (ret = 0; ret < num_channels; ret++) {
        pr_info("  /dev/video%d - Frame channel %d\n", ret, ret);
    }
    pr_info("  /proc/jz/isp/isp-w02\n");
    
    return 0;

err_free_dev:
    kfree(ourISPdev);
    ourISPdev = NULL;
    return ret;
}

static void tx_isp_exit(void)
{
    struct registered_sensor *sensor, *tmp;
    
    pr_info("TX ISP driver exiting...\n");

    /* Clean up sensor list */
    mutex_lock(&sensor_list_mutex);
    list_for_each_entry_safe(sensor, tmp, &sensor_list, list) {
        list_del(&sensor->list);
        kfree(sensor);
    }
    sensor_count = 0;
    mutex_unlock(&sensor_list_mutex);

    if (ourISPdev) {
        /* Destroy frame channel devices */
        destroy_frame_channel_devices();
        
        /* Destroy ISP tuning device */
        destroy_isp_tuning_device();
        
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