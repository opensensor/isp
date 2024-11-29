#include <linux/printk.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/stat.h>
#include <linux/slab.h>
#include <linux/idr.h>
#include <linux/spinlock.h>
#include <linux/sys_soc.h>
#include <linux/err.h>
#include <linux/dma-mapping.h>
#include <linux/firmware.h>
#include <linux/of_device.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/mod_devicetable.h>
#include <linux/fs.h>      // For file operations
#include <linux/cdev.h>    // For character device support
#include <linux/vmalloc.h>
#include <linux/delay.h>   // For msleep
#include <linux/slab.h>      // For kmalloc and kfree
#include <linux/uaccess.h>   // For copy_to_user and copy_from_user
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/interrupt.h>
#include <linux/clk.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/capability.h>
#include <linux/videodev2.h>
#include <linux/errno.h>
#include <linux/wait.h>
#include <media/videobuf2-core.h>
#include <media/videobuf2-dma-contig.h>
#include <media/v4l2-device.h>
#include <linux/semaphore.h>
#include <linux/err.h>         // For IS_ERR and PTR_ERR
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/mutex.h>
#include <linux/videodev2.h>
#include <linux/sched.h>


#include <tx-isp-main.h>
#include <tx-isp.h>
#include <tx-isp-hw.h>
#include <tx-isp-sensor.h>
#include <tx-isp-device.h>
#include <tx-isp-common.h>
#include <tx-isp-debug.h>


#define FS_ATTR_SET     BIT(0)

static DEFINE_MUTEX(tuning_mutex);

// Add these to your global variables
static struct proc_data *proc_data_fs = NULL;
static struct proc_data *proc_data_m0 = NULL;
static struct proc_data *proc_data_w00 = NULL;
static struct proc_data *proc_data_w01 = NULL;
static struct proc_data *proc_data_w02 = NULL;

struct IMPISPDev *ourISPdev = NULL;
uint32_t globe_ispdev = 0x0;
static struct device *tisp_device;
static dev_t tx_sp_dev_number;
static struct class *tisp_class;
static struct af_zone_data af_zone_data;
static uint32_t *g_pools;


// Now update the configurations
static struct tx_isp_link_config config0[] = {
    {
        // Link 0: CSI to VIC
        .src = {
            .name = "tx-isp-csi",
            .type = TX_ISP_PADTYPE_OUTPUT,
            .index = 0
        },
        .dst = {
            .name = "tx-isp-vic",
            .type = TX_ISP_PADTYPE_INPUT,
            .index = 0
        },
        .flag = TX_ISP_PADLINK_CSI | TX_ISP_PADLINK_VIC,
    },
    {
        // Link 1: VIC to DDR
        .src = {
            .name = "tx-isp-vic",
            .type = TX_ISP_PADTYPE_OUTPUT,
            .index = 0
        },
        .dst = {
            .name = "tx-isp-ddr",
            .type = TX_ISP_PADTYPE_INPUT,
            .index = 0
        },
        .flag = TX_ISP_PADLINK_VIC | TX_ISP_PADLINK_DDR,
    },
};

static struct tx_isp_link_config config1[] = {
    {
        // Link 0: CSI to VIC
        .src = {
            .name = "tx-isp-csi",
            .type = TX_ISP_PADTYPE_OUTPUT,
            .index = 0
        },
        .dst = {
            .name = "tx-isp-vic",
            .type = TX_ISP_PADTYPE_INPUT,
            .index = 0
        },
        .flag = TX_ISP_PADLINK_CSI | TX_ISP_PADLINK_VIC,
    },
    {
        // Link 1: VIC primary output to DDR
        .src = {
            .name = "tx-isp-vic",
            .type = TX_ISP_PADTYPE_OUTPUT,
            .index = 0
        },
        .dst = {
            .name = "tx-isp-ddr",
            .type = TX_ISP_PADTYPE_INPUT,
            .index = 0
        },
        .flag = TX_ISP_PADLINK_VIC | TX_ISP_PADLINK_DDR,
    },
    {
        // Link 2: VIC secondary output to DDR
        .src = {
            .name = "tx-isp-vic",
            .type = TX_ISP_PADTYPE_OUTPUT,
            .index = 1
        },
        .dst = {
            .name = "tx-isp-ddr",
            .type = TX_ISP_PADTYPE_INPUT,
            .index = 1
        },
        .flag = TX_ISP_PADLINK_VIC | TX_ISP_PADLINK_DDR,
    },
};

// Main configuration array
static const struct tx_isp_link_configs isp_link_configs[] = {
    [0] = {
        .config = config0,
        .length = ARRAY_SIZE(config0),
    },
    [1] = {
        .config = config1,
        .length = ARRAY_SIZE(config1),
    },
};


/* Global state variables seen in decompiled */
static void *ae_info_mine;
static void *awb_info_mine;
static int ae_algo_comp;
static int awb_algo_comp;

/* Wait queues seen in decompiled code */
static DECLARE_WAIT_QUEUE_HEAD(ae_wait);
static DECLARE_WAIT_QUEUE_HEAD(awb_wait);

// Add these state tracking macros
#define FRAME_QUEUE_MASK  0x0000FFFF
#define FRAME_STATE_MASK  0x0000FFFF

static inline u64 ktime_get_real_ns(void)
{
    struct timespec ts;
    ktime_get_real_ts(&ts);
    return timespec_to_ns(&ts);
}


/* System register access functions */
static inline uint32_t system_reg_read(u32 reg)
{
    if (!ourISPdev || !ourISPdev->reg_base)
        return 0;

    return readl(ourISPdev->reg_base + (reg - ISP_BASE_ADDR));
}


static inline void system_reg_write(u32 reg, u32 val)
{
    void __iomem *addr = ioremap(reg, 4);

    if (!addr)
        return;

    writel(val, addr);
    iounmap(addr);
}


/****
* The following methods are made available to libimp.so
****/
void private_i2c_del_driver(struct i2c_driver *driver)
{
    i2c_del_driver(driver);
}

int private_gpio_request(unsigned int gpio, const char *label)
{
    return gpio_request(gpio, label);
}

void private_gpio_free(unsigned int gpio)
{
    gpio_free(gpio);
}

void private_msleep(unsigned int msecs)
{
    msleep(msecs);
}

void private_clk_disable(struct clk *clk)
{
    clk_disable(clk);
}

void *private_i2c_get_clientdata(const struct i2c_client *client)
{
    return i2c_get_clientdata(client);
}

bool private_capable(int cap)
{
    return capable(cap);
}

void private_i2c_set_clientdata(struct i2c_client *client, void *data)
{
    i2c_set_clientdata(client, data);
}

int private_i2c_transfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num)
{
    return i2c_transfer(adap, msgs, num);
}

int private_i2c_add_driver(struct i2c_driver *driver)
{
    return i2c_add_driver(driver);
}

int private_gpio_direction_output(unsigned int gpio, int value)
{
    return gpio_direction_output(gpio, value);
}

int private_clk_enable(struct clk *clk)
{
    return clk_enable(clk);
}

void private_clk_put(struct clk *clk)
{
    clk_put(clk);
}

int private_clk_set_rate(struct clk *clk, unsigned long rate)
{
    return clk_set_rate(clk, rate);
}

int32_t private_driver_get_interface()
{
    struct jz_driver_common_interfaces *pfaces = NULL;  // Declare pfaces locally
    int32_t result = private_get_driver_interface(&pfaces);  // Call the function with the address of pfaces

    if (result != 0) {
        // Handle error, pfaces should still be NULL if the function failed
        return result;
    }

    // Proceed with further logic, now that pfaces is properly initialized
    // Example: check flags or other interface fields
    if (pfaces != NULL) {
        // You can now access pfaces->flags_0, pfaces->flags_1, etc.
        if (pfaces->flags_0 != pfaces->flags_1) {
            ISP_ERROR("Mismatch between flags_0 and flags_1");
            return -1;  // Some error condition
        }
    }

    return 0;  // Success
}
EXPORT_SYMBOL(private_driver_get_interface);

EXPORT_SYMBOL(private_i2c_del_driver);
EXPORT_SYMBOL(private_gpio_request);
EXPORT_SYMBOL(private_gpio_free);
EXPORT_SYMBOL(private_msleep);
EXPORT_SYMBOL(private_clk_disable);
EXPORT_SYMBOL(private_i2c_get_clientdata);
EXPORT_SYMBOL(private_capable);
EXPORT_SYMBOL(private_i2c_set_clientdata);
EXPORT_SYMBOL(private_i2c_transfer);
EXPORT_SYMBOL(private_i2c_add_driver);
EXPORT_SYMBOL(private_gpio_direction_output);
EXPORT_SYMBOL(private_clk_enable);
EXPORT_SYMBOL(private_clk_put);
EXPORT_SYMBOL(private_clk_set_rate);


// Helper function to calculate proper buffer sizes with alignment
static uint32_t calculate_buffer_size(uint32_t width, uint32_t height, uint32_t format) {
    // For NV12 format
    if (format == V4L2_PIX_FMT_NV12 || format == 0x23) {
        // Base size calculation
        uint32_t y_stride = ALIGN(width, 16);  // Align to 16 bytes instead of 64
        uint32_t y_size = y_stride * ALIGN(height, 2);
        uint32_t uv_stride = y_stride;
        uint32_t uv_size = uv_stride * (ALIGN(height, 2) / 2);

        // Total size without excessive padding
        uint32_t total_size = y_size + uv_size;
        return ALIGN(total_size, 4096);  // Page align the final size
    }

    // Default fallback
    return width * height * 2;
}

static int setup_dma_buffer(struct isp_channel *chn)
{
    size_t size_per_buffer;

    if (!chn->dev || !ourISPdev) {
        pr_err("No device for DMA setup\n");
        return -EINVAL;
    }

    // Calculate size needed per buffer
    size_per_buffer = calculate_buffer_size(chn->width, chn->height, chn->fmt);
    chn->required_size = size_per_buffer;

    pr_info("Setting up DMA buffer from rmem: ch=%d size=%zu\n",
            chn->channel_id, size_per_buffer);

    // Verify we have enough rmem space
    if (size_per_buffer > ourISPdev->rmem_size) {
        pr_err("Required buffer size %zu exceeds rmem size %zu\n",
               size_per_buffer, ourISPdev->rmem_size);
        return -ENOMEM;
    }

    // Use the pre-allocated rmem
    chn->dma_addr = ourISPdev->rmem_addr;
    chn->buf_base = ourISPdev->dma_buf;

    pr_info("DMA buffer mapped from rmem:\n"
            "  Virtual: %p\n"
            "  Physical: %pad\n"
            "  Size: %zu\n",
            chn->buf_base, &chn->dma_addr, size_per_buffer);

    return 0;
}

static int cleanup_buffer(struct isp_channel *chn)
{
    struct video_buffer *buf;
    int i;

    if (!chn->queue || !chn->meta_dma)
        return 0;

    // Free buffer metadata
    for (i = 0; i < chn->queue->buf_count; i++) {
        buf = chn->queue->bufs[i];
        if (buf && buf->meta) {
            dma_free_coherent(chn->dev, sizeof(struct frame_metadata),
                            buf->meta, chn->meta_dma[i]);
            buf->meta = NULL;
        }
    }

    kfree(chn->meta_dma);
    chn->meta_dma = NULL;

    // Free group structure if allocated using regular kmalloc
    if (chn->group) {
        kfree(chn->group);
        chn->group = NULL;
    }

    return 0;
}

// Generic cleanup function that handles both cases:
static void cleanup_buffers(struct isp_channel *chn)
{
    cleanup_buffer(chn);
}

// V4L2 helper functions using direct file operations
static int v4l2_s_fmt(struct file *filp, struct v4l2_format *fmt)
{
    if (!filp || !filp->f_op || !filp->f_op->unlocked_ioctl)
        return -ENODEV;

    return filp->f_op->unlocked_ioctl(filp, VIDIOC_S_FMT, (unsigned long)fmt);
}

static int v4l2_ioctl(struct file *filp, unsigned int cmd, void *arg)
{
    if (!filp || !filp->f_op || !filp->f_op->unlocked_ioctl)
        return -ENODEV;

    return filp->f_op->unlocked_ioctl(filp, cmd, (unsigned long)arg);
}

void tx_isp_free_irq(int32_t* irq_pointer)
{
    if (irq_pointer != 0)
    {
        int32_t irq_number = *(uint32_t*)irq_pointer;

        if (irq_number == 0)
            *(uint32_t*)irq_pointer = 0;
        else
        {
            free_irq(irq_number, irq_pointer);
            *(uint32_t*)irq_pointer = 0;
        }
    }
}

static irqreturn_t tx_vic_irq(int irq, void *dev_id)
{
    struct IMPISPDev *dev = dev_id;
    void __iomem *vic_regs = dev->reg_base + 0x7800;
    uint32_t status, irq_status;

    pr_info("VIC IRQ triggered:\n");

    // Read and clear status
    status = readl(vic_regs + 0xb4);
    irq_status = readl(vic_regs + 0x13C) & status;

    pr_info("  Raw status: 0x%08x\n", status);
    pr_info("  IRQ status: 0x%08x\n", irq_status);
    pr_info("  Control: 0x%08x\n", readl(vic_regs + 0x140));

    // Clear handled interrupts
    writel(status, vic_regs + 0xb4);
    wmb();

    return IRQ_HANDLED;
}

static int check_csi_errors(struct IMPISPDev *dev)
{
    void __iomem *csi_base;
    u32 err1, err2;

    if (!dev || !dev->csi_dev || !dev->csi_dev->csi_regs) {
        return -EINVAL;
    }

    csi_base = dev->csi_dev->csi_regs;

    err1 = readl(csi_base + 0x20);
    err2 = readl(csi_base + 0x24);

    if (err1 || err2) {
        pr_info("CSI Errors: err1=0x%x err2=0x%x\n", err1, err2);
        writel(0xffffffff, csi_base + 0x20);
        writel(0xffffffff, csi_base + 0x24);
        wmb();
    }

    return 0;
}

static void handle_vic_interrupt(struct IMPISPDev *dev, u32 status)
{
    void __iomem *vic_regs = dev->reg_base + 0x300;
    struct isp_channel *chn = &dev->channels[0];
    unsigned long flags;

    pr_info("VIC interrupt: status=0x%08x\n", status);

    // Check the detailed status bits
    pr_info("VIC status details:\n");
    pr_info("  Control: 0x%08x\n", readl(vic_regs + 0x4));
    pr_info("  Route: 0x%08x\n", readl(vic_regs + 0x10));
    pr_info("  Frame Control: 0x%08x\n", readl(vic_regs + 0x14));
}

static void handle_isp_interrupt(struct IMPISPDev *dev, u32 status)
{
    void __iomem *isp_regs = dev->reg_base;

    pr_info("ISP interrupt: status=0x%08x\n", status);

    if (status & 0x4) {
        pr_warn("ISP errorstatus=0x%08x\n",
                 status);
    }

    if (status & 0x8) {
        pr_warn("ISP overflow ");
    }

}

static irqreturn_t tx_isp_irq_handler(int irq, void *dev_id)
{
    struct IMPISPDev *dev = dev_id;
    void __iomem *vic_regs = dev->reg_base + 0x7800;
    u32 status, irq_status;
    bool handled = false;

    pr_info("ISP IRQ triggered:\n");

    // Read status and irq status with proper registers
    status = readl(vic_regs + 0xb4);       // 0xb4 at 0xb4
    irq_status = readl(vic_regs + 0x1e0);  // IRQ_STATUS at 0x1e0

    pr_info("ISP IRQ: status=0x%x irq_status=0x%x mask=0x%x\n",
            status,
            irq_status,
            readl(vic_regs + 0x1e8));      // IRQ_MASK at 0x1e8

    if (status) {
        // Clear status bits that were set
        writel(status, vic_regs + 0xb4);   // Clear 0xb4
        writel(status, vic_regs + 0x1e0);  // Clear IRQ_STATUS
        wmb();
        handled = true;

        // Process frame completion if that bit is set
        if (status & 0x1) {  // Frame done bit
            // TODO
            pr_info("Frame done interrupt\n");
        }
    }

    return handled ? IRQ_HANDLED : IRQ_NONE;
}

static int reset_gpio = GPIO_PA(18);  // Default reset GPIO
static int pwdn_gpio = -1;  // Default power down GPIO disabled



int32_t tx_isp_notify(int32_t arg1, int32_t notify_type)
{
    struct IMPISPDev *dev = ourISPdev;
    struct isp_callback_info **cb_list;
    int32_t result = 0;
    uint32_t notify_flag;

    if (!dev)
        return -EINVAL;

    // Get callback list starting at offset 0x38
    cb_list = (struct isp_callback_info **)((char *)dev + 0x38);
    if (!cb_list)
        return -EINVAL;

    // Extract notification type from upper byte
    notify_flag = notify_type & 0xff000000;

    // Iterate through callback list until offset 0x78
    while (cb_list < (struct isp_callback_info **)((char *)dev + 0x78)) {
        struct isp_callback_info *cb_info = *cb_list;

        if (cb_info) {
            switch (notify_flag) {
            case ISP_NOTIFY_AE:
                if (cb_info->callback_data) {
                    int32_t (*ae_func)(void) =
                        (int32_t (*)(void))((char *)cb_info->callback_data + 0x1c);

                    if (ae_func) {
                        int32_t cb_result = ae_func();
                        if (cb_result) {
                            result = (cb_result != ISP_ERR_CALLBACK) ?
                                     cb_result : ISP_ERR_CALLBACK;
                            goto out;
                        }
                    } else {
                        result = ISP_ERR_CALLBACK;
                    }
                } else {
                    result = ISP_ERR_CALLBACK;
                }
                break;

            case ISP_NOTIFY_STATS:
                if (cb_info->callback_data) {
                    void *stats_data = *(void **)((char *)cb_info->callback_data + 0xc);
                    if (stats_data) {
                        int32_t (*stats_func)(void) =
                            (int32_t (*)(void))((char *)stats_data + 0x8);

                        if (stats_func) {
                            int32_t cb_result = stats_func();
                            result = cb_result ? ISP_ERR_CALLBACK : 0;
                        } else {
                            result = ISP_ERR_CALLBACK;
                        }
                    }
                }
                break;

            default:
                result = 0;
                break;
            }
        }

        cb_list++;
    }

out:
    return (result == ISP_ERR_CALLBACK) ? 0 : result;
}

// Add registration helper
int register_isp_callback(struct IMPISPDev *dev,
                         int type,
                         void *callback_data,
                         int32_t (*func)(void))
{
    struct isp_callback_info *cb_info;

    if (!dev || !callback_data || !func)
        return -EINVAL;

    cb_info = kzalloc(sizeof(*cb_info), GFP_KERNEL);
    if (!cb_info)
        return -ENOMEM;

    cb_info->callback_data = callback_data;

    if (type == ISP_NOTIFY_AE)
        cb_info->ae_cb = func;
    else if (type == ISP_NOTIFY_STATS)
        cb_info->stats_cb = func;

    // Store in device callback list at appropriate offset
    *(struct isp_callback_info **)((char *)dev + 0x38) = cb_info;

    return 0;
}

/*
 * Initialize an ISP module
 * @pdev: Platform device containing module info
 * @sd: Subdevice to initialize
 * Returns 0 on success, negative errno on failure
 */

// Function to deinitialize the ISP module
void tx_isp_module_deinit(struct tx_isp_subdev *tisp_dev)
{
    if (!tisp_dev) return;

    if (tisp_dev->base) {
        iounmap(tisp_dev->base);
        tisp_dev->base = NULL;
    }

    pr_info("ISP module deinitialized successfully\n");
}

// Define OEM driver's expected structures
struct IspSubDevice {
    char device_name[32];     // 0x00: Device name
    uint32_t offset_10;       // 0x10: Referenced in init
    struct IspModule *module_info;  // Module info pointer
};


// Individual device proc handlers
static int isp_fs_show(struct seq_file *m, void *v)
{
    int i;

    if (!ourISPdev) {
        seq_puts(m, "Error: ISP device not initialized\n");
        return 0;
    }

    for (i = 0; i < MAX_CHANNELS; i++) {
        seq_printf(m, "############## framesource %d ###############\n", i);
        seq_printf(m, "chan status: %s\n",
                  (ourISPdev->is_open && ourISPdev->sensor_name[0]) ? "running" : "stop");
    }
    return 0;
}

static int isp_m0_show(struct seq_file *m, void *v)
{
    seq_puts(m, "****************** ISP INFO **********************\n");

    if (!ourISPdev || !ourISPdev->is_open || !ourISPdev->sensor_name[0]) {
        seq_puts(m, "sensor doesn't work, please enable sensor\n");
    } else {
        seq_printf(m, "sensor %s is working\n", ourISPdev->sensor_name);
    }
    return 0;
}

static int isp_w00_show(struct seq_file *m, void *v)
{
    if (!ourISPdev || !ourISPdev->is_open || !ourISPdev->sensor_name[0]) {
        seq_puts(m, "sensor doesn't work, please enable sensor\n");
    } else {
        seq_printf(m, "sensor %s is active\n", ourISPdev->sensor_name);
    }
    return 0;
}

// Add CSI operations handler
static int tx_isp_csi_ioctl(struct IMPISPDev *dev, int cmd)
{
    void __iomem *csi_regs = dev->csi_dev->csi_regs;
    int ret = 0;
    uint32_t reg_val;

    pr_info("CSI IOCTL cmd=0x%x\n", cmd);

    switch (cmd) {
        case 0x200000e:  // Check CSI type 1
            reg_val = readl(dev->reg_base + 0x110 + 0x14);
        if (reg_val == 1) {
            writel(3, dev->reg_base + 0x128);  // Set state 3
        }
        break;

        case 0x200000f:  // Set state 4
            reg_val = readl(dev->reg_base + 0x110 + 0x14);
        if (reg_val == 1) {
            writel(4, dev->reg_base + 0x128);  // Set state 4
        }
        break;

        case 0x200000c:  // CSI init
            ret = csi_core_ops_init(dev, 1);
        break;

        default:
            pr_warn("Unknown CSI IOCTL: 0x%x\n", cmd);
        break;
    }

    return ret;
}

// Add proc handler for isp-w02 frame count
static int isp_w02_show(struct seq_file *m, void *v)
{
    struct isp_channel *chn = m->private;
    struct video_buffer *buf;
    u32 frame_count = 0;
    unsigned long flags;

    if (!chn) {
        seq_puts(m, "Error: ISP device not initialized\n");
        return 0;
    }

    // Lock queue for consistent count
    mutex_lock(&chn->queue->lock);
    spin_lock_irqsave(&chn->queue->queue_lock, flags);

    // Count frames in ready list
    if (!list_empty(&chn->queue->ready_list)) {
        list_for_each_entry(buf, &chn->queue->ready_list, list) {
            if (!(buf->flags & V4L2_BUF_FLAG_DONE)) {
                frame_count++;
            }
        }
    }

    spin_unlock_irqrestore(&chn->queue->queue_lock, flags);
    mutex_unlock(&chn->queue->lock);

    seq_printf(m, "%u\n", frame_count);
    return 0;
}

// Update isp_proc_open to properly pass the data
static int isp_proc_open(struct inode *inode, struct file *file)
{
    struct proc_data *data = PDE_DATA(inode);
    if (!data) {
        return -EINVAL;
    }
    return single_open(file, data->show_func, data->private_data);
}

