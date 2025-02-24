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

#define MAX_CHANNELS 6  // Define the maximum number of frame channels
#define MAX_COMPONENTS 8 // Define the maximum number of components

#define TX_ISP_PLATFORM_MAX_NUM 16
#define T31_ISP_FREQ     250000000   // 250MHz ISP core clock (matches AHB)
#define T31_CGU_ISP_FREQ 125000000   // 125MHz CGU_ISP clock (matches APB)
#define MAX_SENSORS 1
#define SENSOR_NAME_SIZE 32
// Add these at the top with other defines
#define ISP_ALLOC_KMALLOC 0
#define ISP_ALLOC_DMA    1
#define MAX_BUFFERS 64
#define FS_ATTR_SET     BIT(0)
#define FRAME_FMT_SET BIT(1)  // Format has been set for this channel

// ISP Core register offsets (from ISP_BASE)
#define ISP_CTRL_OFFSET     0x0000   // Control register
#define ISP_STATUS_OFFSET   0x0004   // Status register
#define ISP_INT_CLR_OFFSET  0x000C   // Interrupt clear
#define ISP_STREAM_CTRL     0x0010   // Stream control
#define ISP_STREAM_START    0x0014   // Stream start

// Possibly supported video formats
#define ISP_FMT_YUV422    0x0a        // Current YUV422 format code
#define ISP_FMT_NV12      0x3231564e   // NV12 format code (from libimp)
#define V4L2_PIX_FMT_NV12 0x3231564e   // Match libimp's NV12 format code

/* Pad link types */
#define TX_ISP_PADTYPE_INPUT     0
#define TX_ISP_PADTYPE_OUTPUT    1


// VIC States
#define VIC_STATE_INIT      1
#define VIC_STATE_READY     2
#define VIC_STATE_RUNNING   3
#define VIC_STATE_STOPPING  4


struct tx_isp_dev *ourISPdev = NULL;

struct isp_platform_data {
    unsigned long clock_rate;
};
static struct isp_platform_data isp_pdata = {
    .clock_rate = T31_ISP_FREQ,
};


struct frame_source_device {
    struct tx_isp_subdev *sd;
    void *priv;
    struct mutex lock;
    int state;
};

static struct platform_device *tx_isp_pdev;

static void tx_isp_unregister_platforms(struct tx_isp_platform *platforms)
{
    struct tx_isp_platform *platform = platforms;
    struct platform_driver *drv;

    /* Loop through platform array until we hit end (0x10 entries) */
    while (platform != &platforms[TX_ISP_PLATFORM_MAX_NUM]) {
        drv = platform->drv;

        /* If driver exists, unregister it first */
        if (drv) {
            platform_driver_unregister(drv);
        }

        /* If device exists, unregister it */
        if (platform->dev) {
            platform_device_unregister(platform->dev);
        }

        platform++; // Move to next platform entry
    }
}


static int read_proc_value(const char *path, char *buf, size_t size)
{
    struct file *f;
    mm_segment_t old_fs;
    int ret;

    f = filp_open(path, O_RDONLY, 0);
    if (IS_ERR(f))
        return PTR_ERR(f);

    old_fs = get_fs();
    set_fs(KERNEL_DS);

    ret = vfs_read(f, (char __user *)buf, size - 1, &f->f_pos);

    set_fs(old_fs);
    filp_close(f, NULL);

    if (ret > 0) {
        buf[ret] = '\0';
        // Remove trailing newline if present
        if (ret > 0 && buf[ret-1] == '\n')
            buf[ret-1] = '\0';
    }

    return ret;
}

static int read_proc_uint(const char *path, unsigned int *value)
{
    char buf[32];
    int ret;

    ret = read_proc_value(path, buf, sizeof(buf));
    if (ret < 0)
        return ret;

    return kstrtouint(buf, 0, value);
}

int detect_sensor_type(struct tx_isp_dev *dev)
{
    char sensor_name[32];
    unsigned int chip_id = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    int ret;

    // Wait briefly for sensor driver to initialize proc entries
    msleep(100);

    // Read sensor info from proc
    ret = read_proc_value("/proc/jz/sensor/name", sensor_name, sizeof(sensor_name));
    if (ret < 0) {
        pr_err("Failed to read sensor name from proc: %d\n", ret);
        return ret;
    }

    ret = read_proc_uint("/proc/jz/sensor/chip_id", &chip_id);
    if (ret < 0) {
        pr_err("Failed to read sensor chip ID from proc: %d\n", ret);
        return ret;
    }

    ret = read_proc_uint("/proc/jz/sensor/width", &width);
    ret |= read_proc_uint("/proc/jz/sensor/height", &height);
    if (ret < 0) {
        pr_err("Failed to read sensor resolution from proc: %d\n", ret);
        return ret;
    }

    pr_info("Detected sensor: %s (ID: 0x%x) %dx%d\n",
            sensor_name, chip_id, width, height);

    // For now assume MIPI - we could add interface type to proc later
    dev->sensor_type = 1;  // MIPI

    // Set mode
    // dev->sensor_mode = 0x196;
    pr_warn("Unknown sensor %s - using default mode\n", sensor_name);
    dev->sensor_mode = 0x195;

    // Store info in device
    dev->sensor_width = width;
    dev->sensor_height = height;
    strlcpy(dev->sensor_name, sensor_name, sizeof(dev->sensor_name));

//    // Write to hardware register
//    writel(dev->sensor_type, dev->reg_base + 0x110 + 0x14);
    wmb();

    return 0;
}

int setup_i2c_adapter(struct tx_isp_dev *dev)
{
    struct i2c_board_info board_info = {0};  // Initialize to zero
    struct i2c_adapter *adapter;
    struct i2c_client *client;
    char buf[32];
    int ret;

    pr_info("Setting up I2C infrastructure for Image Sensor...\n");

    // Read sensor name
    ret = read_proc_value("/proc/jz/sensor/name", buf, sizeof(buf));
    if (ret < 0) {
        pr_err("Failed to read sensor name: %d\n", ret);
        return ret;
    }
    strncpy(board_info.type, buf, I2C_NAME_SIZE - 1);
    board_info.type[I2C_NAME_SIZE - 1] = '\0';

    // Read I2C address
    ret = read_proc_value("/proc/jz/sensor/i2c_addr", buf, sizeof(buf));
    if (ret < 0) {
        pr_err("Failed to read sensor i2c_addr: %d\n", ret);
        return ret;
    }
    if (sscanf(buf, "0x%x", &board_info.addr) != 1) {
        pr_err("Invalid i2c_addr format\n");
        return -EINVAL;
    }

    // Match the original configuration exactly
    board_info.irq = 68;  // From /proc/interrupts - i2c.0
    board_info.platform_data = dev;
    board_info.flags = I2C_CLIENT_TEN;

    // Request the i2c-dev module first
    ret = request_module("i2c-dev");
    if (ret)
        pr_warn("Could not load i2c-jz module: %d\n", ret);

    // Give some time for driver binding
    msleep(1000);

    adapter = i2c_get_adapter(0);
    if (!adapter) {
        pr_err("Failed to get I2C adapter\n");
        return -ENODEV;
    }

    // Print detailed adapter info
    pr_info("I2C Adapter: name='%s' nr=%d class=%s\n",
            adapter->name ? adapter->name : "unknown",
            adapter->nr,
            adapter->dev.class ? adapter->dev.class->name : "none");

    // Create new I2C device
    client = i2c_new_device(adapter, &board_info);
    if (!client) {
        pr_err("Failed to create I2C device\n");
        i2c_put_adapter(adapter);
        return -ENODEV;
    }

    // Try to get module reference
    if (client->dev.driver && !try_module_get(client->dev.driver->owner)) {
        pr_err("Could not get module reference\n");
        i2c_unregister_device(client);
        i2c_put_adapter(adapter);
        return -ENODEV;
    }

    dev->sensor_i2c_client = client;

    pr_info("I2C sensor initialized: type=%s addr=0x%02x adapter=%p irq=%d\n",
            client->name, client->addr, client->adapter, client->irq);

    i2c_put_adapter(adapter);
    return 0;
}

static int handle_sensor_register(struct tx_isp_subdev *incoming_sd)
{
    struct tx_isp_dev *isp_dev = ourISPdev;
    struct tx_isp_sensor *sensor;
    int ret;

    if (!isp_dev) {
        isp_printf(ISP_ERROR_LEVEL, "No valid ISP device found\n");
        return -EINVAL;
    }

    /* Allocate new sensor */
    sensor = kzalloc(sizeof(*sensor), GFP_KERNEL);
    if (!sensor) {
        isp_printf(ISP_ERROR_LEVEL, "Failed to allocate sensor structure\n");
        return -ENOMEM;
    }

    /* Initialize subdev structure */
    memset(&sensor->sd, 0, sizeof(struct tx_isp_subdev));
    sensor->sd.dev = isp_dev->dev;
    sensor->sd.pdev = isp_dev->pdev;

    /* Initialize sensor list */
    INIT_LIST_HEAD(&sensor->list);

    /* Set as active sensor */
    isp_dev->sensor = sensor;

    /* Setup I2C */
    ret = setup_i2c_adapter(isp_dev);
    if (ret) {
        goto err_free_sensor;
    }

    return 0;

err_free_sensor:
    kfree(sensor);
    return ret;
}



// Example implementation of event callback:
static int tx_isp_event_handler(void *priv, u32 event, void *data)
{
    struct isp_channel_event *evt = priv;
    int ret = 0;
    unsigned long flags;

    if (!evt || !evt->channel)
        return -EINVAL;

	pr_info("Event handler called: event=0x%x data=%p\n", event, data);

    return ret;
}


// Helper to set up pad event handling
static void init_pad_event_handling(struct tx_isp_subdev_pad *pad,
                                  int (*event_handler)(struct tx_isp_subdev_pad *, unsigned int, void *))
{
    if (pad) {
        pad->event = event_handler;
    }
}



static void init_tuning_event_handling(struct tx_isp_dev *dev)
{
    struct tx_isp_subdev *sd = dev->fs_dev->sd;
    if (!sd) return;

    // Register tuning handlers on the main input pad
    if (sd->num_inpads > 0) {
        init_pad_event_handling(&sd->inpads[0], tx_isp_event_handler);
    }
}

struct isp_channel *get_channel_from_pad(struct tx_isp_subdev_pad *pad)
{
    struct tx_isp_dev *dev = ourISPdev;
    int i;

    if (!pad || !pad->sd)
        return NULL;

    // Find channel by matching subdev pointer
    for (i = 0; i < MAX_CHANNELS; i++) {
        if (&dev->channels[i].subdev == pad->sd)
            return &dev->channels[i];
    }

    return NULL;
}


// Event handler for frame source pads
static int fs_pad_event_handler(struct tx_isp_subdev_pad *pad,
                              unsigned int event, void *data)
{
    struct isp_channel *chn;
    int ret = 0;

    chn = get_channel_from_pad(pad);
    if (!chn)
        return -EINVAL;

    pr_info("FS pad event handler: event=0x%x data=%p\n", event, data);
//    switch (event) {
//        case ISP_EVENT_BUFFER_REQUEST:
//            ret = handle_buffer_request(chn, data);
//        break;
//        case ISP_EVENT_BUFFER_ENQUEUE:
//            ret = handle_buffer_enqueue(chn, data);
//        break;
//        // ... other event handling ...
//        default:
//            pr_info("Unhandled event 0x%x\n", event);
//        ret = -ENOIOCTLCMD;
//    }

    return ret;
}


void unregister_channel_callbacks(struct isp_channel_event *evt)
{
    unsigned long flags;

    if (!evt)
        return;

    spin_lock_irqsave(&evt->lock, flags);
    memset(&evt->cb, 0, sizeof(evt->cb));
    spin_unlock_irqrestore(&evt->lock, flags);
}

static int handle_frame_update(struct isp_channel *chn, struct frame_node *node)
{
    struct group_metadata *meta;
    unsigned long flags;
    struct timespec ts;

    pr_info("Handle Frame update: node=%p\n", node);

    if (!chn || !node)
        pr_err("Invalid frame source or node\n");
    return -EINVAL;

    meta = node->metadata;
    if (!meta)
        pr_err("Invalid metadata\n");
    return -EINVAL;


    // Update metadata state
    meta->state = FRAME_STATE_DONE;
    meta->done_flag = 1;
    meta->flags |= V4L2_BUF_FLAG_DONE;

    // Get timestamp
    ktime_get_ts(&ts);
    node->timestamp = ts;
    meta->timestamp = timespec_to_ns(&ts);

    // Wake up any waiters
    wake_up_interruptible(&chn->queue->wait);

    return 0;
}

// Update wrapper function to match libimp expectations
static int group_update_wrapper(void *group, void *frame)
{
    struct isp_channel chn;
    struct frame_node *node;
    uint32_t channel;
    int ret;

    pr_info("Group update wrapper: group=%p frame=%p\n", group, frame);

    if (!group || !frame) {
        pr_err("Invalid group update args\n");
        return -EINVAL;
    }

    // Set thread name to match libimp
    strncpy(current->comm, "group_update", sizeof(current->comm));

    channel = ((struct frame_group *)group)->channel;
    if (channel >= MAX_CHANNELS)
        return -EINVAL;

    chn = ourISPdev->channels[channel];
    node = container_of(frame, struct frame_node, metadata);

    // Handle frame - let userspace handle VBM frame release
    ret = handle_frame_update(&chn, node);

    return ret;
}


