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
// Global VIC DMA state
static u32 vic_mdma_ch0_sub_get_num;  // Channel 0 buffer count
static u32 vic_mdma_ch0_set_buff_index; // Channel 0 buffer index
static irq_handler_t irq_func_cb[32];  // Size matches OEM's loop of 0x20
static int isp_memopt = 1; // TODO
//module_param(isp_memopt, int, 0644);
static atomic_t streaming_channels = ATOMIC_INIT(0);


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

static size_t calculate_buffer_stride(uint32_t width, uint32_t format)
{
    size_t stride;

    // For NV12, stride is based on Y plane width
    if (format == V4L2_PIX_FMT_NV12) {
        // Align Y plane stride to 16 bytes
        stride = ALIGN(width, 16);
    } else {
        // Default to width alignment for other formats
        stride = ALIGN(width, 16);
    }

    return stride;
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

// Helper to get physical address for a specific buffer
static dma_addr_t get_buffer_phys(struct isp_channel *chn, int index)
{
    if (!chn || index < 0 || index >= MAX_BUFFERS)
        return 0;

    // Use aligned stride to calculate offset
    return chn->dma_addr + (index * chn->buffer_stride);
}

// Helper to get virtual address for a specific buffer
static void* get_buffer_virt(struct isp_channel *chn, int index)
{
    if (!chn || index < 0 || index >= MAX_BUFFERS)
        return NULL;

    // Use aligned stride to calculate offset
    return chn->buf_base + (index * chn->buffer_stride);
}

static int cleanup_buffer(struct isp_channel *chn)
{
    struct video_buffer *buf;
    int i;

    if (!chn->queue || !chn->meta_dma)
        return 0;

    // Free buffer metadata
    for (i = 0; i < chn->queue->buffer_count; i++) {
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
// Add to dump_vic_state to decode IRQ bits
void dump_vic_irq_state(struct IMPISPDev *dev) {
    void __iomem *vic_regs = dev->reg_base + VIC_BASE;

    pr_info("VIC IRQ State:\n");
    pr_info("  Enable (0x93c): 0x%08x\n", readl(vic_regs + 0x93c));
    pr_info("  Mask (0x9e8): 0x%08x\n", readl(vic_regs + 0x9e8));
    // Maybe also check status registers
    pr_info("  Status (0xb4): 0x%08x\n", readl(vic_regs + 0xb4));
    pr_info("  Status2 (0xb8): 0x%08x\n", readl(vic_regs + 0xb8));
}

// Add debug to interrupt handler
static irqreturn_t tx_isp_vic_irq(int irq, void *dev_id)
{
    struct IMPISPDev *dev = dev_id;
    void __iomem *vic_regs = dev->reg_base + VIC_BASE;
    u32 status, handled_ints;

    status = readl(vic_regs + 0xb4);
    pr_info("VIC IRQ: status=0x%08x\n", status);

    if (!status)
        return IRQ_NONE;

    // Clear handled interrupts
    handled_ints = status & 0x000033fb; // Match our enable mask
    writel(handled_ints, vic_regs + 0xb4);

    // Process frame done
    if (status & BIT(0)) {
        pr_info("VIC: Frame done interrupt\n");
        // Handle frame completion
    }

    if (status & BIT(1)) {
        pr_info("VIC: DMA done interrupt\n");
    }

    // Report errors
    if (status & BIT(2))
        pr_warn("VIC: Buffer overflow\n");
    if (status & BIT(3))
        pr_warn("VIC: Format error\n");
    if (status & BIT(4))
        pr_warn("VIC: Size error\n");

    return IRQ_HANDLED;
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
    u32 status, csi_err1, csi_err2;

    status = readl(dev->reg_base + ISP_INT_STATUS);

    // Check CSI errors if CSI interrupt is set
    if (status & ISP_INT_CSI) {
        csi_err1 = readl(dev->csi_dev->csi_regs + 0x30);  // CSI ERR1
        csi_err2 = readl(dev->csi_dev->csi_regs + 0x34);  // CSI ERR2

        if (csi_err1 || csi_err2) {
            dev_warn(dev->dev, "CSI Errors - ERR1: 0x%08x ERR2: 0x%08x\n",
                    csi_err1, csi_err2);

            // Clear CSI errors
            writel(csi_err1, dev->csi_dev->csi_regs + 0x30);
            writel(csi_err2, dev->csi_dev->csi_regs + 0x34);
        }
    }

    // Clear ISP interrupts
    writel(status, dev->reg_base + ISP_INT_CLEAR);

    return IRQ_HANDLED;
}

static irqreturn_t tx_isp_threaded_irq_handler(int irq, void *data)
{
    struct IMPISPDev *dev = data;
    void __iomem *regs = dev->reg_base + ISP_BASE;
    u32 status;

    pr_info("ISP IRQ: status=0x%08x\n", status);

    // Read status
    status = readl(regs + 0xb4);
    if (!(status & 0x3f8)) {
        return IRQ_NONE;
    }

    // Handle key status bits
    if (status & BIT(12)) {     // 0x1000
        // VB interrupt
        //schedule_work(&dev->vb_work);
    }

    if (status & BIT(9)) {      // 0x200
        // Overflow handling
        //dev->stats.overflow_count++;
    }

    if (status & BIT(8)) {      // 0x100
        // Frame break handling
        //dev->stats.break_count++;
    }

    return IRQ_HANDLED;
}

static int reset_gpio = GPIO_PA(18);  // Default reset GPIO
static int pwdn_gpio = -1;  // Default power down GPIO disabled

//static inline void tx_vic_disable_irq(struct IMPISPDev *dev)
//{
//    unsigned long flags;
//    void __iomem *vic_regs = dev->reg_base + VIC_BASE;
//
//    spin_lock_irqsave(&dev->vic_dev->lock, flags);
//
//    if (dev->vic_irq_enabled) {
//        dev->vic_irq_enabled = 0;
//        writel(0, vic_regs + 0x93c);  // Disable all VIC interrupts
//        writel(0xffffffff, vic_regs + 0x9e8); // Mask all VIC interrupts
//        wmb();
//    }
//
//    spin_unlock_irqrestore(&dev->vic_dev->lock, flags);
//}


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

// Add proc handler for isp-w02 frame count
static int isp_w02_show(struct seq_file *seq, void *v)
{
    struct isp_proc_data *data = seq->private;
    struct IMPISPDev *dev = data->dev;
    void *node;
    uint32_t count;

    // Get node from offset 0x70 and then 0x3c like the decompiled code
    node = *(void **)(dev + 0x70);
    if (!node || (unsigned long)node >= 0xfffff001)
        return seq_printf(seq, "Can't ops the node!\n");

    node = *(void **)(node + 0x3c);
    if (!node || (unsigned long)node >= 0xfffff001)
        return seq_printf(seq, "Can't ops the node!\n");

    // This appears to be reading some kind of frame/vic count
    count = readl(node + 0xd4);
    seq_printf(seq, "%d\n", count);

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
    int i;

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

    // Copy zone data
    memcpy(info.zones, hist_buf, MAX_AE_ZONES);

    // Copy histogram data
    for (i = 0; i < MAX_HIST_BINS; i++) {
        info.hist_data[i] = readl(dev->reg_base + ISP_AE_HIST_BASE + (i * 4));
    }

    // Copy metrics at specific offsets
    info.metrics[0] = readl(dev->reg_base + ISP_AE_HIST_BASE + 0x400);
    info.metrics[1] = readl(dev->reg_base + ISP_AE_HIST_BASE + 0x404);
    info.metrics[2] = readl(dev->reg_base + ISP_AE_HIST_BASE + 0x408);
    info.metrics[3] = readl(dev->reg_base + ISP_AE_HIST_BASE + 0x40c);

    // Copy additional status bytes
    info.additional[0] = readl(dev->reg_base + ISP_AE_HIST_BASE + 0x414) & 0xFF;
    info.additional[1] = readl(dev->reg_base + ISP_AE_HIST_BASE + 0x418) & 0xFF;
    info.additional[2] = readl(dev->reg_base + ISP_AE_HIST_BASE + 0x41c) & 0xFF;
    info.additional[3] = readl(dev->reg_base + ISP_AE_HIST_BASE + 0x420) & 0xFF;

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

//            // Disable links by clearing link enable register
//            writel(0, ourISPdev->reg_base + ISP_LINK_ENABLE_REG);
//            wmb();  // Ensure write completes

            // Set control structure
            bypass_ctrl.cmd = ISP_CTRL_BYPASS;
            bypass_ctrl.value = ctrl->value;

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
    } __packed req = {0};
    int ret = 0;

    if (!dev || !dev->tuning_data)
        return -EINVAL;

    // Read enable flag
    if (copy_from_user(&req.enable, arg, sizeof(req.enable)))
        return -EFAULT;

    pr_info("Tuning enable: %d\n", req.enable ? 1 : 0);

    mutex_lock(&tuning_mutex);

    // Get current state
    req.state = dev->tuning_data->state;

    // Map registers if not already mapped
    if (!dev->tuning_data->regs) {
        dev->tuning_data->regs = ioremap(0x13380000, 0x1b000);
        if (!dev->tuning_data->regs) {
            ret = -ENOMEM;
            goto out_unlock;
        }
    }

    // Just update state flags, keep mapping
    if (req.enable) {
        if (req.state != 2) {
            dev->tuning_data->state = 2;
            req.state = 2;
        }
    } else {
        if (req.state == 2) {
            dev->tuning_data->state = 0;
            req.state = 0;
        }
    }

    if (copy_to_user(arg, &req, sizeof(req)))
        ret = -EFAULT;

    out_unlock:
        mutex_unlock(&tuning_mutex);
    return ret;
}

static struct tx_isp_subdev_pad *find_subdev_link_pad(struct IMPISPDev *dev,
                                                   const struct tx_isp_link_config *cfg)
{
    struct tx_isp_subdev *sd = dev->fs_dev->sd;
    struct tx_isp_subdev_pad *pad = NULL;

    pr_info("Looking up pad: subdev=%p cfg=%p\n", sd, cfg);
    if (!sd || !cfg) {
        pr_err("Invalid arguments to get_subdev_link_pad\n");
        return NULL;
    }

    pr_info("Pad lookup for %s:\n"
            "  type=%d index=%d\n"
            "  inpads=%p (count=%d)\n"
            "  outpads=%p (count=%d)\n",
            cfg->src.name,
            cfg->src.type,
            cfg->src.index,
            sd->inpads, sd->num_inpads,
            sd->outpads, sd->num_outpads);

    if (cfg->flag & TX_ISP_PADTYPE_INPUT) {
        if (cfg->src.index >= sd->num_inpads) {
            pr_err("Invalid input pad index %d\n", cfg->src.index);
            return NULL;
        }
        pad = &sd->inpads[cfg->src.index];
    } else {
        if (cfg->src.index >= sd->num_outpads) {
            pr_err("Invalid output pad index %d\n", cfg->src.index);
            return NULL;
        }
        pad = &sd->outpads[cfg->src.index];
    }

    // Verify channel data
    if (pad && !pad->priv) {
        pr_warn("Found pad but no channel data, reinitializing...\n");
        struct isp_channel *chn = kzalloc(sizeof(*chn), GFP_KERNEL);
        if (chn) {
            chn->channel_id = pad->index;
            chn->state = 0;
            chn->dev = dev->dev;
            memcpy(&chn->pad, pad, sizeof(struct tx_isp_subdev_pad));
            pad->priv = chn;
            pr_info("Created missing channel data for pad %d\n", pad->index);
        }
    }

    return pad;
}



static int32_t tisp_log2_int_to_fixed(uint32_t value, char precision_bits, char shift_amt)
{
    uint32_t precision = precision_bits;
    uint32_t shift = shift_amt;

    if (value == 0)
        return 0;

    // Find highest set bit position using binary search
    uint32_t curr_val, bit_pos = 0;
    if (value < 0x10000) {
        curr_val = value;
    } else {
        curr_val = value >> 16;
        bit_pos = 16;
    }

    if (curr_val >= 0x100) {
        curr_val >>= 8;
        bit_pos = bit_pos + 8;
    }

    if (curr_val >= 0x10) {
        curr_val >>= 4;
        bit_pos = bit_pos + 4;
    }

    if (curr_val >= 4) {
        curr_val >>= 2;
        bit_pos = bit_pos + 2;
    }

    if (curr_val != 1) {
        bit_pos = bit_pos + 1;
    }

    // Normalize value for fixed-point calculation
    uint32_t normalized;
    if (bit_pos >= 16) {
        normalized = value >> ((bit_pos - 15) & 0x1f);
    } else {
        normalized = value << ((15 - bit_pos) & 0x1f);
    }

    // Iterative fixed-point calculation
    int32_t result = 0;
    for (int32_t i = 0; i < precision; i++) {
        int32_t square = normalized * normalized;
        result <<= 1;

        if (square >= 0) {
            normalized = square >> 15;
        } else {
            result += 1;
            normalized = square >> 16;
        }
    }

    // Combine results with scaling
    return ((bit_pos << (precision & 0x1f)) + result) << (shift & 0x1f) |
           (normalized & 0x7fff) >> ((15 - shift) & 0x1f);
}

static int32_t tisp_log2_fixed_to_fixed(uint32_t input_val, int32_t in_precision, char out_precision)
{
    // Call helper directly with original param signature
    return tisp_log2_int_to_fixed(input_val, out_precision, 0);
}



// Reimplemented to avoid 64-bit division on MIPS32
static int32_t fix_point_div_64(int32_t shift_bits, int32_t scale,
                               int32_t num_low, int32_t num_high,
                               int32_t denom_low, int32_t denom_high)
{
    // Initial result tracking
    int32_t quotient = 0;
    int32_t remainder = num_low;
    int32_t temp_high = num_high;

    // Iterative long division
    for (int i = 0; i < 32; i++) {
        int32_t carry = remainder & 0x80000000;

        // Shift left by 1
        remainder = (remainder << 1) | ((temp_high >> 31) & 1);
        temp_high = temp_high << 1;
        quotient = quotient << 1;

        // See if we can subtract denominator
        if (carry || remainder >= denom_low) {
            remainder = remainder - denom_low;
            if (carry && remainder >= 0) {
                temp_high--;
            }
            quotient |= 1;
        }
    }

    return quotient;
}

static int32_t fix_point_mult2_32(int32_t shift_bits, int32_t multiplier, int32_t multiplicand)
{
    uint32_t mask = 0xffffffff >> (-shift_bits & 0x1f);
    uint32_t high_mult = multiplier >> (shift_bits & 0x1f);
    uint32_t high_cand = multiplicand >> (shift_bits & 0x1f);
    int32_t low_mult = mask & multiplier;
    int32_t low_cand = mask & multiplicand;

    uint64_t cross_prod1 = (uint64_t)low_mult * high_cand;
    uint64_t cross_prod2 = (uint64_t)high_mult * low_cand;

    return (cross_prod1 & 0xffffffff) + cross_prod2 +
           ((uint64_t)high_mult * high_cand << (shift_bits & 0x1f)) +
           ((uint64_t)low_mult * low_cand >> (shift_bits & 0x1f));
}

static int tisp_g_ev_attr(uint32_t *ev_buffer, struct isp_tuning_data *tuning)
{
    // Fill total gain and exposure values
    ev_buffer[0] = tuning->total_gain;                // Total sensor gain
    ev_buffer[1] = tuning->exposure >> 10;            // Normalized exposure value

    // Convert exposure to fixed point representation
    int32_t exp_fixed = tisp_log2_fixed_to_fixed(tuning->exposure, 10, 16);
    ev_buffer[3] = exp_fixed;

    // Calculate exposure vs frame rate compensation
    uint64_t exposure_us = (uint64_t)ev_buffer[0] * 1000000; // Convert to microseconds
    uint32_t exp_comp = fix_point_div_64(0, exp_fixed,
                                      exposure_us & 0xffffffff,
                                      exposure_us >> 32,
                                      (tuning->fps_den >> 16) * (tuning->fps_num & 0xffff),
                                      0);
    ev_buffer[2] = exp_comp;

    // Convert gain values to fixed point
    ev_buffer[4] = tisp_log2_fixed_to_fixed(tuning->max_again, 10, 5);    // Analog gain
    ev_buffer[5] = tisp_log2_fixed_to_fixed(tuning->max_dgain, 10, 5);    // Digital gain
    ev_buffer[6] = tuning->exposure & 0xffff;                             // Integration time

    // Calculate combined gain
    uint32_t total = fix_point_mult2_32(10, tuning->max_again, tuning->max_dgain);
    ev_buffer[7] = total >> 2;

    // Additional gain conversions for min/max values
    ev_buffer[8] = tisp_log2_fixed_to_fixed(tuning->max_again + 4, 10, 5);   // Max analog gain
    ev_buffer[9] = tisp_log2_fixed_to_fixed(tuning->max_dgain + 4, 10, 5);   // Max digital gain
    ev_buffer[10] = tisp_log2_fixed_to_fixed(tuning->max_again >> 1, 10, 5); // Min analog gain (half of max)
    ev_buffer[11] = tisp_log2_fixed_to_fixed(tuning->max_dgain >> 1, 10, 5); // Min digital gain (half of max)

    // FPS and timing related values
    ev_buffer[0x1b] = tuning->fps_num;    // Current FPS numerator
    *(uint16_t*)(&ev_buffer[0x37]) = tuning->fps_den;  // Current FPS denominator

    // Calculate actual frame rate
    uint32_t actual_fps = ((tuning->fps_den & 0xffff) * 1000000) /
                         (tuning->fps_den >> 16) / tuning->fps_num;
    ev_buffer[0x1f] = actual_fps;

    // Store operating mode
    ev_buffer[12] = tuning->running_mode;

    return 0;
}

// Day/Night mode parameters
static struct tiziano_dn_params {
    uint32_t day_params[0x20];   // Day mode params (0x84b50 in OEM)
    uint32_t night_params[0x20]; // Night mode params
} dn_params;

static int tisp_day_or_night_s_ctrl(uint32_t mode)
{
    void __iomem *regs = ourISPdev->reg_base;
    uint32_t bypass_val, top_ctrl;

    if (mode > 1) {
        pr_err("%s: Unsupported mode %d\n", __func__, mode);
        return -EINVAL;
    }

    // Copy appropriate parameter set // TODO
//    if (mode == 0) {
//        memcpy(&dn_params.day_params, day_mode_defaults, sizeof(dn_params.day_params));
//        ourISPdev->day_night = 0;
//    } else {
//        memcpy(&dn_params.night_params, night_mode_defaults, sizeof(dn_params.night_params));
//        ourISPdev->day_night = 1;
//    }

    // Read current top control register
    bypass_val = readl(regs + 0xC);

    // Apply parameters to hardware
    for (int i = 0; i < 0x20; i++) {
        uint32_t *params = mode ? dn_params.night_params : dn_params.day_params;
        uint32_t val = ~(1 << i) & bypass_val;
        val |= params[i] << i;
        bypass_val = val;
    }

    // Set appropriate bypass bits based on chip variant
//    if (ourISPdev->chip_id == 0xa2ea4) { // TODO
//        bypass_val &= 0xb577fffd;
//        top_ctrl = 0x34000009;
//    } else {
        bypass_val &= 0xa1fffff6;
        top_ctrl = 0x880002;
    //}

    bypass_val |= top_ctrl;

    pr_info("%s: Setting top bypass to 0x%x\n", __func__, bypass_val);
    writel(bypass_val, regs + 0xC);

    // Refresh all pipeline stages for mode change
//    tiziano_defog_refresh();
//    tiziano_ae_refresh();
//    tiziano_awb_refresh();
//    tiziano_dmsc_refresh();
//    tiziano_sharpen_refresh();
//    tiziano_mdns_refresh();
//    tiziano_sdns_refresh();
//    tiziano_gib_refresh();
//    tiziano_lsc_refresh();
//    tiziano_ccm_refresh();
//    tiziano_clm_refresh();
//    tiziano_gamma_refresh();
//    tiziano_adr_refresh();
//    tiziano_dpc_refresh();
//    tiziano_af_refresh();
//    tiziano_bcsh_refresh();
//    tiziano_rdns_refresh();
//    tiziano_ydns_refresh();

    // Reset custom mode and update poll state
    ourISPdev->custom_mode = 0;
    ourISPdev->poll_state = ((mode & 0xFF) << 16) | 1;

    // Wake up any waiters
    wake_up_interruptible(&ourISPdev->poll_wait);

    return 0;
}

static int isp_core_tuning_event(struct IMPISPDev *dev, uint32_t event)
{
      pr_info("isp_core_tuning_event: event=0x%x\n", event);
    if (!dev)
        return -EINVAL;

    switch (event) {
        case ISP_TUNING_EVENT_MODE0:
            writel(2, dev->reg_base + 0x40c4);
        break;

        case ISP_TUNING_EVENT_MODE1:
            writel(1, dev->reg_base + 0x40c4);
        break;

        case ISP_TUNING_EVENT_FRAME:
          pr_info("ISP_TUNING_EVENT_FRAME\n");
            //isp_frame_done_wakeup();
        break;

        case ISP_TUNING_EVENT_DN:
        {
            uint32_t dn_mode = readl(dev->reg_base + 0x40a4);
            tisp_day_or_night_s_ctrl(dn_mode); // We'll need this function too
            writel(dn_mode, dev->reg_base + 0x40a4);
        }
        break;

        default:
            return -EINVAL;
    }

    return 0;
}


static int apical_isp_ae_g_roi(struct IMPISPDev *dev, struct isp_core_ctrl *ctrl)
{
    // Directly returns error - not implemented
    return -EINVAL;
}

static int apical_isp_expr_g_ctrl(struct IMPISPDev *dev, struct isp_core_ctrl *ctrl)
{
    uint32_t ev_buffer[32];
    struct {
        int16_t val1;
        int16_t val2;
        int16_t val3;
        int16_t val4;
        int32_t enabled;
    } expr_data;

    int ret = tisp_g_ev_attr(ev_buffer, dev->tuning_data);
    if (ret)
        return ret;

    // Fill expression data from ev buffer values
    expr_data.val1 = ev_buffer[0];
    expr_data.val2 = ev_buffer[1];
    expr_data.val3 = ev_buffer[2];
    expr_data.val4 = ev_buffer[3];
    expr_data.enabled = (ev_buffer[4] > 0) ? 1 : 0;

    if (copy_to_user((void __user *)ctrl->value, &expr_data, sizeof(expr_data)))
        return -EFAULT;

    return 0;
}

static int apical_isp_ev_g_attr(struct IMPISPDev *dev, struct isp_core_ctrl *ctrl)
{
    uint32_t ev_buffer[32]; // Size matches what's used in tisp_g_ev_attr
    struct {
        int32_t val[6];  // Based on how many values are copied in decompiled
    } ev_data;

    int ret = tisp_g_ev_attr(ev_buffer, dev->tuning_data);
    if (ret)
        return ret;

    // Copy values from ev buffer to response structure
    memcpy(ev_data.val, &ev_buffer[1], sizeof(ev_data));  // Skip first value

    if (copy_to_user((void __user *)ctrl->value, &ev_data, sizeof(ev_data)))
        return -EFAULT;

    return 0;
}



static int tiziano_bcsh_update(struct isp_tuning_data *tuning)
{
    struct IMPISPDev *dev = ourISPdev;
    uint32_t ev_shifted = tuning->bcsh_ev >> 10;
    uint32_t interp_values[8];
    int i;

    // Check if EV is below min threshold
    if (tuning->bcsh_au32EvList_now[0] > ev_shifted) {
        // Use minimum values
        tuning->bcsh_saturation_value = tuning->bcsh_au32SminListS_now[0];
        tuning->bcsh_saturation_max = tuning->bcsh_au32SmaxListS_now[0];
        tuning->bcsh_saturation_min = tuning->bcsh_au32SminListM_now[0];
        tuning->bcsh_saturation_mult = tuning->bcsh_au32SmaxListM_now[0];
        return 0;
    }

    // Check if EV is above max threshold
    if (ev_shifted >= tuning->bcsh_au32EvList_now[8]) {
        // Use maximum values
        tuning->bcsh_saturation_value  = tuning->bcsh_au32SminListS_now[8];
        tuning->bcsh_saturation_max = tuning->bcsh_au32SmaxListS_now[8];
        tuning->bcsh_saturation_min = tuning->bcsh_au32SminListM_now[8];
        tuning->bcsh_saturation_mult = tuning->bcsh_au32SmaxListM_now[8];
        // Set other max values...
        return 0;
    }

    // Find interpolation interval
    for (i = 0; i < 8; i++) {
        uint32_t ev_low = tuning->bcsh_au32EvList_now[i];
        uint32_t ev_high = tuning->bcsh_au32EvList_now[i + 1];

        if (ev_shifted >= ev_low && ev_shifted < ev_high) {
            // Linear interpolation between points
            uint32_t range = ev_high - ev_low;
            uint32_t dist = ev_shifted - ev_low;
            uint32_t weight = (dist << 8) / range;  // Fixed point 8.8

            // Interpolate SminListS
            uint32_t v1 = tuning->bcsh_au32SminListS_now[i];
            uint32_t v2 = tuning->bcsh_au32SminListS_now[i + 1];
            tuning->bcsh_saturation_value = v1 + (((v2 - v1) * weight) >> 8);

            // Interpolate SmaxListS
            v1 = tuning->bcsh_au32SmaxListS_now[i];
            v2 = tuning->bcsh_au32SmaxListS_now[i + 1];
            tuning->bcsh_saturation_max = v1 + (((v2 - v1) * weight) >> 8);

            // Interpolate SminListM
            v1 = tuning->bcsh_au32SminListM_now[i];
            v2 = tuning->bcsh_au32SminListM_now[i + 1];
            tuning->bcsh_saturation_min = v1 + (((v2 - v1) * weight) >> 8);

            // Interpolate SmaxListM
            v1 = tuning->bcsh_au32SmaxListM_now[i];
            v2 = tuning->bcsh_au32SmaxListM_now[i + 1];
            tuning->bcsh_saturation_mult = v1 + (((v2 - v1) * weight) >> 8);

            break;
        }
    }

    // Update hardware registers
    writel(tuning->bcsh_saturation_value, dev->reg_base + BCSH_SVALUE_REG);
    writel(tuning->bcsh_saturation_max, dev->reg_base + BCSH_SMAX_REG);
    writel(tuning->bcsh_saturation_min, dev->reg_base + BCSH_SMIN_REG);
    writel(tuning->bcsh_saturation_mult, dev->reg_base + BCSH_SMAX_M_REG);

    return 0;
}


int tisp_bcsh_saturation(struct isp_tuning_data *tuning, uint8_t value)
{
    if (!tuning)
        return -EINVAL;

    tuning->saturation = value;
    return tiziano_bcsh_update(tuning);
}



static int apical_isp_core_ops_g_ctrl(struct IMPISPDev *dev, struct isp_core_ctrl *ctrl)
{
    int ret = 0;
    struct isp_tuning_data *tuning = dev->tuning_data;

    if (!dev || !tuning)
        return -EINVAL;

    //mutex_lock(&tuning->lock);

    pr_info("Get control: cmd=0x%x value=%d\n", ctrl->cmd, ctrl->value);

    // Special case routing for 0x8000024-0x8000027
    if (ctrl->cmd >= 0x8000024) {
        switch(ctrl->cmd) {
            pr_info("Special case routing for 0x8000024-0x8000027\n");
            pr_info("cmd=0x%x\n", ctrl->cmd);
            case 0x8000023:  // AE Compensation
                ctrl->value = tuning->ae_comp;
            break;
            case 0x8000024:  // AE ROI
                ret = apical_isp_ae_g_roi(dev, ctrl);
            break;

            case 0x8000025:  // Expression
                ret = apical_isp_expr_g_ctrl(dev, ctrl);
            break;

            case 0x8000026:  // EV
                ret = apical_isp_ev_g_attr(dev, ctrl);
            break;

            case 0x8000027:  // Total Gain
                // Special case that uses tisp_g_ev_attr
                    uint32_t ev_buffer[32]; // Size from tisp_g_ev_attr
            ret = tisp_g_ev_attr(ev_buffer, tuning);
            if (ret == 0) {
                // Note: Based on decompiled, only copying second word back
                if (copy_to_user((void __user *)ctrl->value, &ev_buffer[1],
                               sizeof(uint32_t))) {
                    ret = -EFAULT;
                               }
            }
            break;

            case 0x8000028:  // Maximum Analog Gain
                ctrl->value = tuning->max_again;
                break;

            case 0x8000029:  // Maximum Digital Gain
                ctrl->value = tuning->max_dgain;
                break;
            case 0x800002c:  // Move state
                ctrl->value = tuning->move_state;
                break;
            case 0x8000039:  // Defog Strength
                ctrl->value = tuning->defog_strength;
                break;

            case 0x8000062:  // DPC Strength
                ctrl->value = tuning->dpc_strength;
                break;

            case 0x80000a2:  // DRC Strength
                ctrl->value = tuning->drc_strength;
                break;

            case 0x8000085:  // Temper Strength
                ctrl->value = tuning->temper_strength;
                break;

            case 0x8000086:  // Sinter Strength
                ctrl->value = tuning->sinter_strength;
                break;

            case 0x800002d:  // AE Statistics
                ret = isp_get_ae_state(dev, ctrl);
                if (ret)
                    goto out;
                break;

            case 0x8000030:  // AE Zone Info
//                ret = isp_get_ae_zone(dev, ctrl);
//                if (ret)
//                    goto out;
                break;

            case 0x8000031:  // AF Zone Info
                ret = isp_get_af_zone(dev, ctrl);
                if (ret)
                    goto out;
                break;
            // Special case handlers
            case 0x8000004: {  // White Balance
                struct {
                    uint32_t r_gain;
                    uint32_t g_gain;
                    uint32_t b_gain;
                    uint32_t color_temp;
                } wb_data;

                wb_data.r_gain = tuning->wb_gains.r;
                wb_data.g_gain = tuning->wb_gains.g;
                wb_data.b_gain = tuning->wb_gains.b;
                wb_data.color_temp = tuning->wb_temp;

//                if (copy_to_user((void __user *)ctrl->value, &wb_data, sizeof(wb_data))) {
//                    ret = -EFAULT;
//                    goto out;
//                }
                break;
            }

            case 0x8000101: {  // BCSH Hue
                struct {
                    uint8_t hue;
                    uint8_t brightness;
                    uint8_t contrast;
                    uint8_t saturation;
                } bcsh_data;

                bcsh_data.hue = tuning->bcsh_hue;
                bcsh_data.brightness = tuning->bcsh_brightness;
                bcsh_data.contrast = tuning->bcsh_contrast;
                bcsh_data.saturation = tuning->bcsh_saturation;

//                if (copy_to_user((void __user *)ctrl->value, &bcsh_data, sizeof(bcsh_data))) {
//                    ret = -EFAULT;
//                    goto out;
//                }
                break;
            }
            case 0x80000e0: { // GET FPS
                struct fps_ctrl {
                    int32_t mode;      // 1 for GET operation
                    uint32_t cmd;      // 0x80000e0 for FPS command
                    uint32_t frame_rate;  // fps_num result
                    uint32_t frame_div;   // fps_den result
                };

                struct fps_ctrl fps_data;

                pr_info("Get FPS\n");
                fps_data.mode = 1;  // GET mode
                fps_data.cmd = 0x80000e0;
                fps_data.frame_rate = 25;
                fps_data.frame_div = 1;

                // Copy back to user - note full structure needs to be copied
//                if (copy_to_user((void __user *)ctrl->value, &fps_data, sizeof(fps_data)))
//                    return -EFAULT;

                break;
            }
            default:
                pr_warn("Unknown m0 control get command: 0x%x\n", ctrl->cmd);
                ret = -EINVAL;
            break;
            }
        goto out;
    }

    switch (ctrl->cmd) {
        pr_info("Get control: cmd=0x%x value=%d\n", ctrl->cmd, ctrl->value);
        case 0x980900:  // Brightness
            ctrl->value = tuning->brightness;
            break;

        case 0x980901:  // Contrast
            ctrl->value = tuning->contrast;
            break;

        case 0x980902:  // Saturation
            ctrl->value = tuning->saturation;
            break;

        case 0x98091b:  // Sharpness
            ctrl->value = tuning->sharpness;
            break;

        case 0x980914:  // HFLIP
            ctrl->value = tuning->hflip;
            break;

        case 0x980915:  // VFLIP
            ctrl->value = tuning->vflip;
            break;

        case 0x8000164:  // ISP_CTRL_BYPASS
            ctrl->value = dev->bypass_enabled;
            break;

        case 0x980918:  // ISP_CTRL_ANTIFLICKER
            ctrl->value = tuning->antiflicker;
            break;

        case 0x8000166:  // ISP_CTRL_SHADING
            ctrl->value = tuning->shading;
            break;
        case 0x80000e1:  // ISP Running Mode
            ctrl->value = tuning->running_mode;
            break;
        case 0x80000e7:  // ISP Custom Mode
            ctrl->value = tuning->custom_mode;
            break;
        default:
            pr_warn("Unknown m0 control get command: 0x%x\n", ctrl->cmd);
            ret = -EINVAL;
            break;
    }

out:
    // pr_info("Mutex unlock\n");
    //mutex_unlock(&tuning->lock);
    return ret;
}

static int apical_isp_core_ops_s_ctrl(struct IMPISPDev *dev, struct isp_core_ctrl *ctrl)
{
    int ret = 0;
    struct isp_tuning_data *tuning = dev->tuning_data;

    if (!dev || !tuning) {
        pr_err("No ISP device or tuning data\n");
        return -EINVAL;
    }
    pr_info("Set control: cmd=0x%x value=%d\n", ctrl->cmd, ctrl->value);

    switch (ctrl->cmd) {
        pr_info("Set control: cmd=0x%x value=%d\n", ctrl->cmd, ctrl->value);
        case 0x980900:  // Brightness
            tuning->brightness = ctrl->value;
            break;

        case 0x980901:  // Contrast
            tuning->contrast = ctrl->value;
            break;

        case 0x980902:  // Saturation
            ret = tisp_bcsh_saturation(tuning, ctrl->value);
            if (ret)
                goto out;
            break;

        case 0x98091b:  // Sharpness
            tuning->sharpness = ctrl->value;
            break;

        case 0x980914:  // HFLIP
            if (!tuning->regs) {
                ret = -EINVAL;
                goto out;
            }
            writel(ctrl->value ? 1 : 0, tuning->regs + 0x3ad * 4);
            tuning->hflip = ctrl->value ? 1 : 0;
            set_framesource_changewait_cnt();
            break;

        case 0x980915:  // VFLIP
            if (!tuning->regs) {
                ret = -EINVAL;
                goto out;
            }
            writel(ctrl->value ? 1 : 0, tuning->regs + 0x3ac * 4);
            tuning->vflip = ctrl->value ? 1 : 0;
            set_framesource_changewait_cnt();
            break;

        case 0x8000164:  // ISP_CTRL_BYPASS
            dev->bypass_enabled = !!ctrl->value;
            break;

        case 0x980918:  // ISP_CTRL_ANTIFLICKER
            if (ctrl->value > 2) {
                ret = -EINVAL;
                goto out;
            }
            tuning->antiflicker = ctrl->value;
            break;

        case 0x8000166:  // ISP_CTRL_SHADING
            tuning->shading = ctrl->value;
            break;

        case 0x800002c:  // Move state
            tuning->move_state = ctrl->value;
            break;

        case 0x8000023:  // AE Compensation
            tuning->ae_comp = ctrl->value;
            break;

        case 0x8000028:  // Maximum Analog Gain
            tuning->max_again = ctrl->value;
            break;

        case 0x8000029:  // Maximum Digital Gain
            tuning->max_dgain = ctrl->value;
            break;

        case 0x8000039:  // Defog Strength
            tuning->defog_strength = ctrl->value;
            break;

        case 0x8000062:  // DPC Strength
            tuning->dpc_strength = ctrl->value;
            break;

        case 0x80000a2:  // DRC Strength
            tuning->drc_strength = ctrl->value;
            break;

        case 0x8000085:  // Temper Strength
            tuning->temper_strength = ctrl->value;
//            writel(ctrl->value, tuning->regs + ISP_TEMPER_STRENGTH);
//            wmb();
            break;

        case 0x8000086:  // Sinter Strength
            tuning->sinter_strength = ctrl->value;
//            writel(ctrl->value, tuning->regs + ISP_SINTER_STRENGTH);
//            wmb();
            break;
        // Special case handlers:
        case 0x8000004: {  // White Balance
            struct {
                uint32_t r_gain;
                uint32_t g_gain;
                uint32_t b_gain;
                uint32_t color_temp;  // From the decompiled WB references
            } wb_data;

//            if (copy_from_user(&wb_data, (void __user *)ctrl->value, sizeof(wb_data))) {
//                ret = -EFAULT;
//                goto out;
////            }
//
//            tuning->wb_gains.r = wb_data.r_gain;
//            tuning->wb_gains.g = wb_data.g_gain;
//            tuning->wb_gains.b = wb_data.b_gain;
//            tuning->wb_temp = wb_data.color_temp;

            // Update hardware if tuning is active
//            writel(wb_data.r_gain, tuning->regs + ISP_WB_R_GAIN);
//            writel(wb_data.g_gain, tuning->regs + ISP_WB_G_GAIN);
//            writel(wb_data.b_gain, tuning->regs + ISP_WB_B_GAIN);
//            wmb();
            break;
        }

        case 0x8000101: {  // BCSH Hue
            struct {
                uint8_t hue;
                uint8_t brightness;
                uint8_t contrast;
                uint8_t saturation;
            } bcsh_data;

//            if (copy_from_user(&bcsh_data, (void __user *)ctrl->value, sizeof(bcsh_data))) {
//                ret = -EFAULT;
//                goto out;
//            }
//
//            tuning->bcsh_hue = bcsh_data.hue;
//            tuning->bcsh_brightness = bcsh_data.brightness;
//            tuning->bcsh_contrast = bcsh_data.contrast;
//            tuning->bcsh_saturation = bcsh_data.saturation;

//            writel(bcsh_data.hue, tuning->regs + ISP_BCSH_HUE);
//            writel(bcsh_data.brightness, tuning->regs + ISP_BCSH_BRIGHTNESS);
//            writel(bcsh_data.contrast, tuning->regs + ISP_BCSH_CONTRAST);
//            writel(bcsh_data.saturation, tuning->regs + ISP_BCSH_SATURATION);
//            wmb();
            break;
        }
        case 0x80000e0: { // SET FPS
//            struct {
//                uint32_t frame_rate;  // fps_num
//                uint32_t frame_div;   // fps_den
//            } fps_data;
//
//            if (copy_from_user(&fps_data, (void __user *)ctrl->value, sizeof(fps_data))) {
//                pr_err("Failed to copy FPS data from user\n");
//                return -EFAULT;
//            }

            // Store in tuning data
//            dev->tuning_data->fps_num = fps_data.frame_rate;
//            dev->tuning_data->fps_den = fps_data.frame_div;
              dev->tuning_data->fps_num = 25;
              dev->tuning_data->fps_den = 1;

            // Update in framesource
//            ret = set_framesource_fps(fps_data.frame_rate, fps_data.frame_div);
//
//            // Handle AE algorithm if enabled
//            if (ret == 0 && dev->ae_algo_enabled) {
//                if (dev->ae_algo_cb)
//                    dev->ae_algo_cb(dev->ae_priv_data, 0, 0);
//            }

            break;
        }
        case 0x80000e1: { // ISP Running Mode
            tuning->running_mode = ctrl->value;
            // From decompiled: This affects day/night mode
            // is_isp_day = (ctrl->value < 1) ? 1 : 0;
            set_framesource_changewait_cnt();
            break;
        }
        case 0x80000e7:  // ISP Custom Mode
            tuning->custom_mode = ctrl->value;
            set_framesource_changewait_cnt();
            break;
        default:
            pr_warn("Unknown ISP control command: 0x%x\n", ctrl->cmd);
            ret = -EINVAL;
            break;
    }

out:
    return ret;
}



/*
 * ISP-M0 IOCTL handler
* ISP_CORE_S_CTRL: Set control 0xc008561c
* ISP_CORE_G_CTRL: Get control 0xc008561b
* ISP_TUNING_ENABLE: Enable tuning 0xc00c56c6
 */

static int isp_m0_chardev_ioctl(struct file *file, unsigned int cmd, void __user *arg)
{
    struct IMPISPDev *dev = ourISPdev;
    struct isp_core_ctrl ctrl;
    int ret = 0;

    pr_info("ISP m0 IOCTL called: cmd=0x%x\n", cmd);

    // Basic validation {
    if (!dev || !dev->tuning_data) {
        pr_err("No ISP device or tuning data\n");
        return -EINVAL;
    }

//    // Verify tuning state is 3
//    if (dev->tuning_data->state != 3) {
//        pr_err("ISP tuning not in correct state (state=%d)\n",
//               dev->tuning_data ? dev->tuning_data->state : -1);
//        return -EINVAL;
//    }

    if (cmd == 0xc00c56c6) {
        pr_info("Tuning IOCTL\n");
        // Extract the actual command from the tuning request
        struct {
            int32_t mode;  // enable flag
            uint32_t cmd;  // The actual command we want
        } req;

        if (copy_from_user(&req, arg, sizeof(req))) {
            pr_err("Failed to copy tuning request from user\n");
            return -EFAULT;
        }

        // Set up the ctrl structure for the core functions
        ctrl.cmd = req.cmd;
        if (req.mode) {
            // GET operation
            pr_info("GET operation\n");
            ret = apical_isp_core_ops_g_ctrl(dev, &ctrl);
            if (ret == 0 || ret == 0xfffffdfd) {
                pr_info("Copying control back to user\n");
                if (copy_to_user(arg, &ctrl, 8))  { // Write result back
                    pr_err("Failed to copy control back to user\n");
                    return -EFAULT;
                }
            }
        } else {
            // SET operation
            pr_info("SET operation\n");
            ret = apical_isp_core_ops_s_ctrl(dev, &ctrl);
            if (ret == 0 || ret == 0xfffffdfd) {
                pr_info("Copying control back to user\n");
                if (copy_to_user(arg, &ctrl, 8))  { // Write result back
                    pr_err("Failed to copy control back to user\n");
                    return -EFAULT;
                }
            } else {
                pr_err("Failed to set control: %d\n", ret);
            }
        }
    } else {
        pr_info("Direct GET/SET operation\n");
        // Direct GET/SET operations
        switch(cmd) {
            case 0xc008561b: { // GET
                if (copy_from_user(&ctrl, arg, 8))
                    pr_err("Failed to copy control from user\n");
                    return -EFAULT;
                ret = apical_isp_core_ops_g_ctrl(dev, &ctrl);
                if (ret == 0 || ret == 0xfffffdfd) {
                    pr_info("Copying control back to user\n");
                    if (copy_to_user(arg, &ctrl, 8))
                        return -EFAULT;
                }
                break;
            }
            case 0xc008561c: { // SET
                if (copy_from_user(&ctrl, arg, 8)) {
                    pr_err("Failed to copy control from user\n");
                    return -EFAULT;
                }
                pr_info("SET operation\n");
                ret = apical_isp_core_ops_s_ctrl(dev, &ctrl);
                break;
            }
            default:
                pr_err("Unknown ISP control command: 0x%x\n", cmd);
                return -EINVAL;
        }
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
    void __iomem *vic_regs = dev->reg_base + 0x7000;
    u32 status;
    bool handled = false;

    // Read status register at 0xb4
    status = readl(vic_regs + 0xb4);  // 0xb4

    pr_info("ISP IRQ: status=0x%x enable=0x%x mask=0x%x\n",
            status,
            readl(vic_regs + 0x93c),     // 0x93c
            readl(vic_regs + 0x9e8));   // 0x9e8

    if (status) {
        // Clear status at both registers
        writel(status, vic_regs + 0xb4);     // 0xb4
        writel(status, vic_regs + 0x0c);       // 0x0c - Mode register also needs clearing
        wmb();
        handled = true;
    }

    return handled ? IRQ_HANDLED : IRQ_NONE;
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


static irqreturn_t isp_irq_thread_handler(int irq, void* data)
{
    struct irq_handler_data *handler_data = (struct irq_handler_data *)data;
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



// DDR operation implementations
static int ddr_g_fmt(struct tx_isp_subdev *sd, struct v4l2_format *f)
{
    // For now just return success
    return 0;
}

static int ddr_s_fmt(struct tx_isp_subdev *sd, struct v4l2_format *f)
{
    // For now just return success
    return 0;
}

static int ddr_streamon(struct tx_isp_subdev *sd, void *data)
{
    // For now just return success
    return 0;
}

static int ddr_streamoff(struct tx_isp_subdev *sd, void *data)
{
    // For now just return success
    return 0;
}

static int ddr_init(struct tx_isp_subdev *sd, int on)
{
    // For now just return success
    return 0;
}

// And we need to define vin_subdev_ops:
static struct tx_isp_subdev_ops vin_subdev_ops = {
    .core = NULL,
    .video = NULL,
    .pad = NULL,
    .sensor = NULL,
};

// For VIC subdev - needs stream and link handling
static struct tx_isp_subdev_video_ops vic_video_ops = {
    .s_stream = vic_s_stream,
    .link_stream = vic_link_stream,
    .link_setup = vic_link_setup,
};

static struct tx_isp_subdev_core_ops vic_core_ops = {
    .init = vic_init,
    .reset = vic_reset,
    .interrupt_service_routine = vic_isr,
    .interrupt_service_thread = vic_isr_thread,
};

static struct tx_isp_subdev_ops vic_ops = {
    .core = &vic_core_ops,
    .video = &vic_video_ops,
};

// For CSI subdev - needs stream control and sync
static struct tx_isp_subdev_video_ops csi_video_ops = {
    .s_stream = csi_s_stream,
    .link_stream = csi_link_stream,
};

static struct tx_isp_subdev_core_ops csi_core_ops = {
    .init = tx_isp_csi_init,
    .reset = csi_reset,
};

static struct tx_isp_subdev_ops csi_ops = {
    .core = &csi_core_ops,
    .video = &csi_video_ops,
};

// For DDR subdev - mainly needs format handling
static struct tx_isp_subdev_pad_ops ddr_pad_ops = {
    .g_fmt = ddr_g_fmt,
    .s_fmt = ddr_s_fmt,
    .streamon = ddr_streamon,
    .streamoff = ddr_streamoff,
};

static struct tx_isp_subdev_core_ops ddr_core_ops = {
    .init = ddr_init,
};

static struct tx_isp_subdev_ops ddr_ops = {
    .core = &ddr_core_ops,
    .pad = &ddr_pad_ops,
};

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

static struct tx_isp_subdev_ops fs_subdev_ops = {
    .core = &fs_core_ops,
    .video = &fs_video_ops,
    .sensor = &fs_sensor_ops,
};

static int tx_isp_fs_probe(struct platform_device *pdev)
{
    struct IMPISPDev *dev = ourISPdev;
    struct frame_source_device *fs_dev;
    struct tx_isp_subdev *sd = NULL;
    int ret;

    pr_info("Frame source probe called\n");

    if (!dev || !dev->reg_base) {
        dev_err(&pdev->dev, "No ISP device data\n");
        return -EINVAL;
    }

    // 1. Allocate frame source device
    fs_dev = devm_kzalloc(&pdev->dev, sizeof(*fs_dev), GFP_KERNEL);
    if (!fs_dev) {
        dev_err(&pdev->dev, "Failed to allocate frame source device\n");
        return -ENOMEM;
    }

    // 2. Allocate subdev
    sd = devm_kzalloc(&pdev->dev, sizeof(*sd), GFP_KERNEL);
    if (!sd) {
        dev_err(&pdev->dev, "Failed to allocate subdev\n");
        ret = -ENOMEM;
        goto err_free_fs;
    }
    fs_dev->sd = sd;

    // 3. Initialize basic state
    mutex_init(&fs_dev->lock);
    fs_dev->state = 0;

    // 4. Store in global early
    dev->fs_dev = fs_dev;

    // 5. Initialize subdev
    ret = tx_isp_subdev_init(pdev, sd, &fs_subdev_ops);
    if (ret) {
        dev_err(&pdev->dev, "Failed to init frame source subdev\n");
        goto err_free_sd;
    }

    // 6. Set up the remaining subdev properties
    tx_isp_set_subdev_hostdata(sd, fs_dev);
    platform_set_drvdata(pdev, &sd->module);

    // 7. Set final state
    fs_dev->state = TX_ISP_MODULE_SLAKE;

    pr_info("Frame source probe completed successfully\n");
    return 0;

    err_free_sd:
        devm_kfree(&pdev->dev, sd);
    err_free_fs:
        devm_kfree(&pdev->dev, fs_dev);
    dev->fs_dev = NULL;
    return ret;
}

static int tx_isp_fs_remove(struct platform_device *pdev)
{
    struct IMPISPDev *dev = platform_get_drvdata(pdev);
    if (dev && dev->fs_dev) {
        kfree(dev->fs_dev);
        dev->fs_dev = NULL;
    }
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


static int configure_phy_timing(struct csi_device *csi)
{
    void __iomem *base = csi->phy_regs;

    // Basic PHY timing params from OEM code
    writel(0x7d, base + 0x00);         // Main timing param
    writel(0x3f, base + 0x128);        // Additional timing

    // Power sequence with timing configuration
    writel(0x0, base + 0x08);          // PHY_SHUTDOWNZ
    writel(0x0, base + 0x0c);          // DPHY_RSTZ
    writel(0x0, base + 0x10);          // CSI2_RESETN
    wmb();
    msleep(10);

    writel(0x1, base + 0x08);          // PHY_SHUTDOWNZ
    wmb();
    msleep(10);

    writel(0x1, base + 0x0c);          // DPHY_RSTZ
    wmb();
    msleep(10);

    writel(0x1, base + 0x10);          // CSI2_RESETN
    wmb();
    msleep(10);

    return 0;
}


static void check_csi_interrupts(struct csi_device *csi, const char *where)
{
    u32 mask1 = readl(csi->csi_regs + 0x28);
    u32 mask2 = readl(csi->csi_regs + 0x2c);
    pr_info("CSI interrupt masks at %s:\n"
            "  MASK1: 0x%08x\n"
            "  MASK2: 0x%08x\n",
            where, mask1, mask2);
}

int tx_isp_csi_init(struct platform_device *pdev)
{
    struct IMPISPDev *dev = ourISPdev;
    struct csi_device *csi_dev;
    struct resource *phy_res, *csi_res;
    struct tx_isp_subdev *sd = NULL;
    void __iomem *cpm_base;
    void __iomem *phy_base;
    void __iomem *csi_base;
    int ret;

    if (!pdev)
        return -EINVAL;

    csi_dev = devm_kzalloc(&pdev->dev, sizeof(*csi_dev), GFP_KERNEL);
    if (!csi_dev)
        return -ENOMEM;

    // 2. Allocate subdev
    sd = devm_kzalloc(&pdev->dev, sizeof(*sd), GFP_KERNEL);
    if (!sd) {
        dev_err(&pdev->dev, "Failed to allocate subdev\n");
        ret = -ENOMEM;
        goto err_free_dev;
    }
    csi_dev->sd = sd;

    phy_res = request_mem_region(MIPI_PHY_ADDR, 0x1000, "mipi-phy");
    if (!phy_res) {
        pr_info("MIPI PHY region already claimed\n");
    }

    csi_res = request_mem_region(ISP_W01_ADDR, 0x1000, "isp-w01");
    if (!csi_res) {
        pr_info("ISP W01 region already claimed\n");
    }

    // Map CPM first for clock setup
    cpm_base = ioremap(CPM_BASE, 0x1000);
    if (!cpm_base) {
        return -ENOMEM;
    }

    // Map MIPI PHY
    phy_base = ioremap(MIPI_PHY_ADDR, 0x1000);
    if (!phy_base) {
        iounmap(cpm_base);
        return -ENOMEM;
    }

    // Map CSI (W01)
    csi_base = ioremap(ISP_W01_ADDR, 0x1000);
    if (!csi_base) {
        iounmap(phy_base);
        iounmap(cpm_base);
        return -ENOMEM;
    }

    // 2. Set up device state and registers
    csi_dev->dev = &pdev->dev;
    csi_dev->cpm_regs = cpm_base;
    csi_dev->phy_regs = phy_base;
    csi_dev->csi_regs = csi_base;
    dev->csi_dev = csi_dev;

    // Initialize locking structures
    mutex_init(&csi_dev->mutex);
    spin_lock_init(&csi_dev->lock);

    // Get the clock and store reference
    csi_dev->clk = devm_clk_get(&pdev->dev, "csi");
    if (IS_ERR(csi_dev->clk)) {
        dev_err(&pdev->dev, "Failed to get CSI clock\n");
        return PTR_ERR(csi_dev->clk);
    }

    return 0;
	err_free_dev:
        kfree(csi_dev);
    err_disable_clk:
        clk_disable_unprepare(csi_dev->clk);
    err_unmap:
        iounmap(csi_dev->csi_regs);
    iounmap(csi_dev->phy_regs);
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
//static dma_addr_t tparams_day_phys;
//static dma_addr_t tparams_night_phys;






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

// Helper to set frame group done flag
static void set_group_done(struct frame_group *group)
{
    if (!group || (unsigned long)group < PAGE_OFFSET) {
        pr_err("Invalid group pointer in set_group_done\n");
        return;
    }

    // Validate state contains magic (0x336ac) before accessing
    if ((group->state & 0xFFFFF000) != 0x336000) {
        pr_err("Invalid group state: 0x%x\n", group->state);
        return;
    }

    // The done flag is at offset 0x3c in the update_block
    uint32_t *done_flag = (uint32_t *)((char *)group->update_block + 0x3c);
    *done_flag = 1;
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

//static void generate_test_pattern(void *buf, int width, int height, uint32_t format)
//{
//    uint8_t *ptr = buf;
//    int x, y;
//    int stride;
//
//    // Calculate stride with alignment
//    stride = ALIGN(width, 32);  // 32-byte alignment
//
//    // For NV12 format
//    if (format == ISP_FMT_NV12) {
//        // Fill Y plane
//        for (y = 0; y < height; y++) {
//            for (x = 0; x < width; x++) {
//                // Create gradient pattern
//                int color = ((x + y) % 256);
//                ptr[y * stride + x] = color;
//            }
//            // Clear padding
//            if (stride > width) {
//                memset(&ptr[y * stride + width], 0, stride - width);
//            }
//        }
//
//        // Fill UV plane
//        ptr += (stride * height);
//        for (y = 0; y < height/2; y++) {
//            for (x = 0; x < width; x += 2) {
//                // Alternate U/V values for color bars
//                ptr[y * stride + x] = 128;     // U
//                ptr[y * stride + x + 1] = 128; // V
//            }
//            // Clear padding
//            if (stride > width) {
//                memset(&ptr[y * stride + width], 128, stride - width);
//            }
//        }
//    }
//    // Add support for other formats as needed
//    else if (format == ISP_FMT_YUV422) {
//        // Similar pattern for YUV422
//        // TODO
//    }
//}

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

    pr_info("Frame queued to done list: flags=0x%x\n", buf->flags);

    wake_up_interruptible(&q->wait);
}


static void generate_test_pattern(u8 *buffer, int width, int height)
{
    int x, y;
    u8 *y_plane = buffer;
    u8 *uv_plane = buffer + (width * height);

    // Generate Y plane pattern
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            // Create various regions:

            // Top left: Black and white checkerboard
            if (y < height/2 && x < width/2) {
                y_plane[y * width + x] = ((x/32 + y/32) % 2) ? 235 : 16;  // White/Black
            }
            // Top right: Vertical bars
            else if (y < height/2) {
                y_plane[y * width + x] = (x % 128) < 64 ? 16 : 235;  // Black/White bars
            }
            // Bottom left: Horizontal bars
            else if (x < width/2) {
                y_plane[y * width + x] = (y % 128) < 64 ? 16 : 235;  // Black/White bars
            }
            // Bottom right: Gray gradient
            else {
                y_plane[y * width + x] = 16 + ((x + y) % 220);  // Gradient pattern
            }
        }
    }

    // Generate UV plane pattern (NV12 format - interleaved U and V)
    for (y = 0; y < height/2; y++) {
        for (x = 0; x < width/2; x++) {
            int uv_index = y * width + x * 2;

            // Top left: Grayscale (neutral UV)
            if (y < height/4 && x < width/4) {
                uv_plane[uv_index] = 128;     // U = 128 (neutral)
                uv_plane[uv_index + 1] = 128; // V = 128 (neutral)
            }
            // Top right: Blue tint
            else if (y < height/4) {
                uv_plane[uv_index] = 255;     // U = 255 (blue)
                uv_plane[uv_index + 1] = 128; // V = 128 (neutral)
            }
            // Bottom left: Red tint
            else if (x < width/4) {
                uv_plane[uv_index] = 128;     // U = 128 (neutral)
                uv_plane[uv_index + 1] = 255; // V = 255 (red)
            }
            // Bottom right: Green tint
            else {
                uv_plane[uv_index] = 0;       // U = 0 (reduce blue)
                uv_plane[uv_index + 1] = 0;   // V = 0 (reduce red)
            }
        }
    }
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

static void check_csi_status(struct IMPISPDev *dev)
{
    void __iomem *csi_regs = dev->csi_dev->csi_regs;
    u32 state = readl(csi_regs + 0x14);  // PHY_STATE
    u32 err1 = readl(csi_regs + 0x20);   // ERR1
    u32 err2 = readl(csi_regs + 0x24);   // ERR2
    u32 data_id = readl(csi_regs + 0x18); // DATA_IDS

    pr_info("CSI Status:\n"
            "  PHY State: 0x%08x\n"
            "  Errors: 0x%08x, 0x%08x\n"
            "  Data ID: 0x%08x\n",
            state, err1, err2, data_id);
}


#define I2C_INTST       (0x2C)  // Interrupt status
#define I2C_STA         (0x70)  // Status register
#define I2C_TXFLR       (0x74)  // Transmit FIFO level
#define I2C_RXFLR       (0x78)  // Receive FIFO level
#define I2C_TXABRT      (0x80)  // Transmit abort source

void check_i2c_status(void)
{
    // Add these reads to your setup function or wherever you want to monitor:
    void __iomem *i2c_base = ioremap(0x10050000, 0x1000);  // I2C0 base address
    if (i2c_base) {
        pr_info("I2C Status: 0x%08x\n", readl(i2c_base + I2C_STA));
        pr_info("I2C Interrupts: 0x%08x\n", readl(i2c_base + I2C_INTST));
        pr_info("I2C TX FIFO: %d\n", readl(i2c_base + I2C_TXFLR));
        pr_info("I2C RX FIFO: %d\n", readl(i2c_base + I2C_RXFLR));
        pr_info("I2C Abort Source: 0x%08x\n", readl(i2c_base + I2C_TXABRT));
        iounmap(i2c_base);
    }
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



static void check_subdev_clocks(struct tx_isp_subdev *sd, const char *name)
{
    int i;

    if (!sd) {
        pr_err("Invalid subdev pointer for %s\n", name);
        return;
    }

    pr_info("Checking clocks for %s:\n", name);

    // Sanity check the clock count
    if (!sd->clks || sd->clk_num <= 0 || sd->clk_num > 8) {
        pr_info("  No clocks configured or invalid clock count: %d\n", sd->clk_num);
        return;
    }

    pr_info("  Number of clocks: %d\n", sd->clk_num);

    for (i = 0; i < sd->clk_num; i++) {
        struct clk *clk = sd->clks[i];
        if (IS_ERR_OR_NULL(clk)) {
            pr_info("  Clock %d: invalid\n", i);
        } else {
            unsigned long rate = clk_get_rate(clk);
            pr_info("  Clock %d: valid, rate=%lu Hz\n", i, rate);
        }
    }
}

int tx_isp_init_chain(struct IMPISPDev *dev) {
    int ret;

    // Initialize in order from sensor to DDR
    if (dev->sensor_sd && dev->sensor_sd->ops && dev->sensor_sd->ops->core) {
        ret = dev->sensor_sd->ops->core->init(dev->sensor_sd, 1);
        if (ret && ret != 0xfffffdfd)
            return ret;
    }

    if (dev->csi_dev && dev->csi_dev->sd && dev->csi_dev->sd->ops->core) {
        ret = dev->csi_dev->sd->ops->core->init(dev->csi_dev->sd, 1);
        if (ret && ret != 0xfffffdfd)
            return ret;
    }

    if (dev->vic_dev && dev->vic_dev->sd && dev->vic_dev->sd->ops->core) {
        ret = dev->vic_dev->sd->ops->core->init(dev->vic_dev->sd, 1);
        if (ret && ret != 0xfffffdfd)
            return ret;
    }

    return 0;
}

int tx_isp_stream_chain(struct IMPISPDev *dev, int enable)
{
    int ret;

    // Enable all stream handlers in order
    if (dev->sensor_sd && dev->sensor_sd->ops && dev->sensor_sd->ops->video) {
        ret = dev->sensor_sd->ops->video->s_stream(dev->sensor_sd, enable);
        if (ret && ret != 0xfffffdfd)
            return ret;
    }

    if (dev->csi_dev && dev->csi_dev->sd && dev->csi_dev->sd->ops->video) {
        ret = dev->csi_dev->sd->ops->video->s_stream(dev->csi_dev->sd, enable);
        if (ret && ret != 0xfffffdfd)
            return ret;
    }

    if (dev->vic_dev && dev->vic_dev->sd && dev->vic_dev->sd->ops->video) {
        ret = dev->vic_dev->sd->ops->video->s_stream(dev->vic_dev->sd, enable);
        if (ret && ret != 0xfffffdfd)
            return ret;
    }

    return 0;
}


int tx_isp_video_s_stream(struct IMPISPDev *dev)
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

//    if (dev->csi_dev && dev->csi_dev->sd && dev->csi_dev->sd->ops->core) {
//        pr_info("Initializing CSI core\n");
//        ret = dev->csi_dev->sd->ops->core->init(dev->csi_dev->sd, 1);
//        if (ret && ret != 0xfffffdfd)
//            return ret;
//    }
//
//    if (dev->vic_dev && dev->vic_dev->sd && dev->vic_dev->sd->ops->core) {
//        pr_info("Initializing VIC core\n");
//        ret = dev->vic_dev->sd->ops->core->init(dev->vic_dev->sd, 1);
//        if (ret && ret != 0xfffffdfd)
//            return ret;
//    }
//
//    if (dev->vin_dev && dev->vin_dev->sd && dev->vin_dev->sd->ops->core) {
//        pr_info("Initializing VIN core\n");
//        ret = dev->vin_dev->sd->ops->core->init(dev->vin_dev->sd, 1);
//        if (ret && ret != 0xfffffdfd)
//            return ret;
//    }

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
//
//    if (dev->csi_dev && dev->csi_dev->sd && dev->csi_dev->sd->ops->video) {
//        pr_info("Starting CSI stream\n");
//        ret = dev->csi_dev->sd->ops->video->s_stream(dev->csi_dev->sd, 1);
//        if (ret && ret != 0xfffffdfd)
//            return ret;
//    }
//
//    if (dev->vic_dev && dev->vic_dev->sd && dev->vic_dev->sd->ops->video) {
//        pr_info("Starting VIC stream\n");
//        ret = dev->vic_dev->sd->ops->video->s_stream(dev->vic_dev->sd, 1);
//        if (ret && ret != 0xfffffdfd)
//            return ret;
//    }
//
//    if (dev->vin_dev && dev->vin_dev->sd && dev->vin_dev->sd->ops->video) {
//        pr_info("Starting VIN stream\n");
//        ret = dev->vin_dev->sd->ops->video->s_stream(dev->vin_dev->sd, 1);
//        if (ret && ret != 0xfffffdfd)
//            return ret;
//    }



//    dump_vic_irq_state(dev);

    return 0;
}

int enable_isp_streaming(struct IMPISPDev *dev, struct file *file, int channel, bool enable)
{
    void __iomem *vic_regs = dev->reg_base + VIC_BASE;
    void __iomem *isp_regs = dev->reg_base;
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

    verify_isp_state(dev);  // Add state verification after changes
    return ret;
}



int tx_isp_setup_default_links(struct IMPISPDev *dev) {
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
//
//void tx_vic_disable_irq(struct IMPISPDev *dev)
//{
//    void __iomem *vic_base = dev->reg_base + VIC_BASE;
//
//    // Disable VIC interrupts
//    writel(0, vic_base + 0x93c);
//    wmb();
//}
//
//static void tx_vic_enable_irq(struct IMPISPDev *dev)
//{
//    void __iomem *vic_base = dev->reg_base + VIC_BASE;
//    void __iomem *intc_base = ioremap(0x10001000, 0x1000);
//
//    if (!intc_base) {
//        pr_err("Failed to map INTC\n");
//        return;
//    }
//
//    // Enable VIC interrupts
//    writel(0x000033fb, vic_base + 0x93c);
//    wmb();
//
//    // Configure INTC for VIC
//    writel((1 << 30), intc_base + 0x0C); // Clear VIC mask bit
//    wmb();
//
//    iounmap(intc_base);
//}

int tx_isp_video_link_stream(struct IMPISPDev *dev)
{
    void __iomem *csi_regs = dev->csi_dev->csi_regs;

    pr_info("Setting up video link datapath\n");

    // Report CSI status
    u32 phy_state = readl(csi_regs + 0x14);
    pr_info("CSI PHY state in link: 0x%08x\n", phy_state);

    dev->csi_dev->state = TX_ISP_MODULE_RUNNING;
    return 0;
}

static int handle_sensor_ioctl(struct IMPISPDev *dev, unsigned int cmd, void __user *arg, struct file *file)
{
    struct i2c_client *client = dev->sensor_i2c_client;
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
    case 0x8038564f:  // T23 sensor register write
    case SENSOR_CMD_WRITE_REG: {
        if (copy_from_user(&reg_data, arg, sizeof(reg_data)))
            return -EFAULT;
        ret = isp_sensor_write_reg(client, reg_data.reg, reg_data.val);
        break;
    }
    case 0xc0385650:  // T23 sensor register read
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

    case SENSOR_CMD_SET_GAIN: {
        uint16_t gain;
        if (get_user(gain, (uint16_t __user *)arg))
            return -EFAULT;
        ret = isp_sensor_write_reg(client, 0x3508, (gain >> 8) & 0xFF);
        if (ret == 0)
            ret = isp_sensor_write_reg(client, 0x3509, gain & 0xFF);
        break;
    }

    case SENSOR_CMD_SET_EXP: {
        uint32_t exp;
        if (get_user(exp, (uint32_t __user *)arg))
            return -EFAULT;
        ret = isp_sensor_write_reg(client, 0x3500, (exp >> 12) & 0xFF);
        if (ret == 0)
            ret = isp_sensor_write_reg(client, 0x3501, (exp >> 4) & 0xFF);
        if (ret == 0)
            ret = isp_sensor_write_reg(client, 0x3502, (exp & 0x0F) << 4);
        break;
    }

    case 0xc0045627: {
        int result_val;
        const char *sensor_name = "sc2336";

        if (copy_from_user(&result_val, (void __user *)arg, sizeof(result_val))) {
            pr_err("Failed to get result value\n");
            return -EFAULT;
        }

        strlcpy(ourISPdev->sensor_name, sensor_name, SENSOR_NAME_SIZE);
        pr_info("Stored sensor name: %s\n", ourISPdev->sensor_name);
        return 0;
    }
    case 0x800c56d6:  // T23 WDR buffer
    case 0x800856d7: {
        struct {
            uint32_t buffer_data;
            uint32_t size;
        } wdr_params = {0};

        if (!ourISPdev) {
            pr_err("ISP device not initialized\n");
            return -ENODEV;
        }

        if (copy_from_user(&wdr_params, (void __user *)arg, sizeof(wdr_params))) {
            pr_err("Failed to copy WDR parameters from user\n");
            return -EFAULT;
        }

        pr_info("WDR buffer data: 0x%x\n", wdr_params.buffer_data);
        break;
    }
    case 0x80045612: { // VIDIOC_STREAMON for sensor
        struct tx_isp_subdev *csi_sd = &dev->csi_dev->sd;
        pr_info("Starting video stream on sensor\n");

        // 1. Setup I2C GPIO for sensor control
        ret = configure_i2c_gpio(dev);
        if (ret) {
            pr_err("Failed to configure I2C GPIO: %d\n", ret);
            return ret;
        }

        // 2. Initialize VIC hardware
        ret = init_vic_control(dev);
        if (ret) {
            pr_err("Failed to initialize VIC: %d\n", ret);
            goto err_disable_gpio;
        }

        // 3. Configure VIC streaming settings
        ret = configure_vic_for_streaming(dev);
        if (ret) {
            pr_err("Failed to configure VIC: %d\n", ret);
            goto err_disable_vic;
        }

        // 5. Then start sensor streaming
        ret = tx_isp_video_s_stream(dev);
        if (ret && ret != 0xfffffdfd) {
            pr_err("Failed to start video stream: %d\n", ret);
            goto err_disable_csi;
        }

        // 4. Enable CSI stream first
        ret = tx_isp_csi_s_stream(dev->csi_dev, 1);
        if (ret) {
            pr_err("Failed to start CSI streaming: %d\n", ret);
            goto err_disable_vic;
        }

        // Small delay to let CSI stabilize
        msleep(5);

        // 6. Setup datapath links
        ret = tx_isp_video_link_stream(dev);
        if (ret) {
            pr_err("Failed to setup video links: %d\n", ret);
            goto err_disable_sensor;
        }


        dump_irq_info();

        // 7. Finally enable interrupts
        // tx_vic_enable_irq(dev);

        dump_csi_registers(dev->csi_dev);

        pr_info("Video streaming started successfully\n");
        return 0;

        err_disable_vic:
            // cleanup_vic(dev);
        err_disable_csi:
            tx_isp_csi_s_stream(dev->csi_dev, 0);
        err_disable_sensor:
            //sensor_s_stream(dev->sensor, 0); // TODO
        err_disable_gpio:
            // cleanup_i2c_gpio(dev);
        return ret;
    }
    case 0x80045613: { // VIDIOC_STREAMOFF for sensor
        struct tx_isp_subdev *csi_sd = &dev->csi_dev->sd;
        pr_info("Stopping video stream\n");

        // Disable interrupts first
        //tx_isp_disable_interrupts(dev);

        // Stop CSI streaming
        ret = csi_s_stream(csi_sd, 0);
        if (ret) {
            pr_err("Failed to stop CSI streaming: %d\n", ret);
            return ret;
        }

        // Disable datapath links
        ret = tx_isp_video_link_stream(dev);
        if (ret) {
            pr_err("Failed to disable video links: %d\n", ret);
        }

        // Disable VIC streaming
        //ret = disable_vic_streaming(dev);
//        if (ret) {
//            pr_err("Failed to disable VIC streaming: %d\n", ret);
//        }

        pr_info("Video streaming stopped successfully\n");
        return 0;
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
            for (int j = 0; j < chn->queue->buffer_count; j++) {
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
        chn->queue->buffer_count = 0;
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
                chn->queue->buffer_count,
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
    struct IMPISPDev *dev = ourISPdev;
    dma_addr_t aligned_base = ALIGN(RMEM_BASE, 32);
    size_t aligned_size = ALIGN(RMEM_SIZE, 32);

    // Missing error checking for ourISPdev
    if (!dev) {
        dev_err(&pdev->dev, "ISP device not initialized\n");
        return -EINVAL;
    }

    dev->rmem_addr = aligned_base;
    dev->rmem_size = aligned_size;

    // Should use devm_ioremap for managed memory
    dev->dma_buf = ioremap(aligned_base, aligned_size);
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
             dev->dma_buf,
             aligned_size,
             (uint32_t)aligned_base & 0x1F,
             (uintptr_t)dev->dma_buf & 0x1F);

    return 0;
}


struct isp_mem_request {
    uint32_t method;        // 0x80: Should match ISP_ALLOC_KMALLOC
    uint32_t phys_addr;     // 0x84: Physical address from IMP_Alloc
    uint32_t virt_addr;     // 0x88: Virtual address
    uint32_t size;          // 0x8C: Buffer size
    uint32_t flags;         // 0x90: Flags/attributes
};

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


// Event handler implementations for each type:
static int handle_buffer_request(struct isp_channel *chn, void *data)
{
    struct isp_buf_request *req = data;
    struct isp_channel_event *evt = chn->event;

    if (!evt || !req)
        return -EINVAL;

    // Call stats callback if provided
    if (evt->cb.stats_cb)
        evt->cb.stats_cb(evt->cb.priv);

    return 0;
}


int tx_isp_send_event_to_remote(struct tx_isp_subdev_pad *pad, unsigned int cmd, void *data)
{
    if (!pad) {
        pr_err("No pad provided\n");
        return -EINVAL;
    }

    // Check if we have a linked sink pad with an event handler
    if (pad->link.sink && pad->link.sink->event) {
        // Forward event to the sink pad's handler
        return pad->link.sink->event(pad->link.sink, cmd, data);
    }

    pr_info("No event handler for pad %p cmd 0x%x\n", pad, cmd);
    return -ENOIOCTLCMD;
}


static int handle_buffer_enqueue(struct isp_channel *chn, void *data)
{
    struct isp_buf_enqueue *enq = data;
    struct isp_channel_event *evt = chn->event;

    if (!evt || !enq)
        return -EINVAL;

    // Forward to remote pads
    if (chn->pad.sd) {
        tx_isp_send_event_to_remote(&chn->pad,
                                   ISP_EVENT_BUFFER_ENQUEUE, enq);
    }

    return 0;
}

static int handle_buffer_done(struct isp_channel *chn, void *data)
{
    struct isp_buf_done *done = data;
    pr_info("Buffer done: index=%d seq=%d\n",
             done->index, done->sequence);
    // Mark buffer as complete and wake waiters
    return 0;
}

static int handle_stream_start(struct isp_channel *chn, void *data)
{
    pr_info("Stream start on channel %d\n", chn->channel_id);
    // Enable hardware stream
    return 0;
}

static int handle_stream_cancel(struct isp_channel *chn, void *data)
{
    pr_info("Stream cancel on channel %d\n", chn->channel_id);
    // Stop hardware stream
    return 0;
}

static int handle_queue_free(struct isp_channel *chn, void *data)
{
    pr_info("Queue free on channel %d\n", chn->channel_id);
    // Cleanup buffer queue
    return 0;
}


// Example implementation of event callback:
static int tx_isp_event_handler(void *priv, u32 event, void *data)
{
    struct isp_channel_event *evt = priv;
    int ret = 0;
    unsigned long flags;

    if (!evt || !evt->channel)
        return -EINVAL;

    spin_lock_irqsave(&evt->lock, flags);

    evt->event_count++;
    atomic_inc(&evt->pending_events);

    switch (event) {
        case ISP_EVENT_BUFFER_DONE:
            if (evt->cb.frame_cb)
                evt->cb.frame_cb(evt->cb.priv);
        break;

        case ISP_EVENT_BUFFER_REQUEST:
            ret = handle_buffer_request(evt->channel, data);
        break;

        case ISP_EVENT_BUFFER_ENQUEUE:
            ret = handle_buffer_enqueue(evt->channel, data);
        break;

        case ISP_EVENT_STREAM_START:
            ret = handle_stream_start(evt->channel, data);
        break;

        case ISP_EVENT_STREAM_CANCEL:
            ret = handle_stream_cancel(evt->channel, data);
        break;

        case ISP_EVENT_QUEUE_FREE:
            ret = handle_queue_free(evt->channel, data);
        break;

        default:
            pr_warn("Unknown event type: 0x%x\n", event);
        evt->error_count++;
        ret = -EINVAL;
    }

    atomic_dec(&evt->pending_events);
    spin_unlock_irqrestore(&evt->lock, flags);

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



static void init_tuning_event_handling(struct IMPISPDev *dev)
{
    struct tx_isp_subdev *sd = dev->fs_dev->sd;
    if (!sd) return;

    // Register tuning handlers on the main input pad
    if (sd->num_inpads > 0) {
        init_pad_event_handling(&sd->inpads[0], tx_isp_event_handler);
    }
}



static int destroy_pad_link(struct tx_isp_subdev_pad *src_pad, struct tx_isp_subdev_pad *sink_pad) {
    if (!src_pad || !sink_pad) {
        return -EINVAL;
    }

    // Clear source pad link info
    if (src_pad->link.state) {
        src_pad->link.source = NULL;
        src_pad->link.sink = NULL;
        src_pad->link.flag = 0;
        src_pad->link.state = 0;
        src_pad->state = TX_ISP_PADSTATE_FREE;  // Using correct enum value
    }

    // Clear sink pad link info
    if (sink_pad->link.state) {
        sink_pad->link.source = NULL;
        sink_pad->link.sink = NULL;
        sink_pad->link.flag = 0;
        sink_pad->link.state = 0;
        sink_pad->state = TX_ISP_PADSTATE_FREE;  // Using correct enum value
    }

    return 0;
}

// Main IOCTL handler for destroying links
static int handle_link_destroy(struct IMPISPDev *dev) {
    int ret = 0;
    int link_idx;
    const struct tx_isp_link_configs *configs;
    struct tx_isp_link_config sink_cfg;  // Moved declaration to top
    struct tx_isp_subdev_pad *src_pad, *sink_pad;
    int i;

    if (!dev) {
        return -EINVAL;
    }

    // Get current link configuration
    link_idx = dev->link_state.current_link;
    if (link_idx < 0) {
        pr_info("No active link configuration to destroy\n");
        return 0;
    }

    configs = &isp_link_configs[link_idx];
    pr_info("Destroying link configuration %d with %d links\n",
            link_idx, configs->length);

    // Process each link in the current configuration
    for (i = 0; i < configs->length; i++) {
        const struct tx_isp_link_config *cfg = &configs->config[i];

        // Find the source pad
        src_pad = find_subdev_link_pad(dev, cfg);
        if (!src_pad) {
            pr_warn("Source pad not found for link %d\n", i);
            continue;
        }

        // Find the sink pad
        sink_cfg.src = cfg->dst;
        sink_cfg.flag = cfg->flag;

        sink_pad = find_subdev_link_pad(dev, &sink_cfg);
        if (!sink_pad) {
            pr_warn("Sink pad not found for link %d\n", i);
            continue;
        }

        // Destroy the link
        ret = destroy_pad_link(src_pad, sink_pad);
        if (ret) {
            pr_err("Failed to destroy link %d: %d\n", i, ret);
            if (ret != -EFAULT) {
                continue;
            }
            return ret;
        }

        // Call video ops stream off if available
        if (src_pad->sd && src_pad->sd->ops &&
            src_pad->sd->ops->video &&
            src_pad->sd->ops->video->s_stream) {
            ret = src_pad->sd->ops->video->s_stream(src_pad->sd, 0);
            if (ret && ret != -EFAULT) {
                pr_warn("Video stream off failed: %d\n", ret);
            }
        }
    }

    // Clear current link configuration
    dev->link_state.current_link = -1;
    pr_info("Link configuration destroyed successfully\n");
    return 0;
}

static int read_proc_value(const char *path, char *buf, size_t size)
{
    struct file *f;
    mm_segment_t old_fs;
    loff_t pos = 0;
    ssize_t n;

    f = filp_open(path, O_RDONLY, 0);
    if (IS_ERR(f))
        return PTR_ERR(f);

    old_fs = get_fs();
    set_fs(get_ds());

    n = vfs_read(f, buf, size - 1, &pos);

    set_fs(old_fs);
    filp_close(f, NULL);

    if (n > 0)
        buf[n] = '\0';

    return n;
}

static int handle_isp_read_sensor_id(struct tx_isp_device *dev, void __user *arg)
{
    struct isp_sensor_id_info id_info = {0};
    char buf[32];
    int ret;

    ret = read_proc_value("/proc/jz/sensor/chip_id", buf, sizeof(buf));
    if (ret < 0) {
        pr_err("Failed to read sensor chip_id: %d\n", ret);
        id_info.status = ret;
    }

    // Parse the chip ID from the proc value (format should be "0x%x\n")
    if (sscanf(buf, "0x%x", &id_info.id) != 1) {
        pr_err("Invalid chip ID format from proc\n");
        id_info.status = -EINVAL;
    }

    id_info.status = 0;  // Success

    return id_info.status;
}

static ssize_t video_input_cmd_set(struct file *file, const char __user *buffer,
                                  size_t count, loff_t *f_pos)
{
    struct seq_file *m = file->private_data;
    struct tx_isp_module *module = (void *)(m->private);
    struct tx_isp_subdev *sd;
    struct vin_device *vin;
    struct tx_isp_sensor *sensor;
    char *buf;
    int ret = 0;

    if (!module) {
        return -EINVAL;
    }

    sd = module_to_subdev(module);
    if (!sd) {
        return -EINVAL;
    }

    vin = tx_isp_get_subdev_hostdata(sd);
    if (!vin || !vin->active) {
        seq_printf(m, "No active sensor found\n");
        return -EINVAL;
    }

    sensor = vin->active;

    // Allocate command buffer
    if (count > 1024) { // Use reasonable buffer size
        buf = kzalloc(count + 1, GFP_KERNEL);
        if (!buf)
            return -ENOMEM;
    } else {
        buf = kzalloc(1024, GFP_KERNEL);
        if (!buf)
            return -ENOMEM;
    }

    if (copy_from_user(buf, buffer, count)) {
        kfree(buf);
        return -EFAULT;
    }
    buf[count] = '\0';

    // Parse commands
    if (!strncmp(buf, "r sen_reg", 9)) {
        unsigned int reg;
        unsigned int val;
        struct tx_isp_dbg_register dbg = {0};

        reg = simple_strtoul(buf + 9, NULL, 0);

        dbg.reg = reg;
        ret = tx_isp_subdev_call(&sensor->sd, core, g_register, &dbg);
        if (ret) {
            seq_printf(m, "Failed to read sensor register 0x%x\n", reg);
        } else {
            seq_printf(m, "Sensor reg read 0x%x = 0x%x\n", reg, (unsigned int)dbg.val);
        }
    }
    else if (!strncmp(buf, "w sen_reg", 9)) {
        unsigned int reg, val;
        char *p;
        struct tx_isp_dbg_register dbg = {0};

        reg = simple_strtoul(buf + 9, &p, 0);
        if (p && *p)
            val = simple_strtoul(p + 1, NULL, 0);
        else {
            kfree(buf);
            return -EINVAL;
        }

        dbg.reg = reg;
        dbg.val = val;
        ret = tx_isp_subdev_call(&sensor->sd, core, s_register, &dbg);
        if (ret)
            seq_printf(m, "Failed to write sensor register\n");
        else
            seq_printf(m, "Wrote 0x%x to sensor reg 0x%x\n", val, reg);
    }

    kfree(buf);
    return count;
}

static int video_input_cmd_show(struct seq_file *m, void *v)
{
    struct tx_isp_module *module = (void *)(m->private);
    struct tx_isp_subdev *sd;
    struct vin_device *vin;

    if (!module)
        return -EINVAL;

    sd = module_to_subdev(module);
    if (!sd)
        return -EINVAL;

    vin = tx_isp_get_subdev_hostdata(sd);
    if (!vin)
        return -EINVAL;

    seq_printf(m, "VIN Device State: %d\n", vin->state);
    if (vin->active)
        seq_printf(m, "Active Sensor: present\n");
    else
        seq_printf(m, "No active sensor\n");

    return 0;
}


static int video_input_cmd_open(struct inode *inode, struct file *file)
{
    return single_open_size(file, video_input_cmd_show, PDE_DATA(inode), 512);
}


static struct file_operations video_input_cmd_fops ={
    .read = seq_read,
    .open = video_input_cmd_open,
    .llseek = seq_lseek,
    .release = single_release,
    .write = video_input_cmd_set,
};
static int tx_isp_vin_probe(struct platform_device *pdev)
{
    struct IMPISPDev *dev = ourISPdev;
    struct vin_device *vin_dev;
    struct tx_isp_subdev *sd = NULL;
    int ret;

    pr_info("VIN probe called\n");

    if (!dev || !dev->reg_base) {
        dev_err(&pdev->dev, "No ISP device data\n");
        return -EINVAL;
    }

    // 1. Allocate VIN device
    vin_dev = devm_kzalloc(&pdev->dev, sizeof(*vin_dev), GFP_KERNEL);
    if (!vin_dev) {
        dev_err(&pdev->dev, "Failed to allocate VIN device\n");
        return -ENOMEM;
    }

    // 2. Allocate subdev
    sd = devm_kzalloc(&pdev->dev, sizeof(*sd), GFP_KERNEL);
    if (!sd) {
        dev_err(&pdev->dev, "Failed to allocate subdev\n");
        ret = -ENOMEM;
        goto err_free_dev;
    }
    vin_dev->sd = sd;

    // 3. Initialize basic state
    mutex_init(&vin_dev->lock);
    INIT_LIST_HEAD(&vin_dev->sensors);
    vin_dev->refcnt = 0;
    vin_dev->active = NULL;

    // 4. Store in global early
    dev->vin_dev = vin_dev;

    // 5. Initialize subdev
    ret = tx_isp_subdev_init(pdev, sd, &vin_subdev_ops);
    if (ret) {
        dev_err(&pdev->dev, "Failed to init VIN subdev\n");
        goto err_free_sd;
    }

    // 6. Set up the remaining subdev properties
    tx_isp_set_subdev_hostdata(sd, vin_dev);
    platform_set_drvdata(pdev, &sd->module);
    tx_isp_set_subdev_debugops(sd, &video_input_cmd_fops);

    // 7. Set final state
    vin_dev->state = TX_ISP_MODULE_SLAKE;

    pr_info("VIN probe completed successfully\n");
    return 0;

    err_free_sd:
        devm_kfree(&pdev->dev, sd);
    err_free_dev:
        dev->vin_dev = NULL;
    devm_kfree(&pdev->dev, vin_dev);
    return ret;
}

void system_irq_func_set(int irq_num, irq_handler_t handler)
{
    if (irq_num < 32)
        irq_func_cb[irq_num] = handler;
}

static void dump_intc_state(void)
{
    void __iomem *intc = ioremap(0x10001000, 0x100);
    if (!intc)
        return;

    pr_info("INTC State:\n");
    pr_info("  Mask: 0x%08x\n", readl(intc + 0x04));  // INTC mask
    pr_info("  Pending: 0x%08x\n", readl(intc + 0x10));  // Pending

    iounmap(intc);
}


// Matching the OEM handler structure
static irqreturn_t tx_vic_irq_handler(int irq, void *dev_id)
{
    struct IMPISPDev *dev = dev_id;
    void __iomem *vic_regs = dev->reg_base + VIC_BASE;
    u32 status = readl(vic_regs + 0xb4);
    u32 dma_status = readl(vic_regs + 0x84);

    pr_info("VIC IRQ: status=0x%08x dma=0x%08x\n",
            status, dma_status);

    // Clear status
    if (status)
        writel(status, vic_regs + 0xb4);
    if (dma_status)
        writel(dma_status, vic_regs + 0x84);
    wmb();

    return IRQ_HANDLED;
}

// Add threaded handler
static irqreturn_t tx_isp_thread_handler(int irq, void *dev_id)
{
    struct IMPISPDev *dev = dev_id;
    // Handle processing that can sleep
    return IRQ_HANDLED;
}


static void tx_vic_process_frame(struct IMPISPDev *dev)
{
    pr_info("VIC frame processing\n");
    // Read VIC status registers
    void __iomem *vic_regs = dev->reg_base + VIC_BASE;
    u32 status = readl(vic_regs + 0x1e0);
    u32 status2 = readl(vic_regs + 0x1e4);

    // Clear status
    writel(status, vic_regs + 0x1f0);
    writel(status2, vic_regs + 0x1f4);

    // Process frame data based on status
    if (status & BIT(0)) {  // Frame done
        dev->frame_count++;
        // Handle frame completion
    }
}


// For Linux 3.10 completion handling
static inline void reinit_completion(struct completion *x)
{
    init_completion(x);
}


// ISP processing thread similar to OEM's isp_fw_process
static int tx_isp_fw_process(void *data)
{
    struct IMPISPDev *dev = data;

    while (!kthread_should_stop()) {
        // Wait for interrupt events
        wait_for_completion_interruptible(&dev->frame_complete);

        // Process received frames
        if (dev->vic_processing) {
            // Handle VIC frame data
            tx_vic_process_frame(dev);
        }

        // Reset completion for next frame
        reinit_completion(&dev->frame_complete);
    }

    return 0;
}

struct tx_isp_irq_handler {
    int irq_number;
    void (*enable)(struct tx_isp_irq_handler *);
    void (*disable)(struct tx_isp_irq_handler *);
    spinlock_t lock;
    void *dev_id;
};

static void tx_isp_enable_irq(struct tx_isp_irq_handler *handler)
{
    if (handler->irq_number)
        enable_irq(handler->irq_number);
}

static void tx_isp_disable_irq(struct tx_isp_irq_handler *handler)
{
    if (handler->irq_number)
        disable_irq(handler->irq_number);
}

int tx_isp_request_irq(struct platform_device *pdev, struct tx_isp_irq_handler *handler)
{
    int ret;
    int irq = 37;

    if (!pdev || !handler) {
        pr_err("%s: invalid parameters\n", __func__);
        return -EINVAL;
    }

    spin_lock_init(&handler->lock);

    ret = request_threaded_irq(irq,
                             tx_isp_irq_handler,  // ISP specific handler
                             tx_isp_threaded_irq_handler,
                             IRQF_SHARED,
                             "isp-m0",  // ISP specific name
                             handler->dev_id);

    if (ret) {
        pr_err("Failed to request ISP IRQ %d: %d\n", irq, ret);
        handler->irq_number = 0;
        return ret;
    }

    handler->enable = tx_isp_enable_irq;
    handler->disable = tx_isp_disable_irq;
    handler->irq_number = irq;

    handler->disable(handler);
    return 0;
}



static irqreturn_t tx_vic_threaded_irq_handler(int irq, void *data)
{
    struct IMPISPDev *dev = data;
    struct vic_device *vic_dev = dev->vic_dev;
    int ch = 0;

    u32 buf_size = dev->sensor_width * dev->sensor_height;
    buf_size <<= 1;

    if (vic_dev->mdma_en == 0) {
        if (ch == 0 && vic_mdma_ch0_sub_get_num > 0) {
            u32 next_idx = (vic_mdma_ch0_set_buff_index + 1) % 5;
            u32 dma_addr = readl(vic_dev->regs + VIC_DMA_ADDR +
                               (vic_mdma_ch0_set_buff_index << 2));

            vic_mdma_ch0_set_buff_index = next_idx;
            writel(dma_addr + buf_size, vic_dev->regs + VIC_DMA_ADDR +
                  (next_idx << 2));

            vic_mdma_ch0_sub_get_num--;
        }

        if (vic_mdma_ch0_sub_get_num == 0) {
            complete(&vic_dev->frame_complete);
        }
    }

    return IRQ_HANDLED;
}


int tx_vic_request_irq(struct platform_device *pdev, struct tx_isp_irq_handler *handler)
{
    int ret;
    int irq = 38;
    void __iomem *intc_base;
    void __iomem *vic_regs = ourISPdev->reg_base + VIC_BASE;
    u32 mask;

    if (!pdev || !handler) {
        pr_err("%s: invalid parameters\n", __func__);
        return -EINVAL;
    }

    if (!handler->dev_id) {
        pr_err("Handler dev_id is NULL\n");
        return -EINVAL;
    }

    // Setup INTC first
    intc_base = ioremap(0x10001000, 0x1000);
    if (!intc_base) {
        pr_err("Failed to map INTC\n");
        return -ENOMEM;
    }

    // Read initial state
    mask = readl(intc_base + 0x04);
    pr_info("Initial INTC IMR: %08x\n", mask);

    // Clear mask bit for VIC (bit 30)
    mask &= ~BIT(30);
    writel(mask, intc_base + 0x04);
    wmb();

    pr_info("INTC IMR after unmasking: %08x\n", readl(intc_base + 0x04));
    pr_info("INTC ISR: %08x\n", readl(intc_base + 0x00));

    iounmap(intc_base);

    spin_lock_init(&handler->lock);

    pr_info("Requesting VIC IRQ %d with dev_id=%p\n", irq, handler->dev_id);

    ret = request_threaded_irq(irq, tx_vic_irq_handler, NULL,
                             IRQF_SHARED,
                             "isp-w02",
                             handler->dev_id);
    // In probe after IRQ request
    if (ret == 0) {
        // Enable test interrupt bit
        writel(0x1, vic_regs + 0x93c);  // Enable bit 0
        wmb();

        // Set the corresponding status bit
        writel(0x1, vic_regs + 0xb4);   // Set bit 0
        wmb();

        msleep(100);

        pr_info("Interrupt test - status: 0x%08x irqen: 0x%08x\n",
                readl(vic_regs + 0xb4),
                readl(vic_regs + 0x93c));
    }

    pr_info("VIC IRQ %d registered successfully with handler=%p\n",
            irq, handler);

    handler->enable = tx_isp_enable_irq;
    handler->disable = tx_isp_disable_irq;
    handler->irq_number = irq;

    handler->enable(handler);

    return 0;
}



int vic_s_stream(struct tx_isp_subdev *sd, int enable)
{
    struct vic_device *vic = tx_isp_get_subdevdata(sd);
    struct IMPISPDev *dev = ourISPdev;
    int ret;

    pr_info("VIC stream %s\n", enable ? "on" : "off");

    if (enable) {
        // Enable VIC IRQ handling before starting stream
        if (!vic->irq_enabled) {
            // Clear any pending
            writel(0xFFFFFFFF, vic->regs + 0xb4);
            wmb();

            // Enable VIC interrupts
            writel(0x000033fb, vic->regs + 0x93c);
            wmb();

            vic->irq_enabled = 1;
            // Call parent's interrupt enable if needed
            if (vic->irq_handler)
                vic->irq_handler(vic->irq_priv);
        }

        // Continue stream enable
        ret = tx_isp_video_link_stream(dev);
        if (ret)
            return ret;

    } else {
        if (vic->irq_enabled) {
            // Disable interrupts
            writel(0, vic->regs + 0x93c);
            wmb();

            vic->irq_enabled = 0;
            if (vic->irq_disable)
                vic->irq_disable(vic->irq_priv);
        }
    }

    return 0;
}


static void tx_isp_disable_interrupts(struct IMPISPDev *dev)
{
    void __iomem * intc = ioremap(T31_INTC_BASE, 0x100);
    void __iomem * vic_base = ioremap(VIC_BASE, 0x1000);
    void __iomem * isp_base = ioremap(ISP_BASE, 0x1000);

    // 1. Mask interrupts at INTC first
    writel(BIT(5) | BIT(6), intc + INTC_IMSR2);

    // 2. Disable peripheral interrupts
    writel(0, vic_base + VIC_IRQ_ENABLE);
    writel(0, isp_base + ISP_INT_MASK);

    // 3. Clear any pending interrupts
    u32 val = readl(vic_base + VIC_IRQ_STATUS);
    if (val)
        writel(val, vic_base + VIC_IRQ_CLEAR);

    val = readl(isp_base + ISP_INT_STATUS);
    if (val)
        writel(val, isp_base + ISP_INT_CLEAR);

    // 4. Unmap registers
    iounmap(intc);
    iounmap(vic_base);
    iounmap(isp_base);
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


    void __iomem *csi_base;
    csi_base = dev->csi_dev->csi_regs;

    if (dev->csi_dev && dev->csi_dev->csi_regs) {
        csi_base = dev->csi_dev->csi_regs;
        u32 mask1 = readl(csi_base + 0x28);
        u32 mask2 = readl(csi_base + 0x2c);
        u32 err1 = readl(csi_base + 0x20);
        u32 err2 = readl(csi_base + 0x24);

        pr_info("CSI interrupt state now:\n"
                "  MASK1: 0x%08x\n"
                "  MASK2: 0x%08x\n"
                "  ERR1: 0x%08x\n"
                "  ERR2: 0x%08x\n",
                mask1, mask2, err1, err2);

        if (err1) {
            pr_info("CSI ERR1 bits set:\n");
            if (err1 & BIT(0)) pr_info("  - SOT sync error\n");
            if (err1 & BIT(1)) pr_info("  - SOT error\n");
            if (err1 & BIT(2)) pr_info("  - Frame sync error\n");
            if (err1 & BIT(3)) pr_info("  - Frame format error\n");
            if (err1 & BIT(4)) pr_info("  - CRC error\n");
            if (err1 & BIT(5)) pr_info("  - Header ECC error\n");
            if (err1 & BIT(6)) pr_info("  - Line sync error\n");
        }

        if (err2) {
            pr_info("CSI ERR2 bits set:\n");
            if (err2 & BIT(0)) pr_info("  - PHY sync error\n");
            if (err2 & BIT(1)) pr_info("  - Overflow error\n");
            if (err2 & BIT(2)) pr_info("  - Reserved error\n");
        }
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

        // Do sensor reset sequence
        pr_info("Initial sensor reset...\n");
        sensor_hw_reset(ourISPdev);

        ret = detect_sensor_type(ourISPdev);
        if (ret) {
            pr_err("Failed to detect sensor type: %d\n", ret);
            return ret;
        }

    pr_info("Sensor registered: %s\n", ourISPdev->sensor_name);
    return 0;
  }
  case VIDIOC_GET_SENSOR_ENUMERATION: {  // 0xc050561a
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

        if (dev->sensor_sd && dev->sensor_sd->ops && dev->sensor_sd->ops->core) {
            pr_info("Initializing sensor core\n");
            ret = dev->sensor_sd->ops->core->g_chip_ident(dev->sensor_sd, dev->chip);
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
    case 0x800456d2: { // VIDIOC_ENABLE_STREAM
        int ret;
        pr_info("Enabling video link stream\n");

        ret = tx_isp_video_link_stream(dev);
        if (ret) {
            pr_err("Failed to enable link stream\n");
            return ret;
        }

        pr_info("Video link stream enabled\n");
        return 0;
    }
//    case 0x800c56d5: { // TX_ISP_SET_BUF 0x800856d5
//        struct imp_buffer_info local_buf_info;
//        struct isp_channel *chn;
//        size_t frame_size;
//        u8 *vbm_entry;
//
//        pr_info("Handling TX_ISP_SET_BUF\n");
//
//        if (!ourISPdev) {
//            pr_err("ISP device not initialized\n");
//            return -EINVAL;
//        }
//
//        pr_info("Copying buffer info from user address %lx\n", arg);
////
////        if (!arg || (unsigned long)arg >= 0xfffff001) {
////            pr_err("Invalid user pointer\n");
////            return -EFAULT;
////        }
////
////        if (copy_from_user(&local_buf_info, (void __user *)arg, sizeof(local_buf_info))) {
////            pr_err("Failed to copy from user\n");
////            return -EFAULT;
////        }
////
////        pr_info("Buffer info: method=0x%x phys=0x%llx size=%zu\n",
////                local_buf_info.method, local_buf_info.phys_addr,
////                local_buf_info.size);
////
////        // Magic number handshake - maintain compatibility
////        if (local_buf_info.method == 0x0) {
////            // Initial phase - tell userspace we support DMA
////            local_buf_info.method = ISP_ALLOC_DMA;
////            local_buf_info.phys_addr = 0x1;
////            local_buf_info.size = 0x1;
////            local_buf_info.virt_addr = (unsigned long)ourISPdev->dma_buf;
////            local_buf_info.flags = 0;
////
////            if (copy_to_user((void __user *)arg, &local_buf_info, sizeof(local_buf_info)))
////                return -EFAULT;
////            return 0;
////        }
////
////        // Second phase - actual buffer setup
////        if (local_buf_info.phys_addr == 0x1) {
////            chn = &ourISPdev->channels[0];
////
////
////            if (!chn) {
////                pr_err("No frame channel initialized\n");
////                return -EINVAL;
////            }
////
////            // Calculate frame size
////            frame_size = ALIGN((chn->width * chn->height * 3) / 2, 4096);
////
////            // Setup DMA buffer if not already done
////            if (!chn->dma_addr) {
////                int ret = setup_dma_buffer(chn);
////                if (ret)
////                    return ret;
////            }
////
////            // Create VBM entry for compatibility
////            vbm_entry = kzalloc(0x80, GFP_KERNEL);
////            if (!vbm_entry)
////                return -ENOMEM;
////
////            // Setup VBM entry to point to our DMA buffer
////            struct vbm_entry_flags {
////                u32 state;
////                u32 ready;
////                u64 sequence;
////                u64 phys_addr;
////                void *virt;
////                u32 size;
////            } __packed;
////
////            struct vbm_entry_flags *flags = (void *)(vbm_entry + 0x48);
////            flags->state = 0;
////            flags->ready = 1;
////            flags->sequence = 0;
////            flags->phys_addr = chn->dma_addr;  // Use our DMA address
////            flags->virt = chn->buf_base;       // Use our DMA virtual address
////            flags->size = frame_size;
////
////            // Store entry
////            if (!chn->vbm_table) {
////                chn->vbm_table = kzalloc(sizeof(void *), GFP_KERNEL);
////                if (!chn->vbm_table) {
////                    kfree(vbm_entry);
////                    return -ENOMEM;
////                }
////            }
////            chn->vbm_table[0] = vbm_entry;
////            chn->state |= BIT(0);
////
////            // Return info about our DMA buffer
////            local_buf_info.method = ISP_ALLOC_DMA;
////            local_buf_info.phys_addr = chn->dma_addr;
////            local_buf_info.size = frame_size;
////            local_buf_info.virt_addr = (unsigned long)chn->buf_base;
////            local_buf_info.flags = 0;
////
////            if (copy_to_user((void __user *)arg, &local_buf_info, sizeof(local_buf_info))) {
////                kfree(vbm_entry);
////                return -EFAULT;
////            }
////
////            pr_info("DMA buffer setup: phys=%pad virt=%p size=%zu\n",
////                    &chn->dma_addr, chn->buf_base, frame_size);
//
//            return 0;
//
////        pr_err("Invalid buffer setup sequence\n");
////        return -EINVAL;
//    }
    case 0x800856d5: { // TX_ISP_SET_BUF 0x800856d5
        struct imp_buffer_info local_buf_info;
        struct isp_channel *chn;
        size_t frame_size;
        u8 *vbm_entry;

        pr_info("Handling TX_ISP_SET_BUF\n");

        if (!ourISPdev) {
            pr_err("ISP device not initialized\n");
            return -EINVAL;
        }

        pr_info("Copying buffer info from user address %lx\n", arg);

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
    case 0x800856d4: {  // Additional buffer setup
        struct imp_buffer_info local_buf_info;
        void __iomem *isp_regs = ourISPdev->reg_base;
        uint32_t stride, size;

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
        buf_info.buffer_start = ourISPdev->dma_addr;
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
        sd = dev->fs_dev->sd;
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
        pr_info("Disabling ISP links\n");
        tx_isp_disable_interrupts(ourISPdev);
        return 0;
    }
    case 0x800456d1: {  // Destroy links
        pr_info("TODO Destroying ISP links\n");
        int __user *result = (int __user *)arg;
        int ret = 0;
        //dump_csi_reg(dev);
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
    case 0x800c56d6:  // T23 WDR buffer
    case 0x800856d7:
    case 0xc0045627:
    case 0x8038564f:  // T23 sensor register write
    case 0xc0385650:  // T23 sensor register read
    case SENSOR_CMD_READ_ID:
    case SENSOR_CMD_WRITE_REG:
    case SENSOR_CMD_READ_REG:
    case SENSOR_CMD_SET_GAIN:
    case SENSOR_CMD_SET_EXP:
    case SENSOR_CMD_STREAM_ON:
    case SENSOR_CMD_STREAM_OFF:
    case 0x80045612 : // 0x80045612 VIDIOC_STREAMON
        pr_info("Sensor command: 0x%x\n", cmd);
        ret = handle_sensor_ioctl(ourISPdev, cmd, argp, file);
        break;
    case 0x805456c1:   // T23 sensor register
    case 0x805056c1: { // tx_isp_sensor_register_sensor
        struct sensor_list_req {
            char name[0x50];  // Based on the 0x50 size buffer in OEM code
        } __attribute__((packed));
        struct sensor_list_req req;

        if (copy_from_user(&req, (void __user *)arg, sizeof(req))) {
            pr_err("Failed to copy sensor register request from user\n");
            return -EFAULT;
        }

        // This maps to the same functionality as VIDIOC_REGISTER_SENSOR
        struct i2c_client *client = dev->sensor_i2c_client;
        if (!client || !client->adapter) {
            pr_err("No I2C client or adapter available\n");
            return -ENODEV;
        }

        ret = detect_sensor_type(ourISPdev);
        if (ret) {
            pr_err("Failed to detect sensor type: %d\n", ret);
            return ret;
        }

        pr_info("Sensor registered: %s\n", ourISPdev->sensor_name);
        return 0;
    }
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

// Add this at the module level to track event handlers
static struct isp_event_context {
    isp_event_cb handler;
    spinlock_t lock;
} event_ctx = {
    .handler = NULL,
    .lock = __SPIN_LOCK_UNLOCKED(event_ctx.lock),
};



struct isp_channel *get_channel_from_pad(struct tx_isp_subdev_pad *pad)
{
    struct IMPISPDev *dev = ourISPdev;
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

    switch (event) {
        case ISP_EVENT_BUFFER_REQUEST:
            ret = handle_buffer_request(chn, data);
        break;
        case ISP_EVENT_BUFFER_ENQUEUE:
            ret = handle_buffer_enqueue(chn, data);
        break;
        // ... other event handling ...
        default:
            pr_info("Unhandled event 0x%x\n", event);
        ret = -ENOIOCTLCMD;
    }

    return ret;
}


// Set up frame info in the control block
// This matches what libimp expects to find
struct frame_control {
    u32 state;           // 0x00: Frame state
    u32 cur_frame;       // 0x04: Current frame index
    u32 frame_count;     // 0x08: Total frame count
    u32 dma_base;        // 0x0C: DMA buffer base address
    u32 frame_size;      // 0x10: Size of each frame
    u32 frame_stride;    // 0x14: Frame stride
    u32 reserved[2];     // 0x18-0x1F: Reserved
} __packed;

static int setup_channel_ctrl_block(struct isp_channel *chn) {
    void __iomem *ctrl_base;
    const unsigned int CTRL_OFFSET = 0x9888;  // Offset from ISP base

    if (!ourISPdev || !ourISPdev->reg_base) {
        pr_err("No ISP device or register base\n");
        return -EINVAL;
    }

    // Calculate control block address from ISP base
    ctrl_base = (void __iomem *)0x13309888;

    // Store for cleanup
    chn->ctrl_block = ctrl_base;

    pr_info("Setting up control block at ISP base + 0x%x = %p\n",
            CTRL_OFFSET, ctrl_base);

    return 0;
}

static int framechan_open(struct inode *inode, struct file *file)
{
    int channel = iminor(inode);
    struct IMPISPDev *dev = ourISPdev;
    struct isp_channel *chn = &dev->channels[channel];
    int ret = 0;
    uint32_t base_offset = 0x1094d4;

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

    // Initialize pad for event handling
    chn->pad.sd = dev->fs_dev->sd;
    chn->pad.type = TX_ISP_PADTYPE_OUTPUT;
    chn->pad.index = channel;
    init_pad_event_handling(&chn->pad, fs_pad_event_handler);

    uint32_t frame_offset = ALIGN(channel * chn->buf_size * chn->buffer_count, 32);
    dma_addr_t buffer_start = ALIGN(dev->dma_addr + base_offset, 32);

    chn->buf_base = (void *)dev->dma_addr + base_offset + frame_offset;
    chn->dma_addr = buffer_start + frame_offset;

    setup_channel_ctrl_block(chn);

    file->private_data = chn;

    pr_info("Framechan%d opened successfully\n", channel);
    //dump_csi_reg(dev);

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

//
//static void generate_test_frame(struct isp_channel *chn, struct video_buffer *vbuf) {
//    // Calculate offset into DMA buffer for this frame
//    uint8_t *data_buf = chn->buf_base + (vbuf->index * chn->required_size);
//    if (!data_buf) {
//        pr_err("Invalid DMA buffer for index %d\n", vbuf->index);
//        return;
//    }
//
//    // Calculate stride with proper alignment
//    int aligned_width = ALIGN(chn->width, 16);
//
//    pr_info("Generating test pattern at %p (aligned width: %d)\n",
//            data_buf, aligned_width);
//
//    // Y plane test pattern
//    uint8_t *y_plane = data_buf;
//    for (int y = 0; y < chn->height; y++) {
//        for (int x = 0; x < chn->width; x++) {
//            int offset = y * aligned_width + x;
//            if (y < chn->height/2) {
//                if (x < chn->width/2) {
//                    // Top left: Checkerboard
//                    y_plane[offset] = ((x/32 + y/32) % 2) ? 235 : 16;
//                } else {
//                    // Top right: Vertical bars
//                    y_plane[offset] = (x % 64) < 32 ? 16 : 235;
//                }
//            } else {
//                if (x < chn->width/2) {
//                    // Bottom left: Horizontal bars
//                    y_plane[offset] = (y % 64) < 32 ? 16 : 235;
//                } else {
//                    // Bottom right: Gradient
//                    y_plane[offset] = (uint8_t)(16 + ((x + y) % 220));
//                }
//            }
//        }
//        // Clear padding
//        if (aligned_width > chn->width) {
//            memset(&y_plane[y * aligned_width + chn->width], 0,
//                   aligned_width - chn->width);
//        }
//    }
//
//    // UV plane comes right after Y plane
//    uint8_t *uv_plane = y_plane + (aligned_width * chn->height);
//    for (int y = 0; y < chn->height/2; y++) {
//        for (int x = 0; x < chn->width/2; x++) {
//            int offset = y * aligned_width + x * 2;
//            if (y < chn->height/4) {
//                if (x < chn->width/4) {
//                    // Top left: Gray
//                    uv_plane[offset] = 128;
//                    uv_plane[offset + 1] = 128;
//                } else {
//                    // Top right: Blue
//                    uv_plane[offset] = 240;
//                    uv_plane[offset + 1] = 128;
//                }
//            } else {
//                if (x < chn->width/4) {
//                    // Bottom left: Red
//                    uv_plane[offset] = 128;
//                    uv_plane[offset + 1] = 240;
//                } else {
//                    // Bottom right: Green
//                    uv_plane[offset] = 16;
//                    uv_plane[offset + 1] = 16;
//                }
//            }
//        }
//        // Clear padding
//        if (aligned_width > chn->width) {
//            memset(&uv_plane[y * aligned_width + chn->width], 128,
//                   aligned_width - chn->width);
//        }
//    }
//
//    // Ensure writes are visible
//    wmb();
//
//    // Update control block to indicate current frame
//    if (chn->ctrl_block) {
//        struct frame_control *ctrl = chn->ctrl_block;
//        ctrl->cur_frame = vbuf->index;
//        wmb();
//    }
//
//    pr_info("Generated test pattern for buffer %d in DMA buffer at %p\n",
//            vbuf->index, data_buf);
//}

struct isp_resolution_block {
    u32 resolution;     // e.g. 07800438 for 1920x1080
    u32 stride;         // 02000200
    u32 reserved[8];    // Space between resolution blocks
    u32 resolution2;    // Repeated resolution value
    u32 enable;         // 00000001
};

struct isp_ctrl_block {
    u32 magic;          // Starting value 01010001
    u32 reserved1[21];  // Space until first resolution block
    struct isp_resolution_block res_1080p;  // 1920x1080 block
    // ... other resolution blocks follow
    // At offset 471: Control/status section
    u32 control_magic;  // f001f001
    u32 green_control[8]; // 40404040 pattern
    u32 status_flags;   // 0011ff00
};

struct isp_block_1080p {
    u32 resolution;     // 0x07800438 (1920x1080)
    u32 stride;         // 0x02000200
    u32 reserved[8];
    u32 resolution2;    // 0x07800438 repeated
    u32 enable;         // 0x00000001
};

struct isp_block_720p {
    u32 resolution;     // 0x050002d0 (1280x720)
    u32 stride;         // 0x03000300
    u32 reserved[8];
    u32 resolution2;    // 0x050002d0 repeated
    u32 enable;         // 0x00000001
};

struct isp_block_480p {
    u32 resolution;     // 0x028001e0 (640x480)
    u32 stride;         // 0x06000480
    u32 reserved[8];
    u32 resolution2;    // 0x028001e0 repeated
    u32 enable;         // 0x00000001
};

void read_test_pattern(void __iomem *ctrl_base) {
    void __iomem *reg = ctrl_base;
    u32 val;
    int i;

    // In your code
    void __iomem *bypass_reg = ioremap(ISP_BYPASS_CTRL, 4);
    if (bypass_reg) {
        // Enable bypass
        writel(1, bypass_reg);
        pr_info("Writing bypass enable: read back 0x%x\n", readl(bypass_reg));
        iounmap(bypass_reg);
    }

    for (i = 0; i < 512; i++) {
        val = readl(reg + (i * 4));
        if (val == 0xf001f001) {
            // Skip past magic + config (8 green registers + status/config)
            void __iomem *frame_start = reg + ((i + 1 + 8 + 13) * 4); // Start after known config values

            // Try writing some non-zero values to what might be frame data
            pr_info("Writing test values to potential frame buffer:\n");
            for (int j = 0; j < 16; j++) {
                writel(0x80808080, frame_start + (j * 4)); // Try neutral gray
            }

            // Read back
            pr_info("Reading back test area:\n");
            for (int j = 0; j < 16; j++) {
                val = readl(frame_start + (j * 4));
                pr_info("  [%d]: 0x%08x\n", j, val);
            }
            return;
        }
    }
}

void dump_frame_buffers(void __iomem *ctrl_base) {
   u32 val;
   void __iomem *block;

   // Parse 1080p blocks (offset 30)
   block = ctrl_base + (30 * 4);
   val = readl(block);

   if (val == 0x07800438) {
       pr_info("\n1080p Configuration Block at offset 30:\n");
       pr_info("Resolution: 1920x1080 (0x%08x)\n", val);
       pr_info("Stride: 0x%08x\n", readl(block + 4));
       pr_info("Resolution2: 0x%08x\n", readl(block + (11 * 4))); // offset 41
       pr_info("Enable: 0x%08x\n", readl(block + (13 * 4))); // offset 43
   }

   // Parse 720p blocks (offset 94)
   block = ctrl_base + (94 * 4);
   val = readl(block);

   if (val == 0x050002d0) {
       pr_info("\n720p Configuration Block at offset 94:\n");
       pr_info("Resolution: 1280x720 (0x%08x)\n", val);
       pr_info("Stride: 0x%08x\n", readl(block + 4));
       pr_info("Resolution2: 0x%08x\n", readl(block + (11 * 4))); // offset 105
       pr_info("Enable: 0x%08x\n", readl(block + (13 * 4))); // offset 107
   }

   // Parse 480p blocks (offset 158)
   block = ctrl_base + (158 * 4);
   val = readl(block);

   if (val == 0x028001e0) {
       pr_info("\n480p Configuration Block at offset 158:\n");
       pr_info("Resolution: 640x480 (0x%08x)\n", val);
       pr_info("Stride: 0x%08x\n", readl(block + 4));
       pr_info("Resolution2: 0x%08x\n", readl(block + (11 * 4))); // offset 169
       pr_info("Enable: 0x%08x\n", readl(block + (13 * 4))); // offset 171
   }

   // Print control register block
   block = ctrl_base + (479 * 4);
   pr_info("\nControl Register Block at offset 479:\n");
   pr_info("Control Magic: 0x%08x\n", readl(block));
   pr_info("Green Registers:\n");
   for (int i = 0; i < 8; i++) {
       pr_info("  [%d]: 0x%08x\n", i, readl(block + ((i + 1) * 4)));
   }
   pr_info("Status: 0x%08x\n", readl(block + (9 * 4)));
   pr_info("Status2: 0x%08x\n", readl(block + (18 * 4)));
   pr_info("Status3: 0x%08x\n", readl(block + (19 * 4)));
   pr_info("Config1: 0x%08x\n", readl(block + (20 * 4)));
   pr_info("Config2: 0x%08x\n", readl(block + (21 * 4)));
}

// Current layout matches what we see in memory
struct ctrl_block {
    u32 tag;         // The 01010001 value
    u32 reserved[21];  // Space until first resolution block (0x07800438)

    // At offset 22: First 1080p block
    u32 res_1080p;   // 0x07800438
    u32 stride_1080p; // 0x02000200
    u32 pad1[8];     // Padding/reserved
    u32 res_1080p_2; // 0x07800438 again
    u32 pad2;
    u32 enable;      // 0x00000001

    // The whole pattern repeats several times for different resolutions
    // We see this at offsets 278, etc.

    // At offset 471: Control/status registers
    u32 control_magic;  // f001f001
    u32 green_regs[8];  // The 40404040 pattern
    u32 status;         // 0011ff00

    // Additional status/control values near the end
    u32 status2;        // 00001133
    u32 status3;        // 00000033
    u32 config1;        // 00500010
    u32 config2;        // 00500010
};

struct nv12_frame_info {
    u32 width;         // 1920 from 0x0780
    u32 height;        // 1080 from 0x0438
    u32 y_stride;      // From 0x0200
    u32 uv_stride;     // From 0x0200
    void __iomem *y_addr;
    void __iomem *uv_addr;
};

void dump_isp_regs(void) {
    void __iomem *isp_base = ioremap(0x13300000, 0x1000);
    int i;
    u32 val;

    if (!isp_base) {
        pr_err("Failed to map ISP registers\n");
        return;
    }

    pr_info("ISP Register Space Dump:\n");

    // Show all main interrupt and control registers
    pr_info("Core ISP Registers:\n");
    pr_info("  Base+0x00: 0x%08x (ID/Version)\n", readl(isp_base + 0x00));
    pr_info("  Base+0x0c: 0x%08x (Control)\n", readl(isp_base + 0x0c));
    pr_info("  Base+0xb0: 0x%08x (IRQ Enable)\n", readl(isp_base + 0xb0));
    pr_info("  Base+0xb4: 0x%08x (IRQ Status)\n", readl(isp_base + 0xb4));

    // Show VIC registers of interest
    pr_info("\nVIC Control/Status:\n");
    pr_info("  Base+0x110: 0x%08x (VIC Control)\n", readl(isp_base + 0x110));
    pr_info("  Base+0x114: 0x%08x (VIC Status)\n", readl(isp_base + 0x114));
    pr_info("  Base+0x840: 0x%08x (VIC Config)\n", readl(isp_base + 0x840));

    // Show complete dump of non-zero registers for reference
    pr_info("\nComplete Register Dump (non-zero values):\n");
    for (i = 0; i < 0x1000; i += 4) {
        val = readl(isp_base + i);
        if (val != 0) {
            pr_info("0x%08x: 0x%08x\n", 0x13300000 + i, val);
        }
    }

    iounmap(isp_base);
}

void print_ctrl_block(struct isp_channel *chn) {
    void __iomem *ctrl_base = ioremap(0x13309888, 512 * 4); // Map enough for all buffers
    u32 val;
    int i;

    if (!ctrl_base) {
        pr_err("Failed to map control block memory\n");
        return;
    }

    pr_info("Control Block Contents:\n");

    // Print magic number
    val = readl(ctrl_base);
    pr_info("Magic: 0x%x\n", val);

    // Print all non-null buffer values
    for (i = 0; i < 512; i++) {
        val = readl(ctrl_base + (i * 4));
        if (val != 0)
            pr_info("Buffer %d: %08x\n", i, val);
    }

    // Now try to find frame data
    //dump_frame_buffers(ctrl_base);
    //read_test_pattern(ctrl_base);

    iounmap(ctrl_base);
}

void test_active_regs(void) {
    void __iomem *isp_base = ioremap(0x13300000, 0x1000);

    if (!isp_base) {
        pr_err("Failed to map ISP registers\n");
        return;
    }

//    pr_info("Testing ISP_COLOR_CFG (0xd4):\n");
//    pr_info("Original: 0x%08x\n", readl(isp_base + 0xd4));
//    writel(0, isp_base + 0xd4);
//    pr_info("After write 0: 0x%08x\n", readl(isp_base + 0xd4));
//    writel(0x0c, isp_base + 0xd4);  // Restore original
//    pr_info("After restore: 0x%08x\n", readl(isp_base + 0xd4));
//
//    pr_info("\nTesting ISP_OUT_CTRL (0xf0):\n");
//    pr_info("Original: 0x%08x\n", readl(isp_base + 0xf0));
//    writel(0, isp_base + 0xf0);
//    pr_info("After write 0: 0x%08x\n", readl(isp_base + 0xf0));
//    writel(0xff808000, isp_base + 0xf0);  // Restore original
//    pr_info("After restore: 0x%08x\n", readl(isp_base + 0xf0));

//    pr_info("Testing test pattern registers:\n");
//    writel(TEST_PATTERN_ENABLE, isp_base + TEST_PATTERN_CTRL_REG);
//    writel(1920 | (1080 << 16), isp_base + TEST_PATTERN_SIZE_REG);
//    wmb();

    iounmap(isp_base);
}

void dump_interrupt_status(struct IMPISPDev *dev) {
    void __iomem *intc_base = ioremap(T31_INTC_BASE, 0x100);
    void __iomem *vic_regs = dev->reg_base + VIC_BASE;

    // INTC status
    pr_info("INTC Status:\n");
    pr_info("  ISR: 0x%08x\n", readl(intc_base + ISRn));
    pr_info("  IMR: 0x%08x\n", readl(intc_base + ICMRn));
    pr_info("  IPR: 0x%08x\n", readl(intc_base + IPMRn));  // Changed IPRn to IPMRn

    // VIC status
    pr_info("VIC Status:\n");
    pr_info("  Control: 0x%08x\n", readl(vic_regs + 0x0));
    pr_info("  Status: 0x%08x\n", readl(vic_regs + 0xb4));
    pr_info("  Status2: 0x%08x\n", readl(vic_regs + 0xb8));
    pr_info("  DMA Status: 0x%08x\n", readl(vic_regs + 0x84));
    pr_info("  IRQ Status: 0x%08x\n", readl(vic_regs + 0x93c));

    iounmap(intc_base);
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

        dump_interrupt_status(ourISPdev);

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


static void dump_frame_group(struct frame_group *group) {
    pr_info("Frame Group Details:\n"
            "  Name: %.20s\n"
            "  Handler fn: %p\n"
            "  Update fn: %p\n"
            "  Group update fn: %p\n"
            "  Self ptr: %p\n"
            "  Channel: %d\n"
            "  State: 0x%x\n"
            "  Handler value: 0x%x\n",
            group->name,
            group->handler_fn,
            group->update_fn,
            group->group_update,
            group->self,
            group->channel,
            group->state,
            group->handler);
}
static bool validate_kernel_pointer(void *ptr, const char *name) {
    // Check for NULL
    if (!ptr) {
        pr_err("%s is NULL\n", name);
        return false;
    }

    // Check if in kernel space (typically 0x7xxxxxxx range)
    unsigned long addr = (unsigned long)ptr;
    if ((addr & 0x70000000) != 0x70000000) {
        pr_err("%s address 0x%lx not in kernel range\n", name, addr);
        return false;
    }

    // Check pointer alignment
    if (addr & (sizeof(void *) - 1)) {
        pr_err("%s address 0x%lx not properly aligned\n", name, addr);
        return false;
    }

    return true;
}

static int dump_mem(const char *desc, void *addr, size_t len) {
    unsigned char *data = (unsigned char *)addr;

    pr_info("%s at %p:\n", desc, addr);
    for (size_t i = 0; i < len; i += 16) {
        pr_info("  %03zx: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
            i,
            data[i+0], data[i+1], data[i+2], data[i+3],
            data[i+4], data[i+5], data[i+6], data[i+7],
            data[i+8], data[i+9], data[i+10], data[i+11],
            data[i+12], data[i+13], data[i+14], data[i+15]);
    }
    return 0;
}

int register_channel_callbacks(struct isp_channel_event *evt,
                             void (*frame_cb)(void *),
                             void (*stats_cb)(void *),
                             void *priv)
{
    unsigned long flags;

    if (!evt)
        return -EINVAL;

    spin_lock_irqsave(&evt->lock, flags);

    evt->cb.frame_cb = frame_cb;
    evt->cb.stats_cb = stats_cb;
    evt->cb.priv = priv;

    spin_unlock_irqrestore(&evt->lock, flags);

    return 0;
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




static int framechan_reqbufs(struct isp_channel *chn, unsigned long arg)
{
    struct reqbuf_request req;
    struct frame_queue *queue;
    struct video_buffer **new_bufs = NULL;
    int ret = 0, i;
    unsigned long flags;

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

    // Clean up existing buffers if any
    if (queue->bufs) {
        for (i = 0; i < queue->buffer_count; i++) {
            if (queue->bufs[i]) {
                // Remove from any lists it might be on
                if (!list_empty(&queue->bufs[i]->list))
                    list_del(&queue->bufs[i]->list);

                // Free metadata and group if they exist
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

    // Allocate array of buffer pointers
    new_bufs = kzalloc(sizeof(struct video_buffer *) * req.count, GFP_KERNEL);
    if (!new_bufs) {
        ret = -ENOMEM;
        goto out_unlock;
    }

    // Calculate frame size
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

        // Initialize metadata
        buf->meta = kzalloc(sizeof(struct frame_metadata), GFP_KERNEL);
        if (!buf->meta) {
            kfree(buf);
            ret = -ENOMEM;
            goto err_free_bufs;
        }

        // Initialize buffer
        buf->index = i;
        buf->type = req.type;
        buf->memory = req.memory;
        buf->queue = queue;
        INIT_LIST_HEAD(&buf->list);

        // Setup metadata with required magic values
        buf->meta->magic = 0x100100;  // Required by libimp
        buf->meta->frame_num = i;
        buf->meta->state = FRAME_STATE_FREE;
        buf->meta->size = chn->required_size;

        // Create and initialize group module
        struct group_module *group = kzalloc(sizeof(*group), GFP_KERNEL);
        if (!group) {
            kfree(buf->meta);
            kfree(buf);
            ret = -ENOMEM;
            goto err_free_bufs;
        }

        // Setup group with required values
        group->update_fn = group_update_wrapper;
        group->self = group;             // Points to itself
        group->channel = chn->channel_id;
        group->handler = 0x9a654;        // Magic value required by libimp
        buf->meta->group_ptr = group;    // Link to buffer metadata

        // Store in array
        new_bufs[i] = buf;

        pr_info("Allocated buffer %d meta=%p group=%p\n",
                i, buf->meta, buf->meta->group_ptr);
    }

    // Update queue with new buffers
    queue->bufs = new_bufs;
    queue->buffer_count = req.count;
    queue->memory_type = req.memory;

    // Initialize lists
    INIT_LIST_HEAD(&queue->ready_list);
    INIT_LIST_HEAD(&queue->done_list);

    spin_unlock_irqrestore(&queue->queue_lock, flags);
    mutex_unlock(&queue->lock);

    // Initialize event system if not done
    if (!chn->event) {
        ret = init_channel_events(chn);
        if (ret)
            goto err_free_bufs;
    }

    // Send buffer request event through pad system
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

    return 0;

    err_cleanup_events:
        if (chn->event) {
            unregister_channel_callbacks(chn->event);
            kfree(chn->event);
            chn->event = NULL;
        }
    err_free_bufs:
    while (--i >= 0) {
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

static int handle_framechan_ioctl(struct IMPISPDev *dev, unsigned int cmd, void __user *arg, struct file *file)
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

static void unregister_subdev(struct platform_device *pdev)
{
    if (pdev)
        platform_device_unregister(pdev);
}

static struct resource tx_isp_vic_resources[] = {
    [0] = {
        .start  = ISP_BASE_ADDR + 0x7000,
        .end    = ISP_BASE_ADDR + 0x7000 + 0x1000 - 1,
        .flags  = IORESOURCE_MEM,
        .name   = "tx-isp-vic-regs",
    },
    [1] = {
        .start  = 38,  // VIC IRQ
        .end    = 38,
        .flags  = IORESOURCE_IRQ,
        .name   = "isp-w02",
    }
};

static int register_isp_subdevices(struct IMPISPDev *dev) {
    int ret;

    pr_info("Creating ISP subdevices...\n");

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

    // Finally set up default links
    ret = tx_isp_setup_default_links(dev);
    if (ret) {
        pr_err("Failed to setup default links: %d\n", ret);
        return ret;
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


int vic_queue_buffer(struct IMPISPDev *dev, dma_addr_t addr)
{
    // Update global counters
    if (vic_mdma_ch0_sub_get_num >= 5) {
        return -EBUSY;  // Buffer queue full
    }

    // Write DMA address
    writel(addr, dev->vic_regs + VIC_DMA_ADDR +
           (vic_mdma_ch0_set_buff_index << 2));

    vic_mdma_ch0_sub_get_num++;

    return 0;
}

// In implementation
static void vic_framedone_irq_function(struct IMPISPDev *dev)
{
    pr_info("VIC frame done interrupt\n");
    // Handle whatever needs to happen on frame done
}

static void vic_dma_irq_function(struct IMPISPDev *dev, int channel)
{
    pr_info("VIC DMA done interrupt\n");
    // Handle DMA completion for given channel
}

// Add to our header file
#define VIC_IRQ_INDEX 0xd

// Define our vic interrupt handling structure to match OEM
struct vic_irq_status {
    uint32_t irq_status;     // At 0x1e0 - raw status
    uint32_t irq_mask;       // At 0x1e4 - mask
    uint32_t irq_status2;    // At 0x1e8 - status after mask
    uint32_t irq_mask2;      // At 0x1ec - mask for second group
    uint32_t irq_status_out; // At 0x1f0 - final masked status
    uint32_t irq_status2_out; // At 0x1f4 - final masked status 2
};


static void tx_vic_irq_cleanup(struct IMPISPDev *dev)
{
    if (dev->vic_dev) {
        dev->vic_dev->irq_enabled = 0;
        dev->vic_dev->irq_handler = NULL;
        dev->vic_dev->irq_disable = NULL;
        dev->vic_dev->irq_priv = NULL;
    }
}


/* Helper to initialize VIC IRQ handling */
int tx_vic_irq_init(struct vic_device *vic_dev,
                    void (*handler)(void *),
                    void (*disable)(void *),
                    void *priv)
{
    void __iomem *vic_regs;

    if (!vic_dev || !ourISPdev || !ourISPdev->reg_base)
        return -EINVAL;

    vic_regs = ourISPdev->reg_base + VIC_BASE;

    // Initialize handler data
    spin_lock_init(&vic_dev->lock);
    vic_dev->irq_enabled = 0;
    vic_dev->irq_handler = handler;
    vic_dev->irq_disable = disable;
    vic_dev->irq_priv = priv;

    // Clear any pending interrupts
    writel(0xFFFFFFFF, vic_regs + 0xb4);
    wmb();

    // Enable VIC interrupts
    writel(0x000033fb, vic_regs + 0x93c);

    // Unmask VIC interrupts
    writel(0x0, vic_regs + 0x9e8);  // Clear mask to enable all

    // Clear any pending
    writel(0xFFFFFFFF, vic_regs + 0xb4);
    wmb();


    // Register handler in system array
    system_irq_func_set(VIC_IRQ_INDEX, tx_vic_irq_handler);

    pr_info("VIC IRQ initialized: enable=0x%08x\n", readl(vic_regs + 0x93c));

    dump_intc_state();

    return 0;
}

int tx_isp_vic_probe(struct platform_device *pdev)
{
    struct IMPISPDev *dev = ourISPdev;
    struct vic_device *vic_dev;
    struct tx_isp_irq_handler *irq_handler;
    struct tx_isp_subdev *sd = NULL;
    int ret;
    void __iomem *vic_base;

    if (!dev || !dev->reg_base) {
        pr_err("Invalid device state in VIC probe\n");
        return -EINVAL;
    }

    vic_dev = devm_kzalloc(&pdev->dev, sizeof(*vic_dev), GFP_KERNEL);
    if (!vic_dev)
        return -ENOMEM;

    vic_dev->state = TX_ISP_MODULE_INIT;
    vic_dev->started = false;
    vic_dev->processing = false;
    mutex_init(&vic_dev->state_lock);
    vic_dev->regs = dev->reg_base + VIC_BASE;
    spin_lock_init(&vic_dev->lock);
    init_completion(&vic_dev->frame_complete);
    dev->vic_dev = vic_dev;

    // Initialize VIC hardware base state
    vic_base = dev->reg_base + VIC_BASE;

    // Disable all interrupts first
    writel(0, vic_base + 0x93c);
    writel(0xffffffff, vic_base + 0xb4);  // Clear status
    writel(0xffffffff, vic_base + 0xb8);  // Clear status2
    wmb();

    // Set initial configuration state
    writel(0x00060003, vic_base + 0x0);   // Enable core
    writel(0x00020000, vic_base + 0x4);   // Base config
    writel(0x00c80000, vic_base + 0x8);   // Extended config
    wmb();

    sd = &vic_dev->sd;
    ret = tx_isp_subdev_init(pdev, sd, &vic_ops);
    if (ret) {
        dev_err(&pdev->dev, "Failed to init VIC subdev\n");
        goto err_free_dev;
    }

    tx_isp_set_subdevdata(sd, vic_dev);

    irq_handler = devm_kzalloc(&pdev->dev, sizeof(*irq_handler), GFP_KERNEL);
    if (!irq_handler)
        return -ENOMEM;

    irq_handler->dev_id = vic_dev;
    ret = tx_vic_request_irq(pdev, irq_handler);
    if (ret)
        return ret;

    vic_dev->irq_handler = irq_handler;

    pr_info("VIC probe completed successfully\n");
    return 0;

err_free_dev:
    devm_kfree(&pdev->dev, vic_dev);
    dev->vic_dev = NULL;
    return ret;
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




static long tisp_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    void __user *argp = (void __user *)arg;
    int ret = 0;

    pr_info("TISP IOCTL called: cmd=0x%x\n", cmd);

    switch (cmd) {
//        case 0x2000000: // Register sensor
//            ret = subdev_sensor_ops_register_sensor(file, argp);
//        break;
//
//        case 0x2000001: // Release sensor
//            ret = subdev_sensor_ops_release_sensor(file, argp);
//        break;
//
//        case 0x2000002: // Enum input
//            ret = subdev_sensor_ops_enum_input(file, argp);
//        break;
//
//        case 0x2000003: // Get input
//            // TODO
//                break;
//
//        case 0x2000004: // Set input
//            ret = subdev_sensor_ops_set_input(file, argp);
//        break;

        // TODO: Add other sensor ops (0x2000005-0x2000012)

        default:
            pr_info("Unhandled TISP ioctl cmd: 0x%x\n", cmd);
        return -EINVAL;
    }

    return ret;
}

static int tisp_open(struct file *file)
{
    pr_info("TISP device opened\n");
    return 0;
}

static int tisp_release(struct file *file)
{
    pr_info("TISP device closed\n");
    return 0;
}


/* Character device operations for /dev/tisp */
static const struct file_operations tisp_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = tisp_ioctl,
    .open = tisp_open,
    .release = tisp_release,
};


static int tx_isp_core_probe(struct platform_device *pdev)
{
    struct IMPISPDev *dev = ourISPdev;
    struct ddr_device *ddr_dev;
    struct tx_isp_subdev *sd = NULL;
    int ret;

    pr_info("Core probe called\n");

    if (!dev || !dev->reg_base) {
        dev_err(&pdev->dev, "No ISP device data\n");
        return -EINVAL;
    }

    // Allocate DDR device
    ddr_dev = devm_kzalloc(&pdev->dev, sizeof(*ddr_dev), GFP_KERNEL);
    if (!ddr_dev) {
        dev_err(&pdev->dev, "Failed to allocate DDR device\n");
        return -ENOMEM;
    }

    // Allocate subdev
    sd = devm_kzalloc(&pdev->dev, sizeof(*sd), GFP_KERNEL);
    if (!sd) {
        dev_err(&pdev->dev, "Failed to allocate subdev\n");
        ret = -ENOMEM;
        goto err_free_ddr;
    }
    ddr_dev->sd = sd;

    // Set up subdev
    ret = tx_isp_subdev_init(pdev, sd, &ddr_ops);
    if (ret) {
        dev_err(&pdev->dev, "Failed to init DDR subdev\n");
        goto err_free_sd;
    }

    // Store in global
    dev->ddr_dev = ddr_dev;

    // Set up remaining properties
    tx_isp_set_subdev_hostdata(sd, ddr_dev);
    platform_set_drvdata(pdev, &sd->module);

    // Initialize core state
    dev->core.state = 1;
    mutex_init(&dev->core.lock);

    // Initialize basic core hardware
    ret = isp_hw_init(dev);
    if (ret) {
        pr_err("Failed to initialize core hardware: %d\n", ret);
        goto err_cleanup_subdev;
    }

    ddr_dev->state = TX_ISP_MODULE_SLAKE;

    // Initialize /dev/tisp character device
    dev_t tisp_dev_number;
    ret = alloc_chrdev_region(&tisp_dev_number, 0, 1, "tisp");
    if (ret)
        goto err_free_dev;

    cdev_init(&ourISPdev->tisp_cdev, &tisp_fops);
    ourISPdev->tisp_cdev.owner = THIS_MODULE;

    ret = cdev_add(&ourISPdev->tisp_cdev, tisp_dev_number, 1);
    if (ret)
        goto err_unregister_tisp;

    // Create tisp device node
    ourISPdev->tisp_device = device_create(tisp_class, NULL, tisp_dev_number, NULL, "tisp");
    if (IS_ERR(ourISPdev->tisp_device)) {
        ret = PTR_ERR(ourISPdev->tisp_device);
        goto err_del_tisp_cdev;
    }

    struct tx_isp_chip_ident *chip = kzalloc(sizeof(*chip), GFP_KERNEL);
    if (!chip) {
        pr_err("Failed to allocate chip ident\n");
        return -ENOMEM;
    }
    ourISPdev->chip = chip;

    pr_info("Core probe complete: dev=%p reg_base=%p\n",
            dev, dev->reg_base);
    return 0;

    err_del_tisp_cdev:
        cdev_del(&ourISPdev->tisp_cdev);
    err_unregister_tisp:
        unregister_chrdev_region(tisp_dev_number, 1);
    err_free_dev:
        devm_kfree(&pdev->dev, ourISPdev);
    err_cleanup_subdev:
        // Add subdev cleanup
        tx_isp_subdev_deinit(sd);
    err_free_sd:
        devm_kfree(&pdev->dev, sd);
    err_free_ddr:
        devm_kfree(&pdev->dev, ddr_dev);
    dev->ddr_dev = NULL;
    return ret;
}

static int tx_isp_core_remove(struct platform_device *pdev)
{
    pr_info("Core remove called\n");
    return 0;
}


static int tx_isp_init_memory(struct IMPISPDev *dev, struct platform_device *pdev)
{
    struct resource *res;
    dma_addr_t aligned_base;
    size_t aligned_size, frame_size;
    int ret = 0;

    /* 1. Map ISP registers */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res) {
        dev_err(&pdev->dev, "No ISP memory resource found\n");
        return -ENODEV;
    }

    dev->resources.mmio_phys = res->start;
    dev->resources.mmio_res = devm_request_mem_region(&pdev->dev,
                                                     res->start,
                                                     resource_size(res),
                                                     "tx-isp-regs");
    if (!dev->resources.mmio_res) {
        dev_err(&pdev->dev, "Failed to request ISP memory region\n");
        return -EBUSY;
    }

    dev->reg_base = devm_ioremap(&pdev->dev, res->start, resource_size(res));
    if (!dev->reg_base) {
        dev_err(&pdev->dev, "Failed to map ISP registers\n");
        return -ENOMEM;
    }

    /* 2. Map CSI registers */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
    if (!res) {
        dev_err(&pdev->dev, "No CSI memory resource found\n");
        return -ENODEV;
    }

    dev->resources.csi_phys = res->start;
    dev->resources.csi_regs = devm_ioremap(&pdev->dev, res->start, resource_size(res));
    if (!dev->resources.csi_regs) {
        dev_err(&pdev->dev, "Failed to map CSI registers\n");
        return -ENOMEM;
    }

    /* 3. Set up DMA/Frame Buffer memory */
    frame_size = ALIGN(1920 * 1080 * 3/2, PAGE_SIZE);
    aligned_base = ALIGN(0x02a80000, 32);  // Start of DMA region
    aligned_size = ALIGN(frame_size * 4, PAGE_SIZE);  // 4 frames

    dev->rmem_addr = aligned_base;
    dev->rmem_size = aligned_size;
    dev->dma_addr = aligned_base;
    dev->dma_size = aligned_size;  // Added this line to store DMA buffer size

    dev->dma_buf = devm_ioremap(&pdev->dev, aligned_base, aligned_size);
    if (!dev->dma_buf) {
        dev_err(&pdev->dev, "Failed to map DMA buffer\n");
        return -ENOMEM;
    }

    /* Clear the frame buffer area */
    memset(dev->dma_buf, 0, aligned_size);

    /* Log all memory mappings */
    dev_info(&pdev->dev, "ISP Memory Initialization Complete:\n"
             "  ISP Registers: phys=0x%08x virt=%p\n"
             "  CSI Registers: phys=0x%08x virt=%p\n"
             "  DMA Buffer: phys=0x%08x virt=%p size=%zu (for %zu frames)\n",
             dev->resources.mmio_phys, dev->reg_base,
             dev->resources.csi_phys, dev->resources.csi_regs,
             (uint32_t)dev->rmem_addr, dev->dma_buf,
             dev->rmem_size, aligned_size / frame_size);

    return 0;
}


static int init_tuning_data(struct IMPISPDev *dev)
{
    struct isp_tuning_data *tuning;

    tuning = kzalloc(sizeof(*tuning), GFP_KERNEL);
    if (!tuning)
        return -ENOMEM;

    mutex_init(&tuning->lock);

    // Initialize defaults
    tuning->regs = NULL;
    tuning->state = 0;
    tuning->brightness = 128;
    tuning->contrast = 128;
    tuning->saturation = 128;
    tuning->sharpness = 128;
    tuning->ae_comp = 0;
    tuning->max_again = 0;
    tuning->max_dgain = 0;
    tuning->move_state = 0;
    tuning->defog_strength = 0;
    tuning->dpc_strength = 0;
    tuning->drc_strength = 0;
    tuning->temper_strength = 0;
    tuning->sinter_strength = 0;

    // Initialize new members
    dev->day_night = 0;
    dev->custom_mode = 0;
    dev->poll_state = 0;
    init_waitqueue_head(&dev->poll_wait);

    atomic_set(&tuning->initialized, 1);
    tuning->g_isp_deamon_info = 0;

    dev->tuning_data = tuning;
    pr_info("ISP M0 device opened, tuning_data=%p\n", tuning);

    return 0;
}

static void cleanup_tuning_data(struct IMPISPDev *dev)
{
    if (dev && dev->tuning_data) {
        // Clean up any resources held by new members if needed
        kfree(dev->tuning_data);
        dev->tuning_data = NULL;
    }
}


static int verify_isp_clocks(struct IMPISPDev *dev) {
    void __iomem *cpm_base = ioremap(0x10000000, 0x1000);
    u32 clkgr, clkgr1;

    if (!cpm_base)
        return -ENOMEM;

    clkgr = readl(cpm_base + 0x20);  // CLKGR offset
    clkgr1 = readl(cpm_base + 0x28); // CLKGR1 offset

    pr_info("Clock Gate Registers:\n");
    pr_info("CLKGR: 0x%08x\n", clkgr);
    pr_info("CLKGR1: 0x%08x\n", clkgr1);

    // Clear IPU/ISP/VIC gate bits if set
    if (clkgr & BIT(14)) {
        pr_info("Enabling IPU clock\n");
        clkgr &= ~BIT(14);
        writel(clkgr, cpm_base + 0x20);
    }

    if (clkgr & BIT(37)) {
        pr_info("Enabling ISP clock\n");
        clkgr &= ~BIT(37);
        writel(clkgr, cpm_base + 0x20);
    }

    if (clkgr & BIT(38)) {
        pr_info("Enabling VIC clock\n");
        clkgr &= ~BIT(38);
        writel(clkgr, cpm_base + 0x20);
    }

    // Verify clocks are enabled
    clkgr = readl(cpm_base + 0x20);
    pr_info("CLKGR after enable: 0x%08x\n", clkgr);

    iounmap(cpm_base);
    return 0;
}


static int tx_isp_probe(struct platform_device *pdev)
{
    struct IMPISPDev *dev = ourISPdev;
    int ret, isp_irq;
    struct tx_isp_irq_handler *irq_handler;
    struct resource *res;

    pr_info("ISP probe called\n");

    dev->dev = &pdev->dev;
    dev->pdev = pdev;
    dev->width = 1920;
    dev->height = 1080;
    dev->buffer_count = ISP_BUF_COUNT;
    // Initialize tuning state
    init_tuning_data(dev);

    // Get memory resource from platform device
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res) {
        dev_err(&pdev->dev, "No memory resource\n");
        return -ENODEV;
    }

    // Single memory initialization call that handles everything
    ret = tx_isp_init_memory(dev, pdev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to initialize ISP memory: %d\n", ret);
        goto err_free_dev;
    }

    /* Initialize hardware resources */
    pr_info("Starting hw resources init\n");
    writel(0, dev->reg_base + ISP_INT_MASK_REG);
    writel(0xFFFFFFFF, dev->reg_base + ISP_INT_CLEAR_REG);
    wmb();

    /* Allocate DDR device */
    dev->ddr_dev = kzalloc(sizeof(struct ddr_device), GFP_KERNEL);
    if (!dev->ddr_dev) {
        pr_err("Failed to allocate DDR device\n");
        ret = -ENOMEM;
        goto err_cleanup_memory;
    }

    /* Configure clocks */
    pr_info("Starting clocks init\n");
    ret = configure_isp_clocks(dev);
    if (ret)
        goto err_cleanup_memory;
    verify_isp_clocks(dev);


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

    /* Setup I2C and sensor */
    pr_info("Starting I2C init\n");
    ret = setup_i2c_adapter(dev);
    if (ret)
        goto err_cleanup_memory;

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


    // Initialize ISP IRQ handling
    pr_info("Starting ISP IRQ init\n");
    ret = tx_isp_irq_init(dev, tx_isp_irq_handler, NULL, dev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to init ISP IRQ: %d\n", ret);
        return ret;
    }

    irq_handler = devm_kzalloc(&pdev->dev, sizeof(*irq_handler), GFP_KERNEL);
    if (!irq_handler)
        return -ENOMEM;

    irq_handler->dev_id = pdev; // Pass vic_dev as context

    ret = tx_isp_request_irq(pdev, irq_handler);
    if (ret)
        return ret;

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
err_unmap_regs:
    devm_iounmap(&pdev->dev, dev->reg_base);
err_free_gpio:
//    devm_gpio_free(&pdev->dev, dev->reset_gpio);
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

    cleanup_tuning_data(dev);

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
    if (dev->isp_irq> 0) {
        dev->isp_irq= 0;
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

struct tx_isp_subdev_pad *find_pad(struct IMPISPDev *dev,
                                  enum imp_isp_mod_type mod,
                                  u32 type,  // 0=sink, 1=source
                                  u32 pad_id)
{
    struct tx_isp_subdev *sd;

    pr_info("find_pad: looking for mod=%d type=%d pad=%d\n", mod, type, pad_id);

    // Get appropriate subdev based on module type
    switch (mod) {
    case IMP_ISP_MOD_CSI:
        if (!dev->csi_dev) {
            pr_err("find_pad: CSI device not present\n");
            return NULL;
        }
        if (!dev->csi_dev->sd) {
            pr_err("find_pad: CSI subdev not initialized\n");
            return NULL;
        }
        sd = dev->csi_dev->sd;
        break;

    case IMP_ISP_MOD_VIC:
        if (!dev->vic_dev) {
            pr_err("find_pad: VIC device not present\n");
            return NULL;
        }
        if (!dev->vic_dev->sd) {
            pr_err("find_pad: VIC subdev not initialized\n");
            return NULL;
        }
        sd = dev->vic_dev->sd;
        break;

    case IMP_ISP_MOD_VIN:
        if (!dev->vin_dev) {
            pr_err("find_pad: VIN device not present\n");
            return NULL;
        }
        if (!dev->vin_dev->sd) {
            pr_err("find_pad: VIN subdev not initialized\n");
            return NULL;
        }
        sd = dev->vin_dev->sd;
        break;

    case IMP_ISP_MOD_CORE:
    case IMP_ISP_MOD_DDR:
        if (!dev->ddr_dev) {
            pr_err("find_pad: DDR/Core device not present\n");
            return NULL;
        }
        if (!dev->ddr_dev->sd) {
            pr_err("find_pad: DDR/Core subdev not initialized\n");
            return NULL;
        }
        sd = dev->ddr_dev->sd;
        break;

    case IMP_ISP_MOD_FS:
        if (!dev->fs_dev) {
            pr_err("find_pad: Frame source device not present\n");
            return NULL;
        }
        if (!dev->fs_dev->sd) {
            pr_err("find_pad: Frame source subdev not initialized\n");
            return NULL;
        }
        sd = dev->fs_dev->sd;
        break;

    default:
        pr_err("find_pad: Invalid module type %d\n", mod);
        return NULL;
    }

    pr_info("find_pad: Found subdev with %d in/%d out pads\n",
             sd->num_inpads, sd->num_outpads);

    // Get appropriate pad array and check index
    if (type == TX_ISP_PADTYPE_INPUT) {
        if (!sd->inpads) {
            pr_err("find_pad: Input pads array not allocated\n");
            return NULL;
        }
        if (pad_id >= sd->num_inpads) {
            pr_err("find_pad: Invalid input pad id %d (max %d)\n",
                  pad_id, sd->num_inpads - 1);
            return NULL;
        }
        pr_info("find_pad: Returning input pad %d\n", pad_id);
        return &sd->inpads[pad_id];
    } else {
        if (!sd->outpads) {
            pr_err("find_pad: Output pads array not allocated\n");
            return NULL;
        }
        if (pad_id >= sd->num_outpads) {
            pr_err("find_pad: Invalid output pad id %d (max %d)\n",
                  pad_id, sd->num_outpads - 1);
            return NULL;
        }
        pr_info("find_pad: Returning output pad %d\n", pad_id);
        return &sd->outpads[pad_id];
    }
}

// Helper functions to replace macros
static inline u32 csi_reg_read(void __iomem *base, unsigned int reg)
{
    return __raw_readl((unsigned int *)(base + reg));
}

static inline void csi_reg_write(void __iomem *base, unsigned int reg, u32 value)
{
    __raw_writel(value, (unsigned int *)(base + reg));
}

static inline u32 csi_core_reg_read(struct tx_isp_csi_device *csi, unsigned int reg)
{
    return csi_reg_read(csi->sd.base, reg);
}

static inline void csi_core_reg_write(struct tx_isp_csi_device *csi, unsigned int reg, u32 value)
{
    csi_reg_write(csi->sd.base, reg, value);
}

static inline u32 csi_phy_reg_read(struct tx_isp_csi_device *csi, unsigned int reg)
{
    return csi_reg_read(csi->phy_base, reg);
}

static inline void csi_phy_reg_write(struct tx_isp_csi_device *csi, unsigned int reg, u32 value)
{
    csi_reg_write(csi->phy_base, reg, value);
}

static inline struct tx_isp_csi_device *get_csi_from_sd(struct tx_isp_subdev *sd)
{
    return container_of(sd, struct tx_isp_csi_device, sd);
}

static int isp_csi_show(struct seq_file *m, void *v)
{
    int len = 0;
    struct tx_isp_module *module = (void *)(m->private);
    struct tx_isp_subdev *sd;
    struct tx_isp_csi_device *csd;
    u32 err1, err2, phy_state;

    if (IS_ERR_OR_NULL(module)) {
        return 0;
    }

    sd = module_to_subdev(module);
    if (IS_ERR_OR_NULL(sd)) {
        return 0;
    }

    csd = tx_isp_get_subdevdata(sd);
    if (IS_ERR_OR_NULL(csd)) {
        ISP_ERROR("The parameter is invalid!\n");
        return 0;
    }

    err1 = csi_core_reg_read(csd, ERR1);
    err2 = csi_core_reg_read(csd, ERR2);

    if (err1 != 0) {
        len += seq_printf(m, "0x0020 is  0x%x\n", err1);
    }

    if (err2 != 0) {
        len += seq_printf(m, "0x0024 is  0x%x\n", err2);
    }

    if ((err1 != 0) || (err2 != 0)) {
        phy_state = csi_core_reg_read(csd, PHY_STATE);
        len += seq_printf(m, "0x0014 is  0x%x\n", phy_state);
    }

    return len;
}

static int dump_isp_csi_open(struct inode *inode, struct file *file)
{
    return single_open_size(file, isp_csi_show, PDE_DATA(inode), 1024);
}


static struct file_operations isp_csi_fops ={
    .read = seq_read,
    .open = dump_isp_csi_open,
    .llseek = seq_lseek,
    .release = single_release,
};


/* CSI initialization */
int tx_isp_csi_probe(struct platform_device *pdev)
{
    struct IMPISPDev *dev = ourISPdev;
    struct csi_device *csi_dev;
    int ret;

    pr_info("CSI probe starting\n");

    if (!dev || !dev->reg_base) {
        dev_err(&pdev->dev, "No ISP device data\n");
        return -EINVAL;
    }

    // 1. Allocate CSI device Full early initialization
    pr_info("Allocating CSI device\n");
    ret = tx_isp_csi_init(pdev);
    if (ret) {
        pr_err("Failed to initialize CSI: %d\n", ret);
        return ret;
    }

    csi_dev = dev->csi_dev;

    // Get subdev and initialize
    pr_info("Initializing CSI subdev\n");
    ret = tx_isp_subdev_init(pdev, csi_dev->sd, &csi_ops);
    if (ret) {
        dev_err(&pdev->dev, "Failed to init CSI subdev\n");
        goto err_free_dev;
    }

    // Enable clock
    pr_info("Enabling CSI clock\n");
    ret = clk_prepare_enable(csi_dev->clk);
    if (ret) {
        dev_err(&pdev->dev, "Failed to enable CSI clock\n");
        goto err_disable_clk;
    }

    // Initialize CSI and PHY - add this block
    pr_info("Initializing CSI PHY\n");
    ret = init_csi_phy(csi_dev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to initialize CSI PHY: %d\n", ret);
        goto err_disable_clk;
    }

    // finish initialization
    tx_isp_set_subdevdata(csi_dev->sd, csi_dev);
    tx_isp_set_subdev_debugops(csi_dev->sd, &isp_csi_fops);
    platform_set_drvdata(pdev, &csi_dev->sd->module);

    csi_dev->state = TX_ISP_MODULE_SLAKE;
    atomic_set(&dev->csi_configured, 0);

    dump_csi_registers(csi_dev);

    pr_info("CSI probe completed successfully\n");
    return 0;

err_disable_clk:
    clk_disable_unprepare(csi_dev->clk);
err_free_dev:
    dev->csi_dev = NULL;
    devm_kfree(&pdev->dev, csi_dev);
    return ret;
}


/* CSI cleanup */
int tx_isp_csi_remove(struct platform_device *pdev)
{
    struct module_info *module = platform_get_drvdata(pdev);
    struct tx_isp_subdev *sd = module_to_subdev(module);
    struct csi_device *csi = tx_isp_get_subdevdata(sd);

    /* Stop streaming if active */
    if (csi->state == TX_ISP_MODULE_RUNNING)
        csi_s_stream(sd, 0);

    /* Disable clock */
    clk_disable_unprepare(csi->clk);

    tx_isp_subdev_deinit(sd);

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
        .start  = CSI_BASE_ADDR,  // 0x10022000
        .end    = CSI_BASE_ADDR + CSI_REG_SIZE - 1,
        .flags  = IORESOURCE_MEM,
        .name   = "csi-regs",
    },
    [2] = {
        .start  = 37,  // ISP IRQ number
        .end    = 37,
        .flags  = IORESOURCE_IRQ,
        .name   = "isp-m0",
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
    irq_data->irq_number = ourISPdev->isp_irq;
    irq_data->handler_function = tx_isp_irq_handler;
    irq_data->disable_function = tx_isp_disable_irq;
    ourISPdev->isp_irq_data = irq_data;

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

int tx_isp_subdev_init_clks(struct tx_isp_subdev *sd, struct platform_device *pdev)
{
    struct IMPISPDev *isp_dev = platform_get_drvdata(pdev);
    struct clk *tmp_clk = NULL;
    int ret = 0;

    pr_info("Initializing clocks for %s\n", pdev->name);

    /* For CSI subdev */
    if (strcmp(pdev->name, "tx-isp-csi") == 0) {
        sd->clk_num = 1;
        sd->clks = kzalloc(sd->clk_num * sizeof(struct clk *), GFP_KERNEL);
        if (!sd->clks)
            return -ENOMEM;

        sd->clks[0] = clk_get(NULL, "csi");
        if (IS_ERR(sd->clks[0])) {
            ret = PTR_ERR(sd->clks[0]);
            pr_err("Failed to get CSI clock: %d\n", ret);
            goto err_free_clks;
        }
        sd->clk_num = 1;
        pr_info("CSI clock initialized: rate=%lu Hz\n", clk_get_rate(sd->clks[0]));
    }
    /* For VIC subdev */
    else if (strcmp(pdev->name, "tx-isp-vic") == 0) {
        sd->clk_num = 1;
        sd->clks = kzalloc(sd->clk_num * sizeof(struct clk *), GFP_KERNEL);
        if (!sd->clks)
            return -ENOMEM;

        // Get the ISP clock directly
        sd->clks[0] = clk_get(NULL, "isp");
        if (IS_ERR(sd->clks[0])) {
            ret = PTR_ERR(sd->clks[0]);
            pr_err("Failed to get ISP clock for VIC: %d\n", ret);
            goto err_free_clks;
        }
        sd->clk_num = 1;
        pr_info("VIC clock initialized with ISP clock: rate=%lu Hz\n",
                clk_get_rate(sd->clks[0]));
    }
    /* For sc2336 sensor */
    else if (strcmp(pdev->name, "sc2336") == 0) {
        tmp_clk = clk_get(NULL, "cgu_cim");  // Try camera interface module clock
        if (IS_ERR(tmp_clk)) {
            ret = PTR_ERR(tmp_clk);
            pr_err("Failed to get CIM clock for sensor: %d\n", ret);
            return ret;
        }

        sd->clks = kzalloc(sizeof(struct clk *), GFP_KERNEL);
        if (!sd->clks) {
            clk_put(tmp_clk);
            return -ENOMEM;
        }

        sd->clks[0] = tmp_clk;
        sd->clk_num = 1;
        pr_info("Sensor clock initialized: rate=%lu Hz\n", clk_get_rate(sd->clks[0]));
    }
    /* For other subdevs - keep working code */
    else if (strcmp(pdev->name, "tx-isp-vin") == 0 ||
             strcmp(pdev->name, "tx-isp-core") == 0 ||
             strcmp(pdev->name, "tx-isp-fs") == 0) {
        sd->clk_num = 1;
        sd->clks = kzalloc(sd->clk_num * sizeof(struct clk *), GFP_KERNEL);
        if (!sd->clks)
            return -ENOMEM;

        sd->clks[0] = clk_get(&pdev->dev, "isp");
        if (IS_ERR(sd->clks[0])) {
            ret = PTR_ERR(sd->clks[0]);
            pr_err("Failed to get ISP clock for %s\n", pdev->name);
            goto err_free_clks;
        }

        sd->clk_num = 1;
        pr_info("ISP clock initialized: rate=%lu Hz\n", clk_get_rate(sd->clks[0]));
    }

    return 0;

err_free_clks:
    kfree(sd->clks);
    sd->clks = NULL;
    sd->clk_num = 0;
    return ret;
}

void tx_isp_subdev_release_clks(struct tx_isp_subdev *sd)
{
    int i;

    if (!sd->clks || !sd->clk_num)
        return;

    for (i = 0; i < sd->clk_num; i++) {
        if (!IS_ERR_OR_NULL(sd->clks[i])) {
            clk_disable_unprepare(sd->clks[i]);
            clk_put(sd->clks[i]);
        }
    }
    kfree(sd->clks);
    sd->clks = NULL;
    sd->clk_num = 0;
}

// Add pad channel setup helper
static int setup_pad_channel(struct tx_isp_subdev_pad *pad, struct IMPISPDev *dev)
{
    struct isp_channel *chn;

    // Allocate channel if not exists
    if (!pad->priv) {
        chn = kzalloc(sizeof(*chn), GFP_KERNEL);
        if (!chn) {
            pr_err("Failed to allocate channel\n");
            return -ENOMEM;
        }

        // Basic channel setup
        chn->channel_id = pad->index;
        chn->dev = dev;
        // Copy the pad structure rather than storing pointer
        memcpy(&chn->pad, pad, sizeof(struct tx_isp_subdev_pad));

        // Store channel in pad private data
        pad->priv = chn;
        pr_info("Created channel %d for pad %d\n", chn->channel_id, pad->index);
    }

    return 0;
}

static void cleanup_pad_channel(struct tx_isp_subdev_pad *pad)
{
    struct isp_channel *chn = pad->priv;
    if (chn) {
        kfree(chn);
        pad->priv = NULL;
    }
}

int tx_isp_subdev_init(struct platform_device *pdev, struct tx_isp_subdev *sd,
                      struct tx_isp_subdev_ops *ops)
{
    struct IMPISPDev *dev = ourISPdev;
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

    /* Initialize clocks if needed */
    pr_info("Initializing clocks if needed\n");
    ret = tx_isp_subdev_init_clks(sd, pdev);
    if (ret) {
        pr_err("Failed to initialize clocks\n");
        return ret;
    }
    check_subdev_clocks(sd, pdev->name);

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
            ret = -ENOMEM;
            goto err_free_clks;
        }

        for (int i = 0; i < sd->num_inpads; i++) {
            sd->inpads[i].sd = sd;
            sd->inpads[i].index = i;
            sd->inpads[i].type = TX_ISP_PADTYPE_INPUT;
            sd->inpads[i].state = TX_ISP_PADSTATE_FREE;
            sd->inpads[i].link.flag = TX_ISP_LINKFLAG_DYNAMIC;

            // Create channel data for each pad
            struct isp_channel *chn = kzalloc(sizeof(*chn), GFP_KERNEL);
            if (!chn) {
                pr_err("Failed to allocate channel for pad %d\n", i);
                ret = -ENOMEM;
                goto err_free_out_channels;
            }

            // Initialize channel
            chn->channel_id = i;
            chn->state = 0;
            chn->dev = dev->dev;
            memcpy(&chn->pad, &sd->inpads[i], sizeof(struct tx_isp_subdev_pad));

            // Store channel as pad's private data
            sd->inpads[i].priv = chn;
            pr_info("Created channel data for input pad %d\n", i);

            init_pad_event_handling(&sd->inpads[i], tx_isp_event_handler);

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
            sd->outpads[i].sd = sd;
            sd->outpads[i].index = i;
            sd->outpads[i].type = TX_ISP_PADTYPE_OUTPUT;
            sd->outpads[i].state = TX_ISP_PADSTATE_FREE;
            sd->outpads[i].link.flag = TX_ISP_LINKFLAG_DYNAMIC;

            // Setup channel and event handling for output pads too
            ret = setup_pad_channel(&sd->outpads[i], dev);
            if (ret) {
                pr_err("Failed to setup channel for output pad %d\n", i);
                goto err_free_out_channels;
            }

            init_pad_event_handling(&sd->outpads[i], tx_isp_event_handler);

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
    if (dev->reg_base) {
        if (!strcmp(pdev->name, "tx-isp-vic")) {
            sd->base = dev->reg_base + 0x7000;
        } else if (!strcmp(pdev->name, "tx-isp-csi")) {
            sd->base = dev->resources.csi_regs;  // Use the already mapped CSI regs
        } else {
            sd->base = dev->reg_base;
        }
        pr_info("Using register base: %p\n", sd->base);
    }

    /* Store the subdev in the appropriate device structure */
    pr_info("Storing subdev in device\n");
    if (!strcmp(pdev->name, "tx-isp-vic")) {
        if (!dev->vic_dev) {
            pr_err("VIC device not initialized\n");
            ret = -EINVAL;
            goto err_free_out_channels;
        }
        dev->vic_dev->sd = sd;
    } else if (!strcmp(pdev->name, "tx-isp-csi")) {
        if (!dev->csi_dev) {
            pr_err("CSI device not initialized\n");
            ret = -EINVAL;
            goto err_free_out_channels;
        }
        dev->csi_dev->sd = sd;
    } else if (!strcmp(pdev->name, "tx-isp-vin")) {
        if (!dev->vin_dev) {
            pr_err("VIN device not initialized\n");
            ret = -EINVAL;
            goto err_free_out_channels;
        }
        dev->vin_dev->sd = sd;
    } else if (!strcmp(pdev->name, "tx-isp-core")) {  // Add this case
        if (!dev->ddr_dev) {
            pr_err("DDR device not initialized\n");
            ret = -EINVAL;
            goto err_free_out_channels;
        }
        dev->ddr_dev->sd = sd;
    } else if (!strcmp(pdev->name, "tx-isp-ddr")) {
        if (!dev->ddr_dev) {
            pr_err("DDR device not initialized\n");
            ret = -EINVAL;
            goto err_free_out_channels;
        }
        dev->ddr_dev->sd = sd;
    } else if (!strcmp(pdev->name, "tx-isp-fs")) {  // Add this case
        if (!dev->fs_dev) {
            pr_err("Frame source device not initialized\n");
            ret = -EINVAL;
            goto err_free_out_channels;
        }
        dev->fs_dev->sd = sd;
    } else if (!strcmp(pdev->name, "sc2336")) {  // Add sensor case
        pr_info("Storing sensor subdev\n");
        dev->sensor_sd = sd;
    } else {
        pr_warn("Unknown subdev name: %s\n", pdev->name);
    }

    /* Initialize the module */
    ret = tx_isp_module_init(pdev, sd);
    if (ret) {
        pr_err("Failed to init module: %d\n", ret);
        goto err_free_out_channels;
    }

    /* Store ISP device reference */
    tx_isp_set_subdev_hostdata(sd, dev);

    pr_info("Subdev %s initialized successfully\n", pdev->name);
    return 0;

/* Error handlers stay the same */
err_free_out_channels:
    if (sd->outpads) {
        for (int i = 0; i < sd->num_outpads; i++) {
            cleanup_pad_channel(&sd->outpads[i]);
        }
        kfree(sd->outpads);
    }

err_free_in_channels:
    if (sd->inpads) {
        for (int i = 0; i < sd->num_inpads; i++) {
            cleanup_pad_channel(&sd->inpads[i]);
        }
        kfree(sd->inpads);
    }
err_free_clks:
    tx_isp_subdev_release_clks(sd);
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