// File operations structs for each device type
static const struct file_operations isp_fs_fops = {
    .owner = THIS_MODULE,
    .open = isp_proc_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

static const struct file_operations isp_m0_fops = {
    .owner = THIS_MODULE,
    .open = isp_proc_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

static const struct file_operations isp_w00_fops = {
    .owner = THIS_MODULE,
    .open = isp_proc_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

static const struct file_operations isp_w02_fops = {
    .owner = THIS_MODULE,
    .open = isp_proc_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};


static int isp_m0_chardev_open(struct inode *inode, struct file *file)
{
    struct IMPISPDev *dev = ourISPdev;
    struct isp_tuning_data *tuning = dev->tuning_data;

    // Store device reference
    file->private_data = dev;
    dev->tuning_enabled = 0;  // Start in disabled state

    pr_info("ISP M0 device opened, tuning_data=%p\n", dev->tuning_data);
    return 0;
}

static int handle_tuning_state_change(struct IMPISPDev *dev, bool enable)
{
    struct isp_channel *chn;
    int i;
    unsigned long flags;

    mutex_lock(&tuning_mutex);

    // Update tuning state first
    dev->tuning_enabled = enable ? 2 : 0;
    wmb(); // Ensure state change is visible

    // Handle each channel
    for (i = 0; i < MAX_CHANNELS; i++) {
        chn = &dev->channels[i];
        if (!chn)
            continue;

        if (enable) {
            // Pause frame processing
            chn->state &= ~BIT(1); // Clear streaming bit
            pr_info("TODO Enable tuning\n");


        } else {
            // Resume frame processing
            chn->state |= BIT(1); // Set streaming bit
        }
    }
    mutex_unlock(&tuning_mutex);

    return 0;
}

static int isp_core_tuning_open(struct IMPISPDev *dev)
{
    struct isp_tuning_data *tuning = dev->tuning_data;
    int ret = 0;

    pr_info("##### %s %d #####\n", __func__, __LINE__);

    // Make sure we have tuning data allocated
    if (!tuning)
        return -EINVAL;

    // Don't touch userspace mutex here - it's managed by IMP_ISP_EnableTuning()

    // Just set state to match userspace
    tuning->state = 2;  // Enabled state

    // Map registers if needed
    if (!tuning->regs) {
        tuning->regs = ioremap(0x13380000, 0x1b000);
        if (!tuning->regs) {
            pr_err("Failed to map tuning registers\n");
            return -ENOMEM;
        }
    }

    // Set default tuning values
    tuning->contrast = 128;
    tuning->brightness = 128;
    tuning->saturation = 128;
    tuning->sharpness = 128;

    pr_info("ISP tuning initialized: %p\n", dev->tuning_data);
    return ret;
}

static int isp_core_tuning_release(struct IMPISPDev *dev)
{
    struct isp_tuning_data *tuning = dev->tuning_data;

    pr_info("##### %s %d #####\n", __func__, __LINE__);

    if (!tuning)
        return 0;

    // Unmap registers but preserve tuning data structure
    if (tuning->regs) {
        iounmap(tuning->regs);
        tuning->regs = NULL;
    }

    // Just clear state - mutex cleanup handled by userspace
    tuning->state = 0;

    return 0;
}

static int isp_m0_chardev_release(struct inode *inode, struct file *file)
{
    struct IMPISPDev *dev = file->private_data;

    if (!dev) {
        return -EINVAL;
    }

    // First disable tuning if it's enabled
    if (dev->tuning_enabled == 2) {
        pr_info("Disabling tuning on release\n");
        isp_core_tuning_release(dev);
        dev->tuning_enabled = 0;
    }

    // Clear private_data
    file->private_data = NULL;

    // Note: Don't free tuning_data here as it might be used by other opens
    // It will be freed when the driver is unloaded

    pr_info("ISP M0 device released\n");
    return 0;
}

static bool is_frame_wait_needed(void)
{
    if (ourISPdev && ourISPdev->frame_wait_cnt > 0) {
        ourISPdev->frame_wait_cnt--;
        wmb();
        return true;
    }
    return false;
}


// Helper for frame wait after flip operations
static void set_framesource_changewait_cnt(void)
{
    if (ourISPdev) {
        ourISPdev->frame_wait_cnt = 3;  // Wait 3 frames for flip to take effect
        wmb();  // Ensure count update is visible
    }
}


// Read AE histogram data from hardware
static int tisp_g_ae_hist(void *buf)
{
    struct ae_hist_data *hist = (struct ae_hist_data *)buf;
    int i;
    u32 reg_val;

    if (!hist) {
        pr_err("Invalid histogram buffer\n");
        return -EINVAL;
    }

    // Read histogram data
    for (i = 0; i < 256; i++) {
        reg_val = system_reg_read(ISP_AE_HIST_BASE + (i * 4));
        hist->histogram[i] = reg_val;
    }

    // Read additional statistics
    for (i = 0; i < 5; i++) {
        reg_val = system_reg_read(ISP_AE_HIST_BASE + 0x400 + (i * 4));
        hist->stats[i * 4] = reg_val & 0xFF;
        hist->stats[i * 4 + 1] = (reg_val >> 8) & 0xFF;
        hist->stats[i * 4 + 2] = (reg_val >> 16) & 0xFF;
        hist->stats[i * 4 + 3] = (reg_val >> 24) & 0xFF;
    }

    // Read status bytes at specific offsets (0x414, 0x418, etc)
    hist->status[0] = system_reg_read(ISP_AE_HIST_BASE + 0x414) & 0xFF;
    hist->status[1] = system_reg_read(ISP_AE_HIST_BASE + 0x418) & 0xFF;
    hist->status[2] = system_reg_read(ISP_AE_HIST_BASE + 0x41c) & 0xFF;
    hist->status[3] = system_reg_read(ISP_AE_HIST_BASE + 0x420) & 0xFF;
    hist->status[4] = system_reg_read(ISP_AE_HIST_BASE + 0x424) & 0xFF;
    hist->status[5] = system_reg_read(ISP_AE_HIST_BASE + 0x428) & 0xFF;

    return 0;
}

// Read AE state from hardware
static int tisp_get_ae_state(struct ae_state_info *state)
{
    if (!state) {
        pr_err("Invalid AE state buffer\n");
        return -EINVAL;
    }

    // Read current exposure value
    state->exposure = system_reg_read(ISP_AE_STATE_BASE + 0x00);

    // Read current gain value
    state->gain = system_reg_read(ISP_AE_STATE_BASE + 0x04);

    // Read status flags
    state->status = system_reg_read(ISP_AE_STATE_BASE + 0x08);

    return 0;
}

// Debug helper functions
static void dump_ae_hist(struct ae_hist_data *hist)
{
    int i;

    pr_info("AE Histogram dump:\n");
    for (i = 0; i < 256; i += 8) {
        pr_info("[%3d] %08x %08x %08x %08x %08x %08x %08x %08x\n",
                i,
                hist->histogram[i],
                hist->histogram[i+1],
                hist->histogram[i+2],
                hist->histogram[i+3],
                hist->histogram[i+4],
                hist->histogram[i+5],
                hist->histogram[i+6],
                hist->histogram[i+7]);
    }

    pr_info("AE Stats: ");
    for (i = 0; i < 0x14; i++) {
        pr_info("%02x ", hist->stats[i]);
    }
    pr_info("\nAE Status: ");
    for (i = 0; i < 0x18; i++) {
        pr_info("%02x ", hist->status[i]);
    }
    pr_info("\n");
}

static void dump_ae_state(struct ae_state_info *state)
{
    pr_info("AE State:\n");
    pr_info("  Exposure: %u\n", state->exposure);
    pr_info("  Gain: %u\n", state->gain);
    pr_info("  Status: 0x%08x\n", state->status);
}

static int isp_get_ae_state(struct IMPISPDev *dev, struct isp_tuning_ctrl *ctrl)
{
    struct ae_state_info state;

    if (!ctrl->data) {
        pr_err("No data pointer for AE state\n");
        return -EINVAL;
    }

    // Get AE state from hardware
    int ret = tisp_get_ae_state(&state);
    if (ret) {
        return ret;
    }

    // Copy state data to user-provided buffer
    if (copy_to_user((void __user *)(unsigned long)ctrl->data,
                     &state, sizeof(state))) {
        return -EFAULT;
                     }

    // Set success in control value
    ctrl->value = 1;
    return 0;
}

static int framechan_get_attr(struct isp_channel *chn,
                            struct imp_channel_attr *attr) {
    if (!chn || !attr) {
        pr_err("Invalid parameters in get attributes\n");
        return -EINVAL;
    }

    // Ensure attributes are initialized (firmware checks this)
    if (!chn->attr.enable) {
        pr_err("Channel attributes not initialized\n");
        return -EINVAL;
    }

    // Copy exactly 0x50 bytes of attribute data
    memcpy(attr, &chn->attr, sizeof(struct imp_channel_attr));

    return 0;
}


// Add IOCTL handler for getting attributes
static long frame_channel_vidioc_get_fmt(struct isp_channel *chn, void __user *arg)
{
    struct imp_channel_attr attr;
    int ret;

    pr_info("Get format entry: arg=%p\n", arg);

    if (!chn) {
        pr_err("Invalid fs or chn\n");
        return -EINVAL;
    }

    ret = framechan_get_attr(chn, &attr);
    if (ret) {
        return ret;
    }

    // Copy to user space
    if (copy_to_user(arg, &attr, sizeof(attr))) {
        pr_err("Failed to copy format struct to user\n");
        return -EFAULT;
    }

    return 0;
}

static int isp_get_ae_zone(struct IMPISPDev *dev, struct isp_tuning_ctrl *ctrl)
{
    struct ae_zone_info info;
    void *hist_buf;
    int ret = 0;

    if (!ctrl->data) {
        pr_err("No data pointer for AE zone\n");
        return -EINVAL;
    }

    // Allocate temporary buffer for histogram data
    hist_buf = kzalloc(0x42c, GFP_KERNEL);
    if (!hist_buf) {
        pr_err("Failed to allocate ae_hist buffer\n");
        return -ENOMEM;
    }

    // Get AE histogram data
    tisp_g_ae_hist(hist_buf);

    // Copy data from histogram buffer to info structure
    info.additional[0] = *((u8 *)(hist_buf + 0x414));
    info.additional[1] = *((u8 *)(hist_buf + 0x418));
    info.additional[2] = *((u8 *)(hist_buf + 0x41c));
    info.additional[3] = *((u8 *)(hist_buf + 0x420));

    info.metrics[0] = *((u16 *)(hist_buf + 0x400));
    info.metrics[1] = *((u16 *)(hist_buf + 0x404));
    info.metrics[2] = *((u16 *)(hist_buf + 0x408));
    info.metrics[3] = *((u16 *)(hist_buf + 0x40c));

    // Copy zone data to user-provided buffer
    if (copy_to_user((void __user *)(unsigned long)ctrl->data,
                     &info, sizeof(info))) {
        ret = -EFAULT;
                     } else {
                         ctrl->value = 1;  // Success
                     }

    kfree(hist_buf);
    return ret;
}


// Update AF zone function to fill complete info
static int tisp_af_get_zone(void)
{
    int i;
    u32 reg_val;

    // Read zone metrics from hardware registers
    for (i = 0; i < MAX_AF_ZONES; i++) {
        reg_val = system_reg_read(ISP_AF_ZONE_BASE + (i * 4));
        af_zone_data.zone_metrics[i] = reg_val;
    }

    // Read AF status
    af_zone_data.status = system_reg_read(ISP_AF_ZONE_BASE + 0x40);

    return 0;
}

// Wrapper function called by tuning interface
static int tisp_g_af_zone(void)
{
    return tisp_af_get_zone();
}

// AF zone get control function
static int apical_isp_af_zone_g_ctrl(u32 user_ptr)
{
    int ret;

    // Get latest zone data from hardware
    ret = tisp_g_af_zone();
    if (ret) {
        pr_err("Failed to get AF zone data\n");
        return ret;
    }

    // Copy zone data to user
    if (copy_to_user((void __user *)user_ptr, &af_zone_data, sizeof(af_zone_data))) {
        pr_err("Failed to copy AF zone data to user\n");
        return -EFAULT;
    }

    return 0;
}

static int apical_isp_ae_hist_origin_g_attr(struct isp_tuning_ctrl *ctrl)
{
    void *hist_buf;
    void *hist_data;
    int ret = 0;

    if (!ctrl->data) {
        pr_err("No data pointer for AE histogram\n");
        return -EINVAL;
    }

    // Allocate temporary buffer
    hist_buf = kzalloc(AE_HIST_BUF_SIZE, GFP_KERNEL);
    if (!hist_buf) {
        pr_err("Failed to allocate ae_hist buffer\n");
        return -ENOMEM;
    }

    // Get histogram data
    tisp_g_ae_hist(hist_buf);

    // Allocate space for user data
    hist_data = kzalloc(AE_HIST_SIZE, GFP_KERNEL);
    if (!hist_data) {
        pr_err("Failed to allocate histogram data buffer\n");
        kfree(hist_buf);
        return -ENOMEM;
    }

    // Copy histogram data
    memcpy(hist_data, hist_buf, AE_HIST_SIZE);

    // Copy to user space
    if (copy_to_user((void __user *)(unsigned long)ctrl->data,
                     hist_data, AE_HIST_SIZE)) {
        ret = -EFAULT;
                     } else {
                         ctrl->value = 1;  // Success
                     }

    kfree(hist_data);
    kfree(hist_buf);
    return ret;
}


// Helper functions to update AF zone data
static void update_af_zone_data(struct af_zone_info *info)
{
    info->zone_status = af_zone_data.status;
    memcpy(info->zone_metrics, af_zone_data.zone_metrics,
           sizeof(uint32_t) * MAX_AF_ZONES);
}


// Update the AF zone get function
static int isp_get_af_zone(struct IMPISPDev *dev, struct isp_tuning_ctrl *ctrl)
{
    struct af_zone_info zones;
    int ret;

    if (!ctrl->data) {
        pr_err("No data pointer for AF zone\n");
        return -EINVAL;
    }

    // Clear structure first
    memset(&zones, 0, sizeof(zones));

    // Get latest zone data
    ret = tisp_af_get_zone();
    if (ret) {
        return ret;
    }

    // Fill in the complete zone info
    update_af_zone_data(&zones);

    // Copy zone data to user-provided buffer
    if (copy_to_user((void __user *)(unsigned long)ctrl->data,
                     &zones, sizeof(zones))) {
        return -EFAULT;
                     }

    // Set success status
    ctrl->value = 1;
    return 0;
}


// Main get control function that handles all tuning gets
static long isp_tuning_get_ctrl(struct IMPISPDev *dev, void __user *arg)
{
    struct {
        u32 cmd;
        u32 value;  // Pointer to user buffer
    } ctrl;
    int ret = 0;

    if (copy_from_user(&ctrl, arg, sizeof(ctrl))) {
        return -EFAULT;
    }

    // Check tuning state
//    if (dev->tuning_enabled != 2) {
//        pr_err("ISP tuning not in correct state (state=%d)\n", dev->tuning_enabled);
//        return -EINVAL;
//    }

    switch (ctrl.cmd) {
        ret = isp_tuning_get_ctrl(dev, arg);
    }

    if (ret) {
        pr_err("%s(%d),ioctl IMAGE_TUNING_CID_AE_ZONE error\n",
               __func__, __LINE__);
    }

    return ret;
}

static int handle_isp_set_ctrl(struct isp_core_ctrl *ctrl, void __user *arg)
{
    struct isp_tuning_data *tuning;
    struct isp_flip_ioctl flip_ctrl;
    struct isp_ctrl_msg msg;
    u8 value = ctrl->value & 0xFF;  // Extract 8-bit value
    int ret = 0;

    pr_info("ISP set control: cmd=0x%x value=%d\n", ctrl->cmd, ctrl->value);

    if (!ourISPdev || !ourISPdev->tuning_data) {
        pr_err("No ISP device or tuning data\n");
        return -EINVAL;
    }

    tuning = ourISPdev->tuning_data;

    switch (ctrl->cmd) {
        case 0x980900:  // Brightness
            tuning->brightness = value;
            pr_info("Set brightness to %d\n", value);
            break;

        case 0x980901:  // Contrast
            tuning->contrast = value;
            pr_info("Set contrast to %d\n", value);
            break;

        case 0x980902:  // Saturation
            tuning->saturation = value;
            pr_info("Set saturation to %d\n", value);
            break;

        case 0x98091b:  // Sharpness
            tuning->sharpness = value;
            pr_info("Set sharpness to %d\n", value);
            break;

        case 0x980914:  // HFLIP
        case 0x980915:  // VFLIP
            // Set flip value in tuning data
            if (ctrl->cmd == 0x980914) {
                tuning->hflip = ctrl->value ? 1 : 0;
            } else {
                tuning->vflip = ctrl->value ? 1 : 0;
            }
            set_framesource_changewait_cnt();

            // Copy back to user
            if (copy_to_user(arg, &msg, sizeof(msg))) {
                pr_err("Failed to copy flip control to user\n");
                return -EFAULT;
            }
            return 0;

        case ISP_CTRL_BYPASS: {  // 0x8000164
            struct {
                u32 cmd;
                u32 value;
            } bypass_ctrl;
            u32 reg_val;

            // Disable links by clearing link enable register
            writel(0, ourISPdev->reg_base + ISP_LINK_ENABLE_REG);
            wmb();  // Ensure write completes

            // Set control structure
//            bypass_ctrl.cmd = ISP_CTRL_BYPASS;
//            bypass_ctrl.value = ctrl->value;

            // Set bypass mode in hardware
            reg_val = readl(ourISPdev->reg_base + ISP_BYPASS_REG);
            if (ctrl->value) {
                reg_val |= BIT(0);  // Enable bypass
                reg_val &= ~BIT(1); // Disable processing
            } else {
                reg_val &= ~BIT(0); // Disable bypass
                reg_val |= BIT(1);  // Enable processing
            }
            writel(reg_val, ourISPdev->reg_base + ISP_BYPASS_REG);
            wmb();  // Ensure write completes

            // Re-enable links
            writel(1, ourISPdev->reg_base + ISP_LINK_ENABLE_REG);
            wmb();  // Ensure write completes

            // Enable routing
            writel(1, ourISPdev->reg_base + ISP_ROUTE_REG);
            wmb();  // Ensure write completes

            // Update device state
            ourISPdev->bypass_enabled = !!ctrl->value;
            pr_info("Set bypass mode to %d\n", ctrl->value);

            // Copy back to user
            if (copy_to_user(arg, &bypass_ctrl, sizeof(bypass_ctrl))) {
                pr_err("Failed to copy bypass control to user\n");
                return -EFAULT;
            }
            break;
        }

        case ISP_CTRL_ANTIFLICKER: {  // 0x980918
            struct {
                u32 cmd;      // 0x980918
                u32 value;    // Antiflicker value
            } antiflick_ctrl;

            // Validate value
            if (ctrl->value > 2) {
                pr_err("Invalid antiflicker value: %d\n", ctrl->value);
                return -EINVAL;
            }

            // Update tuning data
            tuning->antiflicker = ctrl->value;

            // Set control structure
            antiflick_ctrl.cmd = ISP_CTRL_ANTIFLICKER;
            antiflick_ctrl.value = ctrl->value;

            // Copy back to user
            if (copy_to_user(arg, &antiflick_ctrl, sizeof(antiflick_ctrl))) {
                pr_err("Failed to copy antiflicker control to user\n");
                return -EFAULT;
            }

            pr_info("Set antiflicker mode to %d\n", ctrl->value);
            break;
        }

        default:
            pr_info("Unknown ISP control command: 0x%x\n", ctrl->cmd);
            return -EINVAL;
    }

    return 0;
}

static int handle_isp_get_ctrl(struct isp_core_ctrl *ctrl, void __user *arg)
{
    struct isp_tuning_data *tuning;
    pr_info("Get control: cmd=0x%x value=%d\n", ctrl->cmd, ctrl->value);

    if (!ourISPdev || !ourISPdev->tuning_data) {
        pr_err("No ISP device or tuning data\n");
        return -EINVAL;
    }

    tuning = ourISPdev->tuning_data;

    if (copy_to_user(arg, &tuning, sizeof(tuning)))
        return -EFAULT;

    return 0;

    pr_info("Get control 0x%x = %d\n", ctrl->cmd, ctrl->value);
    return 0;
}

int set_framesource_fps(int32_t num, int32_t den)
{
    struct IMPISPDev *dev = ourISPdev;
    struct isp_channel *chn;

    if (!dev)
        return -EINVAL;

    // Update main frame source
    chn = &dev->channels[0];
    chn->attr.fps_num = num;
    chn->attr.fps_den = den;

    return 0;
}

static int get_isp_total_gain(uint32_t *gain)
{
    if (!ourISPdev || !ourISPdev->reg_base) {
        pr_err("No ISP device for gain read\n");
        return -EINVAL;
    }

    // Read gain from hardware registers
    *gain = readl(ourISPdev->reg_base + ISP_TOTAL_GAIN_REG);
    return 0;
}

static int isp_tuning_enable(struct IMPISPDev *dev, void __user *arg)
{
    struct {
        uint32_t enable;
        uint32_t state;
    } __packed req = {0};  // Zero initialize
    int ret = 0;

    if (!dev) {
        pr_err("No ISP device\n");
        return -EINVAL;
    }

    // Read just enable flag first
    if (copy_from_user(&req.enable, arg, sizeof(req.enable)))
        return -EFAULT;

    pr_info("Tuning enable: %d\n", req.enable ? 1 : 0);

    mutex_lock(&tuning_mutex);

    // Get current state, don't modify if already in requested state
    req.state = dev->tuning_data->state;

    if (req.enable) {
        if (req.state != 2) { // Only if not already enabled
            if (!dev->tuning_data->regs) {
                dev->tuning_data->regs = ioremap(0x13380000, 0x1b000);
                if (!dev->tuning_data->regs) {
                    ret = -ENOMEM;
                    goto out_unlock;
                }
            }
            dev->tuning_data->state = 2;
            req.state = 2;
        }
    } else {
        if (req.state == 2) { // Only if currently enabled
            if (dev->tuning_data->regs) {
                iounmap(dev->tuning_data->regs);
                dev->tuning_data->regs = NULL;
            }
            dev->tuning_data->state = 0;
            req.state = 0;
        }
    }

    // Always copy back the current state
    if (copy_to_user(arg, &req, sizeof(req)))
        ret = -EFAULT;

    out_unlock:
        mutex_unlock(&tuning_mutex);
    return ret;
}

/*
 * ISP-M0 IOCTL handler
* ISP_CORE_S_CTRL: Set control 0xc008561c
* ISP_CORE_G_CTRL: Get control 0xc008561b
* ISP_TUNING_ENABLE: Enable tuning 0xc00c56c6
 */
static int isp_m0_chardev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct IMPISPDev *dev = ourISPdev;
    struct isp_tuning_data *tuning = dev->tuning_data;
    struct isp_core_ctrl ctrl;
    struct isp_flip_ioctl flip_ctrl;
    int ret = 0;
    uint32_t value;

    pr_info("ISP m0 IOCTL called: cmd=0x%x\n", cmd);
    if (!dev) {
        pr_err("No ISP device\n");
        return -EINVAL;
    }
    if (!dev->tuning_data) {
        pr_err("No ISP tuning data\n");
        return -EINVAL;
    }

    switch(cmd) {
        case 0xc00c56c6: {  // Tuning IOCTL
            ret = isp_tuning_enable(dev, (void __user *)arg);
            if (ret) {
                pr_err("Failed to enable/disable tuning\n");
                return ret;
            }
            return ret;
        }
        // TODO OEM driver has a lot of implementations under this one tuning cmd
        case 0xc008561c: { // ISP_CORE_S_CTRL
            if (!dev || !dev->tuning_data) {
                pr_err("No ISP device or tuning data\n");
                return -EINVAL;
            }

            // First just read the command
            if (copy_from_user(&cmd, (void __user *)arg, sizeof(cmd))) {
                pr_err("Failed to copy command from user\n");
                return -EFAULT;
            }

            pr_info("SET cmd=0x%x\n", cmd);

            uint32_t ctrl_id = (cmd >> 16) & 0xFF;
            uint32_t ctrl_cmd = cmd & 0xFFFF;

            if (ctrl_id == 0x98 && (ctrl_cmd == 0x980914 || ctrl_cmd == 0x980915)) {
                // It's a flip command, read just one more uint32_t for the value
                if (copy_from_user(&value, (void __user *)arg + sizeof(cmd), sizeof(value))) {
                    return -EFAULT;
                }

                pr_info("FLIP: cmd=0x%x value=%d\n", cmd, value);

                // Handle flip
                if (ctrl_cmd == 0x0914) {  // HFLIP
                    writel(value ? 1 : 0, dev->tuning_data->regs + 0x3ad * 4);
                } else {  // VFLIP
                    writel(value ? 1 : 0, dev->tuning_data->regs + 0x3ac * 4);
                }
                set_framesource_changewait_cnt();
                return 0;
            }

            // Not a flip command, read the rest of the control structure
            struct isp_core_ctrl ctrl;
            ctrl.cmd = cmd;  // Use command we already read
            if (copy_from_user(&ctrl.value, (void __user *)arg + sizeof(cmd),
                              sizeof(ctrl) - sizeof(cmd))) {
                pr_err("Failed to copy rest of control from user\n");
                return -EFAULT;
            }
            ret = handle_isp_set_ctrl(&ctrl, (void __user *)arg);
            if (ret) {
                pr_err("Failed to handle ISP set control\n");
                return ret;
            }

            if (copy_to_user((void __user *)arg, &ctrl, sizeof(ctrl))) {
                pr_err("Failed to copy control back to user\n");
                return -EFAULT;
            }

            return 0;
        }

        case 0xc008561b: { // ISP_CORE_G_CTRL
              pr_info("ISP_CORE_G_CTRL GET cmd=0x%x\n", cmd);
            return frame_channel_vidioc_get_fmt(&dev->channels[0], (void __user *)arg);
        }
        default:
            pr_info("Unknown ISP-M0 IOCTL: 0x%x\n", cmd);
            ret = -ENOTTY;
    }

    return ret;
}


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

// Define module configurations that match what we see being written
struct isp_module_config {
    u32 base_offset;      // Register base offset
    u32 type;            // Module type identifier
    u32 module_id;       // Module ID for linking
    u32 link_mask;       // Linking info
    void *regs;          // Register mapping
    void *priv;          // Private data
};

struct vic_module_config {
    u32 base_offset;     // VIC register offset
    u32 type;           // VIC type identifier
    u32 module_id;      // VIC module ID
    u32 link_mask;      // VIC linking info
    void *regs;         // VIC register mapping
    void *priv;         // VIC private data
};

// Define static configurations
static struct isp_module_config isp_config = {
    .base_offset = 0x0,
    .type = 1,           // Base module type
    .module_id = 0,      // ISP ID
    .link_mask = 0x1,    // Can link to VIC
};

static struct vic_module_config vic_config = {
    .base_offset = 0x300,  // VIC starts at 0x300 offset
    .type = 1,            // Base module type
    .module_id = 1,       // VIC ID
    .link_mask = 0x2,     // Can be linked to
};



#define ISP_MODULE_TYPE_BASE  1  // Base modules like ISP/VIC
#define ISP_MODULE_TYPE_LINK  2  // Link modules that connect them

struct isp_module_info {
    u32 type;                // 1=base, 2=link
    u32 module_id;                  // Module ID for indexing
    u32 link_id;            // For type 2 - which modules to link
    void *config;           // Module specific config
    struct list_head list;  // For module tracking
};



static irqreturn_t isp_irq_handler(int irq, void *dev_id)
{
    struct IMPISPDev *dev = dev_id;
    void __iomem *vic_regs = dev->reg_base + 0x7800;
    u32 status;
    bool handled = false;

    // Read status register at 0xb4
    status = readl(vic_regs + 0xb4);  // 0xb4

    pr_info("ISP IRQ: status=0x%x enable=0x%x mask=0x%x\n",
            status,
            readl(vic_regs + 0x13C),     // 0x13c
            readl(vic_regs + VIC_IRQ_MASK));   // 0x1e8

    if (status) {
        // Clear status at both registers
        writel(status, vic_regs + 0xb4);     // 0xb4
        writel(status, vic_regs + VIC_MODE);       // 0x0c - Mode register also needs clearing
        wmb();
        handled = true;
    }

    return handled ? IRQ_HANDLED : IRQ_NONE;
}

void tx_isp_disable_irq(struct IMPISPDev *dev)
{
    unsigned long flags;
    void __iomem *vic_regs = dev->reg_base + 0x7800;
    pr_info("Disabling ISP IRQs\n");

    spin_lock_irqsave(&dev->irq_data->lock, flags);

    if (dev->irq_enabled) {
        // Disable VIC interrupts
        writel(0, vic_regs + 0x13C);      // 0x13c
        writel(0, vic_regs + VIC_MODE);        // 0x0c
        writel(0, vic_regs + VIC_IRQ_MASK);    // 0x1e8
        wmb();
        dev->irq_enabled = 0;
    }

    spin_unlock_irqrestore(&dev->irq_data->lock, flags);
}

void tx_vic_disable_irq(struct IMPISPDev *dev)
{
    unsigned long flags;
    void __iomem *vic_regs = dev->reg_base + 0x7800;
    pr_info("Disabling VIC IRQs\n");

    spin_lock_irqsave(&dev->lock, flags);

    if (dev->irq_enabled) {
        // Disable VIC interrupts
        writel(0, vic_regs + 0x13C);      // 0x13c
        writel(0, vic_regs + VIC_MODE);        // 0x0c
        writel(0, vic_regs + VIC_IRQ_MASK);    // 0x1e8
        wmb();
        dev->irq_enabled = 0;
    }

    spin_unlock_irqrestore(&dev->lock, flags);
}

static irqreturn_t isp_irq_thread_handle(int irq, void* interrupt_data)
{
    struct irq_handler_data *handler_data = (struct irq_handler_data *)interrupt_data;
    struct irq_task *current_task;
    int i;

    // Validate input
    if (!handler_data) {
        pr_err("NULL interrupt data in thread handler\n");
        return IRQ_NONE;
    }

    pr_info("Thread IRQ handler: data=%p\n", handler_data);

    // Iterate through task array
    for (i = 0; i < MAX_TASKS; i++) {
        current_task = &handler_data->task_list[i];

        // Skip empty task slots
        if (!current_task->task_function)
            continue;

        // Execute task if status is set
        if (current_task->status != 0) {
            pr_info("Executing task %d: func=%p\n", i, current_task->task_function);
            current_task->task_function();
        }
    }

    return IRQ_HANDLED;
}

int tx_isp_enable_irq(struct IMPISPDev *dev)
{
    unsigned long flags;
    void __iomem *vic_regs = dev->reg_base + 0x7800;
    void __iomem *isp_regs = dev->reg_base + ISP_BASE;

    spin_lock_irqsave(&dev->lock, flags);

    if (!dev->irq_enabled) {
        // Enable frame control
        writel(1, vic_regs + VIC_MODE);  // 0x0c instead of 0x008
        wmb();
        udelay(10);

        // Enable VIC IRQs
        writel(VIC_INT_ALL_MASK, vic_regs + VIC_IRQ_MASK);  // 0x1e8
        writel(VIC_INT_ALL_MASK, vic_regs + 0x13C);    // 0x13c
        wmb();
        udelay(10);

        // Enable ISP IRQs
        writel(0x3307, isp_regs + ISP_INT_MASK_OFFSET);
        wmb();

        dev->irq_enabled = 1;

        pr_info("IRQs enabled: mask=0x%x irq_en=0x%x mode=0x%x\n",
                readl(vic_regs + VIC_IRQ_MASK),     // 0x1e8
                readl(vic_regs + 0x13C),       // 0x13c
                readl(vic_regs + VIC_MODE));        // 0x0c
    }

    spin_unlock_irqrestore(&dev->lock, flags);
    return 0;
}

int32_t tx_isp_request_irq(struct platform_device *pdev, struct irq_handler_data* irqHandlerData)
{
    pr_info("Requesting IRQ: pdev=%p data=%p\n", pdev, irqHandlerData);

    if (!pdev || !irqHandlerData) {
        pr_err("%s: Invalid parameters\n", __func__);
        return 0xffffffea;
    }

    // Get IRQ directly from platform device
    int32_t irqNumber = platform_get_irq(pdev, 0);
    if (irqNumber < 0) {
        pr_err("Failed to get platform IRQ\n");
        irqHandlerData->irq_number = 0;
        return irqNumber;
    }

    // Initialize spinlock
    spin_lock_init(&irqHandlerData->lock);

    // Initialize task list
    memset(irqHandlerData->task_list, 0, sizeof(struct irq_task) * MAX_TASKS);

    // Request IRQ with standard flags
    if (request_threaded_irq(irqNumber,
                            isp_irq_handler,
                            isp_irq_thread_handle,
                            IRQF_SHARED,
                            "isp_driver",
                            irqHandlerData) != 0) {
        pr_err("Failed to request IRQ %d\n", irqNumber);
        irqHandlerData->irq_number = 0;
        return 0xfffffffc;
    }

    // Setup handler functions
    irqHandlerData->handler_function = tx_isp_enable_irq;
    irqHandlerData->irq_number = irqNumber;
    irqHandlerData->disable_function = tx_isp_disable_irq;

    tx_isp_disable_irq(irqHandlerData);

    pr_info("IRQ setup complete:\n"
            "  Number: %d\n"
            "  Handler: %p\n"
            "  Disable: %p\n"
            "  Task List: %p (count=%d)\n",
            irqHandlerData->irq_number,
            irqHandlerData->handler_function,
            irqHandlerData->disable_function,
            irqHandlerData->task_list,
            irqHandlerData->task_count);

    return 0;
}





/* Define the proc entry configuration structure */
struct isp_proc_entry_config {
    const char *name;
    const struct proc_ops *fops;
    int (*show_func)(struct seq_file *seq, void *v);
};

/* Define all proc entries statically */
static const struct isp_proc_entry_config isp_proc_configs[] = {
    { "isp-fs",  &isp_fs_fops,  isp_fs_show  },
    { "isp-m0",  &isp_m0_fops,  isp_m0_show  },
    { "isp-w00", &isp_w00_fops, isp_w00_show },
    { "isp-w01", &isp_w00_fops, isp_w00_show }, // Using w00 show function as per original
    { "isp-w02", &isp_w02_fops, isp_w02_show },
    { NULL, NULL, NULL } // Sentinel to mark end of array
};

/* Structure to track created proc entries for cleanup */
struct isp_proc_context {
    struct proc_dir_entry *proc_dir;
    struct isp_proc_data *proc_data[ARRAY_SIZE(isp_proc_configs) - 1];
    struct IMPISPDev *isp_dev;
};

static int create_isp_proc_entries(struct IMPISPDev *isp_dev)
{
    struct isp_proc_context *ctx;
    const struct isp_proc_entry_config *config;
    int i = 0;
    int ret = 0;

    /* Allocate context structure */
    ctx = devm_kzalloc(isp_dev->dev, sizeof(*ctx), GFP_KERNEL);
    if (!ctx)
        return -ENOMEM;

    ctx->isp_dev = isp_dev;

    /* Create the /proc/jz/isp directory */
    ctx->proc_dir = proc_mkdir("jz/isp", NULL);
    if (!ctx->proc_dir) {
        dev_err(isp_dev->dev, "Failed to create /proc/jz/isp directory\n");
        ret = -ENOENT;
        goto cleanup;
    }

    /* Create all configured proc entries */
    for (config = isp_proc_configs; config->name; config++, i++) {
        struct isp_proc_data *proc_data;

        proc_data = devm_kzalloc(isp_dev->dev, sizeof(*proc_data), GFP_KERNEL);
        if (!proc_data) {
            dev_err(isp_dev->dev, "Failed to allocate proc_data for %s\n", config->name);
            ret = -ENOMEM;
            goto cleanup;
        }

        proc_data->show_func = config->show_func;
        proc_data->dev = isp_dev;
        ctx->proc_data[i] = proc_data;

        if (!proc_create_data(config->name, 0444, ctx->proc_dir,
                            config->fops, proc_data)) {
            dev_err(isp_dev->dev, "Failed to create /proc/jz/isp/%s\n", config->name);
            ret = -ENOMEM;
            goto cleanup;
        }
    }

    /* Store context in device structure for later cleanup */
    isp_dev->proc_context = ctx;
    return 0;

cleanup:
    if (ctx->proc_dir) {
        for (config = isp_proc_configs; config->name; config++) {
            remove_proc_entry(config->name, ctx->proc_dir);
        }
        remove_proc_entry("jz/isp", NULL);
    }
    return ret;
}

static void cleanup_isp_proc_entries(struct IMPISPDev *isp_dev)
{
    struct isp_proc_context *ctx = isp_dev->proc_context;
    const struct isp_proc_entry_config *config;

    if (!ctx)
        return;

    if (ctx->proc_dir) {
        for (config = isp_proc_configs; config->name; config++) {
            remove_proc_entry(config->name, ctx->proc_dir);
        }
        remove_proc_entry("jz/isp", NULL);
    }
}

static struct platform_device *create_isp_subdev(struct tx_isp_device_info *info)
{
    struct platform_device *pdev;
    struct resource *res;
    int ret;

    if (!info || !info->name) {
        pr_err("Invalid device info\n");
        return ERR_PTR(-EINVAL);
    }

    // Allocate platform device
    pdev = platform_device_alloc(info->name, -1);
    if (!pdev) {
        pr_err("Failed to allocate platform device\n");
        return ERR_PTR(-ENOMEM);
    }

    // Add resources if memory region specified
    if (info->mem_start && info->mem_size) {
        res = kzalloc(sizeof(*res), GFP_KERNEL);
        if (!res) {
            ret = -ENOMEM;
            goto err_free_device;
        }

        res[0].start = info->mem_start;
        res[0].end = info->mem_start + info->mem_size - 1;
        res[0].flags = IORESOURCE_MEM;
        res[0].name = info->name;

        ret = platform_device_add_resources(pdev, res, 1);
        if (ret) {
            pr_err("Failed to add resources: %d\n", ret);
            kfree(res);
            goto err_free_device;
        }
    }

    // Add IRQ if specified
    if (info->irq >= 0) {
        res = kzalloc(sizeof(*res), GFP_KERNEL);
        if (!res) {
            ret = -ENOMEM;
            goto err_free_device;
        }

        res[0].start = info->irq;
        res[0].end = info->irq;
        res[0].flags = IORESOURCE_IRQ;
        res[0].name = info->name;

        ret = platform_device_add_resources(pdev, res, 1);
        if (ret) {
            pr_err("Failed to add IRQ resource: %d\n", ret);
            kfree(res);
            goto err_free_device;
        }
    }

    // Add platform data if provided
    if (info->plat_data) {
        ret = platform_device_add_data(pdev, info->plat_data,
                                     sizeof(struct isp_platform_data));
        if (ret) {
            pr_err("Failed to add platform data: %d\n", ret);
            goto err_free_device;
        }
    }

    // Setup platform data from main ISP device
    platform_set_drvdata(pdev, ourISPdev);

    // Add the device
    ret = platform_device_add(pdev);
    if (ret) {
        pr_err("Failed to add platform device: %d\n", ret);
        goto err_free_device;
    }

    pr_info("Created ISP subdevice: %s\n", info->name);
    return pdev;

err_free_device:
    platform_device_put(pdev);
    return ERR_PTR(ret);
}

// Define ops for frame source
static struct tx_isp_subdev_core_ops fs_core_ops = {
    .init = NULL,
    .reset = NULL,
    .g_chip_ident = NULL,
    .g_register = NULL,
    .s_register = NULL,
};

static struct tx_isp_subdev_video_ops fs_video_ops = {
    .s_stream = NULL,
};

static struct tx_isp_subdev_sensor_ops fs_sensor_ops = {
    .ioctl = NULL,
};

static struct tx_isp_subdev_ops fs_ops = {
    .core = &fs_core_ops,
    .video = &fs_video_ops,
    .sensor = &fs_sensor_ops,
};

static int tx_isp_fs_probe(struct platform_device *pdev)
{
    struct IMPISPDev *dev = platform_get_drvdata(pdev);
    struct tx_isp_subdev *sd;
    int ret;

    pr_info("Frame source probe called\n");

    if (!dev) {
        pr_err("No ISP device data in frame source probe\n");
        return -EINVAL;
    }

    // Allocate frame source subdevice
    sd = kzalloc(sizeof(*sd), GFP_KERNEL);
    if (!sd)
        return -ENOMEM;

    // Allocate input pads
    sd->num_inpads = 1;
    sd->inpads = kzalloc(sizeof(struct tx_isp_subdev_pad) * sd->num_inpads, GFP_KERNEL);
    if (!sd->inpads) {
        ret = -ENOMEM;
        goto err_free_sd;
    }

    // Allocate output pads
    sd->num_outpads = 1;
    sd->outpads = kzalloc(sizeof(struct tx_isp_subdev_pad) * sd->num_outpads, GFP_KERNEL);
    if (!sd->outpads) {
        ret = -ENOMEM;
        goto err_free_inpads;
    }

    // Initialize pad data
    for (int i = 0; i < sd->num_inpads; i++) {
        sd->inpads[i].sd = sd;
        sd->inpads[i].index = i;
        sd->inpads[i].type = TX_ISP_PADTYPE_INPUT;
        sd->inpads[i].state = TX_ISP_PADSTATE_FREE;
        sd->inpads[i].links_type = TX_ISP_PADLINK_CSI | TX_ISP_PADLINK_VIC;
    }

    for (int i = 0; i < sd->num_outpads; i++) {
        sd->outpads[i].sd = sd;
        sd->outpads[i].index = i;
        sd->outpads[i].type = TX_ISP_PADTYPE_OUTPUT;
        sd->outpads[i].state = TX_ISP_PADSTATE_FREE;
        sd->outpads[i].links_type = TX_ISP_PADLINK_VIC | TX_ISP_PADLINK_DDR;
    }

    // Store in platform device and IMPISPDev
    dev->fs_subdev = sd;
    dev->fs_pdev = pdev;
    platform_set_drvdata(pdev, dev);

    pr_info("Frame source probe complete: dev=%p sd=%p fs_pdev=%p\n",
            dev, sd, dev->fs_pdev);
    return 0;

    err_free_inpads:
        kfree(sd->inpads);
    err_free_sd:
        kfree(sd);
    return ret;
}

static int tx_isp_fs_remove(struct platform_device *pdev)
{
    struct IMPISPDev *dev = platform_get_drvdata(pdev);
    if (dev && dev->fs_subdev) {
        kfree(dev->fs_subdev);
        dev->fs_subdev = NULL;
    }
    return 0;
}


static int enable_csi_clocks(struct IMPISPDev *dev)
{
    void __iomem *cpm_base;
    u32 val;
    int ret;

    pr_info("Setting up CSI clocks and power\n");

    // Map CPM registers
    cpm_base = ioremap(CPM_BASE, 0x100);
    if (!cpm_base) {
        pr_err("Failed to map CPM registers\n");
        return -ENOMEM;
    }

    // Step 1: Assert CSI reset first
    val = readl(cpm_base + CPM_SRSR);
    val |= CPM_CSI_RST_MASK;  // Set reset bit
    writel(val, cpm_base + CPM_SRSR);
    wmb();
    msleep(10);

    // Step 2: Enable power domain
    val = readl(cpm_base + CPM_CLKGR1);
    val &= ~CPM_CSI_PWR_MASK;  // Clear power gate bit
    writel(val, cpm_base + CPM_CLKGR1);
    wmb();
    msleep(10);

    // Step 3: Configure CSI clock via clock divider
    // Set CSI clock to 125MHz (AHB/2)
    writel(0x1, cpm_base + CPM_CSICDR);
    wmb();
    msleep(1);

    // Step 4: Enable CSI clock gate
    val = readl(cpm_base + CPM_CLKGR);
    val &= ~CPM_CSI_CLK_MASK;  // Clear clock gate bit
    writel(val, cpm_base + CPM_CLKGR);
    wmb();
    msleep(10);

    // Step 5: De-assert CSI reset
    val = readl(cpm_base + CPM_SRSR);
    val &= ~CPM_CSI_RST_MASK;  // Clear reset bit
    writel(val, cpm_base + CPM_SRSR);
    wmb();
    msleep(10);

    // Debug output
    pr_info("Clock state after setup:\n");
    pr_info("  CLKGR: 0x%08x\n", readl(cpm_base + CPM_CLKGR));
    pr_info("  CLKGR1: 0x%08x\n", readl(cpm_base + CPM_CLKGR1));
    pr_info("  SRSR: 0x%08x\n", readl(cpm_base + CPM_SRSR));
    pr_info("  CSICDR: 0x%08x\n", readl(cpm_base + CPM_CSICDR));
    pr_info("  PCSR: 0x%08x\n", readl(cpm_base + CPM_PCSR));

    iounmap(cpm_base);

    // Step 6: Enable CSI clock through standard API
    if (dev->csi_clk) {
        ret = clk_prepare_enable(dev->csi_clk);
        if (ret) {
            pr_err("Failed to enable CSI clock: %d\n", ret);
            return ret;
        }
    }

    msleep(20); // Allow clocks to stabilize

    pr_info("CSI clocks and power enabled\n");
    return 0;
}

struct IspDeviceConfig {
    // Core configuration
    uint32_t version;          // 0x00: Device version
    uint32_t subdev_offset;    // 0x04: Points to sub device offset
    char *device_name;         // 0x08: Device name
    char padding[0x2C];        // 0x0C-0x38: Maintain OEM layout for now

    // Device state
    char state_data[0x40];     // 0x38-0x78: Device state data
    uint32_t active;           // 0x78: Activity flag
    void *notify_callback;     // 0x7c: Notification callback (tx_isp_notify)
    uint32_t misc_flags;       // 0x80: Miscellaneous flags
    uint32_t config_flags;     // 0x84: Configuration flags

    // Hardware resources
    void __iomem *regs;        // CSI register mapping
    struct resource *phy_res;   // PHY memory resource
    int irq;                   // IRQ handle
};


int tx_isp_csi_init(struct platform_device *pdev, struct IspDeviceConfig *cfg,
                   int param_value)
{
    struct resource *res;
    void __iomem *mapped_regs;
    int ret;

    if (!pdev || !cfg) {
        pr_err("%s: invalid parameters\n", __func__);
        return -EINVAL;
    }

    // Zero init entire structure
    memset(cfg, 0, sizeof(*cfg));


    void __iomem *csi_base;
    struct csi_device *csi_dev;

    // Allocate CSI device structure
    csi_dev = devm_kzalloc(&pdev->dev, sizeof(*csi_dev), GFP_KERNEL);
    if (!csi_dev) {
        dev_err(&pdev->dev, "Failed to allocate CSI device\n");
        return -ENOMEM;
    }

    // Map CSI registers
    mapped_regs = ioremap(CSI_BASE_ADDR, CSI_REG_SIZE);
    if (!mapped_regs) {
        pr_err("Failed to map CSI registers\n");
        return -ENOMEM;
    }

    // Initialize configuration
    cfg->notify_callback = tx_isp_notify;
    cfg->regs = mapped_regs;
    cfg->config_flags = 0x735e4;  // Required config value for now

    // Setup clocks first
    ret = enable_csi_clocks(ourISPdev);
    if (ret) {
        pr_err("Failed to enable CSI clocks\n");
        iounmap(mapped_regs);
        return ret;
    }

    // Request PHY memory region
    res = request_mem_region(0x10022000, 0x1000, "mipi-phy");
    if (!res) {
        pr_err("Failed to request PHY region\n");
        ret = -ENOMEM;
        goto err_disable_clocks;
    }

    cfg->phy_res = res;

    pr_info("CSI initialized: regs=%p phy_res=%p\n",
            cfg->regs, cfg->phy_res);

    return 0;

err_disable_clocks:
    // Add clock cleanup
    iounmap(mapped_regs);
    return ret;
}

int32_t isp_subdev_release_clks(struct IspSubdev* isp_subdev)
{
    // Check if there are any allocated clocks to release
    void* allocated_clocks = isp_subdev->allocated_clocks;
    if (allocated_clocks != 0)
    {
        void* allocated_clocks_1 = allocated_clocks;
        int32_t i = 0;

        // Loop through the clocks and release each one
        while (i < isp_subdev->num_clocks)
        {
            // Cast allocated_clocks_1 to a pointer to struct clk, and call clk_put
            clk_put(*(struct clk**)allocated_clocks_1);

            // Move to the next clock in the list (assuming each clock is a pointer to struct clk)
            i += 1;
            allocated_clocks_1 += sizeof(struct clk*);  // Move by the size of a pointer
        }

        // Free the allocated memory for clocks
        kfree(allocated_clocks);

        // Set the allocated_clocks pointer to NULL
        isp_subdev->allocated_clocks = NULL;
    }

    return 0;  // Return 0 as per the assembly code
}

/**
 * struct isp_reg_t - ISP register map
 * Based on actual hardware layout and decompiled access patterns
 */
struct isp_reg_t {
    /* Base registers - 0x00-0x0C */
    u32 ctrl;
    u32 status;
    u32 int_mask;
    u32 int_status;

    /* Core configuration - 0x10-0x1C */
    u32 config[4];

    /* Image size registers */
    u32 input_width;   /* Input width register */
    u32 input_height;  /* Input height register */
    u32 output_width;  /* Output width register */
    u32 output_height; /* Output height register */

    /* Image parameters */
    u32 reserved1[36]; /* Padding to 0x9C */
    u32 exposure;      /* 0x9C: Exposure register */
    u32 reserved2[3];
    u32 gain;          /* 0xAC: Gain register */
    u32 reserved3[12]; /* Padding to 0xE4 */
    u32 gain_factor;   /* 0xE4: Gain factor register */
    u32 reserved4[2];
    u32 exp_factor;    /* Fixed 0x400 value */

    /* WDR configuration */
    u32 reserved5[53];  /* Padding to 0x17C */
    u32 wdr_enable;    /* 0x17C: WDR enable register */
    u32 reserved6[31];  /* Additional padding */

    /* Statistics and algorithm registers */
    union {
        struct {
            u32 ae_stats[64];   /* AE statistics */
            u32 awb_stats[64];  /* AWB statistics */
            u32 af_stats[32];   /* AF statistics */
        };
        u32 stats_regs[160];   /* Combined stats access */
    };
};

/* Helper functions for register access */
static inline u32 isp_get_width(struct isp_reg_t *regs)
{
    return readl(&regs->output_width);
}

static inline u32 isp_get_height(struct isp_reg_t *regs)
{
    return readl(&regs->output_height);
}

/**
 * struct isp_awb_info - AWB information structure
 * Based on decompiled access patterns
 */
struct isp_awb_info {
    /* Current settings */
    u32 r_gain;
    u32 g_gain;
    u32 b_gain;

    /* Color temperature */
    u32 curr_ct;    /* Current color temperature */
    u32 target_ct;  /* Target color temperature */

    /* Algorithm state */
    u32 frame_cnt;
    u32 stable_cnt;
    u32 converge_cnt;

    /* Statistics */
    u32 r_avg;
    u32 g_avg;
    u32 b_avg;

    /* Reserved for future use */
    u32 reserved[16];
};


/**
 * struct isp_ae_algo - Auto Exposure algorithm parameters
 * Based on decompiled structure at case 0x800456dd
 */
struct isp_ae_algo {
    u32 magic;              /* Must be 0x336ac */
    u32 version;            /* Algorithm version */
    u32 params[30];         /* Algorithm parameters - size from decompiled */

    /* Window configuration */
    struct {
        u16 x;
        u16 y;
        u16 width;
        u16 height;
    } windows[16];          /* AE windows configuration */

    /* Control parameters */
    u32 target_luminance;
    u32 min_exposure;
    u32 max_exposure;
    u32 min_gain;
    u32 max_gain;

    /* Algorithm state */
    u32 current_luminance;
    u32 converge_cnt;
    u32 stable_cnt;

    /* Reserved for future use */
    u32 reserved[16];
};

/**
 * struct isp_ae_stats - AE statistics structure
 * Based on decompiled memory allocation size
 */
struct isp_ae_stats {
    /* Raw statistics from hardware */
    u32 hist[256];         /* Luminance histogram */
    u32 zones[16][16];     /* Zone-based statistics */

    /* Processed statistics */
    u32 avg_lum;           /* Average luminance */
    u32 max_lum;           /* Maximum luminance */
    u32 min_lum;           /* Minimum luminance */

    /* Window statistics */
    struct {
        u32 sum;           /* Sum of pixels in window */
        u32 count;         /* Number of pixels in window */
    } windows[16];         /* Per-window statistics */
};

/**
 * struct isp_awb_algo - Auto White Balance algorithm parameters
 * Based on decompiled structure at case 0x800456e2
 */
struct isp_awb_algo {
    u32 params[6];         /* Size 0x18 from decompiled */

    /* Color gain settings */
    u32 r_gain;
    u32 g_gain;
    u32 b_gain;

    /* Color temperature range */
    u32 min_color_temp;
    u32 max_color_temp;

    /* Algorithm state */
    u32 current_temp;
    u32 converge_cnt;

    /* Reserved for future use */
    u32 reserved[8];
};



/**
 * tisp_awb_algo_init - Low level AWB algorithm initialization
 * This would be implemented in the hardware-specific layer
 */
static int tisp_awb_algo_init(int enable)
{
    int ret = 0;

    if (enable) {
        /* Initialize AWB algorithm */
        /* Implementation specific to hardware */
    } else {
        /* Disable AWB algorithm */
        /* Implementation specific to hardware */
    }

    return ret;
}

// Function to simulate imp_log_fun in kernel space
static void imp_log_fun(int level, int option, int arg2,
                       const char* file, const char* func, int line,
                       const char* fmt, ...) {
    va_list args;
    char buf[256];

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    switch (level) {
        case IMP_LOG_ERROR:
            pr_err("%s: %s", func, buf);
            break;
        case IMP_LOG_INFO:
            pr_info("%s: %s", func, buf);
            break;
        case IMP_LOG_DEBUG:
            pr_info("%s: %s", func, buf);
            break;
        default:
            pr_info("%s: %s", func, buf);
    }
}

static struct bus_type soc_bus_type = {
	.name  = "soc",
};

static void __iomem *tparams_day;
static void __iomem *tparams_night;
static dma_addr_t tparams_day_phys;
static dma_addr_t tparams_night_phys;
static uint32_t data_a2f64;
static uint32_t isp_memopt = 1;






/* ISP Format definitions - typically seen in t31 ISP */
enum isp_format {
    ISP_FORMAT_RAW = 0,       /* RAW sensor data */
    ISP_FORMAT_RGB = 1,       /* RGB format */
    ISP_FORMAT_YUV = 2,       /* YUV format */
    ISP_FORMAT_BAYER = 3,     /* Bayer pattern */
};

struct imp_mem_info {
    // 0x00-0x5F: Reserved/unused
    char name[32];        // 0x60: Name string (31 chars + null)
    char pad[32];         // Padding to 0x80
    uint32_t method;      // 0x80: Allocation method
    uint32_t phys_addr;   // 0x84: Physical address (this is what kernel needs)
    uint32_t virt_addr;   // 0x88: Virtual address
    uint32_t size;        // 0x8c: Size of allocation
    uint32_t flags;       // 0x90: Flags/attributes
};

// Match the IMP_Get_Info layout exactly
struct alloc_info {
    char unused[0x60];      // 0x00-0x5F: Unused
    char name[32];          // 0x60: Name from IMP_Get_Info
    char padding[28];       // Padding to align to 0x80
    uint32_t method;        // 0x80: Allocation method
    uint32_t phys_addr;     // 0x84: Physical address
    uint32_t virt_addr;     // 0x88: Virtual address
    uint32_t size;          // 0x8C: Buffer size
    uint32_t flags;         // 0x90: Flags/attributes
};






static int isp_hw_init(struct IMPISPDev *dev)
{
    void __iomem *ctrl_reg;
    void __iomem *status_reg;
    unsigned long timeout;
    u32 val;
    int ret;

    if (!dev || !dev->reg_base) {
        pr_err("Invalid ISP device state\n");
        return -EINVAL;
    }

    /* Power up sequence first */
    ret = isp_power_on(dev);
    if (ret) {
        pr_err("Failed to power on ISP\n");
        return ret;
    }

    /* Reset hardware */
    ret = isp_reset_hw(dev);
    if (ret) {
        pr_err("Failed to reset ISP hardware\n");
        return ret;
    }

    ctrl_reg = dev->reg_base + ISP_CTRL_REG;
    status_reg = dev->reg_base + ISP_STATUS_REG;

    /* Read initial values */
    val = readl(ctrl_reg);
    pr_info("Initial ctrl reg value: 0x%08x\n", val);

    /* Enable ISP core */
    writel(0x1, ctrl_reg);
    val = readl(ctrl_reg);
    pr_info("Read back value after enable: 0x%08x\n", val);

    /* Configure MIPI interface */
    val = readl(dev->reg_base + ISP_MIPI_CTRL);
    val |= 0x1;  // Enable MIPI interface
    writel(val, dev->reg_base + ISP_MIPI_CTRL);

   	return 0;
}



static int tx_isp_open(struct file *file)
{
    struct IMPISPDev *dev = ourISPdev;
    int ret = 0;
    int fd;

    pr_info("ISP device open called from pid %d\n", current->pid);

    if (!dev) {
        pr_err("ISP device not initialized\n");
        return -ENODEV;
    }

    // Initialize instance data // TODO
//    dev->fd = fd = get_unused_fd_flags(O_CLOEXEC);
//    if (fd < 0) {
//        kfree(instance);
//        return fd;
//    }

    // instance->file = file;
    file->private_data = dev; // Set the instance as private data

    // Mark device and frame source as open
    dev->is_open = 1;

    pr_info("ISP opened: file=%p fd=%d\n",
            file, fd);

    return 0;
}


static int stop_streaming(struct IMPISPDev *dev)
{
    struct i2c_client *client = dev->sensor_i2c_client;

    // Stop sensor streaming
    if (client)
        isp_sensor_write_reg(client, SENSOR_REG_MODE, 0x00);

    // Disable ISP streaming
    writel(0x00, dev->reg_base + ISP_CTRL_REG);

    // Disable MIPI receiver
    writel(0x00, dev->reg_base + ISP_MIPI_CTRL);

    pr_info("Streaming stopped\n");
    return 0;
}

// Add this structure for subdev state
struct isp_subdev_state {
    struct isp_pad_desc *src_pads;
    struct isp_pad_desc *sink_pads;
    uint16_t num_src_pads;  // 0xca offset
    uint16_t num_sink_pads; // 0xc8 offset
    void __iomem *regs;     // Register mapping
    struct mutex lock;      // Protection
    char name[32];         // Device name
    int bypass_mode;
};



static void dump_frame_format_state(struct isp_channel *chn)
{
    static const char *formats[] = {
        "UNKNOWN",
        "YUV422",
        "NV12",
        "RAW10"
    };
    const char *format_str = "UNKNOWN";

    // Map format code to string
    switch (chn->fmt) {
        case ISP_FMT_YUV422: format_str = formats[1]; break;
        case ISP_FMT_NV12: format_str = formats[2]; break;
    }

    pr_info("Frame Source State:\n");
    pr_info("  Format: %s (0x%x)\n", format_str, chn->fmt);
    pr_info("  Resolution: %dx%d\n", chn->width, chn->height);
    pr_info("  State: %d Flags: 0x%x\n", chn->state, chn->flags);
}

static void frame_done_callback(struct isp_channel *chn, struct video_buffer *vbuf)
{
    unsigned long flags;

    if (!chn || !vbuf)
        return;

    spin_lock_irqsave(&chn->queue->queue_lock, flags);

    // Remove from ready list if there
    if (!list_empty(&vbuf->list)) {
        list_del_init(&vbuf->list);
    }

    // Mark as done and add to done list
    vbuf->flags &= ~V4L2_BUF_FLAG_QUEUED;
    vbuf->flags |= V4L2_BUF_FLAG_DONE;
    list_add_tail(&vbuf->list, &chn->queue->done_list);

    spin_unlock_irqrestore(&chn->queue->queue_lock, flags);

    // Wake up any waiting dqbuf
    wake_up_interruptible(&chn->queue->wait);
}

static int start_streaming(struct IMPISPDev *dev)
{
    void __iomem *regs = dev->reg_base;
    u32 val;
    int ret;

    pr_info("Starting streaming on device %p\n", dev);

    // Reset and clear interrupts
    writel(0, regs + ISP_CTRL_REG);
    writel(0xFFFFFFFF, regs + ISP_INT_CLEAR_REG);
    wmb();

    // Enable ISP core first
    val = readl(regs + ISP_CTRL_REG);
    val |= 0x1;  // Enable core
    writel(val, regs + ISP_CTRL_REG);
    wmb();

    // Enable frame generation
    val = readl(regs + ISP_STREAM_CTRL);
    val |= 0x1;  // Enable streaming
    writel(val, regs + ISP_STREAM_CTRL);
    writel(0x1, regs + ISP_STREAM_START);  // Start capturing
    wmb();

    // Enable frame done interrupt
    val = ISP_INT_FRAME_DONE | ISP_INT_ERR;
    writel(val, regs + ISP_INT_MASK_REG);
    wmb();

    pr_info("ISP streaming started: ctrl=0x%x stream=0x%x mask=0x%x\n",
            readl(regs + ISP_CTRL_REG),
            readl(regs + ISP_STREAM_CTRL),
            readl(regs + ISP_INT_MASK_REG));

    return 0;
}


static int readl_poll_until_timeout(void __iomem *addr, u32 mask, u32 expected,
                                  unsigned int timeout_us)
{
    u32 val;
    unsigned long timeout = jiffies + usecs_to_jiffies(timeout_us);

    while (time_before(jiffies, timeout)) {
        val = readl(addr);
        if ((val & mask) == expected)
            return 0;
        cpu_relax();
        udelay(10);
    }

    return -ETIMEDOUT;
}

static void generate_test_pattern(void *buf, int width, int height, uint32_t format)
{
    uint8_t *ptr = buf;
    int x, y;
    int stride;

    // Calculate stride with alignment
    stride = ALIGN(width, 32);  // 32-byte alignment

    // For NV12 format
    if (format == ISP_FMT_NV12) {
        // Fill Y plane
        for (y = 0; y < height; y++) {
            for (x = 0; x < width; x++) {
                // Create gradient pattern
                int color = ((x + y) % 256);
                ptr[y * stride + x] = color;
            }
            // Clear padding
            if (stride > width) {
                memset(&ptr[y * stride + width], 0, stride - width);
            }
        }

        // Fill UV plane
        ptr += (stride * height);
        for (y = 0; y < height/2; y++) {
            for (x = 0; x < width; x += 2) {
                // Alternate U/V values for color bars
                ptr[y * stride + x] = 128;     // U
                ptr[y * stride + x + 1] = 128; // V
            }
            // Clear padding
            if (stride > width) {
                memset(&ptr[y * stride + width], 128, stride - width);
            }
        }
    }
    // Add support for other formats as needed
    else if (format == ISP_FMT_YUV422) {
        // Similar pattern for YUV422
        // TODO
    }
}

// Get next frame from ready queue
static struct video_buffer* dequeue_ready_frame(struct frame_queue *q)
{
    struct video_buffer *buf = NULL;
    unsigned long flags;

    spin_lock_irqsave(&q->queue_lock, flags);

    if (!list_empty(&q->ready_list)) {
        buf = list_first_entry(&q->ready_list, struct video_buffer, list);
        list_del_init(&buf->list);
        buf->flags &= ~V4L2_BUF_FLAG_QUEUED;
    }

    spin_unlock_irqrestore(&q->queue_lock, flags);

    if (buf)
        pr_info("Dequeued frame from ready: index=%d flags=0x%x\n",
                buf->index, buf->flags);

    return buf;
}

static void queue_done_frame(struct frame_queue *q, struct video_buffer *buf)
{
    unsigned long flags;

    if (!q || !buf)
        return;

    spin_lock_irqsave(&q->queue_lock, flags);

    // Update buffer flags
    buf->flags |= V4L2_BUF_FLAG_DONE;

    // Add to done list
    list_add_tail(&buf->list, &q->done_list);

    spin_unlock_irqrestore(&q->queue_lock, flags);

    pr_info("Frame queued to done list: index=%d flags=0x%x state=%d\n",
            buf->index, buf->flags, buf->meta ? buf->meta->state : -1);

    wake_up_interruptible(&q->wait);
}

static void process_frame_buffer(struct isp_channel *chn, struct video_buffer *buf)
{
    if (!chn || !buf || !buf->data) {
        pr_err("Invalid buffer parameters\n");
        return;
    }

    // Fill with test pattern if needed
    if (chn->attr.enable) {
        generate_test_pattern(buf->data, chn->width, chn->height, chn->fmt);
    }

    // Update buffer metadata
    buf->flags |= V4L2_BUF_FLAG_DONE;
    buf->status = 0;  // No errors

    pr_debug("Processed buffer %d:\n"
             "  Size: %dx%d fmt=%d\n"
             "  Data: %p\n",
             buf->index,
             chn->width, chn->height, chn->fmt,
             buf->data);
}




static void fill_test_pattern(void *buf, int width, int height, uint32_t format)
{
    // Check for atomic context
    if (in_atomic()) {
        // Just clear memory instead of pattern in atomic context
        memset(buf, 0, width * height * 3/2);
        return;
    }

    // Regular pattern fill
    uint8_t *ptr = buf;
    int x, y;

    // For NV12 format
    if (format == ISP_FMT_NV12) {
        // Fill Y plane with gradient pattern
        for (y = 0; y < height; y++) {
            for (x = 0; x < width; x++) {
                ptr[y * width + x] = (x + y) & 0xFF;
            }
        }

        // Fill UV plane
        ptr += width * height;
        memset(ptr, 128, width * height / 2);
    }
}


static int frame_thread(void *arg)
{
    struct frame_thread_data *thread_data = arg;
    struct isp_channel *chn = thread_data->chn;
    struct video_buffer *buf;
    unsigned long flags;

    pr_info("Frame thread starting for channel %d\n", chn->channel_id);

    atomic_set(&thread_data->thread_running, 1);
    wake_up_interruptible(&chn->queue->wait);

    while (!kthread_should_stop() && !atomic_read(&thread_data->should_stop)) {
        // Get buffer from ready list
        spin_lock_irqsave(&chn->queue->queue_lock, flags);
        if (!list_empty(&chn->queue->ready_list)) {
            buf = list_first_entry(&chn->queue->ready_list,
                                 struct video_buffer, list);
            list_del_init(&buf->list);
            pr_info("Dequeued buffer %d from ready list\n", buf->index);
        } else {
            buf = NULL;
        }
        spin_unlock_irqrestore(&chn->queue->queue_lock, flags);

        if (!buf) {
            msleep(10);
            continue;
        }

        // Sync metadata for CPU access
        dma_sync_single_for_cpu(chn->dev, chn->meta_dma[buf->index],
                               sizeof(struct frame_metadata),
                               DMA_FROM_DEVICE);

        // Update metadata state
        buf->meta->state = FRAME_STATE_DONE;
        buf->meta->done_flag = 1;

        // Sync back for device
        dma_sync_single_for_device(chn->dev, chn->meta_dma[buf->index],
                                  sizeof(struct frame_metadata),
                                  DMA_TO_DEVICE);

        // Process buffer and queue to done list
        spin_lock_irqsave(&chn->queue->queue_lock, flags);
        buf->flags |= V4L2_BUF_FLAG_DONE;
        list_add_tail(&buf->list, &chn->queue->done_list);
        pr_info("Queued buffer %d to done list\n", buf->index);
        spin_unlock_irqrestore(&chn->queue->queue_lock, flags);

        wake_up_interruptible(&chn->queue->wait);

        msleep(33); // ~30fps
    }

    pr_info("Frame thread exiting\n");
    atomic_set(&thread_data->thread_running, 0);
    wake_up_interruptible(&chn->queue->wait);
    return 0;
}

static int init_frame_thread(struct isp_channel *chn)
{
    struct frame_thread_data *thread_data;

    if (!chn || !chn->buf_base) {
        pr_err("Invalid channel state for thread init\n");
        return -EINVAL;
    }

    // Check if thread already exists
    if (chn->thread_data) {
        pr_warn("Frame thread already exists\n");
        return -EEXIST;
    }

    thread_data = kzalloc(sizeof(*thread_data), GFP_KERNEL);
    if (!thread_data)
        return -ENOMEM;

    thread_data->chn = chn;
    atomic_set(&thread_data->should_stop, 0);
    atomic_set(&thread_data->thread_running, 0);

    init_waitqueue_head(&chn->queue->wait);

    thread_data->task = kthread_run(frame_thread, thread_data,
                                  "frame-thread-%d", chn->channel_id);
    if (IS_ERR(thread_data->task)) {
        int ret = PTR_ERR(thread_data->task);
        pr_err("Failed to start frame thread: %d\n", ret);
        kfree(thread_data);
        return ret;
    }

    // Store thread data in channel
    chn->thread_data = thread_data;

    // Wait for thread to signal it's running
    wait_event_timeout(chn->queue->wait,
                      atomic_read(&thread_data->thread_running),
                      msecs_to_jiffies(1000));

    pr_info("Frame thread initialized for channel %d\n", chn->channel_id);
    return 0;
}

static int stop_frame_thread(struct isp_channel *chn)
{
    struct frame_thread_data *thread_data = chn->thread_data;

    if (!thread_data)
        return 0;

    // Signal thread to stop
    atomic_set(&thread_data->should_stop, 1);
    wake_up_interruptible(&chn->queue->wait);

    // Wait for thread to finish
    wait_event_timeout(chn->queue->wait,
                      !atomic_read(&thread_data->thread_running),
                      msecs_to_jiffies(1000));

    // Now safe to stop thread
    if (thread_data->task) {
        kthread_stop(thread_data->task);
        thread_data->task = NULL;
    }

    kfree(thread_data);
    chn->thread_data = NULL;

    return 0;
}

int enable_isp_streaming(struct IMPISPDev *dev, struct file *file, int channel, bool enable)
{
    struct isp_channel *chn;
    void __iomem *vic_regs = dev->reg_base + VIC_BASE;
    void __iomem *isp_regs = dev->reg_base + ISP_BASE;
    void __iomem *csi_regs;
    unsigned long flags;
    int ret = 0;
    uint32_t status;
    int timeout;

    if (!dev || !dev->csi_dev) {
        pr_err("Invalid device structure\n");
        return -EINVAL;
    }

    csi_regs = dev->csi_dev->csi_regs;
    if (!csi_regs) {
        pr_err("Invalid CSI registers\n");
        return -EINVAL;
    }

    chn = &dev->channels[channel];
    if (!chn) {
        pr_err("Invalid channel state\n");
        return -EINVAL;
    }

    pr_info("Starting stream setup:\n");
    pr_info("  ISP registers: %p\n", dev->reg_base);

    if (enable) {
        // 1. CSI Configuration first
        mutex_lock(&dev->csi_dev->mutex);
        pr_info("CSI base: %p\n", csi_regs);
        pr_info("CSI mutex locked\n");

        // Clear errors first
        writel(0xffffffff, csi_regs + 0x20); // ERR1
        writel(0xffffffff, csi_regs + 0x24); // ERR2
        wmb();

        // Full reset sequence
        writel(0x0, csi_regs + 0x10);  // CSI2_RESETN
        writel(0x0, csi_regs + 0x0c);  // DPHY_RSTZ
        writel(0x0, csi_regs + 0x08);  // PHY_SHUTDOWNZ
        wmb();
        udelay(100);

        // Configure lanes
        uint32_t lanes_val = readl(csi_regs + 0x04);
        lanes_val = ((2 - 1) & 3) | (lanes_val & 0xfffffffc);
        writel(lanes_val, csi_regs + 0x04);  // N_LANES setup
        writel(0x2B, csi_regs + 0x18);       // DATA_IDS_1 = RAW10
        writel(0x3, csi_regs + 0x04);        // Enable both lanes
        wmb();
        udelay(100);

        // Power up sequence
        writel(0x1, csi_regs + 0x08);  // PHY_SHUTDOWNZ
        wmb();
        udelay(100);

        writel(0x1, csi_regs + 0x0c);  // DPHY_RSTZ
        wmb();
        udelay(100);

        writel(0x1, csi_regs + 0x10);  // CSI2_RESETN
        wmb();
        udelay(100);

        // Set masks
        writel(0x000f0000, csi_regs + 0x28);  // MASK1
        writel(0x01ff0000, csi_regs + 0x2c);  // MASK2
        wmb();

        mutex_unlock(&dev->csi_dev->mutex);

        // Verify CSI signals after full setup
        ret = verify_csi_signals(dev);
        if (ret) {
            pr_err("CSI signal verification failed\n");
            return ret;
        }

        // Reset VIC before configuration
        ret = reset_vic(dev);
        if (ret) {
            return ret;
        }

        spin_lock_irqsave(&dev->vic_lock, flags);

        // First disable everything
        writel(0, vic_regs + 0x000);
        writel(0, vic_regs + 0x140);
        wmb();
        udelay(100);

        // Reset VIC
        writel(0x4, vic_regs + 0x000);
        wmb();
        udelay(100);

        // Wait for reset completion properly
        while(readl(vic_regs + 0x000) & 0x4) {
            udelay(10);
        }

        // Initial mode setup
        writel(2, vic_regs + 0xc);  // Mode = 2
        writel(0x800c0000, vic_regs + 0x10);  // Magic control
        wmb();

        // Enable frame control
        writel(2, vic_regs + VIC_CTRL);         // Set mode
        writel(0x800c0000, vic_regs + VIC_ROUTE); // Configure route
        wmb();
        udelay(10);

        // Enable VIC interrupts
        writel(VIC_INT_ALL_MASK, vic_regs + VIC_IRQ_MASK);
        writel(VIC_INT_ALL_MASK, vic_regs + VIC_IRQ_EN);
        wmb();

        pr_info("Final VIC State:\n");
        pr_info("  Control: 0x%08x\n", readl(vic_regs + VIC_CTRL));
        pr_info("  Status: 0x%08x\n", readl(vic_regs + VIC_STATUS));
        pr_info("  IRQ Enable: 0x%08x\n", readl(vic_regs + VIC_IRQ_EN));
        pr_info("  Frame Control: 0x%08x\n", readl(vic_regs + VIC_FRAME_CTRL));

        spin_unlock_irqrestore(&dev->vic_lock, flags);

//        // Set streaming state properly
//        mutex_lock(&chn->queue->lock);
//        chn->queue->stream_state |= 1;  // Set streaming bit
//        // Update channel state to match 0x3000003 handler
//        chn->state |= BIT(1);  // Set streaming bit
//        chn->flags |= BIT(2);  // Set active bit
//        mutex_unlock(&chn->queue->lock);

        pr_info("Stream enabled!\n");
    } else {
        // Disable streaming
        writel(0, isp_regs + ISP_STREAM_CTRL);
        wmb();
        writel(0, isp_regs + ISP_STREAM_START);
        wmb();

//        // Set streaming state properly
//        mutex_lock(&chn->queue->lock);
//        chn->queue->stream_state &= ~1;  // Clear streaming bit
//        chn->state &= ~BIT(1);  // Clear streaming bit
//        chn->flags &= ~BIT(2);  // Clear active bit
//        mutex_unlock(&chn->queue->lock);

        pr_info("Stream disabled!\n");
    }

    return ret;
}


static int handle_sensor_ioctl(struct IMPISPDev *dev, unsigned int cmd, void __user *arg, struct file *file)
{
    struct i2c_client *client = dev->sensor_i2c_client; // Add this field to IMPISPDev
    struct sensor_reg_data reg_data;
    int ret = 0;
    uint8_t val = 0;

    pr_info("Sensor command: 0x%x\n", cmd);

    if (!client) {
        pr_err("No I2C client available for sensor\n");
        return -ENODEV;
    }

    switch (cmd) {
		case SENSOR_CMD_READ_ID: {
		    uint8_t val_h = 0, val_l = 0;
		    struct i2c_client *client = ourISPdev->sensor_i2c_client;

		    // Try reading ID registers
		    ret = isp_sensor_read_reg(client, 0x3307, &val_h);
		    if (ret) {
		        pr_err("Failed to read ID high byte\n");
		        return ret;
		    }

		    ret = isp_sensor_read_reg(client, 0x3308, &val_l);
		    if (ret) {
		        pr_err("Failed to read ID low byte\n");
		        return ret;
		    }

		    pr_info("Sensor ID: 0x%02x%02x\n", val_h, val_l);
		    return 0;
		}

        case SENSOR_CMD_WRITE_REG: {
            if (copy_from_user(&reg_data, arg, sizeof(reg_data)))
                return -EFAULT;
            ret = isp_sensor_write_reg(client, reg_data.reg, reg_data.val);
            break;
        }

        case SENSOR_CMD_READ_REG: {
            if (copy_from_user(&reg_data, arg, sizeof(reg_data)))
                return -EFAULT;
            ret = isp_sensor_read_reg(client, reg_data.reg, &val);
            if (ret < 0)
                return ret;
            reg_data.val = val;
            ret = copy_to_user(arg, &reg_data, sizeof(reg_data));
            break;
        }
        case 0x80045612: // VIDIOC_STREAMON
        {
            pr_info("Sensor command: 0x%x\n", cmd);
            pr_info("Sensor streamon\n");

            // First configure CSI - this can sleep
            ret = configure_csi_streaming(dev);
            if (ret) {
                pr_err("Failed to configure CSI: %d\n", ret);
                return ret;
            }

            // sensor reset sequence here
            if (reset_gpio != -1) {
                ret = gpio_request(reset_gpio, "sensor_reset");
                if (!ret) {
                    gpio_direction_output(reset_gpio, 0);
                    msleep(20);
                    gpio_direction_output(reset_gpio, 1);
                    msleep(20);
                }
            }

            if (file->private_data) {
                struct isp_channel *chn = file->private_data;
                if (chn && chn->queue) {
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
                        return -EAGAIN;  // -16
                    }

                    // Enable stream
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
                }
            }

            // Then enable stream - atomic part only
            ret = enable_isp_streaming(dev, file, 0, true);
            if (ret == 0) {
                pr_info("Stream enabled!\n");
            }
            return ret;
        }
        case VIDIOC_STREAMOFF:
            pr_info("TODO Stream OFF requested\n");
            //return enable_isp_streaming(ourISPdev, file, 0, false);
        case SENSOR_CMD_SET_GAIN: {
            uint16_t gain;
            if (get_user(gain, (uint16_t __user *)arg))
                return -EFAULT;
            // Convert gain to sensor specific register values
            // This is sensor specific - adjust for SC2336
            ret = isp_sensor_write_reg(client, 0x3508, (gain >> 8) & 0xFF);
            if (ret == 0)
                ret = isp_sensor_write_reg(client, 0x3509, gain & 0xFF);
            break;
        }

        case SENSOR_CMD_SET_EXP: {
            uint32_t exp;
            if (get_user(exp, (uint32_t __user *)arg))
                return -EFAULT;
            // Convert exposure to sensor specific register values
            // This is sensor specific - adjust for SC2336
            ret = isp_sensor_write_reg(client, 0x3500, (exp >> 12) & 0xFF);
            if (ret == 0)
                ret = isp_sensor_write_reg(client, 0x3501, (exp >> 4) & 0xFF);
            if (ret == 0)
                ret = isp_sensor_write_reg(client, 0x3502, (exp & 0x0F) << 4);
            break;
        }
		case 0xc0045627: {
		    int result_val;
            // TODO
		    const char *sensor_name = "sc2336";  // This matches libimp's expectations

		    // Get the -1 value passed in
		    if (copy_from_user(&result_val, (void __user *)arg, sizeof(result_val))) {
		        pr_err("Failed to get result value\n");
		        return -EFAULT;
		    }

		    // Store the sensor name in our structure
		    strlcpy(ourISPdev->sensor_name, sensor_name, SENSOR_NAME_SIZE);

		    pr_info("Stored sensor name: %s\n", ourISPdev->sensor_name);
		    return 0;
		}
		case 0x800856d7: {
		    struct {
		        uint32_t buffer_data;  // User-provided buffer data
		        uint32_t size;         // Calculated size
		    } wdr_params = {0};

		    // Ensure our global device structure is initialized
		    if (!ourISPdev) {
		        pr_err("ISP device not initialized\n");
		        return -ENODEV;
		    }

		    // Copy initial parameters from user
		    if (copy_from_user(&wdr_params, (void __user *)arg, sizeof(wdr_params))) {
		        pr_err("Failed to copy WDR parameters from user\n");
		        return -EFAULT;
		    }

            pr_info("WDR buffer data: 0x%x\n", wdr_params.buffer_data);
		}

        default:
            pr_err("Unknown sensor command: 0x%x\n", cmd);
            return -EINVAL;
    }

    return ret;
}


static int tx_isp_release(struct inode *inode, struct file *file)
{
    struct IMPISPDev *dev = file->private_data;
    struct video_buffer *buf;
    int i;
    unsigned long flags;

    pr_info("\n=== ISP Release Debug ===\n");

    if (!dev)
        return 0;

    // Stop all channels
    for (i = 0; i < MAX_CHANNELS; i++) {
        struct isp_channel *chn = &dev->channels[i];

        if (!chn)
            continue;

        if (chn->state & BIT(1)) { // If streaming
            // Stop streaming
            atomic_set(&chn->thread_should_stop, 1);
            wake_up_interruptible(&chn->queue->wait);

            // Wait for thread to stop
            wait_event_interruptible_timeout(chn->queue->wait,
                !atomic_read(&chn->thread_running), HZ);
        }

        // Lock queue for cleanup
        mutex_lock(&chn->queue->lock);
        spin_lock_irqsave(&chn->queue->queue_lock, flags);

        // Free buffer memory
        if (chn->queue->bufs) {
            for (int j = 0; j < chn->queue->buf_count; j++) {
                buf = chn->queue->bufs[j];
                if (buf) {
                    // Clear lists first
                    list_del_init(&buf->list);

                    // Free buffer data if we allocated it
                    if (buf->data && chn->queue->memory_type == V4L2_MEMORY_KMALLOC) {
                        kfree(buf->data);
                        buf->data = NULL;
                    }

                    // Free the buffer structure
                    kfree(buf);
                    chn->queue->bufs[j] = NULL;
                }
            }

            // Free the buffer pointer array
            kfree(chn->queue->bufs);
            chn->queue->bufs = NULL;
        }

        // Reset queue state
        chn->queue->buf_count = 0;
        chn->queue->memory_type = 0;
        chn->queue->stream_state = 0;
        INIT_LIST_HEAD(&chn->queue->ready_list);
        INIT_LIST_HEAD(&chn->queue->done_list);

        spin_unlock_irqrestore(&chn->queue->queue_lock, flags);
        mutex_unlock(&chn->queue->lock);

        pr_info("Channel %d cleaned up:\n"
                "  Buffers freed: %d\n"
                "  Memory type: %d\n",
                chn->channel_id,
                chn->queue->buf_count,
                chn->queue->memory_type);
    }

    // Reset device state
    dev->is_open = 0;
    memset(dev->sensor_name, 0, SENSOR_NAME_SIZE);
    file->private_data = NULL;

    pr_info("ISP device released\n");
    return 0;
}

/**
 * struct isp_wdr_buf_size - WDR buffer size information
 * Used for TW_ISP_WDR_GET_BUF ioctl
 */
struct isp_wdr_buf_size {
    __u32 size;        /* Required buffer size */
    __u32 tsize;       /* Total size needed */
};


/* Magic number from decompiled code at 0xef64 */


/**
 * private_math_exp2 - Exponential calculation helper
 * Based on decompiled function
 *
 * @value: Input value
 * @shift: Shift amount for fixed-point calculation
 * @round: Rounding value
 *
 * Return: Calculated exponential value
 *
 * This exact prototype matches the decompiled code
 */
uint32_t private_math_exp2(uint32_t val, const unsigned char shift_in,
                          const unsigned char shift_out)
{
    uint64_t result;

    result = (uint64_t)val << shift_in;
    result += (1 << (shift_out - 1));  // Rounding

    return (uint32_t)(result >> shift_out);
}


int register_ae_callback(struct IMPISPDev *dev, void (*frame_cb)(void *),
                        void (*stats_cb)(void *), void *priv)
{
    unsigned long flags;

    if (!dev || !dev->ae_info)
        return -EINVAL;

    spin_lock_irqsave(&dev->ae_info->lock, flags);

    dev->ae_info->cb.frame_cb = frame_cb;
    dev->ae_info->cb.stats_cb = stats_cb;
    dev->ae_info->cb.priv = priv;

    spin_unlock_irqrestore(&dev->ae_info->lock, flags);

    return 0;
}

/**
 * tisp_ae_algo_init implementation with correct return type
 */
int tisp_ae_algo_init(int enable, struct isp_ae_algo *ae)
{
    int ret = 0;

    if (enable) {
        /* Initialize AE algorithm with provided parameters */
        if (!ae)
            return -EINVAL;

        /* Setup AE algorithm parameters */
        /* Implementation specific to hardware */
    } else {
        /* Disable AE algorithm */
        /* Implementation specific to hardware */
    }

    return ret;
}


/**
 * Helper function to disable WDR (implementation would be driver-specific)
 * This matches the tisp_s_wdr_en(0) call in decompiled code
 */ // TODO
static void isp_disable_wdr(struct IMPISPDev *dev)
{
  	pr_info("isp_disable_wdr called\n");
    // Implementation would:
    // 1. Clear WDR enable bits in hardware
    // 2. Reset WDR-related registers
    // 3. Update driver state

    // This is called when buffer setup fails to ensure
    // WDR is properly disabled

    // Actual implementation would depend on specific
    // hardware requirements
}

/**
 * init_ae_algo - Initialize Auto Exposure algorithm
 * @dev: IMPISPDev structure
 * @ae: AE algorithm parameters from userspace
 *
 * Based on decompiled code around case 0x800456dd
 */
int init_ae_algo(struct IMPISPDev *dev, struct isp_ae_algo *ae)
{
    struct ae_info *ae_info;
    struct isp_ae_regs __iomem *regs;

    if (!dev || !dev->ae_info || !ae) {
        return -EINVAL;
    }

    ae_info = dev->ae_info;
    regs = (struct isp_ae_regs __iomem *)(dev->reg_base + ISP_AE_REG_BASE);

    // Initialize hardware values
    ae_info->gain = readl(&regs->gain);
    ae_info->exposure = readl(&regs->exposure);
    ae_info->gain_factor = private_math_exp2(readl(&regs->gain_factor), 0x10, 0xa);
    ae_info->exposure_factor = 0x400;  // Fixed value

    // Copy user space parameters with validation
    if (ae->min_gain < 0x100 || ae->max_gain > 0xFFFF) {
        pr_err("Invalid gain range: min=0x%x max=0x%x\n",
               ae->min_gain, ae->max_gain);
        return -EINVAL;
    }
    ae_info->min_gain = ae->min_gain;
    ae_info->max_gain = ae->max_gain;

    if (ae->min_exposure < 0x100 || ae->max_exposure > 0xFFFF) {
        pr_err("Invalid exposure range: min=0x%x max=0x%x\n",
               ae->min_exposure, ae->max_exposure);
        return -EINVAL;
    }
    ae_info->min_exposure = ae->min_exposure;
    ae_info->max_exposure = ae->max_exposure;

    ae_info->target_luminance = ae->target_luminance;

    // Initialize WDR-specific values if needed
    if (dev->wdr_mode) {
        ae_info->wdr_gain = readl(dev->reg_base + ISP_WDR_REG_BASE + 0x00);
        ae_info->wdr_exposure = readl(dev->reg_base + ISP_WDR_REG_BASE + 0x0C);
    } else {
        ae_info->wdr_gain = readl(dev->reg_base + ISP_WDR_REG_BASE + 0x30);
    }

    return 0;
}

/**
 * init_awb_algo - Initialize Auto White Balance algorithm
 * @dev: IMPISPDev structure
 * @awb: AWB algorithm parameters from userspace
 *
 * Based on decompiled code around case 0x800456e2
 */
int init_awb_algo(struct IMPISPDev *dev, struct isp_awb_algo *awb)
{
    struct isp_awb_info *awb_info;

    /* Initialize algorithm - matches tisp_awb_algo_init(1) at 0xf26c */
    tisp_awb_algo_init(1);

    /* Allocate info structure - matches 0xf290 */
    awb_info = dev->awb_info;
    if (!awb_info) {
        return -ENOMEM;
    }

    /* Store globally - matches assignment at 0xf290 */
    awb_info_mine = awb_info;

    /* Initialize completion tracking - matches 0xf298 */
    awb_algo_comp = 0;

    /* Initialize wait queue - matches 0xf2b8 */
    init_waitqueue_head(&awb_wait);

    return 0;
}


/**
 * struct isp_wdr_buf_info - WDR buffer information structure
 * Based on access patterns in decompiled code around 0x800856d6
 */
struct isp_wdr_buf_info {
    unsigned long addr;
    unsigned int size;
};

/**
 * tisp_s_wdr_en - Enable/disable WDR mode and configure related modules
 * Based on decompiled function at 0x64824
 *
 * @arg1: WDR enable flag (1=enable, 0=disable)
 */
static int tisp_s_wdr_en(uint32_t arg1)
{
    pr_info("TODO WDR enable: %d\n", arg1);

    return 0;
}

// TODO: Implement the following functions based on decompiled code
/* Subsystem WDR enable function declarations */
//static void tisp_dpc_wdr_en(uint32_t en);
//static void tisp_lsc_wdr_en(uint32_t en);
//static void tisp_gamma_wdr_en(uint32_t en);
//static void tisp_sharpen_wdr_en(uint32_t en);
//static void tisp_ccm_wdr_en(uint32_t en);
//static void tisp_bcsh_wdr_en(uint32_t en);
//static void tisp_rdns_wdr_en(uint32_t en);
//static void tisp_adr_wdr_en(uint32_t en);
//static void tisp_defog_wdr_en(uint32_t en);
//static void tisp_mdns_wdr_en(uint32_t en);
//static void tisp_dmsc_wdr_en(uint32_t en);
//static void tisp_ae_wdr_en(uint32_t en);
//static void tisp_sdns_wdr_en(uint32_t en);
//static void tiziano_clm_init(void);
//static void tiziano_ydns_init(void);



/* IOCTL command definitions - completing the set */
#define TX_ISP_SET_AE_ALGO_CLOSE  _IOW('V', 0x56de, int)
#define TX_ISP_SET_AWB_ALGO_CLOSE _IOW('V', 0x56e4, int)

/* Global WDR state tracking - seen in decompiled */
static int wdr_switch = 0;

/**
 * isp_set_wdr_mode - Enable/disable WDR mode
 * Based on decompiled code around 0x800456d8/0x800456d9
 *
 * @isp: ISP device structure
 * @enable: WDR enable flag (1=enable, 0=disable)
 */
static int isp_set_wdr_mode(struct IMPISPDev *dev, int enable)
{
    struct isp_reg_t *regs = dev->reg_base;
    int ret = 0;

    if (enable) {
        /* Enable WDR mode - from case 0x800456d8 */
        isp_reg_write(regs, offsetof(struct isp_reg_t, wdr_enable), 1);

        /* Apply WDR settings from decompiled */
        if (wdr_switch) {
            /* Configure WDR hardware settings */
            if (dev->reg_base) {
                /* Set WDR parameters - matches decompiled at 0xed90 */
                system_reg_write(0x2000013, 1);
                tisp_s_wdr_en(1);

                /* Reset statistics buffers */
                void *stats = dev->reg_base + offsetof(struct isp_reg_t, ae_stats);
                memset(stats, 0, sizeof(u32) * 160);  // Clear all stats registers
            }
            wdr_switch = 1;
        }
    } else {
        /* Disable WDR mode - from case 0x800456d9 */
        isp_reg_write(regs, offsetof(struct isp_reg_t, wdr_enable), 0);

        if (wdr_switch) {
            /* Disable WDR in hardware */
            system_reg_write(0x2000013, 0);
            tisp_s_wdr_en(0);
            wdr_switch = 0;
        }
    }

    return ret;
}

/**
 * calculate_wdr_buffer_size - Calculate required WDR buffer size
 * Based on decompiled code around 0x800856d7
 *
 * @isp: ISP device structure
 * @size: Buffer size information structure
 */
static int calculate_wdr_buffer_size(struct IMPISPDev *dev, struct isp_wdr_buf_size *size)
{
    struct isp_reg_t *regs = dev->reg_base;
    uint32_t wdr_mode;
    int ret = 0;

    /* Get current WDR mode */
    wdr_mode = isp_reg_read(regs, offsetof(struct isp_reg_t, wdr_enable));

    if (wdr_mode == WDR_MODE_LINE) {
        /* Line-interleaved mode */
        size->tsize = (isp_reg_read(regs, offsetof(struct isp_reg_t, output_width)) << 1) *
                      isp_reg_read(regs, offsetof(struct isp_reg_t, output_height));
    } else if (wdr_mode == WDR_MODE_FRAME) {
        /* Frame-based mode - get from hardware register */
        size->tsize = isp_reg_read(regs, offsetof(struct isp_reg_t, gain_factor)); // 0xe8 offset
    } else {
        isp_printf(1, "Not in WDR mode, buffer calculation failed\n");
        return -EINVAL;
    }

    /* Calculate required size based on decompiled logic */
    size->size = size->tsize;  // Base size

    /* Add additional space for statistics if needed */
    if (!isp_memopt) {
        size->size += (size->tsize >> 1) + (size->tsize >> 2);
    }

    return ret;
}


int csi_video_s_stream(struct IMPISPDev *dev, bool enable)
{
    struct isp_channel *chn = &dev->channels[0];

    pr_info("CSI stream config: enable=%d state=%d\n", enable, chn->state);
    // From decompiled: Check sensor type at 0x110+0x14 should be 1
    if (readl(dev->reg_base + 0x110 + 0x14) != 1)
        return 0;

    // Simple state transition based on decompiled:
    // Sets state 4 if enable, 3 if disable
    chn->state = enable ? 4 : 3;
    return 0;
}





static long handle_encoder_ioctls(struct IMPISPDev *dev, unsigned int cmd, void __user *arg)
{
    void __user *argp = (void __user *)arg;
    int ret = 0;

    pr_info("ISP IOCTL Debug:\n");
    pr_info("  Command: 0x%x\n", cmd);
    pr_info("  Type: %c\n", _IOC_TYPE(cmd));
    pr_info("  Number: 0x%x\n", _IOC_NR(cmd));
    pr_info("  Direction: 0x%x\n", _IOC_DIR(cmd));
    pr_info("  Size: %d\n", _IOC_SIZE(cmd));

    switch(cmd) {
        // ... other cases ...

        case VIDIOC_INIT_ENCODER: {
 		    int enable;
		    if (copy_from_user(&enable, argp, sizeof(enable)))
		        return -EFAULT;

		    struct isp_channel *chn = &dev->channels[0];
		    if (!chn)
		        return -EINVAL;
		    pr_info("Encoder initialized: enable=%d\n", enable);
		    return 0;
        }

        case VIDIOC_CREATE_ENCODER_CHN: {
		    struct encoder_chn_attr attr;
		    if (copy_from_user(&attr, argp, sizeof(attr)))
		        return -EFAULT;

            // TODO

		    pr_info("TODO Created encoder channel: %dx%d\n",
		            attr.picWidth, attr.picHeight);
		    return 0;
        }

        case VIDIOC_ENABLE_ENCODER_CHN: {
		    int channel_id;
		    if (copy_from_user(&channel_id, argp, sizeof(channel_id)))
		        return -EFAULT;

		    struct isp_channel *chn = &dev->channels[channel_id];
		    
		    if (!chn)
		        return -EINVAL;

		    pr_info("Encoder channel %d enabled\n", channel_id);
		    return 0;
        }
    }

    return ret;
}

static int tiziano_ae_init(uint32_t gain, uint32_t exposure)
{
    void __iomem *regs = ourISPdev->reg_base;

    // Setup initial AE parameters in hardware
    writel(gain, regs + ISP_REG_STD_GAIN);
    writel(exposure, regs + 0x1000);  // Base exposure reg

    return 0;
}

static int tiziano_gamma_init(void)
{
    void __iomem *regs = ourISPdev->reg_base;

    // Setup gamma registers at 0x1200
    writel(0x0, regs + 0x1200);  // Gamma control
    writel(0x200, regs + 0x1204); // Gamma slope

    return 0;
}

static int tiziano_gib_init(void)
{
    void __iomem *regs = ourISPdev->reg_base;
    // Global illumination balance
    writel(0x1, regs + 0x1300);
    return 0;
}

static int tiziano_lsc_init(void)
{
    void __iomem *regs = ourISPdev->reg_base;
    // Lens shading correction
    writel(0x1, regs + 0x1400);
    return 0;
}

static int tiziano_ccm_init(void)
{
    void __iomem *regs = ourISPdev->reg_base;
    // Color correction matrix
    writel(0x100, regs + 0x1500); // Identity matrix
    return 0;
}

static int tiziano_dmsc_init(void)
{
    void __iomem *regs = ourISPdev->reg_base;
    // Demosaic init
    writel(0x1, regs + 0x1600);
    return 0;
}

static int tiziano_sharpen_init(void)
{
    void __iomem *regs = ourISPdev->reg_base;
    // Edge enhancement/sharpening
    writel(0x10, regs + 0x1700);
    return 0;
}

static int tiziano_sdns_init(void)
{
    void __iomem *regs = ourISPdev->reg_base;
    // Spatial denoising
    writel(0x1, regs + 0x1800);
    return 0;
}

static int tiziano_mdns_init(void)
{
    void __iomem *regs = ourISPdev->reg_base;
    // Motion denoising
    writel(0x1, regs + 0x1900);
    return 0;
}

static int tiziano_rdns_init(void)
{
    void __iomem *regs = ourISPdev->reg_base;
    // Raw denoising
    writel(0x1, regs + 0x1a00);
    return 0;
}

static int tiziano_dpc_init(void)
{
    void __iomem *regs = ourISPdev->reg_base;
    // Defective pixel correction
    writel(0x1, regs + 0x1b00);
    return 0;
}

static int tiziano_hldc_init(void)
{
    void __iomem *regs = ourISPdev->reg_base;
    // Highlight compression
    writel(0x1, regs + 0x1c00);
    return 0;
}

static int tiziano_defog_init(void)
{
    void __iomem *regs = ourISPdev->reg_base;
    // Defogging
    writel(0x0, regs + 0x1d00);
    return 0;
}

static int tiziano_adr_init(void)
{
    void __iomem *regs = ourISPdev->reg_base;
    // Dynamic range adaptation
    writel(0x1, regs + 0x1e00);
    return 0;
}

static int tiziano_af_init(void)
{
    void __iomem *regs = ourISPdev->reg_base;
    // Auto focus
    writel(0x0, regs + 0x1f00);
    return 0;
}

static int tiziano_bcsh_init(void)
{
    void __iomem *regs = ourISPdev->reg_base;
    // Brightness/Contrast/Saturation/Hue
    writel(0x80, regs + 0x2000); // Brightness
    writel(0x80, regs + 0x2004); // Contrast
    writel(0x80, regs + 0x2008); // Saturation
    writel(0x0, regs + 0x200c);  // Hue
    return 0;
}

static int tiziano_ydns_init(void)
{
    void __iomem *regs = ourISPdev->reg_base;
    // Y channel denoising
    writel(0x1, regs + 0x2100);
    return 0;
}


static int tiziano_awb_init(uint32_t gain, uint32_t exposure)
{
    void __iomem *regs = ourISPdev->reg_base;

    // Store initial values
    //ourISPdev->awb_info->gain = gain;
    //ourISPdev->awb_info->exposure = exposure;

    // Set initial color gains
    writel(0x100, regs + 0x1100); // R gain
    writel(0x100, regs + 0x1104); // G gain
    writel(0x100, regs + 0x1108); // B gain

    return 0;
}

static int tiziano_clm_init(void)
{
    void __iomem *regs = ourISPdev->reg_base;
    // Color management
    writel(0x1, regs + 0x2200);
    return 0;
}

static int tiziano_wdr_init(void)
{
    void __iomem *regs = ourISPdev->reg_base;
    // Wide Dynamic Range
    writel(0x1, regs + 0x2300);
    return 0;
}

static int tisp_gb_init(void)
{
    void __iomem *regs = ourISPdev->reg_base;
    // Global balance
    writel(0x1, regs + 0x2400);
    return 0;
}

// WDR enable functions
static int tisp_dpc_wdr_en(int enable)
{
    void __iomem *regs = ourISPdev->reg_base;
    writel(enable ? 0x1 : 0x0, regs + 0x2500);
    return 0;
}


// Update callback functions
static void tisp_tgain_update(void)
{
    // Total gain update callback
    pr_info("Total gain update called\n");
}

static void tisp_again_update(void)
{
    // Analog gain update callback
    pr_info("Analog gain update called\n");
}

static void tisp_ev_update(void)
{
    // Exposure value update callback
    pr_info("EV update called\n");
}

static void tisp_ct_update(void)
{
    // Color temperature update callback
    pr_info("Color temp update called\n");
}

static void tisp_ae_ir_update(void)
{
    // AE IR update callback
    pr_info("AE IR update called\n");
}

static int init_isp_subsystems(struct IMPISPDev *dev)
{
    int ret;

    pr_info("Starting ISP subsystem initialization\n");

    if (!dev || !dev->reg_base) {
        pr_err("Invalid device state in init_isp_subsystems\n");
        return -EINVAL;
    }

    // Create an ae_info structure
    dev->ae_info = kzalloc(sizeof(struct ae_info), GFP_KERNEL);
    if (!dev->ae_info) {
        pr_err("Failed to allocate ae_info structure\n");
        return -ENOMEM;
    }

    // Initialize AE/AWB info with safe defaults
    dev->ae_info->gain = 0x100;     // Default gain
    dev->ae_info->exposure = 0x100;  // Default exposure

    pr_info("Initializing AE/AWB subsystems\n");
    ret = tiziano_ae_init(dev->ae_info->gain, dev->ae_info->exposure);
    if (ret) {
        pr_err("Failed to initialize AE\n");
        return ret;
    }

    dev->awb_info = kzalloc(sizeof(struct awb_info), GFP_KERNEL);
    if (!dev->awb_info) {
        pr_err("Failed to allocate awb_info structure\n");
        return -ENOMEM;
    }

    // Initialize default AWB values
    dev->awb_info->r_gain = 0x100;     // Default red gain
    dev->awb_info->g_gain = 0x100;     // Default green gain
    dev->awb_info->b_gain = 0x100;     // Default blue gain
    dev->awb_info->temperature = 6500;  // Default color temp (K)

    // Initialize AWB algorithm
    ret = tiziano_awb_init(dev->awb_info->r_gain, dev->awb_info->g_gain);
    if (ret) {
        pr_err("Failed to initialize Tiziano AWB\n");
        goto err_free;
    }

    pr_info("TODO Initializing image processing subsystems\n");

    // Initialize each subsystem with error checking
    if ((ret = tiziano_gamma_init()) ||
        (ret = tiziano_gib_init()) ||
        (ret = tiziano_lsc_init()) ||
        (ret = tiziano_ccm_init()) ||
        (ret = tiziano_dmsc_init()) ||
        (ret = tiziano_sharpen_init()) ||
        (ret = tiziano_sdns_init()) ||
        (ret = tiziano_mdns_init()) ||
        (ret = tiziano_clm_init()) ||
        (ret = tiziano_dpc_init()) ||
        (ret = tiziano_hldc_init()) ||
        (ret = tiziano_defog_init()) ||
        (ret = tiziano_adr_init()) ||
        (ret = tiziano_af_init()) ||
        (ret = tiziano_bcsh_init()) ||
        (ret = tiziano_ydns_init()) ||
        (ret = tiziano_rdns_init())) {

        pr_err("Failed to initialize subsystem, ret = %d\n", ret);
        return ret;
    }

    // WDR mode initialization
    if (dev->wdr_mode == 1) {
        pr_info("Initializing WDR mode\n");
        if ((ret = tiziano_wdr_init()) ||
            (ret = tisp_gb_init()) ||
            (ret = tisp_dpc_wdr_en(1))) {
            pr_err("Failed to initialize WDR mode\n");
            return ret;
        }
    }

    pr_info("ISP subsystem initialization complete\n");
    return 0;

err_free:
    kfree(dev->awb_info);
    dev->awb_info = NULL;
    kfree(dev->ae_info);
    dev->ae_info = NULL;
    return ret;
}



// Add these defines at the top
#define ISP_ALLOC_FAILED    0
#define ISP_ALLOC_SUCCESS   1
#define DEFAULT_BUFFER_SIZE (4 * 1024 * 1024)  // 4MB


// Structure to track allocation state
struct isp_alloc_state {
    uint32_t status;         // 0 = not allocated, 1 = allocated
    uint32_t requested_size; // Size requested by userspace
    uint32_t actual_size;    // Size actually allocated
    uint32_t phys_addr;      // Physical address
    uint32_t virt_addr;      // Virtual address
};

static struct isp_alloc_state alloc_state = {
    .status = ISP_ALLOC_FAILED,
    .requested_size = 0,
    .actual_size = 0,
    .phys_addr = 0,
    .virt_addr = 0
};

// Structure for IMP allocation info matching the binary layout
struct imp_alloc_info {
    uint8_t reserved1[0x60];    // 0x00-0x5F: Reserved/padding
    char name[32];              // 0x60: Name string
    uint8_t reserved2[28];      // Padding to 0x80
    uint32_t method;            // 0x80: Allocation method
    uint32_t phys_addr;         // 0x84: Physical address
    uint32_t virt_addr;         // 0x88: Virtual address
    uint32_t size;              // 0x8C: Buffer size
    uint32_t flags;             // 0x90: Flags/attributes
};


// Structure for tracking our memory allocations
// Keep just one structure definition
struct isp_memory_region {
    uint32_t phys_addr;
    void *virt_addr;
    size_t size;
    bool initialized;
};

// Use the original name and structure
static struct isp_memory_region isp_memory = {
    .phys_addr = 0,
    .virt_addr = NULL,
    .size = 0,
    .initialized = false
};



static int init_isp_reserved_memory(struct platform_device *pdev) {
    dma_addr_t aligned_base = ALIGN(RMEM_BASE, 32);
    size_t aligned_size = ALIGN(RMEM_SIZE, 32);

    // Missing error checking for ourISPdev
    if (!ourISPdev) {
        dev_err(&pdev->dev, "ISP device not initialized\n");
        return -EINVAL;
    }

    ourISPdev->rmem_addr = aligned_base;
    ourISPdev->rmem_size = aligned_size;

    // Should use devm_ioremap for managed memory
    ourISPdev->dma_buf = devm_ioremap(&pdev->dev, aligned_base, aligned_size);
    if (!ourISPdev->dma_buf) {
        dev_err(&pdev->dev, "Failed to map DMA buffer\n");
        return -ENOMEM;
    }

    dev_info(&pdev->dev, "Reserved memory initialized:\n"
             "  Physical address: 0x%08x (aligned)\n"
             "  Virtual address: %p\n"
             "  Size: %zu bytes\n"
             "  Alignment check: base=0x%x buf=0x%x\n",
             (uint32_t)aligned_base,
             ourISPdev->dma_buf,
             aligned_size,
             (uint32_t)aligned_base & 0x1F,
             (uintptr_t)ourISPdev->dma_buf & 0x1F);

    return 0;
}


struct isp_mem_request {
    uint32_t method;        // 0x80: Should match ISP_ALLOC_KMALLOC
    uint32_t phys_addr;     // 0x84: Physical address from IMP_Alloc
    uint32_t virt_addr;     // 0x88: Virtual address
    uint32_t size;          // 0x8C: Buffer size
    uint32_t flags;         // 0x90: Flags/attributes
};

struct tx_isp_subdev_pad *find_subdev_link_pad(struct IMPISPDev *dev,
                                              const struct tx_isp_link_config *cfg)
{
    struct tx_isp_subdev *sd = dev->fs_subdev;

    // Validate inputs
    if (!sd || !cfg) {
        pr_err("Invalid arguments to find_subdev_link_pad\n");
        return NULL;
    }

    pr_info("Finding pad for subdev %p:\n", sd);
    pr_info("  Looking for %s type=%d index=%d\n",
            cfg->src.name, cfg->src.type, cfg->src.index);

    // Check type and return appropriate pad
    if (cfg->flag & TX_ISP_PADTYPE_INPUT) {
        if (cfg->src.index >= sd->num_inpads) {
            pr_err("Invalid input pad index %d (max %d)\n",
                  cfg->src.index, sd->num_inpads-1);
            return NULL;
        }
        return &sd->inpads[cfg->src.index];
    } else {
        if (cfg->src.index >= sd->num_outpads) {
            pr_err("Invalid output pad index %d (max %d)\n",
                  cfg->src.index, sd->num_outpads-1);
            return NULL;
        }
        return &sd->outpads[cfg->src.index];
    }
}

static int subdev_video_destroy_link(struct tx_isp_subdev_pad *pad)
{
    if (!pad)
        return -EINVAL;

    // If there's an active link
    if (pad->link.sink) {
        // Clear reverse link
        if (pad->link.reverse)
            memset(pad->link.reverse, 0, sizeof(struct tx_isp_subdev_link));

        // Clear this link
        memset(&pad->link, 0, sizeof(struct tx_isp_subdev_link));

        // Update pad state
        pad->state = TX_ISP_PADSTATE_FREE;  // Need to define this state
    }

    return 0;
}

/**
 * tx_isp_ioctl - IOCTL handler for ISP driver
 * @file: File structure
 * @cmd: IOCTL command
 * @arg: Command argument
 *
 * Based on decompiled tx_isp_unlocked_ioctl function
 */
static long tx_isp_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    void __user *argp = (void __user *)arg;
    struct sensor_list_info sensor_list[MAX_SENSORS];
    int fd = (int)(unsigned long)file->private_data;
    struct isp_channel *chn = NULL;
    struct IMPISPDev *dev = ourISPdev;
    int ret = 0;
    int channel = 0;
    // Get our module from private_data
    struct our_tx_isp_module *module = file->private_data;
    // Then get the subdev that contains it
    struct tx_isp_subdev *sd = container_of(module,
                                               struct tx_isp_subdev,
                                               module);

    pr_info("ISP IOCTL called: cmd=0x%x\n", cmd);  // Add this debug line
    pr_info("\n=== IOCTL Debug ===\n");
    pr_info("cmd=0x%x arg=0x%lx\n", cmd, arg);
    pr_info("file=%p flags=0x%x \n",
            file, file->f_flags);

    // Basic validation
    if (!ourISPdev || !ourISPdev->is_open) {
        pr_err("ISP device not initialized or not open\n");
        return -EINVAL;
    }

    switch (cmd) {
    case VIDIOC_INIT_ENCODER:
        pr_info("Initializing encoder subsystem\n");
        {
            int enable;
            if (copy_from_user(&enable, (void __user *)arg, sizeof(enable)))
                return -EFAULT;

            pr_info("Encoder system initialized with enable=%d\n", enable);
            return 0;
        }
	case VIDIOC_REGISTER_SENSOR: {
		struct i2c_client *client = dev->sensor_i2c_client;
		unsigned char val_h = 0, val_l = 0;

		if (!client || !client->adapter) {
			pr_err("No I2C client or adapter available\n");
			return -ENODEV;
		}

		pr_info("Reading sensor ID registers (addr=0x%02x)\n", client->addr);

		// Read ID registers with better error handling
		ret = isp_sensor_read_reg(client, 0x3107, &val_h);
		if (ret) {
			pr_err("Failed to read ID high byte: %d\n", ret);
			return ret;
		}

		ret = isp_sensor_read_reg(client, 0x3108, &val_l);
		if (ret) {
			pr_err("Failed to read ID low byte: %d\n", ret);
			return ret;
		}

		pr_info("SC2336: ID = 0x%02x%02x\n", val_h, val_l);
		return 0;
	}
	case VIDIOC_GET_SENSOR_ENUMERATION: {
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

	    // Fill in the name for this index
	    snprintf(req.name, sizeof(req.name), "sc2336");

	    // Copy the result back to userspace
	    if (copy_to_user((void __user *)arg, &req, sizeof(req))) {
	        pr_err("Failed to copy result to user\n");
	        return -EFAULT;
	    }

	    pr_info("Provided sensor info for index %d: %s\n", req.idx, req.name);
	    return 0;
	}
    case VIDIOC_GET_SENSOR_INFO: {
        pr_info("ISP IOCTL called: cmd=VIDIOC_GET_SENSOR_INFO\n");
        int __user *result = (int __user *)arg;

        // Check if the sensor is initialized
        if (!ourISPdev || !ourISPdev->sensor_i2c_client) {
            pr_err("Sensor not initialized\n");
            return -ENODEV;
        }

        // Write back 1 (not -1) to indicate sensor is present
        if (put_user(1, result)) {
            pr_err("Failed to update sensor result\n");
            return -EFAULT;
        }

        pr_info("Sensor info request: returning success (1)\n");
        return 0;
    }
    case VIDIOC_ENABLE_STREAM: { // 0x800456d2
        struct sensor_enable_param param;
        void __iomem *vic_regs = ourISPdev->reg_base + 0x7800;
        pr_info("Enabling stream\n");
//    	// Save current state
//    	u32 saved_route = readl(vic_regs + 0x10);
//    	u32 saved_mask = readl(vic_regs + 0xc);

        // The decompiled code shows it's passing 0 directly as arg
        // We shouldn't try to copy from user here since arg is the value
        if (arg != 0) {
            pr_err("Invalid enable stream argument\n");
            return -EINVAL;
        }

//        // 3. Initial mode setup - match streaming enable sequence
//        writel(2, vic_regs + 0x4);           // Control register (Mode = 2)
//        writel(0x800c0000, vic_regs + 0x10); // Route/IRQ Enable
//        wmb();
//        udelay(10);
//
//        // Set the interrupt configuration
//        writel(0x33fb, vic_regs + 0xc);      // IRQ Enable
//        writel(0, vic_regs + 0x8);           // Clear IRQ Mask
//        writel(0x3, vic_regs + 0x10);        // Ensure both routes are set
//        wmb();

        pr_info("VIC state after link op 0x%x:\n"
                "  Route: 0x%08x\n"          // Route register
                "  Enable: 0x%08x\n"         // IRQ Enable
                "  Mask: 0x%08x\n",          // IRQ Mask
                cmd,
                readl(vic_regs + 0x10),      // Route
                readl(vic_regs + 0xc),       // IRQ Enable
                readl(vic_regs + 0x8));      // IRQ Mask
	    return ret;
    }
    case TX_ISP_SET_BUF: {  // 0x800856d5
        struct imp_buffer_info local_buf_info;
        struct isp_channel *chn;
        size_t frame_size;
        u8 *vbm_entry;

        if (!ourISPdev) {
            pr_err("ISP device not initialized\n");
            return -EINVAL;
        }

        // Basic validation
        if (!arg || (unsigned long)arg >= 0xfffff001) {
            pr_err("Invalid user pointer\n");
            return -EFAULT;
        }

        if (copy_from_user(&local_buf_info, (void __user *)arg, sizeof(local_buf_info))) {
            pr_err("Failed to copy from user\n");
            return -EFAULT;
        }

        // Magic number handshake - maintain compatibility
        if (local_buf_info.method == 0x203a726f &&
            local_buf_info.phys_addr == 0x33326373) {
            // Initial phase - tell userspace we support DMA
            local_buf_info.method = ISP_ALLOC_DMA;  // Changed to indicate DMA mode
            local_buf_info.phys_addr = 0x1;
            local_buf_info.size = 0x1;
            local_buf_info.virt_addr = (unsigned long)ourISPdev->dma_buf;
            local_buf_info.flags = 0;

            if (copy_to_user((void __user *)arg, &local_buf_info, sizeof(local_buf_info)))
                return -EFAULT;
            return 0;
        }

        // Second phase - actual buffer setup
        if (local_buf_info.phys_addr == 0x1) {
            chn = &ourISPdev->channels[0];
            

            if (!chn) {
                pr_err("No frame channel initialized\n");
                return -EINVAL;
            }

            // Calculate frame size
            frame_size = ALIGN((chn->width * chn->height * 3) / 2, 4096);

            // Setup DMA buffer if not already done
            if (!chn->dma_addr) {
                int ret = setup_dma_buffer(chn);
                if (ret)
                    return ret;
            }

            // Create VBM entry for compatibility
            vbm_entry = kzalloc(0x80, GFP_KERNEL);
            if (!vbm_entry)
                return -ENOMEM;

            // Setup VBM entry to point to our DMA buffer
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
            flags->phys_addr = chn->dma_addr;  // Use our DMA address
            flags->virt = chn->buf_base;       // Use our DMA virtual address
            flags->size = frame_size;

            // Store entry
            if (!chn->vbm_table) {
                chn->vbm_table = kzalloc(sizeof(void *), GFP_KERNEL);
                if (!chn->vbm_table) {
                    kfree(vbm_entry);
                    return -ENOMEM;
                }
            }
            chn->vbm_table[0] = vbm_entry;
            chn->state |= BIT(0);

            // Return info about our DMA buffer
            local_buf_info.method = ISP_ALLOC_DMA;
            local_buf_info.phys_addr = chn->dma_addr;
            local_buf_info.size = frame_size;
            local_buf_info.virt_addr = (unsigned long)chn->buf_base;
            local_buf_info.flags = 0;

            if (copy_to_user((void __user *)arg, &local_buf_info, sizeof(local_buf_info))) {
                kfree(vbm_entry);
                return -EFAULT;
            }

            pr_info("DMA buffer setup: phys=%pad virt=%p size=%zu\n",
                    &chn->dma_addr, chn->buf_base, frame_size);

            return 0;
        }

        pr_err("Invalid buffer setup sequence\n");
        return -EINVAL;
    }
    case VIDIOC_SET_BUF_INFO: {
        struct sensor_buffer_info buf_info = {0};
        void __user *argp = (void __user *)arg;

        pr_info("tx_isp: Handling ioctl VIDIOC_SET_BUF_INFO\n");

        if (!ourISPdev) {
            pr_err("No ISP device\n");
            return -EINVAL;
        }

        if (copy_from_user(&buf_info, argp, sizeof(buf_info))) {
            pr_err("tx_isp: Failed to copy from user\n");
            return -EFAULT;
        }

        pr_info("Buffer info before update: method=0x%x phys=0x%x size=%u\n",
                buf_info.method, buf_info.buffer_start, buf_info.buffer_size);

        // Use actual mapped addresses
        buf_info.method = ISP_ALLOC_KMALLOC;
        buf_info.buffer_start = ourISPdev->dma_buffer_addr;
        buf_info.buffer_size = ourISPdev->rmem_size - DMA_BUFFER_OFFSET;
        buf_info.virt_addr = (unsigned long)ourISPdev->dma_buf + DMA_BUFFER_OFFSET;
        buf_info.flags = 1;

        pr_info("Buffer info after update:\n");
        pr_info("  method=0x%x\n", buf_info.method);
        pr_info("  phys=0x%x\n", buf_info.buffer_start);
        pr_info("  virt=%p\n", (void *)buf_info.virt_addr);
        pr_info("  size=%u\n", buf_info.buffer_size);
        pr_info("  flags=0x%x\n", buf_info.flags);

        if (copy_to_user(argp, &buf_info, sizeof(buf_info)))
            return -EFAULT;

        pr_info("tx_isp: Buffer setup completed successfully\n");
        return 0;
    }
	    case VIDIOC_S_CTRL: {
	        struct v4l2_control ctrl;
	        struct isp_control_regs __iomem *cregs;
	        struct isp_pipeline_regs __iomem *pregs;
	        int ret = 0;

	        if (copy_from_user(&ctrl, (void __user *)arg, sizeof(ctrl)))
	            return -EFAULT;

	        pr_info("Setting ISP control: id=0x%x val=%d\n",
	                ctrl.id, ctrl.value);

	        // Map control registers
	        cregs = (struct isp_control_regs __iomem *)(ourISPdev->reg_base + 0x1100);
	        // Map pipeline registers
	        pregs = (struct isp_pipeline_regs __iomem *)(ourISPdev->reg_base + 0x1140);

	        // Add range validation
	        if (ctrl.value < 0 || ctrl.value > 255) {
	            pr_warn("Control value %d out of range (0-255)\n", ctrl.value);
	            ctrl.value = clamp_val(ctrl.value, 0, 255);
	        }

	        switch (ctrl.id) {
	            // Pipeline controls
	            case ISP_TUNING_BYPASS:
	                writel(!!ctrl.value, &pregs->bypass_ctrl);
	                break;

	            case ISP_TUNING_RUNNING_MODE:
	                writel(ctrl.value, &pregs->running_mode);
	                break;

	            case ISP_TUNING_AE_COMP:
	                writel(ctrl.value, &pregs->ae_comp);
	                break;

	            case ISP_TUNING_MAX_AGAIN:
	                writel(ctrl.value, &pregs->max_again);
	                break;

	            case ISP_TUNING_MAX_DGAIN:
	                writel(ctrl.value, &pregs->max_dgain);
	                break;

	            case ISP_TUNING_DPC:
	                writel(ctrl.value, &pregs->dpc_strength);
	                break;

	            case ISP_TUNING_DRC:
	                writel(ctrl.value, &pregs->drc_strength);
	                break;

	            case ISP_TUNING_DEFOG:
	                writel(ctrl.value, &pregs->defog_strength);
	                break;

	            default:
	                pr_info("Unknown control ID: 0x%x\n", ctrl.id);
	                break;
	        }

	        wmb();  // Ensure writes complete

	        pr_info("Pipeline state: bypass=%d mode=%d ae_comp=%d\n",
	                readl(&pregs->bypass_ctrl),
	                readl(&pregs->running_mode),
	                readl(&pregs->ae_comp));
	    return 0;
	}
    case 0x800456d0: /* TX_ISP_VIDEO_LINK_SETUP */ {
        void __iomem *vic_regs = ourISPdev->vic_regs;
        void __iomem *isp_regs = ourISPdev->reg_base;
        uint32_t link_config;
        struct IMPISPDev *dev = ourISPdev;
        struct tx_isp_subdev *sd;
        unsigned long flags;
        u32 status;
        int timeout;

        pr_info("Starting link setup IOCTL\n");

        // Just validate we have the device
        if (!dev) {
            pr_err("No ISP device available\n");
            return -ENODEV;
        }

        // Validate user input
        if (copy_from_user(&link_config, (void __user *)arg, sizeof(link_config))) {
            pr_err("Failed to copy link config\n");
            return -EFAULT;
        }

        if (link_config >= 2) {
            pr_err("Invalid link configuration: %d\n", link_config);
            return -EINVAL;
        }

        pr_info("Setting up video link %d\n", link_config);

        // Use the frame source subdev from global device
        sd = dev->fs_subdev;
        if (!sd) {
            pr_err("No frame source subdev available\n");
            return -EINVAL;
        }

        if (!sd->inpads || !sd->outpads) {
            pr_err("Subdev pads not initialized\n");
            pr_info("  inpads=%p\n", sd->inpads);
            pr_info("  outpads=%p\n", sd->outpads);
            return -EINVAL;
        }

        pr_info("Found devices and pads:\n");
        pr_info("  subdev=%p\n", sd);
        pr_info("  num_inpads=%d\n", sd->num_inpads);
        pr_info("  num_outpads=%d\n", sd->num_outpads);

        // Use our static configurations
        const struct tx_isp_link_configs *configs = &isp_link_configs[link_config];

        // Link setup logic
        for (int i = 0; i < configs->length; i++) {
            const struct tx_isp_link_config *cfg = &configs->config[i];
            struct tx_isp_subdev_pad *src_pad, *sink_pad;

            pr_info("Setting up link %d of %d\n", i+1, configs->length);

            // Find source pad with validation
            src_pad = find_subdev_link_pad(dev, cfg);
            if (!src_pad) {
                pr_err("Failed to find source pad\n");
                continue;
            }

            pr_info("Found source pad %p\n", src_pad);

            // Find sink pad with validation
            struct tx_isp_link_config sink_cfg = {
                .src = cfg->dst,  // Use destination as source
                .flag = cfg->flag
            };
            sink_pad = find_subdev_link_pad(dev, &sink_cfg);
            if (!sink_pad) {
                pr_err("Failed to find sink pad\n");
                continue;
            }

            pr_info("Found sink pad %p\n", sink_pad);

            // Create forward link with safety checks
            src_pad->link.source = src_pad;
            src_pad->link.sink = sink_pad;
            src_pad->link.flag = cfg->flag;
            src_pad->link.state = TX_ISP_MODULE_ACTIVATE;
            src_pad->state = TX_ISP_PADSTATE_LINKED;

            // Create reverse link with safety checks
            sink_pad->link.source = sink_pad;
            sink_pad->link.sink = src_pad;
            sink_pad->link.flag = cfg->flag;
            sink_pad->link.state = TX_ISP_MODULE_ACTIVATE;
            sink_pad->state = TX_ISP_PADSTATE_LINKED;
        }

        dev->link_state.current_link = link_config;
        pr_info("Link setup completed successfully\n");
        return 0;
    }
    case TX_ISP_SET_AE_ALGO_OPEN: {
      	pr_info("TX_ISP_SET_AE_ALGO_OPEN\n");
        struct isp_ae_algo ae;
        if (copy_from_user(&ae, argp, sizeof(ae))) {
            dev_err(ourISPdev->dev, "[%s][%d] copy from user error\n",
                    __func__, __LINE__);
            return -EFAULT;
        }
        ret = init_ae_algo(ourISPdev, &ae);
        if (!ret && copy_to_user(argp, &ae, sizeof(ae))) {
            dev_err(ourISPdev->dev, "[%s][%d] copy to user error\n",
                    __func__, __LINE__);
            return -EFAULT;
        }
        break;
    }

    case TX_ISP_SET_AWB_ALGO_OPEN: {
      	pr_info("TX_ISP_SET_AWB_ALGO_OPEN\n");
        struct isp_awb_algo awb;
        if (copy_from_user(&awb, argp, sizeof(awb))) {
            dev_err(ourISPdev->dev, "[%s][%d] copy from user error\n",
                    __func__, __LINE__);
            return -EFAULT;
        }
        ret = init_awb_algo(ourISPdev, &awb);
        if (!ret && copy_to_user(argp, &awb, sizeof(awb))) {
            dev_err(ourISPdev->dev, "[%s][%d] copy to user error\n",
                    __func__, __LINE__);
            return -EFAULT;
        }
        break;
    }

    case TX_ISP_WDR_OPEN: {
      	pr_info("TX_ISP_WDR_OPEN\n");
        // Handle WDR enable - matches case 0x800456d8
        unsigned int wdr_en = 1;
        ret = isp_set_wdr_mode(ourISPdev, wdr_en);
        break;
    }

    case TX_ISP_WDR_CLOSE: {
      	pr_info("TX_ISP_WDR_CLOSE\n");
        // Handle WDR disable - matches case 0x800456d9
        unsigned int wdr_en = 0;
        ret = isp_set_wdr_mode(ourISPdev, wdr_en);
        break;
    }

    case TX_ISP_WDR_GET_BUF: {
        pr_info("TX_ISP_WDR_GET_BUF\n");
        // Calculate and return WDR buffer size - matches case 0x800856d7
        struct isp_wdr_buf_size size;
        ret = calculate_wdr_buffer_size(ourISPdev, &size);
        if (!ret && copy_to_user(argp, &size, sizeof(size))) {
            dev_err(ourISPdev->dev, "[%s][%d] copy to user error\n",
                    __func__, __LINE__);
            return -EFAULT;
        }
        break;
    }
    case 0x800456d3: {  // Disable links
        pr_info("TODO Disabling ISP links\n");
		// TODO
        return 0;
    }
    case 0x800456d1: {  // Destroy links
        pr_info("TODO Destroying ISP links\n");
        int __user *result = (int __user *)arg;
        int ret = 0;
		// TODO
        return ret;
    }
	case VIDIOC_CREATE_ENCODER_GROUP:
	case VIDIOC_CREATE_ENCODER_CHN:
	case VIDIOC_REGISTER_ENCODER_CHN:
	    return handle_encoder_ioctls(ourISPdev, cmd, argp);
    case TX_ISP_SET_AE_ALGO_CLOSE: {
        pr_info("TX_ISP_SET_AE_ALGO_CLOSE\n");
        // TODO
        // Cleanup AE algorithm - matches case 0x800456de
        // tisp_ae_algo_deinit();
        // tisp_ae_algo_init(0, NULL);
        break;
    }

    case TX_ISP_SET_AWB_ALGO_CLOSE: {
      	pr_info("TX_ISP_SET_AWB_ALGO_CLOSE\n");
        // TODO
        // Cleanup AWB algorithm - matches case 0x800456e4
        //tisp_awb_algo_deinit();
        //tisp_awb_algo_init(0);
        break;
    }

    case VIDIOC_SET_ROTATION: {
		pr_info("VIDIOC_SET_ROTATION\n");
        // ret = set_rotation(ourISPdev, argp); // TODO
        break;
	}
	case VIDIOC_STREAMOFF:
		pr_info("TODO VIDIOC_STREAMOFF\n");
        // return enable_isp_streaming(ourISPdev, file, channel, false);
	    break;
    // Add to your framechan_ioctl switch:
    case 0x40145609: {  // QBUF handler
        struct {
            u32 index;
            u32 size;
            u32 timestamp;
            u32 flags;
        } req;

        if (copy_from_user(&req, (void __user *)arg, sizeof(req))) {
            pr_err("QBUF: Failed to copy from user\n");
            return -EFAULT;
        }

        pr_info("QBUF: index=%d size=%d flags=0x%x\n",
                req.index, req.size, req.flags);

	    // TODO

        return 0;
    }
	case VIDIOC_S_CROP: {
	    struct v4l2_crop crop;
	    if (copy_from_user(&crop, argp, sizeof(crop)))
	        return -EFAULT;

	    pr_info("Set crop: %dx%d at (%d,%d)\n",
	            crop.c.width, crop.c.height, crop.c.left, crop.c.top);
	    // TODO: Configure ISP cropping
	    break;
	}
    case 0x800856d7:
    case 0xc0045627:
    case SENSOR_CMD_READ_ID:
	case SENSOR_CMD_WRITE_REG:
	case SENSOR_CMD_READ_REG:
	case SENSOR_CMD_SET_GAIN:
	case SENSOR_CMD_SET_EXP:
	case SENSOR_CMD_STREAM_ON:
	case SENSOR_CMD_STREAM_OFF:
    case VIDIOC_STREAMON: // 0x80045612
        pr_info("Sensor command: 0x%x\n", cmd);
        ret = handle_sensor_ioctl(ourISPdev, cmd, argp, file);
        break;
    default:
        pr_info("Unhandled ioctl cmd: 0x%x\n", cmd);
        return -ENOTTY;
    }

    return ret;
}