static int isp_channel_register_events(struct isp_channel *chn)
{
    // Setup event handler callback
    chn->event_cb = tx_isp_event_handler;
    chn->event_priv = chn;

    pr_info("Registered event handler for channel %d\n", chn->channel_id);
    return 0;
}



static int init_channel_events(struct isp_channel *chn)
{
    struct isp_channel_event *evt;

    evt = kzalloc(sizeof(*evt), GFP_KERNEL);
    if (!evt)
        return -ENOMEM;

    // Initialize event structure
    spin_lock_init(&evt->lock);
    atomic_set(&evt->pending_events, 0);
    evt->event_count = 0;
    evt->error_count = 0;
    evt->channel = chn;

    // Set up default handler
    evt->event_handler = tx_isp_event_handler;
    evt->priv = chn;

    // Store in channel
    chn->event = evt;

    // Register with pad event system
    if (chn->pad.sd) {
        chn->pad.event = tx_isp_send_event_to_remote;
    }

    return isp_channel_register_events(chn);
}

// Helper function to calculate proper buffer sizes with alignment
// Update calculate_buffer_size to enforce proper alignment
static uint32_t calculate_buffer_size(uint32_t width, uint32_t height, uint32_t format) {
    // For NV12 format
    if (format == V4L2_PIX_FMT_NV12) {
        // Align width to 64 bytes for hardware requirements
        uint32_t aligned_width = ALIGN(width, 64);
        uint32_t y_size = aligned_width * ALIGN(height, 2);
        uint32_t uv_size = aligned_width * (ALIGN(height, 2) / 2);

        // Page align the final size
        return ALIGN(y_size + uv_size, PAGE_SIZE);
    }
    return ALIGN(width * height * 2, PAGE_SIZE);
}

static int setup_dma_buffer(struct isp_channel *chn)
{
    size_t size_per_buffer;
    unsigned long offset;

    if (!chn || !ourISPdev) {
        pr_err("No device for buffer setup\n");
        return -EINVAL;
    }

    // Calculate size needed for NV12
    size_per_buffer = calculate_buffer_size(chn->width, chn->height, chn->fmt);
    chn->required_size = size_per_buffer;
    chn->buf_size = size_per_buffer;

    // Calculate stride for proper memory alignment
    chn->buffer_stride = ALIGN(chn->width, 16);  // 16-byte alignment for stride

    // Calculate channel offset
    offset = chn->channel_id * ALIGN(size_per_buffer * MAX_BUFFERS, PAGE_SIZE);
    chn->channel_offset = offset;

    // Set buffer base addresses
    chn->buf_base = ourISPdev->dma_buf + offset;
    chn->dma_addr = ourISPdev->rmem_addr + offset;

    // Make sure memory is page aligned
    if (((unsigned long)chn->buf_base & ~PAGE_MASK) != 0) {
        pr_err("Buffer base not page aligned: %p\n", chn->buf_base);
        return -EINVAL;
    }

    // Clear buffer area
    memset(chn->buf_base, 0, size_per_buffer);

    pr_info("Buffer mapped from rmem:\n"
            "  Virtual: %p\n"
            "  Physical: 0x%08x\n"
            "  Size: %u\n"
            "  Line stride: %u\n"
            "  Channel offset: 0x%x\n",
            chn->buf_base,
            (uint32_t)chn->dma_addr,
            chn->buf_size,
            chn->buffer_stride,
            chn->channel_offset);

    return 0;
}


static void frame_done(struct isp_channel *chn, struct video_buffer *vbuf)
{
    struct isp_channel_event *evt = chn->event;
    unsigned long flags;
    struct frame_group *group;

    if (!evt || !vbuf || !vbuf->meta) {
        pr_err("Invalid parameters in frame_done\n");
        return;
    }

    spin_lock_irqsave(&evt->lock, flags);

//    // Update buffer metadata
//    vbuf->meta->sequence = chn->sequence++;
//    vbuf->meta->timestamp = ktime_get_real_ns();
//    vbuf->meta->state = FRAME_STATE_DONE;

    // Call registered frame callback
    if (evt->cb.frame_cb)
        evt->cb.frame_cb(evt->cb.priv);

//    // Just validate the group pointer libimp provided
//    group = vbuf->meta->group_ptr;
//    if (group && group->handler == 0x9a654) {
//        // Group is valid - set done flag in update block
//        uint32_t *done_flag = (uint32_t *)((char *)group->update_block + 0x3c);
//        *done_flag = 1;
//    }

    // Add to done queue and wake waiters
    list_add_tail(&vbuf->list, &chn->queue->done_list);
    atomic_dec(&chn->queued_bufs);

    evt->event_count++;

    spin_unlock_irqrestore(&evt->lock, flags);

    wake_up_interruptible(&chn->queue->wait);
}



static int frame_thread(void *arg)
{
    struct frame_thread_data *thread_data = arg;
    struct isp_channel *chn = thread_data->chn;
    struct video_buffer *buf;
    unsigned long flags;

    pr_info("Frame thread starting for channel %d\n", chn->channel_id);

    atomic_set(&chn->thread_running, 1);
    wake_up_interruptible(&chn->queue->wait);

    while (!kthread_should_stop() && !atomic_read(&thread_data->should_stop)) {
        spin_lock_irqsave(&chn->queue->queue_lock, flags);

//        pr_info("Frame thread: ready=%d done=%d\n",
//                list_empty(&chn->queue->ready_list),
//                list_empty(&chn->queue->done_list));

        // check_csi_status(ourISPdev);

        if (!list_empty(&chn->queue->ready_list)) {
            buf = list_first_entry(&chn->queue->ready_list,
                                 struct video_buffer, list);

            if (!buf || !buf->meta) {
                spin_unlock_irqrestore(&chn->queue->queue_lock, flags);
                msleep(10);
                continue;
            }

            list_del_init(&buf->list);
            spin_unlock_irqrestore(&chn->queue->queue_lock, flags);

            // We already have the data in rmem - just mark it as done
            frame_done(chn, buf);

        } else {
            spin_unlock_irqrestore(&chn->queue->queue_lock, flags);
            msleep(10);
        }
    }

    atomic_set(&chn->thread_running, 0);
    wake_up_interruptible(&chn->queue->wait);
    return 0;
}

static int init_frame_thread(struct isp_channel *chn)
{
    struct frame_thread_data *thread_data;
    struct vbm_pool *pool;
    unsigned long pool_phys;

    if (!chn || !chn->pool.pool_ptr) {
        pr_err("Invalid channel state - no VBM pool\n");
        return -EINVAL;
    }

    // Convert userspace pointer to physical address
    pool_phys = (unsigned long)chn->pool.pool_ptr;

    // Map the VBM pool safely
    pool = ioremap(pool_phys,
                  sizeof(struct vbm_pool) +
                  MAX_BUFFERS * sizeof(struct vbm_frame));
    if (!pool) {
        pr_err("Failed to map VBM pool\n");
        return -ENOMEM;
    }

    // Initialize pool structure
    memset(pool, 0, sizeof(*pool));
    pool->user_id = 0x336ac;  // Store magic in user_id field
    pool->pool_id = chn->channel_id;
    pool->width = chn->width;
    pool->height = chn->height;
    pool->format = chn->fmt;
    pool->bound_chn = chn->channel_id;

    thread_data = kzalloc(sizeof(*thread_data), GFP_KERNEL);
    if (!thread_data) {
        iounmap(pool);
        return -ENOMEM;
    }

    thread_data->chn = chn;
    atomic_set(&thread_data->should_stop, 0);
    atomic_set(&thread_data->thread_running, 0);

    // Store mapped pool
    chn->mapped_vbm = pool;

    init_waitqueue_head(&chn->queue->wait);

    // Start the thread
    thread_data->task = kthread_run(frame_thread, thread_data,
                                  "frame-thread-%d", chn->channel_id);
    if (IS_ERR(thread_data->task)) {
        int ret = PTR_ERR(thread_data->task);
        iounmap(pool);
        kfree(thread_data);
        return ret;
    }

    chn->thread_data = thread_data;

    // Wait for thread startup
    wait_event_timeout(chn->queue->wait,
                      atomic_read(&thread_data->thread_running),
                      msecs_to_jiffies(1000));

    pr_info("Frame thread initialized for channel %d with VBM pool %p (mapped %p)\n",
            chn->channel_id, chn->pool.pool_ptr, pool);
    return 0;
}



void tx_vic_enable_irq(struct tx_isp_dev *dev)
{
    void __iomem *vic_base = ioremap(0x10023000, 0x1000);    // Interrupt controller
    void __iomem *isp_ctrl = ioremap(0x13300000, 0x10000);
    void __iomem *tuning = ioremap(0x133e0000, 0x10000);
    unsigned long flags;

    if (!vic_base || !isp_ctrl || !tuning) {
        dev_err(dev->dev, "Failed to map registers\n");
        goto cleanup;
    }

    spin_lock_irqsave(&dev->vic_dev->lock, flags);
    if (!dev->vic_dev->irq_enabled) {
        // VIC setup (same as before)
        writel(0x0, vic_base + 0x04);
        writel(0x0, vic_base + 0x08);
        writel(0x0, vic_base + 0x0c);
        wmb();
        writel(0x00000001, vic_base + 0x04);
        wmb();
        writel(0x00000001, vic_base + 0x0c);
        wmb();

        // ISP Control (same as before)
        writel(0x07800438, isp_ctrl + 0x04);
        writel(0xb5742249, isp_ctrl + 0x0c);
        wmb();
        writel(0x00000000, isp_ctrl + 0x08);
        wmb();
        writel(0x8fffffff, isp_ctrl + 0x30);
        wmb();

        // Configure Image Tuning interrupts
        writel(0x07800438, tuning + 0x04);  // IMR - Same as ISP Control
        writel(0x00000002, tuning + 0x0c);  // IMCR
        wmb();
        writel(0x00000000, tuning + 0x08);  // IMSR
        wmb();
        writel(0x00000001, tuning + 0x00);  // Set ISR bit explicitly
        wmb();

        dev->vic_dev->irq_enabled = 1;
    }
    spin_unlock_irqrestore(&dev->vic_dev->lock, flags);

    cleanup:
        if (vic_base)
            iounmap(vic_base);
    if (isp_ctrl)
        iounmap(isp_ctrl);
    if (tuning)
        iounmap(tuning);
}

void tx_vic_disable_irq(struct tx_isp_dev *dev)
{
    void __iomem *isp_ctrl = ioremap(0x13300000, 0x10000);
    unsigned long flags;

    if (!isp_ctrl) {
        dev_err(dev->dev, "Failed to map registers\n");
        goto cleanup;
    }

    spin_lock_irqsave(&dev->vic_dev->lock, flags);
    if (dev->vic_dev->irq_enabled) {

        // Disable ISP Control interrupts
        writel(0x00000000, isp_ctrl + 0x04);  // Clear IMR
        wmb();

        dev->vic_dev->irq_enabled = 0;
    }
    spin_unlock_irqrestore(&dev->vic_dev->lock, flags);

    cleanup:
        if (isp_ctrl)
            iounmap(isp_ctrl);
}



int vic_core_s_stream(struct tx_isp_dev *dev, int enable)
{
    struct vic_device *vic = dev->vic_dev;
    int ret = -EINVAL;

    if (enable == 0) {
        if (vic->state == VIC_STATE_RUNNING) {
            vic->state = VIC_STATE_STOPPING;
            ret = 0;
        }
    } else {
        if (vic->state != VIC_STATE_RUNNING) {
            ret = tx_isp_vic_start(dev);
            if (ret == 0) {
                vic->state = VIC_STATE_RUNNING;
            }
        }
    }

    return ret;
}


int tx_isp_video_s_stream(struct tx_isp_dev *dev)
{
    int ret;

    pr_info("Setting up video link stream\n");

    if (dev->sensor_sd) {
        pr_info("sensor_sd exists\n");
        if (dev->sensor_sd->ops) {
            pr_info("sensor_sd->ops exists\n");
            if (dev->sensor_sd->ops->video) {
                pr_info("sensor_sd->ops->video exists\n");
            } else {
                pr_err("sensor_sd->ops->video is NULL\n");
            }
        } else {
            pr_err("sensor_sd->ops is NULL\n");
        }
    } else {
        pr_err("sensor_sd is NULL\n");
    }

    // Phase 1: Core initialization
    pr_info("Phase 1: Core initialization\n");

    if (dev->sensor_sd && dev->sensor_sd->ops && dev->sensor_sd->ops->core) {
        pr_info("Initializing sensor core\n");
        ret = dev->sensor_sd->ops->core->init(dev->sensor_sd, 1);
        if (ret && ret != 0xfffffdfd) {
            pr_err("Failed to initialize sensor core: %d\n", ret);
            return ret;
        }
    }

    // Phase 2: Stream enabling

    pr_info("Phase 2: Enabling video streams\n");

    if (dev->sensor_sd) {
        pr_info("sensor_sd exists\n");
        if (dev->sensor_sd->ops) {
            pr_info("sensor_sd->ops exists\n");
            if (dev->sensor_sd->ops->video) {
                pr_info("sensor_sd->ops->video exists\n");
            } else {
                pr_err("sensor_sd->ops->video is NULL\n");
            }
        } else {
            pr_err("sensor_sd->ops is NULL\n");
        }
    } else {
        pr_err("sensor_sd is NULL\n");
    }

    if (dev->sensor_sd && dev->sensor_sd->ops && dev->sensor_sd->ops->video) {
        pr_info("Initializing sensor stream on\n");
        ret = dev->sensor_sd->ops->video->s_stream(dev->sensor_sd, 1);
        if (ret && ret != 0xfffffdfd) {
            pr_err("Failed to initialize sensor stream on: %d\n", ret);
            return ret;
        }
    }

    return 0;
}