static int tx_isp_mmap(struct file *filp, struct vm_area_struct *vma) {
    struct IMPISPDev *dev = ourISPdev;
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

static const struct v4l2_file_operations isp_v4l2_fops = {
    .owner          = THIS_MODULE,
    .open           = tx_isp_open,
    .release        = tx_isp_release,
    .unlocked_ioctl = tx_isp_ioctl,
    .mmap = tx_isp_mmap,
};


/* Configure DEIR control registers */
static void configure_deir_control(void) // TODO
{
    /* Add your DEIR control register configurations here */
    /* Use readl/writel for register access */
    pr_info("TODO Configuring DEIR control registers\n");
}

static int tiziano_load_parameters(const char *filename, void __iomem *dest, size_t offset)
{
    struct file *file;
    mm_segment_t old_fs;
    loff_t pos = 0;
    ssize_t bytes_read;
    size_t file_size;
    char *buffer = NULL;
    int ret = 0;

    pr_info("Loading parameters from file: %s\n", filename);

    old_fs = get_fs();
    set_fs(KERNEL_DS);

    file = filp_open(filename, O_RDONLY, 0);
    set_fs(old_fs);

    if (IS_ERR(file)) {
        pr_err("Failed to open parameter file: %s\n", filename);
        return -ENOENT;
    }

    file_size = i_size_read(file->f_path.dentry->d_inode);
    if (file_size <= 0) {
        pr_err("Parameter file is empty or inaccessible: %s\n", filename);
        ret = -EINVAL;
        goto close_file;
    }

    // Step 3: Allocate a buffer to read the file
    buffer = vmalloc(file_size);
    if (!buffer) {
        pr_err("Failed to allocate memory for parameters\n");
        ret = -ENOMEM;
        goto close_file;
    }

    // Step 4: Read the file contents into the buffer
    old_fs = get_fs();
    set_fs(KERNEL_DS);
    bytes_read = kernel_read(file, pos, buffer, file_size);
    set_fs(old_fs);

    if (bytes_read != file_size) {
        pr_err("Failed to read the entire parameter file\n");
        ret = -EIO;
        goto free_buffer;
    }

    pr_info("Copying %zu bytes to destination\n", offset);
    memcpy_toio(dest, buffer, offset);
    pr_info("Copy successful\n");

    free_buffer:
        vfree(buffer);
    close_file:
        filp_close(file, NULL);
    return ret;
}

static struct isp_platform_data isp_pdata = {
    .clock_rate = T31_ISP_FREQ,
};






static int tisp_driver_init(struct device *dev)
{
    int ret;
    const char *param_file = "/etc/sensor/sc2336-t31.bin";
    uint32_t reg_val;

    pr_info("Starting tisp_driver_init...\n");

    // Verify registers are mapped using global ourISPdev
    if (!ourISPdev || !ourISPdev->reg_base) {
        dev_err(dev, "ISP registers not mapped!\n");
        return -EFAULT;
    }

    // Step 1: Allocate memory using vmalloc
    tparams_day = vmalloc(ISP_OFFSET_PARAMS);
    if (!tparams_day) {
        dev_err(dev, "Failed to allocate tparams_day\n");
        return -ENOMEM;
    }
    memset(tparams_day, 0, ISP_OFFSET_PARAMS);

    tparams_night = vmalloc(ISP_OFFSET_PARAMS);
    if (!tparams_night) {
        dev_err(dev, "Failed to allocate tparams_night\n");
        vfree(tparams_day);
        return -ENOMEM;
    }
    memset(tparams_night, 0, ISP_OFFSET_PARAMS);

    dev_info(dev, "tparams_day and tparams_night buffers allocated successfully\n");

    // Step 2: Load parameters from the file
    ret = tiziano_load_parameters(param_file, tparams_day, ISP_OFFSET_PARAMS);
    if (ret) {
        dev_err(dev, "Failed to load parameters from file\n");
        goto cleanup;
    }
    dev_info(dev, "Parameters loaded successfully from %s\n", param_file);

    // Step 3: Handle isp_memopt settings
    if (isp_memopt == 1) {
        dev_info(dev, "Applying isp_memopt settings\n");
        writel(0, tparams_day + 0x264);
        writel(isp_memopt, tparams_day + 0x26c);
        writel(0, tparams_day + 0x27c);
        writel(0, tparams_day + 0x274);

        writel(0, tparams_night + 0x264);
        writel(isp_memopt, tparams_night + 0x26c);
        writel(0, tparams_night + 0x27c);
        writel(0, tparams_night + 0x274);
    }

    // Step 4: Write the parameters to hardware registers using global ourISPdev->reg_base
    writel((unsigned long)tparams_day, ourISPdev->reg_base + 0x84b50);
    dev_info(dev, "tparams_day written to register successfully\n");

    return 0;

cleanup:
    if (tparams_night)
        vfree(tparams_night);
    if (tparams_day)
        vfree(tparams_day);
    return ret;
}

static int setup_channel_attrs(struct isp_channel *chn, struct imp_channel_attr *attr)
{
    // Store new attributes
    memcpy(&chn->attr, attr, sizeof(*attr));

    // For channel 1, ensure proper scaling setup
    if (chn->channel_id == 1) {
        chn->attr.crop_enable = 0;
        chn->attr.crop.width = 1920;
        chn->attr.crop.height = 1080;
        chn->attr.crop.x = 0;
        chn->attr.crop.y = 0;

        chn->attr.scaler_enable = 1;
        chn->attr.scaler_outwidth = 640;
        chn->attr.scaler_outheight = 360;

        // Set picture dimensions to match output
        chn->attr.picwidth = 640;
        chn->attr.picheight = 360;
    } else {
        // For other channels, set picture dimensions to match input
        chn->attr.picwidth = chn->attr.width;
        chn->attr.picheight = chn->attr.height;
    }

    pr_info("  Resolution: %dx%d\n", chn->attr.width, chn->attr.height);
    pr_info("  Crop: enable=%d %dx%d at (%d,%d)\n",
            chn->attr.crop_enable,
            chn->attr.crop.width, chn->attr.crop.height,
            chn->attr.crop.x, chn->attr.crop.y);
    pr_info("  Scale: enable=%d %dx%d\n",
            chn->attr.scaler_enable,
            chn->attr.scaler_outwidth, chn->attr.scaler_outheight);
    pr_info("  Picture: %dx%d\n",
            chn->attr.picwidth, chn->attr.picheight);

    return 0;
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

// Helper function to generate Y plane test pattern
static void generate_y_pattern(u8 *buffer, int width, int height, int y_offset)
{
    int x, y;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            // Create some kind of visible pattern
            // This creates diagonal stripes
            buffer[y * width + x] = ((x + y + y_offset) / 32) % 2 ? 235 : 16;
        }
    }
}

// Helper function to generate UV plane test pattern
static void generate_uv_pattern(u8 *buffer, int width, int height, int y_offset)
{
    int x, y;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x += 2) {
            // Alternate between a few colors for UV values
            buffer[y * width + x] = 128;     // U value
            buffer[y * width + x + 1] = 128; // V value
        }
    }
}


static int init_frame_group(struct isp_channel *chn)
{
    struct frame_group *group;
    struct group_data *data;
    void __iomem *mapped_addr;
    unsigned long phys_mem;
    unsigned long imp_group_update = 0x12f70;
    int order = get_order(PAGE_ALIGN(sizeof(*data)));

    // Try to allocate at a higher address first
    phys_mem = __get_free_pages(GFP_KERNEL | GFP_DMA | __GFP_HIGHMEM, order);
    if (!phys_mem) {
        // Fallback to normal allocation
        phys_mem = __get_free_pages(GFP_KERNEL | GFP_DMA, order);
        if (!phys_mem) {
            pr_err("Failed to allocate physical memory\n");
            return -ENOMEM;
        }
    }

    // Rest of function same but add more debug
    mapped_addr = ioremap(virt_to_phys((void *)phys_mem), PAGE_ALIGN(sizeof(*data)));
    if (!mapped_addr) {
        pr_err("Failed to map memory\n");
        free_pages(phys_mem, order);
        return -ENOMEM;
    }

    // Use this mapped memory for our data structure
    data = (struct group_data *)mapped_addr;
    memset(data, 0, sizeof(*data));

    group = kzalloc(sizeof(*group), GFP_KERNEL | GFP_DMA);
    if (!group) {
        pr_err("Failed to allocate frame group\n");
        iounmap(mapped_addr);
        free_pages(phys_mem, order);
        return -ENOMEM;
    }

    // Set up structures
    data->base = group;
    data->update_ptr = group;
    data->channel = chn->channel_id;
    data->state = 0;
    data->flags = 0;
    data->handler = 0x9a654;

    // Initialize frame group
    snprintf(group->name, sizeof(group->name), "group-%d", chn->channel_id);
    group->handler_fn = data;
    group->update_fn = data;
    group->group_update = (void*)imp_group_update;
    group->self = (void*)virt_to_phys((void *)phys_mem);
    group->channel = chn->channel_id;
    group->state = 0;
    group->handler = 0x9a654;

    // Add debug about key offsets
    pr_info("Frame group structure details:\n"
            "  Group kernel: %p\n"
            "  Data kernel: %p\n"
            "  Physical mem: 0x%lx\n"
            "  Mapped addr: %p\n"
            "  Data->base offset: 0x%lx\n"
            "  Data->update_ptr offset: 0x%lx\n"
            "  Expected write at: 0x%lx\n",
            group, data, phys_mem, mapped_addr,
            offsetof(struct group_data, base),
            offsetof(struct group_data, update_ptr),
            phys_mem + 0x11b4);

    chn->group = group;
    chn->group_data = data;
    chn->group_phys_mem = phys_mem;
    chn->group_mapped_addr = mapped_addr;

    return 0;
}