/**
 * tx_isp_unlocked_ioctl - IOCTL handler for ISP driver
 * @file: File structure
 * @cmd: IOCTL command
 * @arg: Command argument
 */
static long tx_isp_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    void __user *argp = (void __user *)arg;
    int ret = 0;
    struct tx_isp_module *module = file->private_data;
    struct tx_isp_subdev *sd = module_to_subdev(module);
    struct tx_isp_dev *isp_dev = ourISPdev;

    pr_info("ISP IOCTL called: cmd=0x%x\n", cmd);
    pr_info("\n=== IOCTL Debug ===\n");
    pr_info("cmd=0x%x arg=0x%lx\n", cmd, arg);
    pr_info("file=%p flags=0x%x\n",
            file, file->f_flags);

    switch (cmd) {
    case 0x800856d7: { /* WDR buffer get */
        uint32_t buf_size = 0;
        u32 mode;

        /* Get parent device pointer to access core sd */
        struct tx_isp_subdev *core_sd = platform_get_drvdata(isp_dev->core_pdev);

        /* Check WDR mode through core subdev registers */
        mode = tx_isp_sd_readl(core_sd, ISP_STATUS);
        mode &= BIT(3); /* WDR mode bit */

        if (mode) {
            /* Get dimensions from sensor video structure */
            struct tx_isp_video_in *video = &sd->active_sensor->video;
            buf_size = (video->vi_max_width * video->vi_max_height * 3) / 2;
        } else {
            isp_printf(1, "Not in WDR mode\n");
            return -EINVAL;
        }

        if (copy_to_user(argp, &buf_size, sizeof(buf_size)))
            return -EFAULT;

        return 0;
    }
    case 0x800456d0: { /* Video link setup */
        uint32_t link_config;

        if (copy_from_user(&link_config, argp, sizeof(link_config)))
            return -EFAULT;

        /* Validate link config */
        if (link_config >= 2) {
            isp_printf(2, "Invalid link configuration\n");
            return -EINVAL;
        }

        /* Store current link in isp device */
        isp_dev->active_link = link_config;

        return 0;
    }

    case 0x800856d4: { /* Buffer setup */
//        struct isp_buf_info buf_info;
//
//        if (copy_from_user(&buf_info, argp, sizeof(buf_info)))
//            return -EFAULT;
//
//        /* Get dimensions from sensor video structure */
//        struct tx_isp_video_in *video = &sd->active_sensor->video;
//        uint32_t stride = ALIGN(video->vi_max_width, 8);
//        uint32_t size = stride * video->vi_max_height;
//
//        /* Configure DMA registers through VIC subdev */
//        struct tx_isp_subdev *vic_sd = platform_get_drvdata(isp_dev->vic_pdev);
//
//        tx_isp_sd_writel(vic_sd, VIC_BUFFER_ADDR, buf_info.paddr);
//        tx_isp_sd_writel(vic_sd, VIC_FRAME_SIZE, size);
//
//        /* Enable DMA */
//        tx_isp_sd_writel(vic_sd, 0x7838, 0);
//        tx_isp_sd_writel(vic_sd, 0x783c, 1);
        struct imp_buffer_info local_buf_info;

        pr_info("Handling tx_isp_set_buf additional setup (0x800856d4)\n");

        if (copy_from_user(&local_buf_info, (void __user *)arg, 8)) {
            pr_err("Failed to copy buffer info from user\n");
            return -EFAULT;
        }

//        // Calculate stride - aligned to 8 bytes ((width + 7) >> 3) << 3
//        stride = ALIGN(ourISPdev->width, 8);
//        size = stride * ourISPdev->height;
//
//        // Setup DMA registers
//        writel(local_buf_info.phys_addr, isp_regs + 0x7820);  // Base address
//        writel(stride, isp_regs + 0x7824);                    // Stride
//        writel(size + local_buf_info.phys_addr, isp_regs + 0x7828); // Next buffer
//        writel(stride, isp_regs + 0x782c);                    // Next stride
//
//        // Setup secondary buffers
//        writel(size + local_buf_info.phys_addr, isp_regs + 0x7830);
//        writel(((ALIGN(ourISPdev->width + 0x1f, 32) + 7) >> 3) << 3, isp_regs + 0x7834);

//        // Configure additional buffer regions
//        if (isp_memopt) {
//            writel(size + local_buf_info.phys_addr, isp_regs + 0x7840);
//            writel(0, isp_regs + 0x7844);
//            writel(size + local_buf_info.phys_addr, isp_regs + 0x7848);
//            writel(0, isp_regs + 0x784c);
//            writel(size + local_buf_info.phys_addr, isp_regs + 0x7850);
//            writel(0, isp_regs + 0x7854);
//        } else {
//            // Configure with stride for each region
//            uint32_t extra_stride = ((ALIGN(ourISPdev->width, 32) + 7) >> 3) << 3;
//            uint32_t extra_size = ((ALIGN(ourISPdev->height + 0xf, 16) + 1) * extra_stride);
//            uint32_t offset = extra_size;
//
//            writel(size + offset + local_buf_info.phys_addr, isp_regs + 0x7840);
//            writel(extra_stride, isp_regs + 0x7844);
//
//            offset *= 2;
//            writel(size + local_buf_info.phys_addr + offset, isp_regs + 0x7848);
//            writel(extra_stride, isp_regs + 0x784c);
//
//            offset += extra_size;
//            writel(size + local_buf_info.phys_addr + offset, isp_regs + 0x7850);
//            writel(extra_stride, isp_regs + 0x7854);
//        }

//        // Final configuration
//        writel(0, isp_regs + 0x7838);
//        writel(1, isp_regs + 0x783c);

        pr_info("TODO ISP DMA registers configured\n");

        return 0;
    }

    case 0x800456d8: { /* Enable WDR */
        struct tx_isp_subdev *core_sd = platform_get_drvdata(isp_dev->core_pdev);
        u32 ctrl;

        /* Set WDR enable bit in control register */
        ctrl = tx_isp_sd_readl(core_sd, ISP_CTRL);
        ctrl |= ISP_CTRL_WDR_EN;
        tx_isp_sd_writel(core_sd, ISP_CTRL, ctrl);

        /* Configure sensor if needed */
        if (sd->active_sensor && sd->active_sensor->attr.sensor_ctrl.alloc_dgain) {
            ret = tx_isp_send_event_to_remote(sd->inpads, 0x2000013, NULL);
            if (ret && ret != -0x203)
                return ret;
        }

        return 0;
    }

    case 0x800456d9: { /* Disable WDR */
        struct tx_isp_subdev *core_sd = platform_get_drvdata(isp_dev->core_pdev);
        u32 ctrl;

        /* Clear WDR enable bit */
        ctrl = tx_isp_sd_readl(core_sd, ISP_CTRL);
        ctrl &= ~ISP_CTRL_WDR_EN;
        tx_isp_sd_writel(core_sd, ISP_CTRL, ctrl);

        /* Configure sensor if needed */
        if (sd->active_sensor && sd->active_sensor->attr.sensor_ctrl.alloc_dgain) {
            ret = tx_isp_send_event_to_remote(sd->inpads, 0x2000013, NULL);
            if (ret && ret != -0x203)
                return ret;
        }

        return 0;
    }

    case 0xc0385650: { /* Get sensor register */
        struct tx_isp_dbg_register reg;

        if (copy_from_user(&reg, argp, sizeof(reg)))
            return -EFAULT;

        /* Use event system to access sensor */
        ret = tx_isp_send_event_to_remote(sd->inpads, cmd, &reg);
        if (ret && ret != -0x203)
            return ret;

        if (copy_to_user(argp, &reg, sizeof(reg)))
            return -EFAULT;

        return 0;
    }

    case 0x8038564f: { /* Set sensor register */
        struct tx_isp_dbg_register reg;

        if (copy_from_user(&reg, argp, sizeof(reg)))
            return -EFAULT;

        /* Use event system to access sensor */
        ret = tx_isp_send_event_to_remote(sd->inpads, cmd, &reg);
        if (ret && ret != -0x203)
            return ret;

        return 0;
    }

    case 0x805056c1: { /* TX_ISP_SENSOR_REGISTER */
        ret = handle_sensor_register(sd);
        return ret;
    }

    case 0xc050561a: { /* TX_ISP_SENSOR_ENUM_INPUT */
    struct sensor_list_req {
          int idx;    // Input index
          char name[32];  // Output name
      } __attribute__((packed));

      struct sensor_list_req req;

      // Get the request struct from userspace
      if (copy_from_user(&req, (void __user *)arg, sizeof(req))) {
          pr_err("Failed to copy request from user\n");
          return -EFAULT;
      }

      // Check if index is valid
      if (req.idx >= MAX_SENSORS) {
          return -EINVAL;
      }

      detect_sensor_type(isp_dev);

      // Fill in the name for this index
      snprintf(req.name, sizeof(req.name), isp_dev->sensor_name);

      // Copy the result back to userspace
      if (copy_to_user((void __user *)arg, &req, sizeof(req))) {
          pr_err("Failed to copy result to user\n");
          return -EFAULT;
      }

      pr_info("Provided sensor info for index %d: %s\n", req.idx, req.name);
      return 0;
  }
    case 0xc0045627: {
        int result_val;
        pr_info("user space notifying us of sensor name we sent it\n");
        return 0;
    }
 case 0x800856d5: { // TX_ISP_SET_BUF 0x800856d5
    struct imp_buffer_info local_buf_info;
    struct isp_channel *chn;
    size_t frame_size;
    u8 *vbm_entry;
    int ret;

    pr_info("Handling TX_ISP_SET_BUF\n");

    if (!ourISPdev) {
        pr_err("ISP device not initialized\n");
        return -EINVAL;
    }

    // Initialize channel and queue first
    chn = &ourISPdev->channels[0];
    if (!chn->queue) {
        pr_info("Initializing queue for channel 0\n");
        chn->queue = kzalloc(sizeof(struct frame_queue), GFP_KERNEL);
        if (!chn->queue) {
            pr_err("Failed to allocate frame queue\n");
            return -ENOMEM;
        }
        mutex_init(&chn->queue->lock);
        chn->queue->stream_state = 0;
    }

    if (!arg || (unsigned long)arg >= 0xfffff001) {
        pr_err("Invalid user pointer\n");
        return -EFAULT;
    }

    if (copy_from_user(&local_buf_info, (void __user *)arg, sizeof(local_buf_info))) {
        pr_err("Failed to copy from user\n");
        return -EFAULT;
    }

    pr_info("Buffer info: method=0x%x phys=0x%llx size=%zu\n",
            local_buf_info.method, local_buf_info.phys_addr,
            local_buf_info.size);

    // First phase - handshake with userspace
    if (local_buf_info.method) {
        local_buf_info.method = ISP_ALLOC_DMA;
        local_buf_info.phys_addr = 0x1;
        local_buf_info.size = 0x1;
        local_buf_info.virt_addr = (unsigned long)ourISPdev->dma_buf;
        local_buf_info.flags = 0;

        pr_info("First phase complete - indicated DMA support\n");

        if (copy_to_user((void __user *)arg, &local_buf_info, sizeof(local_buf_info)))
            return -EFAULT;
        return 0;
    }

    // Second phase - actual buffer setup
    if (local_buf_info.phys_addr == 0x1) {
        // Calculate frame size
        frame_size = ALIGN((chn->width * chn->height * 3) / 2, 4096);

        // Setup DMA buffer if not already done
        if (!chn->dma_addr) {
            ret = setup_dma_buffer(chn);
            if (ret) {
                pr_err("Failed to setup DMA buffer: %d\n", ret);
                return ret;
            }
        }

        // Create VBM entry for compatibility
        vbm_entry = kzalloc(0x80, GFP_KERNEL);
        if (!vbm_entry) {
            pr_err("Failed to allocate VBM entry\n");
            return -ENOMEM;
        }

        // Setup VBM entry
        struct vbm_entry_flags {
            u32 state;
            u32 ready;
            u64 sequence;
            u64 phys_addr;
            void *virt;
            u32 size;
        } __packed;

        struct vbm_entry_flags *flags = (void *)(vbm_entry + 0x48);
        flags->state = 0;
        flags->ready = 1;
        flags->sequence = 0;
        flags->phys_addr = chn->dma_addr;
        flags->virt = chn->buf_base;
        flags->size = frame_size;

        // Store entry
        if (!chn->vbm_table) {
            chn->vbm_table = kzalloc(sizeof(void *), GFP_KERNEL);
            if (!chn->vbm_table) {
                pr_err("Failed to allocate VBM table\n");
                kfree(vbm_entry);
                return -ENOMEM;
            }
        }
        chn->vbm_table[0] = vbm_entry;

        // Set proper state flags
        chn->state = 3;  // Ready for streaming
        chn->state |= BIT(0);  // Has buffer

        // Return buffer info to userspace
        local_buf_info.method = ISP_ALLOC_DMA;
        local_buf_info.phys_addr = chn->dma_addr;
        local_buf_info.size = frame_size;
        local_buf_info.virt_addr = (unsigned long)chn->buf_base;
        local_buf_info.flags = 0;

        if (copy_to_user((void __user *)arg, &local_buf_info, sizeof(local_buf_info))) {
            pr_err("Failed to copy buffer info to user\n");
            kfree(vbm_entry);
            return -EFAULT;
        }

        pr_info("Buffer setup complete: phys=%pad virt=%p size=%zu state=%d\n",
                &chn->dma_addr, chn->buf_base, frame_size, chn->state);
        return 0;
    }

    pr_err("Invalid buffer setup sequence\n");
    return -EINVAL;
}
    case 0x805056c2: { /* Release sensor */
        struct tx_isp_sensor_register_info reg_info;

        if (copy_from_user(&reg_info, argp, sizeof(reg_info)))
            return -EFAULT;

        /* Use event system for sensor release */
        ret = tx_isp_send_event_to_remote(sd->inpads, TX_ISP_EVENT_SENSOR_RELEASE, &reg_info);
        if (ret && ret != -0x203)
            return ret;

        return 0;
    }
    case 0x40045626: {  // VIDIOC_GET_SENSOR_INFO
        pr_info("ISP IOCTL called: cmd=VIDIOC_GET_SENSOR_INFO\n");
        int __user *result = (int __user *)arg;

        // Check if the sensor is initialized
        if (!isp_dev || !isp_dev->sensor_i2c_client) {
            pr_err("Sensor not initialized\n");
            return -ENODEV;
        }

        if (isp_dev->sensor_sd && isp_dev->sensor_sd->ops && isp_dev->sensor_sd->ops->core) {
            pr_info("Initializing sensor core\n");
            ret = isp_dev->sensor_sd->ops->core->g_chip_ident(isp_dev->sensor_sd, isp_dev->chip);
            if (ret && ret != 0xfffffdfd)
                return ret;
        }

        // Write back 1 (not -1) to indicate sensor is present
        if (put_user(1, result)) {
            pr_err("Failed to update sensor result\n");
            return -EFAULT;
        }

        pr_info("Sensor info request: returning success (1)\n");
        return 0;
    }
    case 0x80045612: { // VIDIOC_STREAMON for sensor
//        void __iomem *vic_base = ioremap(0x10023000, 0x1000);  // VIC base from docs
//        int ret;
//
//        pr_info("Starting video stream on sensor\n");
//
//        if (!vic_base) {
//            pr_err("Failed to map VIC registers\n");
//            return -ENOMEM;
//        }

        // 1. Configure VIC/DMA
//        ret = vic_core_s_stream(isp_dev, 1);
//        if (ret) {
//            pr_err("Failed to configure VIC: %d\n", ret);
//            iounmap(vic_base);
//            goto err_cleanup;
//        }

        // 2. Configure interrupts
        //tx_vic_enable_irq(isp_dev);
//        pr_info("VIC interrupts configured:\n");
//        pr_info("  GPIO INT: 0x%08x\n", readl(vic_base + 0x00));
//        pr_info("  GPIO MASK: 0x%08x\n", readl(vic_base + 0x04));
//        pr_info("  INTC MASK: 0x%08x\n", readl(vic_base + 0x08));
//        pr_info("  INTC TYPE: 0x%08x\n", readl(vic_base + 0x0c));
//
//
//        // 3. Enable sensor test pattern and start streaming
//        ret = tx_isp_video_s_stream(isp_dev);
//        if (ret && ret != 0xfffffdfd) {
//            pr_err("Failed to start video stream: %d\n", ret);
//            iounmap(vic_base);
//            goto err_disable_vic;
//        }
//
//        iounmap(vic_base);
//        pr_info("Video streaming started successfully\n");
        return 0;

//        err_disable_vic:
//            vic_core_s_stream(isp_dev, 0);
//        tx_vic_disable_irq(isp_dev);
//        err_cleanup:
//            iounmap(vic_base);
//        return ret;
    }
case 0x800456d2: { // VIDIOC_ENABLE_STREAM
    pr_info("=== IOCTL Debug ===\n");
    pr_info("cmd=0x%x arg=0x%lx\n", cmd, arg);
    pr_info("file=%p flags=0x%x\n", file, file->f_flags);
    pr_info("private_data=%p\n", file->private_data);

    // Before enabling stream
    pr_info("Enabling video link stream\n");

    // You might want to verify device state here
    if (!file->private_data) {
        pr_err("No private data found\n");
        return -EINVAL;
    }

    // For now, just return success without actually enabling
    pr_info("Video link stream enabled\n");
    return 0;
}
    default:
        pr_info("Unhandled ioctl cmd: 0x%x\n", cmd);
        return -ENOTTY;
    }

    return ret;
}
int tx_isp_open(struct inode *inode, struct file *file)
{
    struct tx_isp_dev *isp = ourISPdev;  // Get from global like old code
    struct tx_isp_subdev **sd;
    struct tx_isp_subdev **end;
    int ret = 0;

    if (!isp) {
        pr_err("ISP device not initialized\n");
        return -ENODEV;
    }

    /* Check if already opened */
    if (isp->refcnt) {
        isp->refcnt++;
        return 0;
    }

    /* Mark as open */
    isp->is_open = true;
    isp->instance = -1;

    /* Store dev in file private_data */
    file->private_data = isp;

    /* Iterate through subdevices */
    sd = &isp->sensor_sd;
    end = (struct tx_isp_subdev **)((char *)isp + 0x6c);

    while (sd < end) {
        if (*sd) {
            struct tx_isp_subdev_ops *ops = (*sd)->ops;
            if (ops && ops->core && ops->core->init) {
                ret = ops->core->init(*sd, 1);
                if (ret != 0 && ret != -0x203) {
                    break;
                }
                ret = -0x203;
            } else {
                ret = -0x203;
            }
        }
        sd++;
    }

    /* Return 0 if all devices returned -0x203 or NULL */
    if (ret == -0x203) {
        pr_info("ISP opened successfully\n");
        return 0;
    }

    return ret;
}