static int framechan_open(struct inode *inode, struct file *file)
{
    int channel = iminor(inode);
    struct IMPISPDev *dev = ourISPdev;
    struct isp_channel *chn = &dev->channels[channel];
    int ret;

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

    // Initialize frame group first since other init may depend on it
    ret = init_frame_group(chn);
    if (ret) {
        pr_err("Failed to initialize frame group: %d\n", ret);
        kfree(chn->queue);
        chn->queue = NULL;
        return ret;
    }

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

    // Initialize lists in queue
    INIT_LIST_HEAD(&chn->queue->ready_list);
    INIT_LIST_HEAD(&chn->queue->done_list);
    spin_lock_init(&chn->queue->queue_lock);

    // Start in state 3 (ready for streaming)
    chn->state = 3;
    chn->queue->stream_state = 0;

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

    if (!chn || !chn->queue) {
        pr_err("Invalid channel state\n");
        return -EINVAL;
    }

    // Copy buffer info from userspace
    if (copy_from_user(&v4l2_buf, (void __user *)arg, sizeof(v4l2_buf))) {
        pr_err("Failed to copy buffer from user\n");
        return -EFAULT;
    }

    pr_info("Queue buffer request: index=%d type=%d memory=%d count=%d\n",
            v4l2_buf.index, v4l2_buf.type, v4l2_buf.memory, chn->queue->buf_count);

    mutex_lock(&chn->queue->lock);
    spin_lock_irqsave(&chn->queue->queue_lock, flags);

    // Validate buffer index
    if (v4l2_buf.index >= chn->queue->buf_count) {
        pr_err("Invalid buffer index %d\n", v4l2_buf.index);
        ret = -EINVAL;
        goto unlock;
    }

    // Get buffer
    vbuf = chn->queue->bufs[v4l2_buf.index];
    if (!vbuf) {
        pr_err("No buffer at index %d\n", v4l2_buf.index);
        ret = -EINVAL;
        goto unlock;
    }

    // Check buffer type and memory
    if (vbuf->type != v4l2_buf.type) {
        pr_err("Invalid buffer type\n");
        ret = -EINVAL;
        goto unlock;
    }

    // Remove from lists if present
    if (!list_empty(&vbuf->list)) {
        list_del_init(&vbuf->list);
    }

    // Make buffer available for DMA
    dma_sync_single_for_device(chn->dev,
                              chn->dma_addr + (vbuf->index * chn->required_size),
                              chn->required_size,
                              DMA_FROM_DEVICE);

    // Clear flags and add to ready list
    vbuf->flags &= ~V4L2_BUF_FLAG_DONE;
    vbuf->flags |= V4L2_BUF_FLAG_QUEUED;
    vbuf->status = 0;
    list_add_tail(&vbuf->list, &chn->queue->ready_list);

    pr_info("Buffer queued: index=%d flags=0x%x\n", vbuf->index, vbuf->flags);

unlock:
    spin_unlock_irqrestore(&chn->queue->queue_lock, flags);
    mutex_unlock(&chn->queue->lock);
    return ret;
}

static int framechan_dqbuf(struct isp_channel *chn, unsigned long arg)
{
    struct v4l2_buffer v4l2_buf;
    struct video_buffer *vbuf = NULL;
    unsigned long flags;
    int ret;

    pr_info("DQBUF processing start for channel %d\n", chn->channel_id);

    // Get buffer request from userspace
    if (copy_from_user(&v4l2_buf, (void __user *)arg, sizeof(v4l2_buf))) {
        pr_err("Failed to copy from user\n");
        return -EFAULT;
    }

    // Wait for buffer with timeout and better error handling
    ret = wait_event_interruptible_timeout(chn->queue->wait,
            !list_empty(&chn->queue->done_list) ||
            !(chn->queue->stream_state & 1),
            msecs_to_jiffies(1000));

    if (ret == 0) {
        pr_err("DQBUF timeout waiting for buffer\n");
        return -ETIMEDOUT;
    }
    if (ret == -ERESTARTSYS) {
        pr_err("DQBUF wait interrupted\n");
        return ret;
    }

    // Get buffer with proper locking
    mutex_lock(&chn->queue->lock);
    spin_lock_irqsave(&chn->queue->queue_lock, flags);

    if (!list_empty(&chn->queue->done_list)) {
        vbuf = list_first_entry(&chn->queue->done_list,
                               struct video_buffer, list);
        list_del_init(&vbuf->list);

        pr_info("DQBUF dequeued buffer %d with flags 0x%x\n",
                vbuf->index, vbuf->flags);
    } else {
        spin_unlock_irqrestore(&chn->queue->queue_lock, flags);
        mutex_unlock(&chn->queue->lock);
        return -EAGAIN;
    }

    // Fill v4l2_buffer info
    v4l2_buf.index = vbuf->index;
    v4l2_buf.type = vbuf->type;
    v4l2_buf.memory = vbuf->memory;
    v4l2_buf.flags = vbuf->flags;
    v4l2_buf.field = V4L2_FIELD_NONE;
    v4l2_buf.sequence = vbuf->status & 0xFF000000;
    v4l2_buf.m.offset = vbuf->index * chn->required_size;
    v4l2_buf.length = chn->required_size;
    v4l2_buf.bytesused = chn->required_size; // Add this

    // Reset buffer state
    vbuf->flags &= ~(V4L2_BUF_FLAG_QUEUED | V4L2_BUF_FLAG_DONE);

    spin_unlock_irqrestore(&chn->queue->queue_lock, flags);
    mutex_unlock(&chn->queue->lock);

    if (copy_to_user((void __user *)arg, &v4l2_buf, sizeof(v4l2_buf))) {
        pr_err("Failed to copy to user\n");
        return -EFAULT;
    }

    pr_info("DQBUF complete: buffer %d\n", vbuf->index);
    return 0;
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

static int get_channel_pool(struct isp_channel *chn)
{
    if (!chn->pool.bound)
        return -1;
    return chn->pool.pool_id;
}


static void init_channel_attributes(struct isp_channel *chn)
{
    // Set enable flag to indicate attributes are initialized
    chn->attr.enable = 1;

    // Initialize other fields as needed...
    chn->attr.width = 1920;
    chn->attr.height = 1080;
    chn->attr.format = V4L2_PIX_FMT_NV12;
    chn->attr.picwidth = 1920;
    chn->attr.picheight = 1080;
    chn->attr.crop_enable = 0;
    chn->attr.crop.x = 0;
    chn->attr.crop.y = 0;
    chn->attr.crop.width = 1920;
    chn->attr.crop.height = 1080;
    chn->attr.scaler_enable = 0;
    chn->attr.scaler_outwidth = 1920;
    chn->attr.scaler_outheight = 1080;
    chn->attr.fps_num = 30;
    chn->attr.fps_den = 1;
    chn->attr.reserved[0] = 0;
    chn->attr.reserved[1] = 0;
    chn->attr.reserved[2] = 0;
    chn->attr.reserved[3] = 0;
    chn->attr.reserved[4] = 0;

    // Set flags indicating attributes are initialized
    chn->flags |= FS_ATTR_SET;
}

struct reqbuf_request {
    __u32 count;
    __u32 type;
    __u32 memory;
    __u32 capabilities;
    __u32 reserved[1];
} __attribute__((packed));  // Make sure no padding

static int framechan_reqbufs(struct isp_channel *chn, unsigned long arg)
{
    struct reqbuf_request req;
    struct frame_queue *queue;
    struct video_buffer **new_bufs;
    size_t buf_size, meta_size;
    int ret = 0, i;
    unsigned long flags;

    pr_info("Request buffers for channel %d\n", chn->channel_id);

    if (!chn || !ourISPdev) {
        pr_err("Invalid channel or ISP device\n");
        return -EINVAL;
    }

    if (copy_from_user(&req, (void __user *)arg, sizeof(req))) {
        pr_err("Failed to copy request from user\n");
        return -EFAULT;
    }

    // Validate request parameters - accept both MMAP and USERPTR
    if (req.count > 0 &&
        req.memory != V4L2_MEMORY_MMAP &&
        req.memory != V4L2_MEMORY_USERPTR) {
        pr_err("Unsupported memory type: %d\n", req.memory);
        return -EINVAL;
    }

    queue = chn->queue;
    if (!queue) {
        pr_err("Channel queue not initialized\n");
        return -EINVAL;
    }

    // Buffer size from channel - page aligned
    // Calculate sizes
    buf_size = calculate_buffer_size(chn->width, chn->height, chn->fmt);
    meta_size = sizeof(struct frame_metadata);  // Add this struct definition
    chn->required_size = buf_size;

    // Setup DMA buffer if not already done
    if (!chn->dma_addr) {
        ret = setup_dma_buffer(chn);
        if (ret) {
            pr_err("Failed to setup DMA buffer: %d\n", ret);
            return ret;
        }
    }

    mutex_lock(&queue->lock);
    spin_lock_irqsave(&queue->queue_lock, flags);

    // Free existing buffers
    if (queue->bufs) {
        for (i = 0; i < queue->buf_count; i++) {
            if (queue->bufs[i]) {
                if (!list_empty(&queue->bufs[i]->list))
                    list_del(&queue->bufs[i]->list);
                kfree(queue->bufs[i]);
            }
        }
        kfree(queue->bufs);
        queue->bufs = NULL;
        queue->buf_count = 0;
    }

    // If count is 0, just cleanup and return
    if (req.count == 0) {
        queue->memory_type = 0;
        spin_unlock_irqrestore(&queue->queue_lock, flags);
        mutex_unlock(&queue->lock);
        return 0;
    }

    // Allocate buffer array first
    new_bufs = kzalloc(sizeof(struct video_buffer *) * req.count, GFP_ATOMIC);
    if (!new_bufs) {
        ret = -ENOMEM;
        goto out_unlock;
    }

    // Setup DMA buffer if not already done
    if (!chn->dma_addr) {
        chn->required_size = buf_size + meta_size; // Include metadata size
        ret = setup_dma_buffer(chn);
        if (ret) {
            pr_err("Failed to setup DMA buffer: %d\n", ret);
            return ret;
        }
    }

    // Allocate metadata DMA address array
    chn->meta_dma = kzalloc(sizeof(dma_addr_t) * req.count, GFP_ATOMIC);
    if (!chn->meta_dma) {
        ret = -ENOMEM;
        kfree(new_bufs);  // Don't forget to free new_bufs before returning
        goto out_unlock;
    }

    for (i = 0; i < req.count; i++) {
        struct video_buffer *buf = kzalloc(sizeof(*buf), GFP_ATOMIC);
        if (!buf) {
            ret = -ENOMEM;
            goto err_free_bufs;
        }

        // Setup buffer info
        buf->index = i;
        buf->type = req.type;
        buf->memory = req.memory;
        buf->flags = 0;
        buf->queue = queue;
        INIT_LIST_HEAD(&buf->list);
        buf->data = chn->buf_base + (i * buf_size);

        // Allocate metadata with GFP_ATOMIC
        buf->meta = dma_alloc_coherent(chn->dev, sizeof(struct frame_metadata),
                                     &chn->meta_dma[i], GFP_ATOMIC);
        if (!buf->meta) {
            kfree(buf);
            ret = -ENOMEM;
            goto err_free_bufs;
        }

        // Initialize metadata
        buf->meta->magic = 0x100100;
        buf->meta->group_ptr = chn->group;
        buf->meta->frame_num = buf->index;
        buf->meta->ref_count = 1;
        buf->meta->state = FRAME_STATE_FREE;
        buf->meta->flags = 0;
        buf->meta->size = buf_size;
        buf->meta->type = chn->fmt;
        buf->meta->handler = (void *)0x9a654;
        buf->meta->channel = chn->channel_id;
        buf->meta->sequence = 0;
        buf->meta->timestamp = ktime_get_real_ns();
        memset(buf->meta->padding, 0, sizeof(buf->meta->padding));
        buf->meta->done_flag = 0;

        // Store in array
        new_bufs[i] = buf;
        list_add_tail(&buf->list, &queue->ready_list);

        pr_info("Buffer %d setup:\n"
                "  Buffer addr: %p\n"
                "  Virtual: %p\n"
                "  Physical: 0x%x\n"
                "  Meta addr: %p\n"
                "  Meta DMA: %pad\n",
                i, buf, buf->data,
                chn->dma_addr + (i * buf_size),
                buf->meta, &chn->meta_dma[i]);
    }

    queue->bufs = new_bufs;
    queue->buf_count = req.count;
    queue->memory_type = req.memory;

    pr_info("Request buffers complete:\n"
            "  Count: %d\n"
            "  Base addr: %p\n"
            "  Memory type: %d\n",
            queue->buf_count, chn->buf_base, queue->memory_type);

    spin_unlock_irqrestore(&queue->queue_lock, flags);
    mutex_unlock(&queue->lock);
    return 0;

err_free_bufs:
    while (--i >= 0) {
        if (new_bufs[i]) {
            list_del(&new_bufs[i]->list);
            kfree(new_bufs[i]);
        }
    }
    kfree(new_bufs);

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
    struct IMPISPDev *dev = ourISPdev;
    int ret = 0;

    pr_info("Frame channel IOCTL: cmd=0x%x arg=0x%lx\n", cmd, arg);

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
        ret = handle_sensor_ioctl(ourISPdev, cmd, (void __user *)arg, file);
        break;
    default:
        pr_info("Unhandled ioctl cmd: 0x%x\n", cmd);
        ret = -ENOTTY;
    }

    //mutex_unlock(&chn->lock);
    return ret;
}