static int framechan_dqbuf(struct isp_channel *chn, unsigned long arg)
{
    struct v4l2_buffer v4l2_buf;
    struct video_buffer *vbuf;
    unsigned long flags;
    int ret;

    if (copy_from_user(&v4l2_buf, (void __user *)arg, sizeof(v4l2_buf)))
        return -EFAULT;

    // Count current buffers
    spin_lock_irqsave(&chn->queue->queue_lock, flags);
    int ready = 0, done = 0;
    struct video_buffer *temp;
    list_for_each_entry(temp, &chn->queue->ready_list, list) ready++;
    list_for_each_entry(temp, &chn->queue->done_list, list) done++;
    pr_info("Ch%d DQBUF start: ready=%d done=%d queued=%d\n",
            chn->channel_id, ready, done, atomic_read(&chn->queued_bufs));
    spin_unlock_irqrestore(&chn->queue->queue_lock, flags);

    // Wait for a completed frame
    ret = wait_event_interruptible_timeout(
        chn->queue->wait,
        !list_empty(&chn->queue->done_list) && (chn->queue->stream_state & 1),
        msecs_to_jiffies(1000));

    if (ret <= 0) {
        pr_err("Ch%d DQBUF timeout/error: %d\n", chn->channel_id, ret);
        return ret ? ret : -ETIMEDOUT;
    }

    spin_lock_irqsave(&chn->queue->queue_lock, flags);

    if (!list_empty(&chn->queue->done_list)) {
        vbuf = list_first_entry(&chn->queue->done_list,
                               struct video_buffer, list);

        // Remove from done list but don't reinit
        list_del(&vbuf->list);

        v4l2_buf.index = vbuf->index;
        v4l2_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        v4l2_buf.bytesused = chn->required_size;
        v4l2_buf.flags = vbuf->flags;
        v4l2_buf.field = V4L2_FIELD_NONE;
        v4l2_buf.memory = chn->queue->memory_type;
        v4l2_buf.length = chn->required_size;
        v4l2_buf.sequence = vbuf->meta->sequence;
        v4l2_buf.timestamp.tv_sec = vbuf->meta->timestamp / 1000000;
        v4l2_buf.timestamp.tv_usec = vbuf->meta->timestamp % 1000000;

        // For mmap, set the offset
        if (chn->queue->memory_type == V4L2_MEMORY_MMAP) {
            v4l2_buf.m.offset = vbuf->index * chn->required_size;
        }

        // Update buffer tracking
        vbuf->flags &= ~V4L2_BUF_FLAG_QUEUED;
        atomic_dec(&chn->queued_bufs);
        //dump_csi_reg(ourISPdev);
        pr_info("Ch%d DQBUF: returned buffer %d seq=%d\n",
                chn->channel_id, vbuf->index, v4l2_buf.sequence);

        ret = 0;
    } else {
        ret = -EAGAIN;
    }

    spin_unlock_irqrestore(&chn->queue->queue_lock, flags);

    if (ret == 0) {
        if (copy_to_user((void __user *)arg, &v4l2_buf, sizeof(v4l2_buf)))
            return -EFAULT;
    }

    return ret;
}

static int frame_channel_vidioc_fmt(struct isp_channel *chn, void __user *arg)
{
    struct frame_fmt fmt;
    size_t buf_size;
    int aligned_width;

    pr_info("Format entry\n");
    if (copy_from_user(&fmt, arg, sizeof(fmt)))
        return -EFAULT;

    // Setting format
    aligned_width = (fmt.width + 15) & ~15;  // 16-byte aligned width

    // Determine format from input first
    if (strncmp(fmt.pix_str, "YU12", 4) == 0) {
        chn->fmt = 0x22;  // YUV422
    } else if (strncmp(fmt.pix_str, "NV12", 4) == 0) {
        chn->fmt = 0x23;  // NV12
    } else if (strncmp(fmt.pix_str, "RAW", 3) == 0) {
        chn->fmt = 0xf;   // RAW
    } else {
        pr_err("Unsupported pixel format: %.4s\n", fmt.pix_str);
        return -EINVAL;
    }

    // Calculate buffer size based on format
    if (chn->fmt == 0x23) {  // NV12
        aligned_width = (fmt.width + 15) & ~15;  // 16-byte aligned
        fmt.bytesperline = aligned_width;

        // Base NV12 size (Y + UV/2) plus per-line metadata
        buf_size = (aligned_width * fmt.height * 3/2) + (fmt.width * 12);
    } else if (chn->fmt == 0x22) {  // YUV422
        aligned_width = (fmt.width + 15) & ~15;
        fmt.bytesperline = aligned_width * 2;
        buf_size = (aligned_width * fmt.height * 2) + (fmt.width * 12);
    } else if (chn->fmt == 0xf) {  // RAW
        aligned_width = (fmt.width + 15) & ~15;
        fmt.bytesperline = aligned_width;
        buf_size = (aligned_width * fmt.height) + (fmt.width * 12);
    }

    pr_info("Size calculation for channel %d: base=%zu metadata=%zu total=%zu\n",
            chn->channel_id,
            (aligned_width * fmt.height * 3/2),
            (fmt.width * 12),
            buf_size);


    // Set all frame source state attributes
    chn->width = fmt.width;
    chn->height = fmt.height;
    chn->fmt = chn->fmt;  // Keep existing format

    // Always initialize ALL fields of attr
    chn->attr.width = fmt.width;
    chn->attr.height = fmt.height;
    chn->attr.format = chn->fmt;
    chn->attr.picwidth = fmt.width;
    chn->attr.picheight = fmt.height;
    chn->attr.crop_enable = 0;
    chn->attr.crop.x = 0;
    chn->attr.crop.y = 0;
    chn->attr.crop.width = fmt.width;
    chn->attr.crop.height = fmt.height;
    chn->attr.scaler_enable = 0;
    chn->attr.scaler_outwidth = fmt.width;
    chn->attr.scaler_outheight = fmt.height;
    chn->attr.fps_num = 30;
    chn->attr.fps_den = 1;
    chn->attr.reserved[0] = 0;
    chn->attr.reserved[1] = 0;
    chn->attr.reserved[2] = 0;
    chn->attr.reserved[3] = 0;
    chn->attr.reserved[4] = 0;

    // Set detailed channel attributes
    chn->attr.enable = 1;
    chn->attr.width = fmt.width;
    chn->attr.height = fmt.height;
    chn->attr.format = chn->fmt;
    chn->attr.picwidth = fmt.width;
    chn->attr.picheight = fmt.height;

    // Initialize crop and scaling to disabled
    chn->attr.crop_enable = 0;
    chn->attr.crop.x = 0;
    chn->attr.crop.y = 0;
    chn->attr.crop.width = fmt.width;
    chn->attr.crop.height = fmt.height;
    chn->attr.scaler_enable = 0;
    chn->attr.scaler_outwidth = fmt.width;
    chn->attr.scaler_outheight = fmt.height;

    // Default frame rate
    chn->attr.fps_num = 30;
    chn->attr.fps_den = 1;

    // Store in our channel structure
    chn->width = fmt.width;
    chn->height = fmt.height;
    chn->fmt = chn->fmt;
    chn->required_size = buf_size;

    // Set states
    chn->flags |= FS_ATTR_SET;
    chn->state |= FRAME_FMT_SET;

    fmt.sizeimage = buf_size;

    pr_info("Updated format for channel %d:\n", chn->channel_id);
    pr_info("  Width: %d (aligned: %d)\n", fmt.width, aligned_width);
    pr_info("  Height: %d\n", fmt.height);
    pr_info("  Final size: %zu\n", buf_size);
    pr_info("  Stride: %d\n", fmt.bytesperline);

    if (copy_to_user(arg, &fmt, sizeof(fmt)))
        return -EFAULT;

    return 0;
}


static int framechan_open(struct inode *inode, struct file *file)
{
    int channel = iminor(inode);
    struct tx_isp_dev *dev = ourISPdev;
    struct isp_channel *chn = &dev->channels[channel];
    int ret = 0;
    uint32_t base_offset = 0x1094d4;

    pr_info("Frame channel open\n");

    // Validate channel number
    if (channel >= MAX_CHANNELS) {
        pr_err("Invalid channel number %d\n", channel);
        return -EINVAL;
    }

    // Allocate queue if not already done
    if (!chn->queue) {
        chn->queue = kzalloc(sizeof(struct frame_queue), GFP_KERNEL);
        if (!chn->queue) {
            pr_err("Failed to allocate queue for channel %d\n", channel);
            return -ENOMEM;
        }
    }

    pr_info("Frame channel open: channel=%d\n", channel);

    // Initialize basic state
    mutex_init(&chn->queue->lock);
    init_waitqueue_head(&chn->queue->wait);

    // Initialize atomic variables
    atomic_set(&chn->thread_running, 0);
    atomic_set(&chn->thread_should_stop, 0);

    // Initialize channel properties with defaults
    chn->channel_id = channel;
    chn->width = (channel == 0) ? 1920 : 640;
    chn->height = (channel == 0) ? 1080 : 360;
    chn->fmt = ISP_FMT_NV12;
    chn->is_open = 1;

    // Set the device pointer for DMA operations
    chn->dev = dev->dev;

    pr_info("Frame channel open II: channel=%d\n", channel);

    // Initialize lists in queue
    INIT_LIST_HEAD(&chn->queue->ready_list);
    INIT_LIST_HEAD(&chn->queue->done_list);
    spin_lock_init(&chn->queue->queue_lock);

    // Start in state 3 (ready for streaming)
    chn->state = 3;
    chn->queue->stream_state = 0;

    pr_info("Frame channel open III: channel=%d\n", channel);

//    // Initialize pad for event handling
//    chn->pad.sd = dev->fs_dev->sd;
//    chn->pad.type = TX_ISP_PADTYPE_OUTPUT;
//    chn->pad.index = channel;
//    init_pad_event_handling(&chn->pad, fs_pad_event_handler);

    uint32_t frame_offset = ALIGN(channel * chn->buf_size * chn->buffer_count, 32);
    dma_addr_t buffer_start = ALIGN(dev->dma_addr + base_offset, 32);

    chn->buf_base = (void *)dev->dma_addr + base_offset + frame_offset;
    chn->dma_addr = buffer_start + frame_offset;

    file->private_data = chn;

    pr_info("Framechan%d opened successfully\n", channel);

    return 0;
}

static int framechan_check_status(struct isp_channel *chn, unsigned long arg)
{
    int status = 0;

    // Add streaming state
    if (chn->state & BIT(0))
        status |= BIT(0);  // Streaming active bit

    if (copy_to_user((void __user *)arg, &status, sizeof(status)))
        return -EFAULT;

    return 0;
}

static int framechan_qbuf(struct isp_channel *chn, unsigned long arg)
{
    struct v4l2_buffer v4l2_buf;
    struct video_buffer *vbuf;
    unsigned long flags;
    int ret = 0;

    if (copy_from_user(&v4l2_buf, (void __user *)arg, sizeof(v4l2_buf)))
        return -EFAULT;

    if (v4l2_buf.index >= chn->queue->buffer_count) {
        pr_err("Invalid buffer index %d\n", v4l2_buf.index);
        return -EINVAL;
    }

    vbuf = chn->queue->bufs[v4l2_buf.index];
    if (!vbuf) {
        pr_err("NULL buffer at index %d\n", v4l2_buf.index);
        return -EINVAL;
    }

    spin_lock_irqsave(&chn->queue->queue_lock, flags);

    // Check if buffer is already queued
    if (vbuf->flags & V4L2_BUF_FLAG_QUEUED) {
        pr_err("Buffer %d already queued\n", vbuf->index);
        ret = -EINVAL;
        goto out_unlock;
    }

    // Store userspace buffer pointer from v4l2_buf
    if (v4l2_buf.memory == V4L2_MEMORY_USERPTR) {
        vbuf->data = (void *)v4l2_buf.m.userptr;
    } else {
        vbuf->data = (void *)(unsigned long)(v4l2_buf.m.offset + chn->group_offset);
    }

    // Keep metadata from v4l2_buf
    if (vbuf->meta) {
        vbuf->meta->size = v4l2_buf.length;
        vbuf->meta->state = FRAME_STATE_QUEUED;
        vbuf->meta->done_flag = 0;
    }

    // Add to ready list
    vbuf->flags = V4L2_BUF_FLAG_QUEUED;
    list_add_tail(&vbuf->list, &chn->queue->ready_list);
    atomic_inc(&chn->queued_bufs);
    //dump_csi_reg(ourISPdev);
    pr_info("Ch%d QBUF: buffer %d queued (total=%d)\n",
            chn->channel_id, vbuf->index,
            atomic_read(&chn->queued_bufs));
//    pr_info("  userptr=%lx offset=%x data=%p\n",
//            v4l2_buf.m.userptr, v4l2_buf.m.offset, vbuf->data);

    out_unlock:
        spin_unlock_irqrestore(&chn->queue->queue_lock, flags);
    return ret;
}

struct frame_header {
    uint32_t magic;          // 0x80
    uint32_t secondary;      // 0xe1
    uint32_t width_check;    // aligned_width/2
    uint32_t final_check;    // 0x147
} __attribute__((packed));


// Initialize frame group structure
static struct frame_group* init_frame_group(struct isp_channel *chn)
{
    struct frame_group *group;

    // Use channel's existing group if available
    if (chn->group)
        return chn->group;

    group = kzalloc(sizeof(struct frame_group), GFP_KERNEL);
    if (!group)
        return NULL;

    // Set required fields
    snprintf(group->name, sizeof(group->name), "group-ch%d", chn->channel_id);
    group->handler_fn = NULL;  // Set by userspace
    group->update_fn = group_update_wrapper;
    group->group_update = NULL; // Set by userspace
    group->self = group;  // Points to itself
    group->channel = chn->channel_id;
    group->state = 0x336000;  // Magic state value
    group->handler = 0x9a654;  // Required magic value

    // Store in channel
    chn->group = group;

    pr_info("Initialized frame group for channel %d\n", chn->channel_id);
    return group;
}

// Initialize frame metadata structure
static void init_frame_metadata(struct frame_metadata *meta,
                              struct isp_channel *chn,
                              int index)
{
    if (!meta || !chn)
        return;

    meta->magic = 0x100100;  // Required magic
    meta->frame_num = index;
    meta->ref_count = 1;
    meta->state = FRAME_STATE_FREE;
    meta->size = chn->required_size;
    meta->channel = chn->channel_id;
    meta->handler = (void *)0x9a654; // Required magic

    // Link to channel's frame group
    struct frame_group *group = init_frame_group(chn);
    if (group) {
        meta->group_ptr = group;
        meta->done_flag = 0;
    }

    pr_info("Initialized frame metadata for buffer %d\n", index);
}


static int framechan_reqbufs(struct isp_channel *chn, unsigned long arg)
{
    struct reqbuf_request req;
    struct frame_queue *queue;
    struct video_buffer **new_bufs = NULL;
    int ret = 0, i;
    unsigned long flags;
    void *y_virt = NULL, *uv_virt = NULL;
    dma_addr_t y_phys = 0, uv_phys = 0;
    void __iomem *isp_ctrl = NULL;

    if (!chn || !chn->queue) {
        pr_err("Invalid channel state\n");
        return -EINVAL;
    }

    queue = chn->queue;

    // Copy request struct from user
    if (copy_from_user(&req, (void __user *)arg, sizeof(req))) {
        pr_err("Failed to copy request from user\n");
        return -EFAULT;
    }

    mutex_lock(&queue->lock);
    spin_lock_irqsave(&queue->queue_lock, flags);

    // Cannot change buffers while streaming
    if (queue->stream_state & 1) {
        pr_err("Cannot change buffer count while streaming\n");
        ret = -EBUSY;
        goto out_unlock;
    }

    // Clean up existing buffers and DMA memory
    if (queue->bufs) {
        for (i = 0; i < queue->buffer_count; i++) {
            if (queue->bufs[i]) {
                if (!list_empty(&queue->bufs[i]->list))
                    list_del(&queue->bufs[i]->list);
                if (queue->bufs[i]->meta) {
                    if (queue->bufs[i]->meta->group_ptr)
                        kfree(queue->bufs[i]->meta->group_ptr);
                    kfree(queue->bufs[i]->meta);
                }
                kfree(queue->bufs[i]);
            }
        }
        kfree(queue->bufs);
        queue->bufs = NULL;
        queue->buffer_count = 0;
    }

    // Clean up any existing DMA buffers
    if (chn->dma_y_virt) {
        dma_free_coherent(chn->dev, chn->width * chn->height,
                         chn->dma_y_virt, chn->dma_y_phys);
        chn->dma_y_virt = NULL;
    }
    if (chn->dma_uv_virt) {
        dma_free_coherent(chn->dev, (chn->width * chn->height) / 2,
                         chn->dma_uv_virt, chn->dma_uv_phys);
        chn->dma_uv_virt = NULL;
    }

    // Store VBM info from request
    chn->pool.pool_ptr = req.pool_ptr;
    chn->pool.ctrl_ptr = req.ctrl_ptr;
    chn->pool.pool_flags = req.pool_flags;

    // Handle buffer deallocation request
    if (req.count == 0) {
        spin_unlock_irqrestore(&queue->queue_lock, flags);
        mutex_unlock(&queue->lock);
        return 0;
    }

    // Limit max buffers to 40 as seen in binary
    if (req.count > 40) {
        pr_warn("Limiting buffer count from %d to 40\n", req.count);
        req.count = 40;
    }

    // Allocate DMA buffers for NV12 format
    size_t y_size = chn->width * chn->height;
    size_t uv_size = y_size / 2;

    y_virt = dma_alloc_coherent(chn->dev, y_size, &y_phys, GFP_KERNEL);
    if (!y_virt) {
        pr_err("Failed to allocate Y plane DMA buffer\n");
        ret = -ENOMEM;
        goto out_unlock;
    }

    uv_virt = dma_alloc_coherent(chn->dev, uv_size, &uv_phys, GFP_KERNEL);
    if (!uv_virt) {
        pr_err("Failed to allocate UV plane DMA buffer\n");
        ret = -ENOMEM;
        goto out_free_y;
    }

    // Store DMA buffers in channel
    chn->dma_y_virt = y_virt;
    chn->dma_y_phys = y_phys;
    chn->dma_uv_virt = uv_virt;
    chn->dma_uv_phys = uv_phys;

    // Map ISP control registers and write DMA addresses
    isp_ctrl = ioremap(0x13300000, 0x10000);
    if (!isp_ctrl) {
        pr_err("Failed to map ISP control registers\n");
        ret = -ENOMEM;
        goto out_free_uv;
    }

    writel(y_phys, isp_ctrl + 0x008);   // Y plane physical address
    writel(uv_phys, isp_ctrl + 0x00c);  // UV plane physical address
    wmb();

    // Rest of buffer allocation
    new_bufs = kzalloc(sizeof(struct video_buffer *) * req.count, GFP_KERNEL);
    if (!new_bufs) {
        ret = -ENOMEM;
        goto out_unmap;
    }

    if (!chn->required_size) {
        chn->required_size = calculate_buffer_size(chn->width,
                                                 chn->height,
                                                 chn->fmt);
    }

    // Allocate and initialize each buffer
    for (i = 0; i < req.count; i++) {
        struct video_buffer *buf = kzalloc(sizeof(*buf), GFP_KERNEL);
        if (!buf) {
            ret = -ENOMEM;
            goto err_free_bufs;
        }

        buf->meta = kzalloc(sizeof(struct frame_metadata), GFP_KERNEL);
        if (!buf->meta) {
            kfree(buf);
            ret = -ENOMEM;
            goto err_free_bufs;
        }

        buf->index = i;
        buf->type = req.type;
        buf->memory = req.memory;
        buf->queue = queue;
        INIT_LIST_HEAD(&buf->list);

        buf->meta->magic = 0x100100;
        buf->meta->frame_num = i;
        buf->meta->state = FRAME_STATE_FREE;
        buf->meta->size = chn->required_size;

        struct group_module *group = kzalloc(sizeof(*group), GFP_KERNEL);
        if (!group) {
            kfree(buf->meta);
            kfree(buf);
            ret = -ENOMEM;
            goto err_free_bufs;
        }

        group->update_fn = group_update_wrapper;
        group->self = group;
        group->channel = chn->channel_id;
        group->handler = 0x9a654;
        buf->meta->group_ptr = group;

        new_bufs[i] = buf;
    }

    // Update queue
    queue->bufs = new_bufs;
    queue->buffer_count = req.count;
    queue->memory_type = req.memory;

    INIT_LIST_HEAD(&queue->ready_list);
    INIT_LIST_HEAD(&queue->done_list);

    spin_unlock_irqrestore(&queue->queue_lock, flags);
    mutex_unlock(&queue->lock);

    // Initialize events if needed
    if (!chn->event) {
        ret = init_channel_events(chn);
        if (ret)
            goto err_free_all;
    }

    // Send buffer request event
    if (chn && chn->pad.sd) {
        struct isp_buf_request evt_req = {
            .count = req.count,
            .size = chn->required_size
        };

        ret = tx_isp_send_event_to_remote(&chn->pad,
                                         ISP_EVENT_BUFFER_REQUEST,
                                         &evt_req);
        if (ret && ret != -ENOIOCTLCMD)
            goto err_cleanup_events;
    }

    if (isp_ctrl)
        iounmap(isp_ctrl);

    return 0;

err_cleanup_events:
    if (chn->event) {
        unregister_channel_callbacks(chn->event);
        kfree(chn->event);
        chn->event = NULL;
    }
err_free_all:
err_free_bufs:
    if (new_bufs) {
        for (i = 0; i < req.count; i++) {
            if (new_bufs[i]) {
                if (!list_empty(&new_bufs[i]->list))
                    list_del(&new_bufs[i]->list);
                if (new_bufs[i]->meta) {
                    if (new_bufs[i]->meta->group_ptr)
                        kfree(new_bufs[i]->meta->group_ptr);
                    kfree(new_bufs[i]->meta);
                }
                kfree(new_bufs[i]);
            }
        }
        kfree(new_bufs);
    }
out_unmap:
    if (isp_ctrl)
        iounmap(isp_ctrl);
out_free_uv:
    if (uv_virt)
        dma_free_coherent(chn->dev, uv_size, uv_virt, uv_phys);
out_free_y:
    if (y_virt)
        dma_free_coherent(chn->dev, y_size, y_virt, y_phys);
out_unlock:
    spin_unlock_irqrestore(&queue->queue_lock, flags);
    mutex_unlock(&queue->lock);
    return ret;
}