static void cleanup_frame_thread(struct isp_channel *chn)
{
    struct frame_thread_data *thread_data;
    unsigned long flags;

    if (!chn)
        return;

    thread_data = chn->thread_data;
    if (!thread_data)
        return;

    // First disable streaming
    spin_lock_irqsave(&chn->queue->queue_lock, flags);
    chn->queue->stream_state &= ~1;
    spin_unlock_irqrestore(&chn->queue->queue_lock, flags);

    // Signal thread to stop and wake it
    atomic_set(&thread_data->should_stop, 1);
    if (thread_data->task) {
        wake_up_interruptible(&chn->queue->wait);

        // Wait for thread to exit
        wait_event_timeout(chn->queue->wait,
                          !atomic_read(&thread_data->thread_running),
                          msecs_to_jiffies(1000));

        kthread_stop(thread_data->task);
        thread_data->task = NULL;
    }

    // Now safe to free thread data
    kfree(thread_data);
    chn->thread_data = NULL;
}

static void cleanup_frame_group(struct isp_channel *chn)
{
    if (chn) {
        if (chn->group_data) {
            kfree(chn->group_data);
            chn->group_data = NULL;
        }
        if (chn->group) {
            kfree(chn->group);
            chn->group = NULL;
        }
    }
}

static int framechan_release(struct inode *inode, struct file *file)
{
    struct isp_channel *chn = file->private_data;
    struct video_buffer *buf, *tmp;
    unsigned long flags;

    if (!chn)
        return 0;

    pr_info("Starting channel %d cleanup\n", chn->channel_id);

    // First mark channel as not streaming
    if (chn->queue) {
        spin_lock_irqsave(&chn->queue->queue_lock, flags);
        chn->queue->stream_state = 0;
        spin_unlock_irqrestore(&chn->queue->queue_lock, flags);
    }

    // Clean up frame group first since it may reference other structures
    cleanup_frame_group(chn);

    // Clean up remaining resources...
    cleanup_buffer(chn);

    // Clear channel state
    chn->is_open = 0;
    file->private_data = NULL;

    pr_info("Channel %d cleanup complete\n", chn->channel_id);
    return 0;
}


// Add mmap handler to map this memory to userspace
static int framechan_mmap(struct file *file, struct vm_area_struct *vma)
{
    struct isp_channel *chn = file->private_data;
    unsigned long size = vma->vm_end - vma->vm_start;
    unsigned long pfn;

    // Check if this is a frame group mapping request
    if (vma->vm_pgoff == (virt_to_phys(chn->group) >> PAGE_SHIFT)) {
        // Map frame group
        if (size > sizeof(struct frame_group))
            return -EINVAL;

        pfn = virt_to_phys(chn->group) >> PAGE_SHIFT;
        return remap_pfn_range(vma, vma->vm_start, pfn,
                             size, vma->vm_page_prot);
    }

    // Calculate start PFN from DMA physical address
    pfn = chn->dma_addr >> PAGE_SHIFT;

    // Set memory type for DMA
    vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);

    // Map DMA memory to userspace
    if (io_remap_pfn_range(vma, vma->vm_start,
                          pfn,
                          size,
                          vma->vm_page_prot)) {
        pr_err("Failed to map memory to userspace\n");
        return -EAGAIN;
                          }

    pr_info("Mapped DMA memory to userspace:\n"
            "  User VA: 0x%lx - 0x%lx\n"
            "  Phys: 0x%x\n"
            "  Size: %lu\n",
            vma->vm_start, vma->vm_end,
            chn->dma_addr, size);

    return 0;
}

static struct file_operations framechan_fops = {
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

static int csi_sensor_ops_ioctl(void *arg1, int cmd)
{
    struct IMPISPDev *dev = (struct IMPISPDev *)arg1;
    int ret = 0;

    if (!dev || (unsigned long)dev >= 0xfffff001) {
        return 0;
    }

    switch(cmd) {
        case 0x200000e:  // CSI Type 1 check
            if (readl(dev->reg_base + 0x110 + 0x14) == 1) {
                writel(3, dev->reg_base + 0x128);  // Set state 3
            }
            break;

        case 0x200000f:  // CSI State 4 check
            if (readl(dev->reg_base + 0x110 + 0x14) == 1) {
                writel(4, dev->reg_base + 0x128);  // Set state 4
            }
            break;

        case 0x200000c:  // CSI Init
            ret = csi_core_ops_init(dev, 1);
            break;

        default:
            break;
    }

    return 0;
}






static int init_core_state(struct IMPISPDev *dev)
{
    // Initialize core state


    return 0;
}


static int init_irq_handler(struct IMPISPDev *dev)
{
    int ret;

    // Get the IRQ number from platform device
    dev->irq = platform_get_irq(to_platform_device(dev->dev), 0);
    if (dev->irq < 0) {
        pr_err("Failed to get platform IRQ number: %d\n", dev->irq);
        return dev->irq;
    }

    // Register IRQ handler with correct flags
    ret = devm_request_irq(dev->dev, dev->irq, tx_isp_irq_handler,
                        IRQF_SHARED | IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
                          "tx-isp", dev);
    if (ret) {
        dev_err(dev->dev, "Failed to request IRQ %d: %d\n",
                dev->irq, ret);
        return ret;
    }

    // Initialize state
    spin_lock_init(&dev->irq_data->lock);
    dev->irq_enabled = 1;

    // Debug output
    pr_info("IRQ initialized: number=%d handler=%p\n",
            dev->irq, tx_isp_irq_handler);

    return 0;
}


/* Memory initialization */


static void cleanup_isp_subsystems(struct IMPISPDev *dev)
{
    pr_info("TODO Cleaning up ISP subsystems\n");

    /* Disable all interrupts */
//    writel(0x0, dev->reg_base + ISP_INT_MASK_REG);
//    wmb();

    // TODO
}

static void tx_isp_cleanup_resources(struct IMPISPDev *dev)
{
    /* Unmap ISP registers */
    if (dev->reg_base) {
        devm_iounmap(dev->dev, dev->reg_base);
        dev->reg_base = NULL;
    }

    /* Release ISP MMIO region */
    if (dev->resources.mmio_res) {
        release_resource(dev->resources.mmio_res);
        devm_kfree(dev->dev, dev->resources.mmio_res);
        dev->resources.mmio_res = NULL;
    }

    /* Unmap CSI registers */
    if (dev->resources.csi_regs) {
        devm_iounmap(dev->dev, dev->resources.csi_regs);
        dev->resources.csi_regs = NULL;
    }

    /* Release CSI memory region */
    if (dev->resources.csi_res) {
        release_resource(dev->resources.csi_res);
        devm_kfree(dev->dev, dev->resources.csi_res);
        dev->resources.csi_res = NULL;
    }
}

/* Resource initialization function */
static int tx_isp_init_resources(struct IMPISPDev *dev, struct platform_device *pdev)
{
    /* Map ISP registers - use known hardware addresses */
    dev->resources.mmio_phys = ISP_BASE_ADDR;  // 0x13300000
    dev->resources.mmio_res = request_mem_region(dev->resources.mmio_phys,
                                               ISP_REG_SIZE,  // 0x10000
                                               "tx-isp-regs");
    if (!dev->resources.mmio_res) {
        pr_err("Failed to request ISP memory region at 0x%08x\n",
               dev->resources.mmio_phys);
        return -EBUSY;
    }

    dev->reg_base = ioremap(dev->resources.mmio_phys, ISP_REG_SIZE);
    if (!dev->reg_base) {
        pr_err("Failed to map ISP registers\n");
        goto err_release_mmio;
    }

    /* Map CSI registers */
    dev->resources.csi_phys = CSI_BASE_ADDR;  // 0x10022000
    dev->resources.csi_res = request_mem_region(dev->resources.csi_phys,
                                              CSI_REG_SIZE,  // 0x1000
                                              "tx-isp-csi");
    if (!dev->resources.csi_res) {
        pr_err("Failed to request CSI memory region at 0x%08x\n",
               dev->resources.csi_phys);
        goto err_unmap_mmio;
    }

    dev->resources.csi_regs = ioremap(dev->resources.csi_phys, CSI_REG_SIZE);
    if (!dev->resources.csi_regs) {
        pr_err("Failed to map CSI registers\n");
        goto err_release_csi;
    }

    pr_info("Resources initialized:\n");
    pr_info("  ISP: phys=0x%08x virt=%p\n",
            dev->resources.mmio_phys, dev->reg_base);
    pr_info("  CSI: phys=0x%08x virt=%p\n",
            dev->resources.csi_phys, dev->resources.csi_regs);

    return 0;

err_release_csi:
    release_mem_region(dev->resources.csi_phys, CSI_REG_SIZE);
err_unmap_mmio:
    iounmap(dev->reg_base);
err_release_mmio:
    release_mem_region(dev->resources.mmio_phys, ISP_REG_SIZE);
    return -ENOMEM;
}


static void unregister_subdev(struct platform_device *pdev)
{
    if (pdev)
        platform_device_unregister(pdev);
}


static struct resource tx_isp_vic_resources[] = {
    [0] = {
        .start  = ISP_BASE_ADDR + 0x7800, // VIC offset
        .end    = ISP_BASE_ADDR + 0x7800 + 0x1000 - 1,
        .flags  = IORESOURCE_MEM,
        .name   = "tx-isp-vic-regs",
    },
    [1] = {
        .start  = 37,  // VIC gets IRQ 37
        .end    = 37,
        .flags  = IORESOURCE_IRQ,
        .name   = "tx-isp-vic-irq",
    }
};

static int register_isp_subdevices(struct IMPISPDev *dev) {
    int ret;

    pr_info("Creating ISP subdevices...\n");

    // VIC subdevice
    dev->vic_pdev = platform_device_alloc("tx-isp-vic", 0);
    if (!dev->vic_pdev)
        return -ENOMEM;

    ret = platform_device_add_resources(dev->vic_pdev, tx_isp_vic_resources,
                                      ARRAY_SIZE(tx_isp_vic_resources));
    if (ret) {
        platform_device_put(dev->vic_pdev);
        return ret;
    }
    platform_set_drvdata(dev->vic_pdev, dev);
    ret = platform_device_add(dev->vic_pdev);
    if (ret) {
        platform_device_put(dev->vic_pdev);
        return ret;
    }

    // CSI subdevice
    dev->csi_pdev = platform_device_alloc("tx-isp-csi", 0);
    if (!dev->csi_pdev) {
        dev_err(dev->dev, "Failed to allocate CSI device\n");
        ret = -ENOMEM;
        goto err_unregister_vic;
    }
    platform_set_drvdata(dev->csi_pdev, dev);
    ret = platform_device_add(dev->csi_pdev);
    if (ret) {
        platform_device_put(dev->csi_pdev);
        goto err_unregister_vic;
    }

    // VIN subdevice
    dev->vin_pdev = platform_device_alloc("tx-isp-vin", 0);
    if (!dev->vin_pdev) {
        dev_err(dev->dev, "Failed to allocate VIN device\n");
        ret = -ENOMEM;
        goto err_unregister_csi;
    }
    platform_set_drvdata(dev->vin_pdev, dev);
    ret = platform_device_add(dev->vin_pdev);
    if (ret) {
        platform_device_put(dev->vin_pdev);
        goto err_unregister_csi;
    }

    // Core subdevice
    dev->core_pdev = platform_device_alloc("tx-isp-core", 0);
    if (!dev->core_pdev) {
        dev_err(dev->dev, "Failed to allocate Core device\n");
        ret = -ENOMEM;
        goto err_unregister_vin;
    }
    platform_set_drvdata(dev->core_pdev, dev);
    ret = platform_device_add(dev->core_pdev);
    if (ret) {
        platform_device_put(dev->core_pdev);
        goto err_unregister_vin;
    }

    // Frame source subdevice
    dev->fs_pdev = platform_device_alloc("tx-isp-fs", 0);
    if (!dev->fs_pdev) {
        dev_err(dev->dev, "Failed to allocate FS device\n");
        ret = -ENOMEM;
        goto err_unregister_core;
    }
    platform_set_drvdata(dev->fs_pdev, dev);
    ret = platform_device_add(dev->fs_pdev);
    if (ret) {
        platform_device_put(dev->fs_pdev);
        goto err_unregister_core;
    }

    // Register the misc device for /dev/isp-m0
    ret = misc_register(&isp_m0_miscdev);
    if (ret) {
        pr_err("Failed to register isp-m0 misc device: %d\n", ret);
        goto err_unregister_core;
    }

    dev_info(dev->dev, "ISP subdevices registered successfully\n");
    return 0;

err_unregister_core:
    platform_device_unregister(dev->core_pdev);
err_unregister_vin:
    platform_device_unregister(dev->vin_pdev);
err_unregister_csi:
    platform_device_unregister(dev->csi_pdev);
err_unregister_vic:
    platform_device_unregister(dev->vic_pdev);
    return ret;
}

int tx_isp_vic_probe(struct platform_device *pdev)
{
    struct IMPISPDev *dev = platform_get_drvdata(pdev);
    int ret;

    pr_info("VIC probe called for device %p\n", dev);

    if (!dev || !dev->reg_base) {
        pr_err("Invalid device state in VIC probe\n");
        return -EINVAL;
    }

    // Map VIC registers at correct offset
    dev->vic_regs = dev->reg_base + 0x7800;

    // Initialize lock
    spin_lock_init(&dev->vic_lock);
    mutex_init(&dev->core.lock);  // Matches OEM mutex init

    // Set initial state to 1 as per OEM
    // dev->status.vic.state = 1;

    pr_info("VIC probe complete: regs=%p\n", dev->vic_regs);
    return 0;
}

static int tx_isp_vin_probe(struct platform_device *pdev)
{
    struct IMPISPDev *dev = platform_get_drvdata(pdev);

    pr_info("VIN probe called for device %p\n", dev);

    if (!dev) {
        pr_err("Invalid device state in VIN probe\n");
        return -EINVAL;
    }

    // Initialize mutex for VIN state changes
    mutex_init(&dev->vin.lock);

    pr_info("VIN probe complete\n");
    return 0;
}

static int tx_isp_vic_remove(struct platform_device *pdev)
{
    struct IMPISPDev *dev = platform_get_drvdata(pdev);

    pr_info("VIC remove called\n");

    if (dev) {
        dev->vic_regs = NULL;
        dev->vic_pdev = NULL;
    }

    return 0;
}

static int tx_isp_vin_remove(struct platform_device *pdev)
{
    struct IMPISPDev *dev = platform_get_drvdata(pdev);

    pr_info("VIN remove called\n");

    if (dev) {
        dev->vin_pdev = NULL;
    }

    return 0;
}

static int tx_isp_core_probe(struct platform_device *pdev)
{
    struct IMPISPDev *dev = platform_get_drvdata(pdev);
    int ret;

    pr_info("Core probe called\n");

    if (!dev || !dev->reg_base) {
        dev_err(&pdev->dev, "No ISP device data\n");
        return -EINVAL;
    }

    // For core probe, we don't need to request the memory region again
    // since it's already mapped in the main ISP probe
    dev->core.state = 1;  // Initialize core state
    mutex_init(&dev->core.lock);

    // Initialize basic core hardware
    ret = isp_hw_init(dev);
    if (ret) {
        pr_err("Failed to initialize core hardware: %d\n", ret);
        return ret;
    }

    pr_info("Core probe complete: dev=%p reg_base=%p\n",
            dev, dev->reg_base);
    return 0;
}

static int tx_isp_core_remove(struct platform_device *pdev)
{
    pr_info("Core remove called\n");
    return 0;
}



static int register_vic_irq(struct IMPISPDev *dev)
{
    int ret;

//    // Set up handler function - this matches OEM code
//    dev->handler_function = tx_isp_enable_irq; // Main ISP IRQ enable function // TODO vic enable?
//    dev->handler_data = dev;                   // Pass dev as context
//    dev->irq_enabled = 0;  // Start disabled

    ret = request_threaded_irq(37,
                             isp_irq_handler,  // This handles VIC interrupts
                             NULL,
                             IRQF_SHARED | IRQF_TRIGGER_HIGH,
                             "tx-isp-vic",
                             dev);
    if (ret) {
        pr_err("Failed to request VIC IRQ %d: %d\n", dev->irq, ret);
        return ret;
    }

    // Initially disable
    tx_vic_disable_irq(dev);

    pr_info("VIC IRQ %d registered with handler %p\n",
            dev->irq, isp_irq_handler);

    return 0;
}

int init_vic_control(struct IMPISPDev *dev)
{
    void __iomem *vic_regs = dev->reg_base + 0x7800;
    int ret;

    pr_info("Initializing VIC control...\n");

    // 1. First disable everything like we do in streaming disable
    writel(0, vic_regs + 0x000);
    writel(0, vic_regs + 0x140);
    wmb();
    udelay(100);

    // 2. Reset VIC
    writel(0x4, vic_regs + 0x000);  // Reset command
    wmb();
    udelay(100);
    while(readl(vic_regs + 0x000) & 0x4) {
        udelay(10);
    }

    // 3. Initial mode setup - match streaming enable sequence
    writel(2, vic_regs + 0xc);  // Mode = 2
    writel(0x800c0000, vic_regs + 0x10);  // Magic control
    wmb();

    // 4. Poll until VIC status is 0
    uint32_t status;
    do {
        status = readl(vic_regs + 0x1E0);
    } while (status != 0);

    // 5. Configure VIC with correct magic values
    writel(0x100010, vic_regs + 0x1a4);
    writel(0x4210, vic_regs + 0x1ac);
    writel(0x10, vic_regs + 0x1b0);
    writel(0, vic_regs + 0x1b4);
    wmb();

    // 6. Control sequence - THIS IS KEY
    writel(2, vic_regs + 0x140);
    wmb();
    writel(1, vic_regs + 0x140);  // Back to 1 as per OEM
    wmb();

    // 7. Register IRQ after control setup
    ret = register_vic_irq(dev);
    if (ret) {
        pr_err("Failed to register VIC IRQ: %d\n", ret);
        return ret;
    }

    // 8. Now enable interrupts
//    writel(1, vic_regs + 0x1E0);
//    writel(1, vic_regs + 0x008);
//    writel(1, vic_regs + 0x13c);
//    writel(0xFFFFFFFF, vic_regs + 0x1E8);
//    wmb();

    // Verify configuration
    pr_info("VIC Configuration:\n");
    pr_info("  Control:     0x%08x\n", readl(vic_regs + 0x140));
    pr_info("  IRQ Enable:  0x%08x\n", readl(vic_regs + 0x13c));
    pr_info("  Frame Ctrl:  0x%08x\n", readl(vic_regs + 0x008));
    pr_info("  Int Mask:    0x%08x\n", readl(vic_regs + 0x1E8));
    pr_info("  IRQ Status:  0x%08x\n", readl(vic_regs + 0x1e0));
    pr_info("  Status:      0x%08x\n", readl(vic_regs + 0x1E0));

    return 0;
}


static int tx_isp_probe(struct platform_device *pdev)
{
    struct IMPISPDev *dev = ourISPdev;
    int ret;
    struct resource *res;

    pr_info("ISP probe called\n");

    dev->dev = &pdev->dev;
    platform_set_drvdata(pdev, dev);
    // Initialize tuning state
    dev->tuning_data->state = 0;
    dev->tuning_data->regs = NULL;
    dev->tuning_data->contrast = 128;
    dev->tuning_data->brightness = 128;
    dev->tuning_data->saturation = 128;
    dev->tuning_data->sharpness = 128;

    // Get memory resource from platform device
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res) {
        dev_err(&pdev->dev, "No memory resource\n");
        return -ENODEV;
    }