static int framechan_streamoff(struct isp_channel *chn, unsigned long arg)
{
    struct video_buffer *buf;
    uint32_t type;
    int i;
    unsigned long flags;

    if (!chn || !chn->queue) {
        pr_err("Invalid channel state\n");
        return -EINVAL;
    }

    if (copy_from_user(&type, (void __user *)arg, sizeof(type)))
        return -EFAULT;

    if (type != V4L2_BUF_TYPE_VIDEO_CAPTURE) {
        pr_err("Invalid buffer type\n");
        return -EINVAL;
    }

    mutex_lock(&chn->queue->lock);
    spin_lock_irqsave(&chn->queue->queue_lock, flags);

    // Clear streaming state first
    chn->queue->stream_state = 0;

    // Move all buffers from ready and done lists back to array
    list_for_each_entry(buf, &chn->queue->ready_list, list) {
        buf->flags = 0;
        buf->status = 0;
    }
    INIT_LIST_HEAD(&chn->queue->ready_list);

    list_for_each_entry(buf, &chn->queue->done_list, list) {
        buf->flags = 0;
        buf->status = 0;
    }
    INIT_LIST_HEAD(&chn->queue->done_list);

    spin_unlock_irqrestore(&chn->queue->queue_lock, flags);
    mutex_unlock(&chn->queue->lock);

    pr_info("Stream disabled for channel %d\n", chn->channel_id);
    return 0;
}

static int framechan_get_frame_status(struct isp_channel *chn, unsigned long arg)
{
    uint32_t frame_status = 0;
    int ready_count = 0, done_count = 0;
    struct video_buffer *buf;
    unsigned long flags;

    if (!chn || !chn->queue) {
        pr_err("Invalid channel state\n");
        return -EINVAL;
    }

    mutex_lock(&chn->queue->lock);
    spin_lock_irqsave(&chn->queue->queue_lock, flags);

    // Check streaming state
    if (chn->queue->stream_state & 1) {
        frame_status |= BIT(31); // Set streaming bit

        // Count ready buffers
        list_for_each_entry(buf, &chn->queue->ready_list, list) {
            ready_count++;
        }

        // Count done buffers
        list_for_each_entry(buf, &chn->queue->done_list, list) {
            done_count++;
        }

        frame_status |= (ready_count & 0xFFFF);    // Ready count in low bits
        if (done_count > 0)
            frame_status |= BIT(16);  // Frame done bit
    }

    spin_unlock_irqrestore(&chn->queue->queue_lock, flags);
    mutex_unlock(&chn->queue->lock);

    if (copy_to_user((void __user *)arg, &frame_status, sizeof(frame_status)))
        return -EFAULT;

    return 0;
}

static int handle_framechan_ioctl(struct tx_isp_dev *dev, unsigned int cmd, void __user *arg, struct file *file)
{
    int ret = 0;
    struct isp_channel *chn;

    if (!file || !file->private_data) {
        pr_err("No channel data in file\n");
        return -EINVAL;
    }

    chn = file->private_data;

    switch (cmd) {
    case 0x80045612: // VIDIOC_STREAMON  0x80045612
        pr_info("Framechan Streamon command: 0x%x\n", cmd);

        mutex_lock(&chn->queue->lock);

        // Check state must be 3
        if (chn->state != 3) {
            pr_err("Invalid channel state %d for streaming\n", chn->state);
            mutex_unlock(&chn->queue->lock);
            return -EINVAL;
        }

        // Check not already streaming
        if (chn->queue->stream_state & 1) {
            pr_err("streamon: already streaming\n");
            mutex_unlock(&chn->queue->lock);
            return -EAGAIN;
        }

        // Enable frame queue
        chn->queue->stream_state |= 1;
        chn->state = 4;

        // Start frame processing thread
        ret = init_frame_thread(chn);
        if (ret) {
            pr_err("Failed to start frame thread: %d\n", ret);
            chn->queue->stream_state &= ~1;
            chn->state = 3;
            mutex_unlock(&chn->queue->lock);
            return ret;
        }

        pr_info("Enabled streaming on channel %d\n", chn->channel_id);
        mutex_unlock(&chn->queue->lock);
        return 0;

    case VIDIOC_STREAMOFF:
        pr_info("Frame channel streamoff requested\n");
        mutex_lock(&chn->queue->lock);

        if (!(chn->queue->stream_state & 1)) {
            pr_info("Stream already off\n");
            mutex_unlock(&chn->queue->lock);
            return 0;
        }

        // Stop streaming
        chn->queue->stream_state &= ~1;
        chn->state = 3;

        // Stop frame thread
        if (chn->frame_thread) {
            kthread_stop(chn->frame_thread);
            chn->frame_thread = NULL;
        }

        mutex_unlock(&chn->queue->lock);
        return 0;

    // Handle other frame channel specific commands...

    default:
        pr_err("Unknown frame channel command: 0x%x\n", cmd);
        return -EINVAL;
    }

    return ret;
}



int enable_isp_streaming(struct tx_isp_dev *dev, struct file *file, int channel, bool enable)
{
    void __iomem *vic_regs = ioremap(0x10023000, 0x1000);
    void __iomem *isp_regs = ioremap(0x13300000, 0x10000);
    unsigned long flags;
    int ret = 0;

    if (enable) {
        spin_lock_irqsave(&dev->vic_dev->lock, flags);

        // Enable interrupts using known working registers
        writel(0xffffffff, isp_regs + 0xb0);  // IRQ Enable
        writel(0xffffffff, isp_regs + 0xb4);  // Clear any pending
        wmb();

        // Enable streaming
        writel(0x1, isp_regs + ISP_STREAM_START);
        wmb();
        writel(0x1, isp_regs + ISP_STREAM_CTRL);
        wmb();

        dev->irq_enabled = 1;  // Mark as enabled

        unlock:
                spin_unlock_irqrestore(&dev->vic_dev->lock, flags);
    } else {
        spin_lock_irqsave(&dev->vic_dev->lock, flags);

//        // Disable streaming first
//        writel(0x0, isp_regs + ISP_STREAM_CTRL);
//        writel(0x0, isp_regs + ISP_STREAM_START);
//        wmb();
//
//        // Then disable IRQs
//        tx_vic_disable_irq(dev);  // Disable VIC IRQs
//        tx_isp_disable_irq(dev);  // Disable ISP IRQs

        spin_unlock_irqrestore(&dev->vic_dev->lock, flags);
    }

    iounmap(vic_regs);
    iounmap(isp_regs);

    return ret;
}



/***
0xc07056c3 - VIDIOC_S_FMT - Set format
0xc0145608 - VIDIOC_REQBUFS - Request buffers
0xc044560f - VIDIOC_QBUF - Queue buffer
0x80045612 - VIDIOC_STREAMON - Start streaming
0x400456bf - Custom command for frame status/count
0xc0445611 - VIDIOC_DQBUF - Dequeue buffer
**/
static long framechan_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct isp_channel *chn = file->private_data;
    struct tx_isp_dev *dev = ourISPdev;
    int ret = 0;

    // pr_info("Frame channel IOCTL: cmd=0x%x arg=0x%lx\n", cmd, arg);

    if (!chn) {
        pr_err("No framesource state in IOCTL\n");
        return -EINVAL;
    }

    //mutex_lock(&chn->lock);

    switch (cmd) {
    case 0xc0145608: // VIDIOC_REQBUFS
        ret = framechan_reqbufs(chn, arg);
        break;
    case 0xc044560f: // VIDIOC_QBUF
        ret = framechan_qbuf(chn, arg);
        break;
    case 0xc0445611: // VIDIOC_DQBUF
        ret = framechan_dqbuf(chn, arg);
        break;
    case VIDIOC_STREAMOFF:
        pr_info("TODO Stream OFF requested\n");
        for (int i = 0; i < MAX_CHANNELS; i++) {
            struct isp_channel *chn = &dev->channels[i];
            if (!chn)
                continue;

            if (chn->state & BIT(1)) {
                chn->state &= ~BIT(1);
            }
        }
        return enable_isp_streaming(dev, file, 0, false);
    case 0x400456bf: {  // frame status handler
        ret = framechan_get_frame_status(chn, arg);
        break;
    }
    case 0xc07056c3: // VIDIOC_S_FMT
        pr_info("VIDIOC_S_FMT: arg=0x%lx\n", arg);
        // If type has high bit set, it's a GET operation
        ret = frame_channel_vidioc_fmt(chn, (void __user *)arg);
        break;
    case 0xc0505640: { // VIDIOC_SET_CHN_ATTR
        struct frame_chan_attr attr;
        void *dst;
        struct frame_fmt fmt;
        int ret;

        if (copy_from_user(&attr, (void __user *)arg, sizeof(attr)))
            return -EFAULT;

        // Validate format
        if (attr.format != 0x22 && (attr.format & 0xf) != 0) {
            pr_err("Invalid format type\n");
            return -EINVAL;
        }
        // TODO this was wrong
//        // Get the correct destination
//        dst = fs + 0x20;
//
//        // Ensure first 32-bit value is non-zero to mark attributes as set
//        if (attr.width == 0)
//            attr.width = 1920;  // Default if not set
//
//        // Copy exactly 0x50 bytes
//        memcpy(dst, &attr, 0x50);
//
//        // Mark attributes as set
//        chn->flags |= FS_ATTR_SET;
//
//        // Setup format structure based on attributes
//        memset(&fmt, 0, sizeof(fmt));
//        fmt.type = 1;
//        fmt.width = attr.width;
//        fmt.height = attr.height;
//        if (attr.format == 0x22) {
//            memcpy(fmt.pix_str, "YU12", 4);
//        } else {
//            memcpy(fmt.pix_str, "NV12", 4);
//        }
//        fmt.field = 0;
//        fmt.colorspace = 8;
//
//        // Update format settings
//        ret = frame_channel_vidioc_fmt(cnh, &fmt, false);
//        if (ret)
//            return ret;

        // Copy back to userspace
        if (copy_to_user((void __user *)arg, &attr, sizeof(attr)))
            pr_err("Failed to copy attr to user\n");
            return -EFAULT;

        break;
    }
    case 0x80045612 : // VIDIOC_STREAMON
        pr_info("Framechan Streamon command: 0x%x\n", cmd);
        ret = handle_framechan_ioctl(ourISPdev, cmd, (void __user *)arg, file);
        break;
    default:
        pr_info("Unhandled ioctl cmd: 0x%x\n", cmd);
        ret = -ENOTTY;
    }

    //mutex_unlock(&chn->lock);
    return ret;
}



static int framechan_release(struct inode *inode, struct file *file)
{
    struct isp_channel *chn = file->private_data;
    struct video_buffer *buf;
    int i;

    if (!chn)
        return 0;

    pr_info("Starting channel %d cleanup\n", chn->channel_id);

    // Stop any active streaming
    if (chn->queue->stream_state & 1) {
        chn->queue->stream_state = 0;
        wake_up_interruptible(&chn->queue->wait);
    }

    // Wait for thread to stop
    if (chn->thread_data) {
        atomic_set(&chn->thread_data->should_stop, 1);
        wake_up_interruptible(&chn->queue->wait);
        while (atomic_read(&chn->thread_running)) {
            msleep(1);
        }
    }

    // Free buffers
    if (chn->queue && chn->queue->bufs) {
        for (i = 0; i < chn->queue->buffer_count; i++) {
            buf = chn->queue->bufs[i];
            if (buf) {
                if (buf->meta) {
                    dma_free_coherent(chn->dev, sizeof(struct frame_metadata),
                                    buf->meta, chn->meta_dma[i]);
                }
                kfree(buf);
            }
        }
        kfree(chn->queue->bufs);
        chn->queue->bufs = NULL;
        chn->queue->buffer_count = 0;
    }

    // Clear VMA reference
    chn->vma = NULL;

    pr_info("Channel %d cleanup complete\n", chn->channel_id);
    return 0;
}


// Add mmap handler to map this memory to userspace
static int framechan_mmap(struct file *file, struct vm_area_struct *vma)
{
    struct isp_channel *chn = file->private_data;
    size_t size = vma->vm_end - vma->vm_start;

    if (!chn || !chn->buf_base) {
        pr_err("No channel or buffer base in mmap\n");
        return -EINVAL;
    }

    // Don't allow mapping beyond buffer
    if (size > chn->required_size) {
        pr_err("mmap size too large: %zu > %u\n", size, chn->required_size);
        return -EINVAL;
    }

    // Set memory type for proper caching
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

    // Map the physical memory to userspace
    if (remap_pfn_range(vma,
                        vma->vm_start,
                        chn->dma_addr >> PAGE_SHIFT,
                        size,
                        vma->vm_page_prot)) {
        pr_err("Failed to map buffer to userspace\n");
        return -EAGAIN;
                        }

    // Store vma for cleanup
    chn->vma = vma;

    pr_info("Mapped buffer to userspace:\n"
            "  User VA: 0x%lx - 0x%lx\n"
            "  Physical: 0x%08x\n"
            "  Size: %zu\n",
            vma->vm_start, vma->vm_end,
            (uint32_t)chn->dma_addr,
            size);

    return 0;
}


// Update file operations to include mmap
static const struct file_operations framechan_fops = {
    .owner = THIS_MODULE,
    .open = framechan_open,
    .release = framechan_release,
    .unlocked_ioctl = framechan_ioctl,
    .mmap = framechan_mmap,
};

// Add these globals
static struct cdev framechan_cdevs[MAX_CHANNELS];  // Array of cdevs
static dev_t framechan_dev;
static struct class *framechan_class;

static int create_framechan_devices(struct device *dev)
{
    int i, ret;
    dev_t curr_dev;

    pr_info("Creating frame channel devices...\n");

    // Allocate device numbers
    ret = alloc_chrdev_region(&framechan_dev, 0, MAX_CHANNELS, "framechan");
    if (ret) {
        dev_err(dev, "Failed to allocate char device region\n");
        return ret;
    }

    // Create device class
    framechan_class = class_create(THIS_MODULE, "framechan_class");
    if (IS_ERR(framechan_class)) {
        pr_err("Failed to create framechan class\n");
        ret = PTR_ERR(framechan_class);
        goto err_unregister_region;
    }

    // Initialize frame sources first
    for (i = 0; i < MAX_CHANNELS; i++) {
        struct isp_channel *chn = &ourISPdev->channels[i];

        // Initialize basic state
        chn->channel_id = i;
        chn->state = 0;  // Initial state
        chn->width = 1920;  // Default resolution
        chn->height = 1080;

        chn->queue = kzalloc(sizeof(struct frame_queue), GFP_KERNEL);
        if (!chn->queue) {
            ret = -ENOMEM;
            goto err_cleanup_channels;
        }

        mutex_init(&chn->queue->lock);
        spin_lock_init(&chn->queue->queue_lock);
        INIT_LIST_HEAD(&chn->queue->ready_list);
        INIT_LIST_HEAD(&chn->queue->done_list);

        pr_info("Initialized frame source %d:\n", i);
        pr_info("  width=%d height=%d\n", chn->width, chn->height);
    }

    // Create each device node
    for (i = 0; i < MAX_CHANNELS; i++) {
        curr_dev = MKDEV(MAJOR(framechan_dev), i);

        // Initialize the cdev for this channel
        cdev_init(&framechan_cdevs[i], &framechan_fops);
        framechan_cdevs[i].owner = THIS_MODULE;

        ret = cdev_add(&framechan_cdevs[i], curr_dev, 1);
        if (ret) {
            pr_err("Failed to add cdev for framechan%d: %d\n", i, ret);
            goto err_cleanup_channels;
        }

        // Create the device node
        struct device *device = device_create(framechan_class, NULL, curr_dev,
                                           NULL, "framechan%d", i);
        if (IS_ERR(device)) {
            pr_err("Failed to create device for framechan%d\n", i);
            ret = PTR_ERR(device);
            goto err_cleanup_channels;
        }

        pr_info("Created device framechan%d: major=%d, minor=%d\n",
                i, MAJOR(curr_dev), MINOR(curr_dev));
    }

    pr_info("Frame channel devices created successfully\n");
    return 0;

err_cleanup_channels:
    // Clean up any channels we managed to create
    while (--i >= 0) {
        device_destroy(framechan_class, MKDEV(MAJOR(framechan_dev), i));
        cdev_del(&framechan_cdevs[i]);
    }
    class_destroy(framechan_class);
err_unregister_region:
    unregister_chrdev_region(framechan_dev, MAX_CHANNELS);
    return ret;
}


/* Static link configurations table */
static const struct link_config configs[][2] = {
    // These would be initialized with the link configurations
    // Each entry seems to be 0x14 bytes based on increment in loop
};

/* Array of counts for each configuration */
static const int config_counts[] = {
    // Referenced at 0x6ad80
    // Contains number of links for each configuration
};

static struct tx_isp_subdev_pad *find_subdev_link_pad(struct tx_isp_dev *isp, const struct link_pad_desc *desc)
{
    struct tx_isp_subdev **sd = (struct tx_isp_subdev **)((char *)isp + 0x38);
    struct tx_isp_subdev **end = (struct tx_isp_subdev **)((char *)isp + 0x78);

    while (sd < end) {
        struct tx_isp_subdev *subdev = *sd;
        if (subdev) {
            const char *name1 = subdev->module.name;
            const char *name2 = desc->name;

            /* Compare names */
            while (*name1 && *name1 == *name2) {
                name1++;
                name2++;
            }

            if (*name1 == *name2) {
                /* Check pad type and get appropriate pad array */
                if (desc->type == 1) {
                    if (desc->index < subdev->num_inpads) {
                        return (struct tx_isp_subdev_pad *)((char *)subdev->inpads + (desc->index * 0x24));
                    }
                } else if (desc->type == 2) {
                    if (desc->index < subdev->num_outpads) {
                        return (struct tx_isp_subdev_pad *)((char *)subdev->outpads + (desc->index * 0x24));
                    }
                }
                ISP_ERROR("Can't find the matched pad!\n");
                return NULL;
            }
        }
        sd++;
    }
    return NULL;
}

static int subdev_video_destroy_link(struct tx_isp_subdev_pad *pad)
{
    if (!pad)
        return -EINVAL;

    /* Only destroy if there is an active link */
    if (pad->state == TX_ISP_PADSTATE_LINKED) {
        struct tx_isp_subdev_link *link = &pad->link;
        struct tx_isp_subdev_pad *linked_pad;

        /* Get the other pad in the link */
        if (link->source == pad)
            linked_pad = link->sink;
        else
            linked_pad = link->source;

        /* Clear link state */
        memset(&pad->link, 0, sizeof(struct tx_isp_subdev_link));
        pad->state = TX_ISP_PADSTATE_FREE;

        /* Clear linked pad if it exists */
        if (linked_pad) {
            memset(&linked_pad->link, 0, sizeof(struct tx_isp_subdev_link));
            linked_pad->state = TX_ISP_PADSTATE_FREE;
        }
    }

    return 0;
}

int tx_isp_video_link_destroy(struct tx_isp_dev *isp)
{
    int ret = 0;
    int config_idx = isp->instance;

    if (config_idx >= 0) {
        int num_links = config_counts[config_idx];
        const struct link_config *curr_config = &configs[config_idx][0];
        int i;

        for (i = 0; i < num_links; i++) {
            struct tx_isp_subdev_pad *src_pad;
            struct tx_isp_subdev_pad *dst_pad;

            /* Find source and destination pads */
            src_pad = find_subdev_link_pad(isp, &curr_config->src);
            dst_pad = find_subdev_link_pad(isp, &curr_config->dst);

            if (src_pad && dst_pad) {
                /* Destroy the link */
                ret = subdev_video_destroy_link(src_pad);
                if (ret != 0 && ret != -0x203) { // 0xfffffdfd
                    break;
                }
            }

            curr_config++; // Move to next config (0x14 bytes)
        }

        /* Clear instance ID */
        isp->instance = -1;  // 0xffffffff
    }

    return ret;
}

static int tx_isp_release(struct inode *inode, struct file *file)
{
    struct tx_isp_dev *isp = file->private_data;
    struct tx_isp_subdev **sd_start;
    struct tx_isp_subdev **sd_end;
    struct tx_isp_subdev **sd;
    int ret = 0;
    int num_devs = 0;

    /* Handle refcount */
    if (isp->refcnt) {
        isp->refcnt--;
        return 0;
    }

    /* Since we know the offsets and the layout matches the platform devices,
     * we can iterate through them in a safer way */
    sd_start = (struct tx_isp_subdev **)&isp->sensor_sd;

    /* Calculate end based on number of platform devices we have */
    if (isp->pdev) num_devs++;
    if (isp->vic_pdev) num_devs++;
    if (isp->csi_pdev) num_devs++;
    if (isp->vin_pdev) num_devs++;
    if (isp->core_pdev) num_devs++;
    if (isp->fs_pdev) num_devs++;

    sd_end = sd_start + num_devs;

    /* Iterate through subdevices */
    for (sd = sd_start; sd < sd_end; sd++) {
        if (*sd) {
            struct tx_isp_subdev_ops *ops = (*sd)->ops;
            if (ops && ops->core && ops->core->init) {
                ret = ops->core->init(*sd, 0); // Pass 0 to deinit
                if (ret != 0 && ret != -0x203) {
                    return ret;
                }
                ret = -0x203;
            }
        }
    }

    /* Check instance and destroy links if needed */
    if (ret == -0x203 || ret == 0) {
        if (isp->instance >= 0) {
            tx_isp_video_link_destroy(isp);
        }
    }

    return 0;
}

static int tx_isp_mmap(struct file *filp, struct vm_area_struct *vma) {
    struct tx_isp_dev *dev = ourISPdev;
    unsigned long size = vma->vm_end - vma->vm_start;

    pr_info("tx_isp: mmap request size=%lu\n", size);

    if (size > dev->rmem_size) {
        pr_err("tx_isp: mmap size too large\n");
        return -EINVAL;
    }

    // Set up continuous memory mapping
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

    if (remap_pfn_range(vma,
                        vma->vm_start,
                        dev->rmem_addr >> PAGE_SHIFT,
                        size,
                        vma->vm_page_prot)) {
        pr_err("tx_isp: mmap failed\n");
        return -EAGAIN;
    }

    pr_info("tx_isp: mmap completed: virt=0x%lx size=%lu\n",
            vma->vm_start, size);

    return 0;
}

/* Character device operations */
static const struct file_operations tx_isp_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = tx_isp_unlocked_ioctl,
    .open = tx_isp_open,
    .release = tx_isp_release,
    .mmap = tx_isp_mmap,
};

static void remove_framechan_devices(void)
{
    int i;

    if (framechan_class) {
        for (i = 0; i < MAX_CHANNELS; i++) {
            device_destroy(framechan_class, MKDEV(MAJOR(framechan_dev), i));
            cdev_del(&framechan_cdevs[i]);
        }
        class_destroy(framechan_class);
    }

    if (framechan_dev)
        unregister_chrdev_region(framechan_dev, MAX_CHANNELS);
}