    // Reserved memory initialization
    ret = init_isp_reserved_memory(pdev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to init reserved memory\n");
        return ret;
    }

    // Request and map the memory region
    dev->resources.mmio_phys = res->start;
    dev->resources.mmio_res = devm_request_mem_region(&pdev->dev,
                                                     res->start,
                                                     resource_size(res),
                                                     pdev->name);
    if (!dev->resources.mmio_res) {
        dev_err(&pdev->dev, "Failed to request memory region\n");
        return -EBUSY;
    }

    dev->reg_base = devm_ioremap(&pdev->dev, res->start, resource_size(res));
    if (!dev->reg_base) {
        dev_err(&pdev->dev, "Failed to map registers\n");
        return -ENOMEM;
    }

    /* Setup GPIO configuration */
    pr_info("Starting GPIO config init\n");
    ret = init_gpio_config(dev);
    if (ret)
        goto err_cleanup_state;

    /* Initialize hardware resources */
    pr_info("Starting hw resources init\n");
    ret = init_hw_resources(dev);
    if (ret)
        goto err_free_gpio;

    /* Setup interrupt handling */
    pr_info("Starting IRQ handler init\n");
    ret = init_irq_handler(dev);
    if (ret)
        goto err_remove_csi;

    /* Initialize memory system */
    pr_info("Starting memory init\n");
    ret = tx_isp_init_memory(dev);
    if (ret)
        goto err_remove_csi;

    /* Configure clocks */
    pr_info("Starting clocks init\n");
    ret = configure_isp_clocks(dev);
    if (ret)
        goto err_cleanup_memory;


    /* Initialize proc entries */
    pr_info("Starting proc entries init\n");
    ret = create_isp_proc_entries(dev);
    if (ret) {
        goto err_cleanup_memory;
    }

    /* Initialize ISP subsystems */
   	pr_info("Starting subsystems init\n");
    ret = init_isp_subsystems(dev);
    if (ret)
        goto err_cleanup_i2c;

    /* Create frame channel devices */
    ret = create_framechan_devices(&pdev->dev);
    if (ret)
        goto err_cleanup_subsys;

    // Initialize CSI - do this early
    ret = init_csi_early(dev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to initialize CSI: %d\n", ret);
        goto err_free_dev;
    }

    /* Setup I2C and sensor */
    pr_info("Starting I2C init\n");
    ret = setup_i2c_adapter(dev);
    if (ret)
        goto err_cleanup_memory;

    // Do sensor reset sequence
    pr_info("Initial sensor reset...\n");
    sc2336_hw_reset(ourISPdev);

    ret = init_vic_control(ourISPdev);
    if (ret) {
        goto err_cleanup_proc;
    }
    for (int i = 0; i < MAX_CHANNELS; i++) {
        struct isp_channel *chn = &dev->channels[i];
        if (!chn)
            continue;

        // Initialize basic state
        chn->channel_id = i;
        chn->state = 0;  // Initial state
        chn->width = 1920;  // Default resolution
        chn->height = 1080;
    }

	/* Register misc device */
    pr_info("Starting sub device init\n");
    /* Register subdevices */
    ret = register_isp_subdevices(dev);
    if (ret)
        goto err_cleanup_devices;

    dev_info(&pdev->dev, "TX-ISP probe completed successfully\n");
    return 0;


err_cleanup_devices:
    remove_framechan_devices();
err_cleanup_subsys:
    cleanup_isp_subsystems(dev);
err_cleanup_proc:
    cleanup_isp_proc_entries(dev);
err_cleanup_i2c:
    cleanup_i2c_adapter(dev);
err_cleanup_memory:
    tx_isp_cleanup_memory(dev);
err_remove_csi:
    tx_isp_csi_remove(dev);
err_free_irq:
    devm_free_irq(&pdev->dev, dev->irq, dev);
err_unmap_regs:
    devm_iounmap(&pdev->dev, dev->reg_base);
err_free_gpio:
    devm_gpio_free(&pdev->dev, dev->reset_gpio);
err_cleanup_state:
    // State cleanup if needed
err_free_dev:
    devm_kfree(&pdev->dev, dev);
    return ret;
}

static void cleanup_modules(struct IMPISPDev *dev)
{
    struct isp_module_info *mod, *tmp;
    unsigned long flags;

    spin_lock_irqsave(&dev->modules_lock, flags);
    list_for_each_entry_safe(mod, tmp, &dev->modules, list) {
        list_del(&mod->list);
        kfree(mod);
    }
    spin_unlock_irqrestore(&dev->modules_lock, flags);
}


static int tx_isp_remove(struct platform_device *pdev)
{
    struct IMPISPDev *dev = platform_get_drvdata(pdev);
    struct isp_channel *chn;
    int i;

    dev_info(dev->dev, "ISP device removal started\n");

    if (!dev)
        return 0;

    /* First stop all active channels */
    for (i = 0; i < MAX_CHANNELS; i++) {
        chn = &dev->channels[i];
        

        if (chn && chn->is_open) {
            /* Stop streaming if active */
            if (chn->state == FC_STATE_STREAMING) {
                writel(0x0, dev->reg_base + ISP_STREAM_START + (i * 0x100));
                writel(0x0, dev->reg_base + ISP_STREAM_CTRL + (i * 0x100));
                wmb();
            }

            /* Free channel structure */
            kfree(chn);
        }
    }

    // Unregister the misc device
    misc_deregister(&isp_m0_miscdev);
    cleanup_isp_subsystems(dev);
    cleanup_modules(dev);

    tx_isp_cleanup_resources(dev);

    /* Free IRQ resources */
    if (dev->irq > 0) {
        free_irq(dev->irq, dev);
        dev->irq = 0;
    }

    // Now safe to free tuning data
    if (dev->tuning_data) {
        kfree(dev->tuning_data);
        dev->tuning_data = NULL;
    }

    /* Clean up reserved memory */
    if (dev->rmem_addr) {
        dma_free_coherent(dev->dev, dev->rmem_size,
                         dev->dma_buf, dev->rmem_addr);
        dev->dma_buf = NULL;
    }

    /* Clean up clocks */
    if (dev->cgu_isp) {
        clk_disable_unprepare(dev->cgu_isp);
        clk_put(dev->cgu_isp);
        dev->cgu_isp = NULL;
    }

    if (dev->isp_clk) {
        clk_disable_unprepare(dev->isp_clk);
        clk_put(dev->isp_clk);
        dev->isp_clk = NULL;
    }

    /* Free device structure */
    platform_set_drvdata(pdev, NULL);
    kfree(dev);

    dev_info(dev->dev, "ISP device removed\n");
    return 0;
}



/* Platform device ID matching table */
static struct platform_device_id tisp_platform_ids[] = {
    {
        .name = "tx-isp",
        .driver_data = 0,
    },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(platform, tisp_platform_ids);



// Define the attribute show function
static ssize_t isp_status_show(struct device *dev,
                              struct device_attribute *attr, char *buf)
{
    if (!ourISPdev)
        return sprintf(buf, "ISP device not initialized\n");

    return sprintf(buf, "ISP Status:\n"
                       "  Open: %s\n"
                       "  Sensor: %s\n",
                       //"  WDR Mode: %d\n",
                       ourISPdev->is_open ? "yes" : "no",
                       ourISPdev->sensor_name[0] ? ourISPdev->sensor_name : "none"
                       //ourISPdev->wdr_mode
                   );
}

// Create the device attribute
static DEVICE_ATTR(status, S_IRUGO, isp_status_show, NULL);

static int isp_dev_uevent(struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}

static void dump_subsystem_state(void __iomem *base)
{
    pr_info("ISP Subsystem State:\n");
    pr_info("Control: 0x%08x\n", readl(base + 0x00));
    pr_info("Status:  0x%08x\n", readl(base + 0x04));
    pr_info("Config:  0x%08x\n", readl(base + 0x08));
}

static struct resource tx_isp_resources[] = {
    [0] = {
        .start  = ISP_BASE_ADDR,
        .end    = ISP_BASE_ADDR + ISP_MAP_SIZE - 1,
        .flags  = IORESOURCE_MEM,
        .name   = "tx-isp-regs",
    },
    [1] = {
        .start  = 36,  // ISP IRQ number
        .end    = 36,
        .flags  = IORESOURCE_IRQ,
        .name   = "tx-isp-irq",
    }
};

/* Driver private data */
struct tx_isp_driver_data {
    dev_t dev_num;
    struct class *class;
    struct platform_device *pdev;
    struct device *char_device;
};

static struct tx_isp_driver_data *tx_isp_data;

/* Character device operations */
static const struct file_operations tx_isp_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = tx_isp_ioctl,
    .open = tx_isp_open,
    .release = tx_isp_release,
    .mmap = tx_isp_mmap,
};

/* Platform driver structure */
static struct platform_driver tx_isp_platform_driver = {
    .probe = tx_isp_probe,
    .remove = tx_isp_remove,
    .driver = {
        .name = "tx-isp",
        .owner = THIS_MODULE,
    },
    .id_table = tisp_platform_ids,  // Add this
};

static struct platform_driver tx_isp_vic_driver = {
    .probe = tx_isp_vic_probe,
    .remove = tx_isp_vic_remove,
    .driver = {
        .name = "tx-isp-vic",
        .owner = THIS_MODULE,
    },
};

static struct platform_driver tx_isp_csi_driver = {
    .probe = tx_isp_csi_probe,
    .remove = tx_isp_csi_remove,
    .driver = {
        .name = "tx-isp-csi",
        .owner = THIS_MODULE,
    },
};

struct platform_driver tx_isp_vin_driver = {
    .probe = tx_isp_vin_probe,
    .remove = tx_isp_vin_remove,
    .driver = {
        .name = "tx-isp-vin",
        .owner = THIS_MODULE,
    },
};

struct platform_driver tx_isp_core_driver = {
    .probe = tx_isp_core_probe,
    .remove = tx_isp_core_remove,
    .driver = {
        .name = "tx-isp-core",
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


/* Main init first creates char device infrastructure needed by all submodules */
static int __init tx_isp_init(void)
{
    int ret;
    struct platform_device *pdev;

    pr_info("Loading ISP driver...\n");

    // 0. Allocate ourISPdev device strcture for this driver
    ourISPdev = kzalloc(sizeof(struct IMPISPDev), GFP_KERNEL);
    if (!ourISPdev) {
        ret = -ENOMEM;
        goto err_unreg_device;
    }
    pr_info("ourISPdev allocated at %p\n", ourISPdev);

    // 0.b Allocate tuning data
    ourISPdev->tuning_data = kzalloc(sizeof(struct isp_tuning_data), GFP_KERNEL);
    if (!ourISPdev->tuning_data) {
        ret = -ENOMEM;
        goto err_unreg_device;
    }

    spin_lock_init(&ourISPdev->lock);
    ourISPdev->irq_enabled = 0;
    ourISPdev->irq_handler = NULL;  // Don't set this yet

    // 1. Create the tx-isp character device first
    ret = alloc_chrdev_region(&tx_sp_dev_number, 0, 1, "tx-isp");
    if (ret < 0) {
        pr_err("Failed to allocate device number\n");
        return ret;
    }

    // 2. Create device class
    tisp_class = class_create(THIS_MODULE, "tx-isp");
    if (IS_ERR(tisp_class)) {
        ret = PTR_ERR(tisp_class);
        goto err_unreg_chrdev;
    }

    // Set class permissions
    tisp_class->dev_uevent = isp_dev_uevent;

    // 3. Register platform driver first
    ret = platform_driver_register(&tx_isp_platform_driver);
    if (ret) {
        pr_err("Failed to register ISP platform driver\n");
        goto err_destroy_class;
    }

    // 4. Create and register platform device with resources
    pdev = platform_device_alloc("tx-isp", -1);
    if (!pdev) {
        ret = -ENOMEM;
        goto err_unreg_driver;
    }

    ret = platform_device_add_resources(pdev, tx_isp_resources,
                                      ARRAY_SIZE(tx_isp_resources));
    if (ret) {
        goto err_put_device;
    }

    // Add platform data
    ret = platform_device_add_data(pdev, &isp_pdata, sizeof(isp_pdata));
    if (ret) {
        goto err_put_device;
    }

    ret = platform_device_add(pdev);
    if (ret) {
        goto err_put_device;
    }


    // Initialize IRQ handler data
    struct irq_handler_data *irq_data = kzalloc(sizeof(*irq_data), GFP_KERNEL);
    if (!irq_data) {
        dev_err(&pdev->dev, "Failed to allocate IRQ data\n");
        ret = -ENOMEM;
        goto err_unreg_device;
    }

    spin_lock_init(&irq_data->lock);
    irq_data->irq_number = ourISPdev->irq;
    irq_data->handler_function = NULL;  // Will be set in init_isp_interrupts
    irq_data->disable_function = NULL;  // Will be set in init_isp_interrupts
    ourISPdev->irq_data = irq_data;

    // Request IRQ directly from platform device
    int irq = platform_get_irq(pdev, 0);
    if (irq < 0) {
        dev_err(&pdev->dev, "Failed to get IRQ\n");
        ret = irq;
        kfree(irq_data);
        goto err_unreg_device;
    }

    // Store IRQ data
    ourISPdev->irq_data = irq_data;
    // ourISPdev->irq = irq;
    // Get IRQ number from platform device
    ourISPdev->irq = platform_get_irq(pdev, 0);
    if (ourISPdev->irq < 0) {
        dev_err(&pdev->dev, "Failed to get IRQ\n");
        ret = ourISPdev->irq;
        goto err_unreg_device;
    }

    // Initialize character device
    cdev_init(&ourISPdev->cdev, &tx_isp_fops);
    ourISPdev->cdev.owner = THIS_MODULE;

    ret = cdev_add(&ourISPdev->cdev, tx_sp_dev_number, 1);
    if (ret)
        goto err_free_dev;

    // Create device node
    tisp_device = device_create(tisp_class, NULL, tx_sp_dev_number, NULL, "tx-isp");
    if (IS_ERR(tisp_device)) {
        ret = PTR_ERR(tisp_device);
        goto err_del_cdev;
    }

    // Store platform device pointer
    ourISPdev->pdev = pdev;

    // Register subdevice drivers
    ret = platform_driver_register(&tx_isp_csi_driver);
    if (ret) {
        pr_err("Failed to register CSI driver: %d\n", ret);
        goto err_unreg_vic;
    }

    ret = platform_driver_register(&tx_isp_vic_driver);
    if (ret) {
        pr_err("Failed to register VIC driver: %d\n", ret);
        goto err_destroy_device;
    }

    ret = platform_driver_register(&tx_isp_vin_driver);
    if (ret) {
        pr_err("Failed to register VIN driver: %d\n", ret);
        goto err_unreg_csi;
    }

    ret = platform_driver_register(&tx_isp_core_driver);
    if (ret) {
        pr_err("Failed to register Core driver: %d\n", ret);
        goto err_unreg_vin;
    }

    ret = platform_driver_register(&tx_isp_fs_driver);
    if (ret) {
        pr_err("Failed to register FS driver: %d\n", ret);
        goto err_unreg_core;
    }

    // Debug state dump
    void __iomem *debug_base = ioremap(ISP_PHYS_BASE, ISP_REG_SIZE);
    if (debug_base) {
        dump_subsystem_state(debug_base);
        iounmap(debug_base);
    }

    pr_info("ISP driver and submodules loaded successfully\n");
    return 0;

err_unreg_core:
    platform_driver_unregister(&tx_isp_core_driver);
err_unreg_vin:
    platform_driver_unregister(&tx_isp_vin_driver);
err_unreg_csi:
    platform_driver_unregister(&tx_isp_csi_driver);
err_unreg_vic:
    platform_driver_unregister(&tx_isp_vic_driver);
err_destroy_device:
    device_destroy(tisp_class, tx_sp_dev_number);
err_del_cdev:
    cdev_del(&ourISPdev->cdev);
err_free_dev:
    kfree(ourISPdev);
err_unreg_device:
    platform_device_unregister(pdev);
err_put_device:
    platform_device_put(pdev);
err_unreg_driver:
    platform_driver_unregister(&tx_isp_platform_driver);
err_destroy_class:
    class_destroy(tisp_class);
err_unreg_chrdev:
    unregister_chrdev_region(tx_sp_dev_number, 1);
    return ret;
}

static void __exit tx_isp_exit(void)
{
    struct tx_isp_driver_data *data = tx_isp_data;

    if (!data)
        return;

    platform_driver_unregister(&tx_isp_platform_driver);
    platform_device_unregister(data->pdev);
    device_destroy(data->class, data->dev_num);
    class_destroy(data->class);
    unregister_chrdev_region(data->dev_num, 1);
    kfree(data);
    tx_isp_data = NULL;

    pr_info("TX-ISP driver unloaded\n");
}


int tx_isp_module_init(struct platform_device *pdev, struct tx_isp_subdev *sd)
{
    struct tx_isp_module *module;

    // Basic validation
    if (!sd) {
        pr_err("Invalid parameters in module init\n");
        return -EINVAL;
    }

    module = &sd->module;

    // Initialize module first
    memset(module, 0, sizeof(*module));

    // Set module name and device
    if (pdev) {
        module->name = pdev->name;
        module->dev = &pdev->dev;
    } else {
        module->name = "unnamed";
    }

    // Initialize pad information (already handled in subdev_init)
    // Just verify it's properly initialized
    if (sd->num_inpads > 0 && !sd->inpads) {
        pr_err("Input pad count set but no pads allocated\n");
        return -EINVAL;
    }
    if (sd->num_outpads > 0 && !sd->outpads) {
        pr_err("Output pad count set but no pads allocated\n");
        return -EINVAL;
    }

    // Store ops if provided (though this should already be handled in subdev_init)
    if (pdev && pdev->dev.platform_data) {
        struct tx_isp_subdev_ops *ops = pdev->dev.platform_data;
        if (!sd->ops) {  // Only set if not already set
            sd->ops = ops;
        }
    }

    // Initialize miscdevice structure if needed
    module->miscdev.minor = MISC_DYNAMIC_MINOR;
    module->miscdev.name = module->name;
    module->miscdev.fops = NULL;  // Will be set later by tx_isp_set_module_nodeops

    pr_info("Module initialized: %s (in:%d out:%d)\n",
            module->name, sd->num_inpads, sd->num_outpads);
    return 0;
}


int tx_isp_subdev_init(struct platform_device *pdev, struct tx_isp_subdev *sd,
                      struct tx_isp_subdev_ops *ops)
{
    struct IMPISPDev *dev = ourISPdev;
    int ret;

    pr_info("Starting subdev init for %s\n",
            pdev ? pdev->name : "unknown");

    if (!sd || !ops) {
        pr_err("Invalid parameters\n");
        return -EINVAL;
    }

    pr_info("Platform device init: pdev=%p dev=%p\n", pdev, dev);

    if (!dev) {
        pr_err("No ISP device data found\n");
        return -EINVAL;
    }

    if (!pdev) {
        pr_err("No platform device specified\n");
        return -EINVAL;
    }

    // Initialize subdev structure
    memset(sd, 0, sizeof(*sd));
    sd->ops = ops;

    // Determine pad counts first based on device type
    if (!strcmp(pdev->name, "tx-isp-vic")) {
        sd->num_inpads = 1;
        sd->num_outpads = 2;
    }
    else if (!strcmp(pdev->name, "tx-isp-csi")) {
        sd->num_inpads = 1;
        sd->num_outpads = 1;
    }
    else {
        sd->num_inpads = 1;
        sd->num_outpads = 1;
    }

    // Allocate input pads
    if (sd->num_inpads) {
        sd->inpads = kzalloc(sizeof(struct tx_isp_subdev_pad) * sd->num_inpads, GFP_KERNEL);
        if (!sd->inpads) {
            pr_err("Failed to allocate input pads\n");
            ret = -ENOMEM;
            goto err_free;
        }

        // Initialize each input pad
        for (int i = 0; i < sd->num_inpads; i++) {
            sd->inpads[i].sd = sd;
            sd->inpads[i].index = i;
            sd->inpads[i].type = TX_ISP_PADTYPE_INPUT;
            sd->inpads[i].state = TX_ISP_PADSTATE_FREE;
            // Set appropriate link types based on device
            if (!strcmp(pdev->name, "tx-isp-vic"))
                sd->inpads[i].links_type = TX_ISP_PADLINK_VIC;
            else if (!strcmp(pdev->name, "tx-isp-csi"))
                sd->inpads[i].links_type = TX_ISP_PADLINK_VIC;
            else
                sd->inpads[i].links_type = TX_ISP_PADLINK_VIC | TX_ISP_PADLINK_ISP;
        }
    }

    // Allocate output pads
    if (sd->num_outpads) {
        sd->outpads = kzalloc(sizeof(struct tx_isp_subdev_pad) * sd->num_outpads, GFP_KERNEL);
        if (!sd->outpads) {
            pr_err("Failed to allocate output pads\n");
            ret = -ENOMEM;
            goto err_free_in;
        }

        // Initialize each output pad
        for (int i = 0; i < sd->num_outpads; i++) {
            sd->outpads[i].sd = sd;
            sd->outpads[i].index = i;
            sd->outpads[i].type = TX_ISP_PADTYPE_OUTPUT;
            sd->outpads[i].state = TX_ISP_PADSTATE_FREE;
            // Set appropriate link types based on device
            if (!strcmp(pdev->name, "tx-isp-vic"))
                sd->outpads[i].links_type = TX_ISP_PADLINK_DDR| TX_ISP_PADLINK_VIC;
            else if (!strcmp(pdev->name, "tx-isp-csi"))
                sd->outpads[i].links_type = TX_ISP_PADLINK_VIC;
            else
                sd->outpads[i].links_type = TX_ISP_PADLINK_VIC | TX_ISP_PADLINK_ISP;
        }
    }

    // Map to appropriate register space
    if (dev->reg_base) {
        if (!strcmp(pdev->name, "tx-isp-vic")) {
            sd->base = dev->reg_base + 0x7800;
        }
        else if (!strcmp(pdev->name, "tx-isp-csi")) {
            if (!dev->csi_dev || !dev->csi_dev->csi_regs) {
                pr_err("CSI registers not ready\n");
                ret = -EINVAL;
                goto err_free_out;
            }
            sd->base = dev->csi_dev->csi_regs;
        }
        else {
            sd->base = dev->reg_base;
        }
        pr_info("Using register base: %p\n", sd->base);
    }

    // Initialize the module
    ret = tx_isp_module_init(pdev, sd);
    if (ret) {
        pr_err("Failed to init module: %d\n", ret);
        goto err_free_out;
    }

    // Store ISP device reference
    sd->host_priv = dev;

    pr_info("Subdev initialized successfully\n");
    return 0;

err_free_out:
    if (sd->outpads)
        kfree(sd->outpads);
err_free_in:
    if (sd->inpads)
        kfree(sd->inpads);
err_free:
    memset(sd, 0, sizeof(*sd));
    return ret;
}
EXPORT_SYMBOL(tx_isp_subdev_init);


void tx_isp_subdev_deinit(struct tx_isp_subdev *sd)
{
    int ret;
    void *memory_region;
    int32_t irq_number;
    struct isp_device_config *dev_config = NULL;

    pr_info("Starting subdev deinit\n");

    if (!sd) {
        pr_err("NULL subdev in deinit\n");
        return;
    }

    // First cleanup any active links
    if (sd->inpads) {
        for (int i = 0; i < sd->num_inpads; i++) {
            if (sd->inpads[i].state == TX_ISP_PADSTATE_LINKED) {
                // Destroy any active links
                if (sd->inpads[i].link.sink) {
                    sd->inpads[i].link.sink->state = TX_ISP_PADSTATE_FREE;
                    memset(&sd->inpads[i].link.sink->link, 0,
                           sizeof(struct tx_isp_subdev_link));
                }
                memset(&sd->inpads[i].link, 0, sizeof(struct tx_isp_subdev_link));
            }
        }
    }

    if (sd->outpads) {
        for (int i = 0; i < sd->num_outpads; i++) {
            if (sd->outpads[i].state == TX_ISP_PADSTATE_LINKED) {
                // Destroy any active links
                if (sd->outpads[i].link.sink) {
                    sd->outpads[i].link.sink->state = TX_ISP_PADSTATE_FREE;
                    memset(&sd->outpads[i].link.sink->link, 0,
                           sizeof(struct tx_isp_subdev_link));
                }
                memset(&sd->outpads[i].link, 0, sizeof(struct tx_isp_subdev_link));
            }
        }
    }

    // Release clocks first
    isp_subdev_release_clks(sd);

    // Handle memory region and IRQ
    memory_region = sd->base;
    if (memory_region != NULL) {
        int32_t start_addr = *(uint32_t*)memory_region;
        uint32_t size = *((uint32_t*)((char*)memory_region + 4)) + 1 - start_addr;
        release_mem_region(start_addr, size);
        sd->base = NULL;
    }

    // Handle IRQ cleanup
    irq_number = sd->irqdev.irq;
    if (irq_number != 0) {
        tx_isp_free_irq(&sd->irqdev.irq);
    }

    // Free pad memory if allocated
    if (sd->inpads) {
        // Make sure any event handlers are cleaned up
        for (int i = 0; i < sd->num_inpads; i++) {
            if (sd->inpads[i].event) {
                sd->inpads[i].event = NULL;
            }
            if (sd->inpads[i].priv) {
                kfree(sd->inpads[i].priv);
                sd->inpads[i].priv = NULL;
            }
        }
        kfree(sd->inpads);
        sd->inpads = NULL;
    }

    if (sd->outpads) {
        // Make sure any event handlers are cleaned up
        for (int i = 0; i < sd->num_outpads; i++) {
            if (sd->outpads[i].event) {
                sd->outpads[i].event = NULL;
            }
            if (sd->outpads[i].priv) {
                kfree(sd->outpads[i].priv);
                sd->outpads[i].priv = NULL;
            }
        }
        kfree(sd->outpads);
        sd->outpads = NULL;
    }

    // Reset pad counts
    sd->num_inpads = 0;
    sd->num_outpads = 0;

    pr_info("Subdev %p cleanup complete\n", sd);
}
EXPORT_SYMBOL(tx_isp_subdev_deinit);



module_init(tx_isp_init);
module_exit(tx_isp_exit);

MODULE_AUTHOR("Matt Davis <matteius@gmail.com>");
MODULE_DESCRIPTION("TX-ISP Camera Driver");
MODULE_LICENSE("GPL");