int tx_isp_probe(struct platform_device *pdev)
{
    struct tx_isp_dev *isp = ourISPdev;
    int ret;
    int i;


    printk("tx_isp_probe\n");
    /* Allocate and initialize sensor subdev */
    isp->sensor_sd = private_kmalloc(sizeof(struct tx_isp_subdev), GFP_KERNEL);
    if (!isp->sensor_sd) {
        ISP_ERROR("Failed to allocate sensor subdev\n");
        private_kfree(isp);
        return -ENOMEM;
    }
    memset(isp->sensor_sd, 0, sizeof(struct tx_isp_subdev));

    /* Link the sensor subdev to the main device */
    isp->sensor_sd->dev = &pdev->dev;
    isp->sensor_sd->pdev = pdev;

    /* Create proc entries */
    ret = tx_isp_proc_init(isp);
    if (ret) {
        ISP_ERROR("Failed to create proc entries\n");
        goto err_free;
    }

    // Initialize other ISP device fields
    isp->dev = &pdev->dev;

    /* Create frame channel devices */
    ret = create_framechan_devices(&pdev->dev);
    if (ret)
        goto err_cleanup_subsys;

    for (int i = 0; i < MAX_CHANNELS; i++) {
        struct isp_channel *chn = &isp->channels[i];
        if (!chn)
            continue;

        // Initialize basic state
        chn->channel_id = i;
        chn->state = 0;  // Initial state
        chn->width = 1920;  // Default resolution
        chn->height = 1080;
    }

    ISP_INFO("TX ISP driver probed successfully\n");
    return 0;


err_cleanup_subsys:
    remove_framechan_devices();
err_free:
    private_kfree(isp);
    return ret;
}

int tx_isp_remove(struct platform_device *pdev)
{
    struct tx_isp_dev *isp = platform_get_drvdata(pdev);
    struct tx_isp_platform platforms[TX_ISP_PLATFORM_MAX_NUM];
    int i = 0;

    if (!isp)
        return -EINVAL;

    /* Build platforms array from the platform device members */
    if (isp->pdev) {
        platforms[i].dev = isp->pdev;
        i++;
    }
    if (isp->vic_pdev) {
        platforms[i].dev = isp->vic_pdev;
        i++;
    }
    if (isp->csi_pdev) {
        platforms[i].dev = isp->csi_pdev;
        i++;
    }
    if (isp->vin_pdev) {
        platforms[i].dev = isp->vin_pdev;
        i++;
    }
    if (isp->core_pdev) {
        platforms[i].dev = isp->core_pdev;
        i++;
    }
    if (isp->fs_pdev) {
        platforms[i].dev = isp->fs_pdev;
        i++;
    }

    /* Unregister all platform devices */
    tx_isp_unregister_platforms(platforms);

    /* Free IRQ */
    free_irq(isp->isp_irq, isp);

    /* Cleanup subsystems */
    tx_isp_sysfs_exit(isp);
    tx_isp_proc_exit(isp);

    /* Cleanup channels */
    for (i = 0; i < ISP_MAX_CHAN; i++) {
        tx_isp_frame_chan_deinit(&isp->channels[i]);
    }

    /* Release memory regions */
    if (isp->mem_region)
        release_mem_region(isp->mem_region->start, resource_size(isp->mem_region));

    if (isp->csi_region)
        release_mem_region(isp->csi_region->start, resource_size(isp->csi_region));

    /* Clear driver data */
    platform_set_drvdata(pdev, NULL);

    /* Free ISP device structure */
    kfree(isp);

    return 0;
}

/* Hardware initialization and management */
int configure_isp_clocks(struct tx_isp_dev *dev)
{
    struct device *device = dev->dev;
    struct device_node *np = device->of_node;
    int ret;

    pr_info("Configuring ISP system clocks\n");

    dev->csi_clk = devm_clk_get(device, "csi");
    if (IS_ERR(dev->csi_clk)) {
        ret = PTR_ERR(dev->csi_clk);
        pr_err("Failed to get CSI clock: %d\n", ret);
        return ret;
    }

    /* Core clocks - Critical for ISP operation */
    dev->isp_clk = devm_clk_get(device, "isp");
    if (IS_ERR(dev->isp_clk)) {
        ret = PTR_ERR(dev->isp_clk);
        pr_err("Failed to get ISP core clock: %d\n", ret);
        return ret;
    }

    dev->cgu_isp = devm_clk_get(device, "cgu_isp");
    if (IS_ERR(dev->cgu_isp)) {
        ret = PTR_ERR(dev->cgu_isp);
        pr_err("Failed to get CGU ISP clock: %d\n", ret);
        return ret;
    }

    pr_info("CSI clock initialized: rate=%lu Hz\n", clk_get_rate(dev->csi_clk));

    /* Enable CSI clock */
    ret = clk_prepare_enable(dev->csi_clk);
    if (ret) {
        pr_err("Failed to enable CSI clock: %d\n", ret);
        return ret;
    }

    /* Set and enable CGU ISP clock */
    ret = clk_set_rate(dev->cgu_isp, T31_CGU_ISP_FREQ);
    if (ret) {
        pr_err("Failed to set CGU ISP clock rate: %d\n", ret);
        return ret;
    }

    ret = clk_prepare_enable(dev->cgu_isp);
    if (ret) {
        pr_err("Failed to enable CGU ISP clock: %d\n", ret);
        return ret;
    }

    /* Enable ISP core clock */
    ret = clk_prepare_enable(dev->isp_clk);
    if (ret) {
        pr_err("Failed to enable ISP core clock: %d\n", ret);
        goto err_disable_cgu;
    }

    /* Optional system clocks */
    dev->ipu_clk = devm_clk_get(device, "ipu");
    if (!IS_ERR(dev->ipu_clk)) {
        ret = clk_prepare_enable(dev->ipu_clk);
        if (ret) {
            pr_warn("Failed to enable IPU clock: %d\n", ret);
            dev->ipu_clk = NULL;
        }
    } else {
        dev->ipu_clk = NULL;
    }

    /* Optional peripheral clocks from device tree */
    struct device_node *clk_node;
    const char *clk_name;
    u32 clk_rate;

    for_each_child_of_node(np, clk_node) {
        if (of_property_read_string(clk_node, "clock-name", &clk_name))
            continue;

        struct clk *periph_clk = devm_clk_get(device, clk_name);
        if (IS_ERR(periph_clk)) {
            pr_warn("Optional clock %s not available\n", clk_name);
            continue;
        }

        if (!of_property_read_u32(clk_node, "clock-rate", &clk_rate)) {
            ret = clk_set_rate(periph_clk, clk_rate);
            if (ret) {
                pr_warn("Failed to set %s clock rate: %d\n", clk_name, ret);
                continue;
            }
        }

        ret = clk_prepare_enable(periph_clk);
        if (ret) {
            pr_warn("Failed to enable %s clock: %d\n", clk_name, ret);
            continue;
        }
    }

    /* Log configured clock rates */
    pr_info("Clock configuration completed. Rates:\n");
    pr_info("  CSI Core: %lu Hz\n", clk_get_rate(dev->csi_clk));
    pr_info("  ISP Core: %lu Hz\n", clk_get_rate(dev->isp_clk));
    pr_info("  CGU ISP: %lu Hz\n", clk_get_rate(dev->cgu_isp));
    if (dev->csi_clk)
        pr_info("  CSI: %lu Hz\n", clk_get_rate(dev->csi_clk));
    if (dev->ipu_clk)
        pr_info("  IPU: %lu Hz\n", clk_get_rate(dev->ipu_clk));

    return 0;

err_disable_cgu:
    clk_disable_unprepare(dev->cgu_isp);
    return ret;
}

/* Platform driver structures */
static struct platform_driver tx_isp_driver = {
    .probe = tx_isp_probe,
    .remove = tx_isp_remove,
    .driver = {
        .name = "tx-isp",
        .owner = THIS_MODULE,
    },
};

struct resource tx_isp_resources[] = {
    {
        .start = TX_ISP_CORE_BASE,
        .end   = TX_ISP_CORE_BASE + 0x1000 - 1,
        .flags = IORESOURCE_MEM,
        .name  = "isp-core"
    },
    {
        .start  = 37,  // ISP IRQ number
        .end    = 37,
        .flags  = IORESOURCE_IRQ,
        .name   = "isp-m0",
    }
};

// Add new char device ops for libimp
static const struct file_operations isp_m0_chardev_fops = {
    .owner = THIS_MODULE,
    .open = isp_m0_chardev_open,
    .release = isp_m0_chardev_release,
    .unlocked_ioctl = isp_m0_chardev_ioctl,
};

// Add misc device for /dev/isp-m0
static struct miscdevice isp_m0_miscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "isp-m0",
    .fops = &isp_m0_chardev_fops,  // Use char device ops
};


static int tx_isp_init(void)
{
    int ret;
    struct tx_isp_dev *isp;
    struct platform_device *pdev;
    static struct isp_platform_data isp_pdata = {
        .clock_rate = T31_ISP_FREQ,
    };

    /* Allocate ISP device structure */
    isp = private_kmalloc(sizeof(struct tx_isp_dev), GFP_KERNEL);
    if (!isp) {
        ISP_ERROR("Failed to allocate camera device\n");
        return -ENOMEM;
    }
    memset(isp, 0, sizeof(struct tx_isp_dev));
    ourISPdev = isp;

    /* Initialize spinlock */
    spin_lock_init(&isp->lock);

    /* Setup misc device */
    isp->miscdev.minor = MISC_DYNAMIC_MINOR;
    isp->miscdev.name = "tx-isp";
    isp->miscdev.fops = &tx_isp_fops;
    ret = misc_register(&isp->miscdev);
    if (ret) {
        ISP_ERROR("Failed to register tx-isp device\n");
        goto err_free;
    }

    /* Register platform driver first */
    ret = platform_driver_register(&tx_isp_driver);
    if (ret) {
        ISP_ERROR("Failed to register tx-isp driver\n");
        goto err_proc;
    }

    /* Create platform device properly */
    pdev = platform_device_alloc("tx-isp", -1);
    if (!pdev) {
        ret = -ENOMEM;
        goto err_driver;
    }

    ret = platform_device_add_resources(pdev, tx_isp_resources,
                                      ARRAY_SIZE(tx_isp_resources));
    if (ret)
        goto err_put_device;

    ret = platform_device_add_data(pdev, &isp_pdata, sizeof(isp_pdata));
    if (ret)
        goto err_put_device;

    ret = platform_device_add(pdev);
    if (ret)
        goto err_put_device;

    /* Store device pointers */
    isp->dev = &pdev->dev;
    platform_set_drvdata(pdev, isp);
    tx_isp_pdev = pdev;

    /* Register subdevice drivers */
    ret = tx_isp_subdev_register_drivers(isp);
    if (ret) {
        ISP_ERROR("Failed to register subdevice drivers\n");
        goto err_device;
    }

    // Register the misc device for /dev/isp-m0
    ret = misc_register(&isp_m0_miscdev);
    if (ret) {
        pr_err("Failed to register isp-m0 misc device: %d\n", ret);
        goto err_subdev;
    }

    // Finally set up default links
    ret = tx_isp_setup_default_links(isp);
    if (ret) {
        pr_err("Failed to setup default links: %d\n", ret);
        goto err_subdev;
    }

    configure_isp_clocks(isp);

    isp_printf(ISP_INFO_LEVEL, "TX ISP driver initialized\n");
    return 0;

err_subdev:
    tx_isp_subdev_unregister_drivers();
err_device:
    platform_device_del(pdev);
err_put_device:
    platform_device_put(pdev);
err_driver:
    platform_driver_unregister(&tx_isp_driver);
err_proc:
    tx_isp_proc_exit(isp);
err_misc:
    misc_deregister(&isp->miscdev);
err_free:
    private_kfree(isp);
    return ret;
}

static void tx_isp_exit(void)
{
    /* Unregister subdevice drivers first */
    tx_isp_subdev_unregister_drivers();

    /* Then unregister core driver */
    platform_driver_unregister(&tx_isp_driver);

    /* Finally unregister platform device */
    platform_device_unregister(tx_isp_pdev);

    /* Cleanup ISP device */
    if (ourISPdev) {
        tx_isp_proc_exit(ourISPdev);
        misc_deregister(&ourISPdev->miscdev);
        private_kfree(ourISPdev);
        ourISPdev = NULL;
    }

    isp_printf(ISP_INFO_LEVEL, "TX ISP driver removed\n");
}

EXPORT_SYMBOL(tx_isp_init);
EXPORT_SYMBOL(tx_isp_exit);

MODULE_AUTHOR("Matt Davis <matteius@gmail.com>");
MODULE_DESCRIPTION("TX-ISP Camera Driver");
MODULE_LICENSE("GPL");



module_init(tx_isp_init);
module_exit(tx_isp_exit);

MODULE_AUTHOR("Matt Davis <matteius@gmail.com>");
MODULE_DESCRIPTION("TX-ISP Camera Driver");
MODULE_LICENSE("GPL");