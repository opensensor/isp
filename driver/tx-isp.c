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


static inline u64 ktime_get_real_ns(void)
{
    struct timespec ts;
    ktime_get_real_ts(&ts);
    return timespec_to_ns(&ts);
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

/**
 * isp_get_subdev - Get subdevice by index
 * @isp: ISP device
 * @index: Subdevice index
 *
 * Helper function to safely access subdevice array
 */
static inline struct tx_isp_subdev *isp_get_subdev(struct isp_device *isp, int index)
{
    if (index >= ARRAY_SIZE(isp->subdevs))
        return NULL;
    return isp->subdevs[index];
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
    void __iomem *vic_regs = dev->reg_base + VIC_BASE;
    uint32_t status, irq_status;

    // Read and clear status
    status = readl(vic_regs + VIC_STATUS);
    irq_status = readl(vic_regs + VIC_IRQ_EN) & status;

    pr_info("VIC IRQ triggered:\n");
    pr_info("  Raw status: 0x%08x\n", status);
    pr_info("  IRQ status: 0x%08x\n", irq_status);
    pr_info("  Control: 0x%08x\n", readl(vic_regs + VIC_CTRL));
    pr_info("  Frame ctrl: 0x%08x\n", readl(vic_regs + VIC_FRAME_CTRL));

    // Clear handled interrupts
    writel(status, vic_regs + VIC_STATUS);
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
    struct isp_framesource_state *fs = &dev->frame_sources[0];
    unsigned long flags;

    pr_info("VIC interrupt: status=0x%08x\n", status);

    // Check the detailed status bits
    pr_info("VIC status details:\n");
    pr_info("  Control: 0x%08x\n", readl(vic_regs + 0x4));
    pr_info("  Route: 0x%08x\n", readl(vic_regs + 0x10));
    pr_info("  Frame Control: 0x%08x\n", readl(vic_regs + 0x14));

    // TODO
//    if (fs && fs->fc) {
//        struct frame_source_channel *fc = fs->fc;
//        spin_lock_irqsave(&fc->queue.lock, flags);
//
//        if (fc->queue.frames) {
//            struct frame_node *node = &fc->queue.frames[fc->write_idx % fc->buf_cnt];
//            node->flags |= V4L2_BUF_FLAG_DONE;
//            list_move_tail(&node->list, &fc->queue.done_list);
//            fc->write_idx++;
//
//            // Wake up waiters
//            wake_up_interruptible(&fc->queue.wait);
//        }
//
//        spin_unlock_irqrestore(&fc->queue.lock, flags);
//    }
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
    void __iomem *vic_regs = dev->reg_base + 0x300;
    void __iomem *isp_regs = dev->reg_base;
    u32 vic_status, isp_status, ctrl_status;

    pr_info("IRQ %d fired\n", irq);

    // Read all status registers
    vic_status = readl(vic_regs + 0x0);
    isp_status = readl(isp_regs + 0x1e0);
    ctrl_status = readl(vic_regs + 0x4);

    pr_info("IRQ status check:\n");
    pr_info("  VIC status: 0x%08x\n", vic_status);
    pr_info("  ISP status: 0x%08x\n", isp_status);
    pr_info("  Control: 0x%08x\n", ctrl_status);

    if (irq == 36) {
        if (vic_status) {
            // Clear status first
            writel(vic_status, vic_regs + 0x0);
            wmb();
            handle_vic_interrupt(dev, vic_status);

            // Restore route to both IRQs
            writel(0x3, vic_regs + 0x10);
            wmb();

            return IRQ_HANDLED;
        }
    }
    else if (irq == 37) {
        if (isp_status) {
            writel(isp_status, isp_regs + 0x1e0);
            wmb();
            handle_isp_interrupt(dev, isp_status);

            // Restore route here too
            writel(0x3, vic_regs + 0x10);
            wmb();

            return IRQ_HANDLED;
        }
    }

    return IRQ_NONE;
}


static int reset_gpio = GPIO_PA(18);  // Default reset GPIO
static int pwdn_gpio = -1;  // Default power down GPIO disabled



// Function definition based on the assembly
int32_t tx_isp_notify(int32_t arg1, int32_t notificationType)
{
    uint32_t globe_ispdev_1 = globe_ispdev;
    int32_t* deviceListPointer = (int32_t*)((char*)globe_ispdev_1 + 0x38); // Casting to correct pointer type
    int32_t resultStatus = 0;
    int32_t operationResult = 0;
    int32_t notificationFlag = (notificationType & 0xff000000);
    void* devicePointer = (void*)*((uint32_t*)deviceListPointer);

    while (true)
    {
        if (devicePointer == 0)
        {
            deviceListPointer = &deviceListPointer[1];
        }
        else
        {
            int32_t (*callbackFunction)(void); // Function pointer declaration with a prototype

            if (notificationFlag == 0x1000000)
            {
                void* callbackData = *(void**)((char*)devicePointer + 0xc4);

                if (callbackData != 0)
                {
                    callbackFunction = (int32_t (*)(void))*(uint32_t*)((char*)callbackData + 0x1c);

                    if (callbackFunction == 0)
                    {
                        resultStatus = 0xfffffdfd;
                    }
                    else
                    {
                        resultStatus = 0;

                        if (operationResult != 0)
                        {
                            resultStatus = 0xfffffdfd;

                            if (operationResult != 0xfffffdfd)
                                return operationResult;
                        }
                    }
                }
                else
                {
                    resultStatus = 0xfffffdfd;
                }
            }
            else if (notificationFlag == 0x2000000)
            {
                void* secondaryCallbackData = (void*)*(uint32_t*)(*(uint32_t*)((char*)devicePointer + 0xc4) + 0xc);
                resultStatus = 0xfffffdfd;

                if (secondaryCallbackData != 0)
                {
                    callbackFunction = (int32_t (*)(void))*(uint32_t*)((char*)secondaryCallbackData + 8);
                    if (callbackFunction != 0)
                    {
                        operationResult = callbackFunction();
                        resultStatus = (operationResult == 0) ? 0 : 0xfffffdfd;
                    }
                }
            }
            else
            {
                resultStatus = 0;
            }

            deviceListPointer = &deviceListPointer[1];
        }

        if ((char*)globe_ispdev_1 + 0x78 == (char*)deviceListPointer)
            break;

        devicePointer = *(void**)deviceListPointer;
    }

    if (resultStatus == 0xfffffdfd)
        return 0;

    return resultStatus;
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

static int isp_w02_show(struct seq_file *m, void *v)
{
    if (!ourISPdev || !ourISPdev->is_open || !ourISPdev->sensor_name[0]) {
        seq_puts(m, "sensor doesn't work, please enable sensor\n");
        return 0;
    }

    // If we have WDR mode info in ourISPdev, we could use it here
    // seq_printf(m, " %d, %d\n", ourISPdev->wdr_mode ? 1 : 0, 0);  // Example values
	seq_printf(m, " %d, %d\n", 0, 0);
// TOODO
    // Add any other relevant info from ourISPdev structure
    // Could add frame/buffer stats if we're tracking them

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
    void *tuning_data;

    // Match libimp's expectation: allocate 0x1c bytes
    tuning_data = kzalloc(0x1c, GFP_KERNEL);
    if (!tuning_data) {
        return -ENOMEM;
    }

    file->private_data = ourISPdev;
    return 0;
}

static int isp_m0_chardev_release(struct inode *inode, struct file *file)
{
    // Free tuning data TODO
    return 0;
}


static long isp_m0_chardev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    // Handle specific ioctls that libimp sends to /dev/isp-m0
    switch(cmd) {
        case ISP_TUNING_ENABLE: {  // 0xc00c56c6
            int enable;
            if (copy_from_user(&enable, (void __user *)arg, sizeof(enable)))
                return -EFAULT;

            // TODO
            //ourISPdev->tuning_enabled = enable;
            return 0;
        }
        default:
            return -ENOTTY;
    }
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



int tx_isp_request_irq(int32_t irqNumber, struct irq_handler_data *irqHandlerData)
{
    pr_info("Requesting IRQ: pdev=%p data=%p\n", irqNumber, irqHandlerData);

    if (!irqHandlerData) {
        pr_err("%s: Invalid parameters\n", __func__);
        return -EINVAL;
    }

    // Get platform IRQ
    if (irqNumber < 0) {
        pr_err("Failed to get platform IRQ: %d\n", irqNumber);
        irqHandlerData->irq_number = 0;
        return irqNumber;
    }

    pr_info("Using platform IRQ number: %d\n", irqNumber);

    // Initialize spinlock for IRQ handling
    spin_lock_init(&irqHandlerData->lock);

    // Initialize task list
    memset(irqHandlerData->task_list, 0, sizeof(struct irq_task) * MAX_TASKS);
    irqHandlerData->task_count = 0;

    // Request IRQ with flags for shared line
    int ret = request_irq(irqNumber,
                         tx_isp_irq_handler,
                         IRQF_SHARED,      // Linux 3.10 flags
                         "tx-isp-vic",
                         irqHandlerData);
    if (ret) {
        pr_err("Failed to request IRQ %d: %d\n", irqNumber, ret);
        irqHandlerData->irq_number = 0;
        return ret;
    }

    // Store values after successful registration
    irqHandlerData->irq_number = irqNumber;
    irqHandlerData->handler_function = tx_isp_irq_handler;
    irqHandlerData->disable_function = tx_isp_disable_irq;

//    // Start with IRQs disabled
//    if (irqHandlerData->disable_function) {
//        irqHandlerData->disable_function(irqHandlerData);
//    }

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

    // Store in platform device and IMPISPDev
    dev->fs_subdev = sd;
    dev->fs_pdev = pdev;
    platform_set_drvdata(pdev, dev);

    pr_info("Frame source probe complete: dev=%p sd=%p fs_pdev=%p\n",
            dev, sd, dev->fs_pdev);
    return 0;
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
 * struct isp_ae_info - AE information structure
 * Based on decompiled access patterns
 */
struct isp_ae_info {
    /* Current settings */
    u32 gain;
    u32 exposure;
    u32 gain_factor;
    u32 exposure_factor;    /* Fixed 0x400 from decompiled */

    /* WDR specific settings */
    u32 wdr_gain;
    u32 wdr_exposure;

    /* Algorithm state */
    u32 frame_cnt;
    u32 stable_cnt;
    u32 converge_cnt;

    /* Statistics */
    u32 current_lum;
    u32 target_lum;
    u32 avg_lum;

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


static inline void system_reg_write(u32 reg, u32 val)
{
    void __iomem *addr = ioremap(reg, 4);

    if (!addr)
        return;

    writel(val, addr);
    iounmap(addr);
}


static int tx_isp_open(struct file *file)
{
    struct isp_instance *instance;
    int ret = 0;
    int fd;

    pr_info("ISP device open called from pid %d\n", current->pid);

    if (!ourISPdev) {
        pr_err("ISP device not initialized\n");
        return -ENODEV;
    }

    // Allocate instance tracking
    instance = kzalloc(sizeof(*instance), GFP_KERNEL);
    if (!instance)
        return -ENOMEM;

    // Get frame source pointer
    struct isp_framesource_state *fs = &ourISPdev->frame_sources[0];

    // Initialize instance data
    instance->fd = fd = get_unused_fd_flags(O_CLOEXEC);
    if (fd < 0) {
        kfree(instance);
        return fd;
    }

    instance->file = file;
    instance->fs = fs;
    file->private_data = instance;

    // Mark device and frame source as open
    ourISPdev->is_open = 1;
    fs->is_open = 1;

    pr_info("ISP opened: file=%p fs=%p instance=%p fd=%d\n",
            file, fs, instance, fd);

    return 0;
}


// Update cleanup function to just free info structures:
static void cleanup_buffer_info(struct IMPISPDev *dev)
{
    if (dev) {
//        if (dev->wdr_buf_info) {
//            kfree(dev->wdr_buf_info);
//            dev->wdr_buf_info = NULL;
//        }
    }
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


/**
 * struct isp_link_config - Link configuration data from decompiled
 * Based on the array at offset 0x6ad7c and 0x6ad80 in decompiled
 */
//struct isp_link_config {
//    struct isp_pad_desc *pads;
//    int num_pads;
//};
//  TODO what is this about?
//static struct isp_link_config link_configs[] = {
//    [0] = {
//        .pads = &(struct isp_pad_desc){
//            .type = 1,  // Source pad
//            .link_state = LINK_STATE_SOURCE,
//            .flags = PAD_FL_SOURCE
//        },
//        .num_pads = 2  // Matches 0x6ad80 entry
//    },
//    [1] = {
//        .pads = &(struct isp_pad_desc){
//            .type = 2,  // Sink pad
//            .link_state = LINK_STATE_SOURCE,
//            .flags = PAD_FL_SINK
//        },
//        .num_pads = 2  // Matches 0x6ad80 entry
//    }
//};




static void dump_frame_format_state(struct isp_framesource_state *fs)
{
    static const char *formats[] = {
        "UNKNOWN",
        "YUV422",
        "NV12",
        "RAW10"
    };
    const char *format_str = "UNKNOWN";

    // Map format code to string
    switch (fs->fmt) {
        case ISP_FMT_YUV422: format_str = formats[1]; break;
        case ISP_FMT_NV12: format_str = formats[2]; break;
    }

    pr_info("Frame Source State:\n");
    pr_info("  Format: %s (0x%x)\n", format_str, fs->fmt);
    pr_info("  Resolution: %dx%d\n", fs->width, fs->height);
    pr_info("  Buffer: size=%u count=%d\n", fs->buf_size, fs->buf_cnt);
    pr_info("  State: %d Flags: 0x%x\n", fs->state, fs->flags);

    if (fs->fc) {
        struct frame_source_channel *fc = fs->fc;
        pr_info("  DMA: addr=0x%08x base=%p\n",
                (u32)fc->dma_addr, fc->buf_base);
    }
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



static int handle_stream_enable(struct IMPISPDev *dev, bool enable)
{
    struct isp_framesource_state *fs;
    int ret;

    // Get frame source for channel 0
    fs = &dev->frame_sources[0];
    if (!fs || !fs->is_open) {
        pr_err("Frame source not initialized\n");
        return -EINVAL;
    }

    return 0;
}



static int handle_sensor_ioctl(struct IMPISPDev *dev, unsigned int cmd, void __user *arg, struct file *file)
{
    struct i2c_client *client = dev->sensor_i2c_client; // Add this field to IMPISPDev
    struct sensor_reg_data reg_data;
    int ret = 0;
    uint8_t val = 0;

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
        case VIDIOC_STREAMON:  // 0x80045612
        {
            pr_info("Sensor command: 0x%x\n", cmd);
            pr_info("Sensor streamon\n");
            ret = enable_isp_streaming(ourISPdev, file, 0, true);
            if (ret == 0) {
                pr_info("Stream enabled");
            }
            return ret;
        }

        case VIDIOC_STREAMOFF:
            pr_info("Stream OFF requested\n");
            return enable_isp_streaming(ourISPdev, file, 0, false);
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
    struct isp_instance *instance = file->private_data;

    pr_info("\n=== ISP Release Debug ===\n");
    pr_info("file=%p flags=0x%x fd=%d\n",
            file, file->f_flags, instance ? instance->fd : -1);

    if (instance) {
        // Stop streaming if active
        if (instance->fs && instance->fs->state == 2) {
            enable_isp_streaming(ourISPdev, file, 0, false);
        }

        // Add CSI cleanup here - before frame source cleanup  TODO

        if (instance->fd >= 0) {
            put_unused_fd(instance->fd);
        }

        kfree(instance);
    }

    file->private_data = NULL;
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

/* Global state variables seen in decompiled */
static void *ae_info_mine;
static void *ae_statis_mine;
static void *awb_info_mine;
static int ae_algo_comp;
static int awb_algo_comp;

/* Wait queues seen in decompiled code */
static DECLARE_WAIT_QUEUE_HEAD(ae_wait);
static DECLARE_WAIT_QUEUE_HEAD(awb_wait);

/* System register access functions */
static inline u32 system_reg_read(u32 reg)
{
    void __iomem *addr = ioremap(reg, 4);
    u32 val;

    if (!addr)
        return 0;

    val = readl(addr);
    iounmap(addr);

    return val;
}

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
static void isp_disable_wdr(struct isp_device *isp)
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
    struct isp_ae_info *ae_info;
    struct isp_reg_t *regs;

    /* Validate magic number - check at 0xef64 */
    if (ae->magic != AE_ALGO_MAGIC) {
        dev_err(dev->dev, "[ %s:%d ] Ae Algo Function registration failed\n",
                __func__, __LINE__);
        return -EINVAL;
    }

    /* Initialize algorithm - matches tisp_ae_algo_init(1, ae) at 0xefa4 */
    tisp_ae_algo_init(1, ae);

    /* Allocate statistics buffer - matches 0xf048/0xf068 */
    ae_info = kmalloc(sizeof(*ae_info), GFP_KERNEL);
    if (!ae_info)
        return -ENOMEM;

    ae_statis_mine = kmalloc(sizeof(struct isp_ae_stats), GFP_KERNEL);
    if (!ae_statis_mine) {
        kfree(ae_info);
        return -ENOMEM;
    }

    /* Store globally - matches 0xf060 */
    ae_info_mine = ae_info;

    /* Get register base */
    regs = (struct isp_reg_t *)dev->reg_base;  // Cast the void* regs to proper type

    /* Setup initial parameters - matches logic from 0xefb4 to 0xf040 */
    ae_info->gain = readl(&regs->gain);
    ae_info->exposure = readl(&regs->exposure);
    ae_info->gain_factor = private_math_exp2(readl(&regs->gain_factor), 0x10, 0xa);
    ae_info->exposure_factor = 0x400; // Fixed value from 0xefdc

    /* Setup WDR-specific parameters if enabled */
    if (readl(&regs->wdr_enable)) {
        ae_info->wdr_gain = system_reg_read(0x1000);
        ae_info->wdr_exposure = system_reg_read(0x100c);
    } else {
        ae_info->wdr_gain = system_reg_read(0x1030);
    }

    /* Initialize completion tracking - matches 0xf070 */
    ae_algo_comp = 0;

    /* Initialize wait queue - matches 0xf090 */
    init_waitqueue_head(&ae_wait);

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
    awb_info = kmalloc(sizeof(*awb_info), GFP_KERNEL);
    if (!awb_info)
        return -ENOMEM;

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



// TODO
/**
 * setup_wdr_buffers - Configure WDR buffer memory regions
 * @isp: ISP device structure
 * @buf_info: WDR Buffer information from userspace
 *
 * Based on decompiled code at case 0x800856d6
 */
//int setup_wdr_buffers(struct isp_device *isp, struct isp_wdr_buf_info *buf_info)
//{
//    uint32_t required_size;
//    uint32_t line_width;
//    uint32_t height;
//    struct isp_reg_t *regs = isp->regs;  // Use main register block
//
//    /* Debug print matching decompiled code */
//    dev_dbg(isp->dev, "%s:%d::tsize is %d, buf info size: %d\n",
//            __func__, __LINE__, required_size, buf_info->size);
//
//    /* Get WDR mode from registers */
//    uint32_t wdr_mode = isp_reg_read(regs, ISP_WDR_MODE_OFFSET);
//
//    /* Calculate required buffer size based on WDR mode */
//    switch (wdr_mode) {
//    case WDR_MODE_LINE:
//        /* Line-interleaved WDR mode */
//        line_width = isp_reg_read(regs, ISP_WDR_WIDTH_OFFSET) << 1;
//        height = isp_reg_read(regs, ISP_WDR_HEIGHT_OFFSET);
//        required_size = line_width * height;
//        break;
//
//    case WDR_MODE_FRAME:
//        /* Frame-based WDR mode */
//        required_size = isp_reg_read(regs, ISP_WDR_FRAME_OFFSET);
//        line_width = isp_reg_read(regs, ISP_WDR_WIDTH_OFFSET) << 1;
//        height = required_size / line_width;
//        break;
//
//    default:
//        /* Not in WDR mode */
//        dev_err(isp->dev, "Not in WDR mode, buffer setup not needed\n");
//        return -EINVAL;
//    }
//
//    /* Verify buffer size is sufficient */
//    if (buf_info->size < required_size) {
//        dev_err(isp->dev, "%s,%d: buf size too small\n", __func__, __LINE__);
//
//        /* Clear WDR mode and disable WDR */
//        isp_reg_write(regs, ISP_WDR_MODE_OFFSET, WDR_MODE_NONE);
//        isp_disable_wdr(isp);
//
//        return -EINVAL;
//    }
//
//    /* Program WDR buffer registers */
//    system_reg_write(WDR_REG_BUF, buf_info->addr);
//    system_reg_write(WDR_REG_LINE, line_width);
//    system_reg_write(WDR_REG_HEIGHT, height);
//
//    dev_dbg(isp->dev, "WDR buffer setup complete: addr=0x%08x, line=%d, height=%d\n",
//            buf_info->addr, line_width, height);
//
//    return 0;
//}

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
static int isp_set_wdr_mode(struct isp_device *isp, int enable)
{
    struct isp_reg_t *regs = isp->regs;
    int ret = 0;

    if (enable) {
        /* Enable WDR mode - from case 0x800456d8 */
        isp_reg_write(regs, offsetof(struct isp_reg_t, wdr_enable), 1);

        /* Apply WDR settings from decompiled */
        if (wdr_switch) {
            /* Configure WDR hardware settings */
            if (isp->regs) {
                /* Set WDR parameters - matches decompiled at 0xed90 */
                system_reg_write(0x2000013, 1);
                tisp_s_wdr_en(1);

                /* Reset statistics buffers */
                void *stats = isp->regs + offsetof(struct isp_reg_t, ae_stats);
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
static int calculate_wdr_buffer_size(struct isp_device *isp, struct isp_wdr_buf_size *size)
{
    struct isp_reg_t *regs = isp->regs;
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




static int isp_core_tuning_open(struct IMPISPDev *dev)
{
  // TODO
    return 0;
}

static int isp_core_tuning_release(struct IMPISPDev *dev)
{
	pr_info("##### %s %d #####\n", __func__, __LINE__);
	// TODO
    return 0;
}

int csi_video_s_stream(struct IMPISPDev *dev, bool enable)
{
    struct isp_framesource_state *fs = &dev->frame_sources[0];

    pr_info("CSI stream config: enable=%d state=%d\n", enable, fs->state);
    // From decompiled: Check sensor type at 0x110+0x14 should be 1
    if (readl(dev->reg_base + 0x110 + 0x14) != 1)
        return 0;

    // Simple state transition based on decompiled:
    // Sets state 4 if enable, 3 if disable
    fs->state = enable ? 4 : 3;
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

		    struct isp_framesource_state *fs = &dev->frame_sources[0];
		    struct frame_source_channel *fc = fs->fc;
		    if (!fc)
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

		    struct isp_framesource_state *fs = &dev->frame_sources[channel_id];
		    struct frame_source_channel *fc = fs->fc;
		    if (!fc)
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
    //ourISPdev->awb_info.gain = gain;
    //ourISPdev->awb_info.exposure = exposure;

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

    // Initialize AE/AWB info with safe defaults  // TODO
//    dev->ae_info.gain = 0x100;     // Default gain
//    dev->ae_info.exposure = 0x100;  // Default exposure

//    dev->awb_info.gain = 0x100;
//    dev->awb_info.exposure = 0x100;

    pr_info("Initializing AE/AWB subsystems\n");
//    ret = tiziano_ae_init(dev->ae_info.gain, dev->ae_info.exposure);
//    if (ret) {
//        pr_err("Failed to initialize AE\n");
//        return ret;
//    }

    // ret = tiziano_awb_init(dev->awb_info.gain, dev->awb_info.exposure);
    if (ret) {
        pr_err("Failed to initialize AWB\n");
        return ret;
    }

    pr_info("TODO Initializing image processing subsystems\n");
//
//    // Initialize each subsystem with error checking
//    if ((ret = tiziano_gamma_init()) ||
//        (ret = tiziano_gib_init()) ||
//        (ret = tiziano_lsc_init()) ||
//        (ret = tiziano_ccm_init()) ||
//        (ret = tiziano_dmsc_init()) ||
//        (ret = tiziano_sharpen_init()) ||
//        (ret = tiziano_sdns_init()) ||
//        (ret = tiziano_mdns_init()) ||
//        (ret = tiziano_clm_init()) ||
//        (ret = tiziano_dpc_init()) ||
//        (ret = tiziano_hldc_init()) ||
//        (ret = tiziano_defog_init()) ||
//        (ret = tiziano_adr_init()) ||
//        (ret = tiziano_af_init()) ||
//        (ret = tiziano_bcsh_init()) ||
//        (ret = tiziano_ydns_init()) ||
//        (ret = tiziano_rdns_init())) {
//
//        pr_err("Failed to initialize subsystem, ret = %d\n", ret);
//        return ret;
//    }
//
//    // WDR mode initialization
//    if (dev->wdr_mode == 1) {
//        pr_info("Initializing WDR mode\n");
//        if ((ret = tiziano_wdr_init()) ||
//            (ret = tisp_gb_init()) ||
//            (ret = tisp_dpc_wdr_en(1))) {
//            pr_err("Failed to initialize WDR mode\n");
//            return ret;
//        }
//    }
//
//    // Event system initialization
//    pr_info("Initializing event system\n");
//    ret = tisp_event_init();
//    if (ret) {
//        pr_err("Failed to initialize event system\n");
//        return ret;
//    }
//
//    // Setup callbacks
//    tisp_event_set_cb(4, tisp_tgain_update);
//    tisp_event_set_cb(5, tisp_again_update);
//    tisp_event_set_cb(7, tisp_ev_update);
//    tisp_event_set_cb(9, tisp_ct_update);
//    tisp_event_set_cb(8, tisp_ae_ir_update);

    pr_info("ISP subsystem initialization complete\n");
    return 0;
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



static int init_isp_reserved_memory(struct platform_device *pdev)
{
    dma_addr_t aligned_base = ALIGN(RMEM_BASE, 32);
    size_t aligned_size = ALIGN(RMEM_SIZE, 32);

    ourISPdev->dma_addr = aligned_base;
    ourISPdev->dma_size = aligned_size;

    ourISPdev->dma_buf = ioremap(aligned_base, aligned_size);
    if (!ourISPdev->dma_buf) {
        return -ENOMEM;
    }

    pr_info("tx_isp: Reserved memory initialized:\n");
    pr_info("  Physical address: 0x%08x (aligned)\n", (uint32_t)aligned_base);
    pr_info("  Virtual address: %p\n", ourISPdev->dma_buf);
    pr_info("  Size: %zu bytes\n", aligned_size);
    pr_info("  Alignment check: base=0x%x buf=0x%x\n",
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
// TODO possibly reintegrate
static long handle_get_buf_ioctl(struct IMPISPDev *dev, unsigned long arg) {
  	struct isp_mem_request req;
    void __user *argp = (void __user *)arg;
    int channel = 0;  // Default to first channel
    struct isp_framesource_state *fs;

    if (!dev || !dev->dma_buf) {
        pr_err("tx_isp: Invalid device state\n");
        return -EINVAL;
    }

    // Get frame source for channel
    fs = &dev->frame_sources[channel];
    if (!fs || !fs->is_open) {
        pr_err("tx_isp: No frame source initialized\n");
        return -EINVAL;
    }

    if (!fs) {
        pr_err("tx_isp: No frame source initialized\n");
        return -EINVAL;
    }

    if (copy_from_user(&req, argp, sizeof(req))) {
        pr_err("tx_isp: Failed to copy from user\n");
        return -EFAULT;
    }

    pr_info("tx_isp: SET_BUF request: method=0x%x phys=0x%x size=0x%x\n",
            req.method, req.phys_addr, req.size);

    // Magic sequence check
    if (req.method == 0x203a726f && req.phys_addr == 0x33326373) {
        // Maintain exact same values for initial request
        req.method = ISP_ALLOC_KMALLOC;
        req.phys_addr = 0x1;  // Initial magic value
        req.size = 0x1;       // Initial size
        req.virt_addr = (unsigned long)dev->dma_buf;
        req.flags = 0;

        // TODO Store actual values in our buffer info

        // Also update frame source buffer info
        fs->dma_addr = dev->dma_addr;
        fs->buf_size = dev->dma_size;

        pr_info("tx_isp: Magic allocation setup: phys=0x%x virt=%p size=0x%x\n",
                (unsigned int)dev->dma_addr, dev->dma_buf, dev->dma_size);

        if (copy_to_user(argp, &req, sizeof(req)))
            return -EFAULT;
        return 0;
    }

    // Handle actual memory request
    if (req.phys_addr == 0x1) {
        struct frame_source_channel *fc = fs->fc;
        if (!fc) {
            pr_err("tx_isp: No channel data\n");
            return -EINVAL;
        }

        // Allocate and setup VBM entry
        void *vbm_entry = kzalloc(0x80, GFP_KERNEL);
        if (!vbm_entry) {
            pr_err("tx_isp: Failed to allocate VBM entry\n");
            return -ENOMEM;
        }

        // Setup VBM state flags
        *(u32 *)(vbm_entry + 0x48) = 0;        // Buffer free
        *(u32 *)(vbm_entry + 0x4c) = 1;        // Ready for use
        *(u64 *)(vbm_entry + 0x68) = 0;        // Clear sequence
        *(u64 *)(vbm_entry + 0x70) = dev->dma_addr;
        *(u64 *)(vbm_entry + 0x28) = (u64)dev->dma_buf;
        *(u32 *)(vbm_entry + 0x30) = dev->dma_addr;

        // Store in VBM slot 0
        // TODO
//        fc->vbm_table[0] = vbm_entry;
//        fc->state |= BIT(0);

        // Return actual memory info
        req.method = ISP_ALLOC_KMALLOC;
        req.phys_addr = dev->dma_addr;
        req.size = dev->dma_size;
        req.virt_addr = (unsigned long)dev->dma_buf;
        req.flags = 0;

        pr_info("tx_isp: Memory allocation complete with VBM setup: phys=0x%x virt=%p size=0x%x\n",
                (unsigned int)dev->dma_addr, dev->dma_buf, dev->dma_size);

        if (copy_to_user(argp, &req, sizeof(req)))
            return -EFAULT;
        return 0;
    }

    pr_err("tx_isp: Unhandled request\n");
    return -EINVAL;
}

struct isp_pad_info *find_subdev_link_pad(struct IMPISPDev *dev, struct isp_pad_info *pad_info)
{
    // Start at 0x38 offset in device structure
    struct tx_isp_subdev **subdev_ptr = (struct tx_isp_subdev **)((char *)dev + 0x38);
    struct tx_isp_subdev *subdev;

    // Check each subdev until 0x78 offset
    while ((char *)subdev_ptr < ((char *)dev + 0x78)) {
        subdev = *subdev_ptr;
        if (subdev) {
            // Compare names
            const char *dev_name = subdev->module.name;
            const char *pad_name = pad_info->name;

            // Character-by-character comparison
            while (true) {
                if (*dev_name != *pad_name)
                    break;

                if (*dev_name == 0) // Match found
                    break;

                dev_name++;
                pad_name++;
            }

            // Names match - handle pad lookup
            if (*dev_name == *pad_name) {
                // Source pad
                if (pad_info->type == 1) {
                    if (pad_info->pad_idx < subdev->num_outpads) {
                        return (struct isp_pad_info *)(subdev->outpads +
                               (pad_info->pad_idx * 0x24));
                    }
                }
                // Sink pad
                else if (pad_info->type == 2) {
                    if (pad_info->pad_idx < subdev->num_inpads) {
                        return (struct isp_pad_info *)(subdev->inpads +
                               (pad_info->pad_idx * 0x24));
                    }
                }

                pr_err("Can't find the matched pad!\n");
                return NULL;
            }
        }
        subdev_ptr++;
    }

    return NULL;
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
    struct isp_framesource_state *fs = NULL;
    struct isp_instance *instance;
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
        void __iomem *vic_regs = ourISPdev->reg_base + VIC_BASE;
    	// Save current state
    	u32 saved_route = readl(vic_regs + 0x10);
    	u32 saved_mask = readl(vic_regs + 0xc);

        // The decompiled code shows it's passing 0 directly as arg
        // We shouldn't try to copy from user here since arg is the value
        if (arg != 0) {
            pr_err("Invalid enable stream argument\n");
            return -EINVAL;
        }

        // Don't try to copy param, just enable
        ret = handle_stream_enable(ourISPdev, true);
        // TODO
//        if (ret == 0) {
//            ourISPdev->stream_count += 2;
//            pr_info("Stream enabled, count=%d\n", ourISPdev->stream_count);
//        }

// 3. Initial mode setup - match streaming enable sequence
writel(2, vic_regs + 0x4);           // Control register (Mode = 2)
writel(0x800c0000, vic_regs + 0x10); // Route/IRQ Enable
wmb();
udelay(10);

// Set the interrupt configuration
writel(0x33fb, vic_regs + 0xc);      // IRQ Enable
writel(0, vic_regs + 0x8);           // Clear IRQ Mask
writel(0x3, vic_regs + 0x10);        // Ensure both routes are set
wmb();

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
    case ISP_TUNING_ENABLE: {  // 0xc00c56c6
        int enable;
        if (copy_from_user(&enable, (void __user *)arg, sizeof(enable)))
            return -EFAULT;

        pr_info("ISP tuning %s requested\n", enable ? "enable" : "disable");

        if (enable) {
            ret = isp_core_tuning_open(ourISPdev);
        } else {
            ret = isp_core_tuning_release(ourISPdev);
        }

        // Store tuning state
        // ourISPdev->tuning_enabled = enable;
        return ret;
    }
case TX_ISP_SET_BUF: {  // 0x800856d5
    struct imp_buffer_info local_buf_info;
    struct isp_framesource_state *fs;
    size_t frame_size;
    u8 *vbm_entry;

    if (!ourISPdev) {
        pr_err("ISP device not initialized\n");
        return -EINVAL;
    }

    // Same validation as OEM
    if (!arg || (unsigned long)arg >= 0xfffff001) {
        pr_err("Invalid user pointer\n");
        return -EFAULT;
    }

    if (copy_from_user(&local_buf_info, (void __user *)arg, sizeof(local_buf_info))) {
        pr_err("Failed to copy from user\n");
        return -EFAULT;
    }

    // Check for magic initialization sequence like OEM
    if (local_buf_info.method == 0x203a726f && local_buf_info.phys_addr == 0x33326373) {
        // Return initial magic values exactly like OEM
        local_buf_info.method = ISP_ALLOC_KMALLOC;
        local_buf_info.phys_addr = 0x1;
        local_buf_info.size = 0x1;
        local_buf_info.virt_addr = (unsigned long)ourISPdev->dma_buf;
        local_buf_info.flags = 0;

        if (copy_to_user((void __user *)arg, &local_buf_info, sizeof(local_buf_info))) {
            pr_err("Failed to copy initial values to user\n");
            return -EFAULT;
        }
        return 0;
    }

    // Handle second phase exactly like OEM
    if (local_buf_info.phys_addr == 0x1) {
        // Get frame source state
        fs = &ourISPdev->frame_sources[0];
        frame_size = (fs->width * fs->height * 3) / 2;

        // Allocate VBM entry exactly like OEM
        vbm_entry = kzalloc(0x80, GFP_KERNEL);
        if (!vbm_entry) {
            pr_err("Failed to allocate VBM entry\n");
            return -ENOMEM;
        }

        // Setup VBM state flags exactly like OEM
        *(u32 *)(vbm_entry + 0x48) = 0;        // Buffer free
        *(u32 *)(vbm_entry + 0x4c) = 1;        // Ready for use
        *(u64 *)(vbm_entry + 0x68) = 0;        // Clear sequence
        *(u64 *)(vbm_entry + 0x70) = ourISPdev->dma_addr;  // Physical address
        *(u64 *)(vbm_entry + 0x28) = (u64)ourISPdev->dma_buf;
        *(u32 *)(vbm_entry + 0x30) = ourISPdev->dma_addr;

        // Store in VBM slot 0 like OEM
        //struct frame_source_channel *fc = ourISPdev->frame_sources[0].private;
//        fc->vbm_table[0] = vbm_entry;
//        fc->state |= BIT(0);  // Set streaming bit

        // Return actual values like OEM
        local_buf_info.method = ISP_ALLOC_KMALLOC;
        local_buf_info.phys_addr = ourISPdev->dma_addr;
        local_buf_info.size = ourISPdev->dma_size;
        local_buf_info.virt_addr = (unsigned long)ourISPdev->dma_buf;
        local_buf_info.flags = 0;

        if (copy_to_user((void __user *)arg, &local_buf_info, sizeof(local_buf_info))) {
            pr_err("Failed to copy buffer info to user\n");
            kfree(vbm_entry);
            return -EFAULT;
        }
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
        buf_info.buffer_start = ourISPdev->dma_addr;
        buf_info.buffer_size = ourISPdev->dma_size;
        buf_info.virt_addr = (unsigned long)ourISPdev->dma_buf;
        buf_info.flags = 1;

        // TODO Store consistent info
        // memcpy(ourISPdev->buffer_info, &buf_info, sizeof(buf_info));

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
	            // Existing controls...

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
        uint32_t link_config;
        struct tx_isp_device *isp_dev = container_of(file->private_data,
                                                    struct tx_isp_device,
                                                    module);

        if (copy_from_user(&link_config, (void __user *)arg, sizeof(link_config))) {
            pr_err("[%s][%d] copy from user error\n",
                   "tx_isp_video_link_setup", 264);
            return -EFAULT;
        }

        if (link_config >= 2) {
            pr_err("link(%d) is invalid!\n", link_config);
            return -EINVAL;
        }

        pr_info("Setting up video link %d\n", link_config);

        // Use our static configurations
        const struct tx_isp_link_configs *configs = &isp_link_configs[link_config];
        struct tx_isp_subdev *sd = container_of(file->private_data,
                                              struct tx_isp_subdev,
                                              module);

        // Validate
        if (!sd || !isp_dev) {
            pr_err("Invalid device state\n");
            return -EINVAL;
        }

        pr_info("Found devices: isp=%p sd=%p\n", isp_dev, sd);

        // Continue with rest of link setup...
        for (int i = 0; i < configs->length; i++) {
            const struct tx_isp_link_config *cfg = &configs->config[i];

            // Find source pad
            struct tx_isp_subdev_pad *src_pad = find_subdev_link_pad(sd,
                                                                    &cfg->src);
            // Find sink pad
            struct tx_isp_subdev_pad *sink_pad = find_subdev_link_pad(sd,
                                                                     &cfg->dst);

            if (!src_pad || !sink_pad)
                continue;

            // Verify link compatibility
            if (!(src_pad->links_type & sink_pad->links_type & cfg->flag)) {
                pr_err("The link type is mismatch!\n");
                return -EINVAL;
            }

            // Check pad states
            if (src_pad->state == TX_ISP_PADSTATE_ACTIVE ||
                sink_pad->state == TX_ISP_PADSTATE_ACTIVE) {
                pr_err("Please stop active links firstly\n");
                return -EINVAL;
            }

            // Handle existing links
            if (src_pad->state == TX_ISP_PADSTATE_LINKED &&
                sink_pad != src_pad->link.sink) {
                subdev_video_destroy_link(src_pad);
            }

            if (sink_pad->state == TX_ISP_PADSTATE_LINKED &&
                src_pad != sink_pad->link.source) {
                subdev_video_destroy_link(sink_pad);
            }

            // Create forward link
            src_pad->link.source = src_pad;
            src_pad->link.sink = sink_pad;
            src_pad->link.flag = cfg->flag;
            src_pad->link.state = TX_ISP_MODULE_ACTIVATE;

            // Create reverse link
            sink_pad->link.source = sink_pad;
            sink_pad->link.sink = src_pad;
            sink_pad->link.flag = cfg->flag;
            sink_pad->link.state = TX_ISP_MODULE_ACTIVATE;

            // Connect reverse pointers
            src_pad->link.reverse = &sink_pad->link;
            sink_pad->link.reverse = &src_pad->link;

            // Update pad states
            src_pad->state = TX_ISP_PADSTATE_LINKED;
            sink_pad->state = TX_ISP_PADSTATE_LINKED;
        }

        isp_dev->active_link = link_config;
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
        kfree(ae_info_mine);
        kfree(ae_statis_mine);
        break;
    }

    case TX_ISP_SET_AWB_ALGO_CLOSE: {
      	pr_info("TX_ISP_SET_AWB_ALGO_CLOSE\n");
        // TODO
        // Cleanup AWB algorithm - matches case 0x800456e4
        //tisp_awb_algo_deinit();
        //tisp_awb_algo_init(0);
        kfree(awb_info_mine);
        break;
    }

    case VIDIOC_SET_ROTATION: {
		pr_info("VIDIOC_SET_ROTATION\n");
        // ret = set_rotation(ourISPdev, argp); // TODO
        break;
	}
	case VIDIOC_STREAMOFF:
		pr_info("VIDIOC_STREAMOFF\n");
        return enable_isp_streaming(ourISPdev, file, channel, false);
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
case 0x400456bf: {  // DQBUF handler
    struct {
        u32 cmd;
        u32 pad;
        u32 seq;
        u32 flags;
        u32 params[4];
    } req;

    if (!arg || copy_from_user(&req, (void __user *)arg, sizeof(req))) {
        pr_err("DQBUF: Failed to copy from user\n");
        return -EFAULT;
    }

    struct frame_source_channel *fc = fs->fc;

    pr_info("DQBUF call: cmd=0x%x seq=%d flags=0x%x streaming=%d\n",
            req.cmd, req.seq, req.flags, (fc->state & BIT(0)));

    if (req.cmd == 0xffffffff) {
        if ((fc->state & BIT(0)) == 0) {
            pr_err("DQBUF: Not streaming\n");
            return -EINVAL;
        }

        if (!fc->buf_base || !fc->buf_size) {
            pr_err("DQBUF: Invalid buffer state\n");
            return -EINVAL;
        }

        void *buffer = fc->buf_base + fc->channel_offset;
        size_t frame_size;

        pr_info("Generating frame %d at %p offset=0x%x\n",
                fc->sequence, buffer, fc->channel_offset);

        // Generate pattern
        int width = fs->width;
        int height = fs->height;
        int y_size = width * height;
        u8 *dst = buffer;

        // Super obvious pattern - full frame color alternation
        u8 y_val = (fc->sequence % 2) ? 235 : 16;  // Alternate black/white
        memset(dst, y_val, y_size);

        // UV plane - alternate between strong colors
        u8 *uv = dst + y_size;
        switch(fc->sequence % 4) {
            case 0:  // Red
                for (int i = 0; i < y_size/2; i += 2) {
                }
                break;
            case 1:  // Blue
                for (int i = 0; i < y_size/2; i += 2) {
                    uv[i] = 240;     // U = blue
                    uv[i + 1] = 128; // V = neutral
                }
                break;
            case 2:  // Green
                for (int i = 0; i < y_size/2; i += 2) {
                    uv[i] = 0;       // U = green
                    uv[i + 1] = 0;   // V = green
                }
                break;
            case 3:  // Yellow
                for (int i = 0; i < y_size/2; i += 2) {
                    uv[i] = 0;       // U = yellow
                    uv[i + 1] = 240; // V = yellow
                }
                break;
        }

        wmb();
        frame_size = y_size + (y_size/2);

        pr_info("Generated frame %d: color=%d y=%d size=%d\n",
                fc->sequence, fc->sequence % 4, y_val, frame_size);

        struct {
            u32 status;
            u32 size;
            u32 timestamp;
            u32 flags;
            u32 sequence;
            u32 offset;
            u32 buf_size;
            u32 pad;
            u32 width;
            u32 height;
            u32 format;
            u32 reserved[5];
        } resp = {
            .status = 0,
            .size = frame_size,
            .timestamp = jiffies_to_msecs(jiffies),
            .flags = 3,
            .sequence = fc->sequence++,
            .offset = fc->channel_offset,
            .buf_size = fc->buf_size,
            .width = fs->width,
            .height = fs->height,
            .format = fs->fmt
        };

        pr_info("DQBUF response: seq=%d size=%d offset=0x%x buf=%p\n",
                resp.sequence, resp.size, resp.offset, buffer);

        if (copy_to_user((void __user *)arg, &resp, sizeof(resp))) {
            pr_err("DQBUF: Failed to copy to user\n");
            return -EFAULT;
        }

        return 0;
    }
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
    case VIDIOC_STREAMON:
        pr_info("Sensor command: 0x%x\n", cmd);
        ret = handle_sensor_ioctl(ourISPdev, cmd, argp, file);
        break;

    default:
        dev_dbg(ourISPdev->dev, "Unhandled ioctl cmd: 0x%x\n", cmd);
        return -ENOTTY;
    }

    return ret;
}

static int tx_isp_mmap(struct file *filp, struct vm_area_struct *vma) {
    struct IMPISPDev *dev = ourISPdev;
    unsigned long size = vma->vm_end - vma->vm_start;

    pr_info("tx_isp: mmap request size=%lu\n", size);

    if (size > dev->dma_size) {
        pr_err("tx_isp: mmap size too large\n");
        return -EINVAL;
    }

    // Set up continuous memory mapping
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

    if (remap_pfn_range(vma,
                        vma->vm_start,
                        dev->dma_addr >> PAGE_SHIFT,
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
static void configure_deir_control(void)
{
    /* Add your DEIR control register configurations here */
    /* Use readl/writel for register access */
}

// Add validation for parameter loading
static int validate_parameter_load(void __iomem *dest, size_t offset)
{
    uint32_t test_val;

    // Try writing a test pattern
    writel(0xdeadbeef, dest);
    wmb();
    test_val = readl(dest);

    if (test_val != 0xdeadbeef) {
        pr_err("Parameter write test failed: wrote 0xdeadbeef read 0x%08x\n", test_val);
        return -EIO;
    }

    // Reset test location
    writel(0, dest);
    wmb();
    return 0;
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

    sc2336_hw_reset(ourISPdev);

    // Enable sensor clock
    writel(0x1, ourISPdev->reg_base + ISP_CLK_CTRL);
    msleep(10);

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

static int setup_channel_attrs(struct isp_framesource_state *fs, struct channel_attr *attr)
{
    // Store new attributes
    memcpy(&fs->attr, attr, sizeof(*attr));

    // For channel 1, ensure proper scaling setup
    if (fs->chn_num == 1) {
        fs->attr.crop.enable = 0;
        fs->attr.crop.width = 1920;
        fs->attr.crop.height = 1080;
        fs->attr.scaler.enable = 1;
        fs->attr.scaler.outwidth = 640;
        fs->attr.scaler.outheight = 360;
    }

    return 0;
}

// And add buffer setup in open
int framechan_open(struct inode *inode, struct file *file) {
    struct isp_framesource_state *fs;
    int minor = iminor(inode);
    uint32_t base_offset = 0x1094d4;  // From decompiled code

    // Initialize the channel's private data
    fs = &ourISPdev->frame_sources[minor];
    if (!fs->is_open) {
        struct frame_source_channel *fc;

        // Allocate channel data
        fc = kzalloc(sizeof(*fc), GFP_KERNEL);
        if (!fc)
            return -ENOMEM;

        fs->width = 1920;
        fs->height = 1080;
        fs->buf_cnt = 4;
        fs->buf_size = fs->width * fs->height * 2;

        // Map memory with proper offsets
        fs->buf_base = ourISPdev->dma_buf + base_offset +
                      (minor * fs->buf_size * fs->buf_cnt);
        fs->dma_addr = ourISPdev->dma_addr + base_offset +
                      (minor * fs->buf_size * fs->buf_cnt);

        // Initialize channel data
        fc->buf_base = fs->buf_base;
        fc->dma_addr = fs->dma_addr;
        fc->buf_size = fs->buf_size;
        fc->state = 1;  // Ready state
        fc->channel_offset = minor * (fs->buf_size * fs->buf_cnt);

        fs->fc = fc;
        fs->is_open = 1;
    }

    // Important: Save fs pointer
    file->private_data = fs;

    pr_info("Opened framechan%d: fs=%p base=%p\n",
            minor, fs, fs->buf_base);

    return 0;
}


// Helper function to calculate proper buffer sizes with alignment
static uint32_t calculate_buffer_size(uint32_t width, uint32_t height, uint32_t format) {
    // Base size for NV12 format
    uint32_t y_stride = ALIGN(width, 64);  // Align stride to 64 bytes
    uint32_t y_size = y_stride * ALIGN(height, 2);  // Align height to 2
    uint32_t uv_stride = y_stride;
    uint32_t uv_size = uv_stride * (ALIGN(height, 2) / 2);

    // Total size with alignment
    uint32_t total_size = y_size + uv_size;
    return ALIGN(total_size, 4096);  // Page align the total buffer size
}

static int framechan_qbuf(struct frame_source_channel *fc, unsigned long arg)
{
    struct v4l2_buffer v4l2_buf;
    unsigned long flags;
    int ret = 0;

    if (copy_from_user(&v4l2_buf, (void __user *)arg, sizeof(v4l2_buf))) {
        dev_err(fc->dev, "Failed to copy from user\n");
        return -EFAULT;
    }

//    /* Validate buffer type */
//    if (v4l2_buf.type != fc->buf_type) {
//        dev_err(fc->dev, "qbuf: invalid buffer type\n");
//        return -EINVAL;
//    }
//
//    /* Validate buffer index */
//    if (v4l2_buf.index >= fc->queue.buffer_count) {
//        dev_err(fc->dev, "qbuf: buffer index out of range\n");
//        return -EINVAL;
//    }
    //  TODO Finish this
    return 0;
}


static int framechan_reqbufs(struct frame_source_channel *fc, unsigned long arg)
{
    struct v4l2_requestbuffers req;
    int i;

    if (copy_from_user(&req, (void __user *)arg, sizeof(req)))
        return -EFAULT;

    pr_info("TODO: framechan_reqbufs\n");
    return 0;
}

static int framechan_streamon(struct frame_source_channel *fc, unsigned long arg)
{
    int ret;
    u32 val;

   	pr_info("TODO: framechan_streamon\n");
	// TODO
    /* Configure CSI interface */
//    ret = configure_mipi_csi(fc->isp);
//    if (ret) {
//        dev_err(fc->dev, "Failed to configure MIPI CSI: %d\n", ret);
//        fc->state = FC_STATE_ERROR;
//        return ret;
//    }

    // TODO
    /* Clear any pending interrupts */
//    writel(0xffffffff, fc->regs + ISP_INT_CLEAR_REG);
//    wmb();
//
//    /* Get first buffer in queue */
//    struct frame_buffer *buf = list_first_entry(&fc->queue.active_queue,
//                                              struct frame_buffer, list);
//
//    /* Setup DMA with first buffer */
//    writel(buf->dma_addr, fc->regs + ISP_BUF0_OFFSET);
//    writel(buf->size, fc->regs + ISP_BUF0_OFFSET + 0x4);
//    wmb();

    // TODO
    /* Calculate register offsets based on channel */
//    u32 ctrl_offset = ISP_STREAM_CTRL + (fc->channel_id * 0x100);
//    u32 start_offset = ISP_STREAM_START + (fc->channel_id * 0x100);
//
//    /* Enable stream processing */
//    writel(0x1, fc->regs + ctrl_offset);
//    wmb();
//    msleep(1); /* Required delay between ctrl and start */
//    writel(0x1, fc->regs + start_offset);
//    wmb();
//
//    /* Update channel state */
//    fc->state = FC_STATE_STREAMING;
//    fc->queue.streaming = 1;
//    fc->frame_count = 0;
//    fc->sequence = 0;
//
//    /* Enable interrupts */
//    val = readl(fc->regs + ISP_INT_MASK_REG);
//    val |= ISP_INT_FRAME_MASK(fc->channel_id);
//    writel(val, fc->regs + ISP_INT_MASK_REG);
//    wmb();
//
//    dev_info(fc->dev, "Channel %d streaming enabled: ctrl=0x%x start=0x%x\n",
//             fc->channel_id, readl(fc->regs + ctrl_offset),
//             readl(fc->regs + start_offset));

    return 0;
}

static int framechan_streamoff(struct frame_source_channel *fc, unsigned long arg)
{
    unsigned long flags;
    struct frame_buffer *buf, *tmp;
    u32 val;

 	pr_info("TODO: framechan_streamoff\n");
    return 0;
}

static int framechan_dqbuf(struct frame_source_channel *fc, unsigned long arg)
{
    return -1;
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
    struct frame_source_channel *fc = file->private_data;
    int ret = 0;
    pr_info("Frame channel IOCTL: cmd=0x%x arg=0x%lx\n", cmd, arg);

    mutex_lock(&fc->lock);

    switch (cmd) {
    case 0xc0145608: // VIDIOC_REQBUFS
        ret = framechan_reqbufs(fc, arg);
        break;
    case 0xc044560f: // VIDIOC_QBUF
        ret = framechan_qbuf(fc, arg);
        break;
    case 0xc0445611: // VIDIOC_DQBUF
        ret = framechan_dqbuf(fc, arg);
        break;
    case 0x80045612: // VIDIOC_STREAMON
        ret = framechan_streamon(fc, arg);
        break;
    case VIDIOC_STREAMOFF:
        ret = framechan_streamoff(fc, arg);
        break;
    // ... other commands ...
    default:
        dev_dbg(fc->dev, "Unhandled ioctl cmd: 0x%x\n", cmd);
        ret = -ENOTTY;
    }

    mutex_unlock(&fc->lock);
    return ret;
}

static int framechan_release(struct inode *inode, struct file *file)
{
    struct isp_framesource_state *fs = file->private_data;
    struct frame_source_channel *fc;

    if (!fs)
        return 0;

    fc = fs->fc;
    if (fc) {
        // Clean up channel data
        if (fs->state == 2) {
            // Disable streaming first
            writel(0, ourISPdev->reg_base + ISP_STREAM_START);
            writel(0, ourISPdev->reg_base + ISP_STREAM_CTRL);
            wmb();
            fs->state = 1;
        }

        kfree(fc);
        fs->fc = NULL;
    }

    fs->is_open = 0;
    return 0;
}

static struct file_operations framechan_fops = {
    .owner = THIS_MODULE,
    .open = framechan_open,
    .release = framechan_release,
    .unlocked_ioctl = framechan_ioctl,
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
  // TODO
//    atomic_set(&dev->core_state, 0);
//    atomic_set(&dev->vic_state, 0);
//    atomic_set(&dev->fw_state, 0);
//
//    spin_lock_init(&dev->state_lock);
//    spin_lock_init(&dev->vic_lock);

    return 0;
}



static int init_irq_handler(struct IMPISPDev *dev)
{
    int ret;

    dev->irq = platform_get_irq(to_platform_device(dev->dev), 0);
    if (dev->irq < 0)
        return dev->irq;

    ret = devm_request_irq(dev->dev, dev->irq, tx_isp_irq_handler,
                          IRQF_SHARED, "tx-isp", dev);
    if (ret) {
        dev_err(dev->dev, "Failed to request IRQ: %d\n", ret);
        return ret;
    }

    return 0;
}



/* Memory initialization */


static void cleanup_isp_subsystems(struct IMPISPDev *dev)
{
    /* Disable all interrupts */
    writel(0x0, dev->reg_base + ISP_INT_MASK_REG);
    wmb();

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


static int register_isp_subdevices(struct IMPISPDev *dev) {
    int ret;

    pr_info("Creating ISP subdevices...\n");

    // VIC subdevice
    dev->vic_pdev = platform_device_alloc("tx-isp-vic", 0);
    if (!dev->vic_pdev)
        return -ENOMEM;
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

static int tx_isp_vic_probe(struct platform_device *pdev)
{
    struct IMPISPDev *dev = platform_get_drvdata(pdev);

    pr_info("VIC probe called for device %p\n", dev);

    if (!dev || !dev->reg_base) {
        pr_err("Invalid device state in VIC probe\n");
        return -EINVAL;
    }

    // Map VIC registers at correct offset
    dev->vic_regs = dev->reg_base + VIC_BASE;

    // Initialize lock
    spin_lock_init(&dev->vic_lock);

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

static int tx_isp_probe(struct platform_device *pdev)
{
    struct IMPISPDev *dev = ourISPdev;
    int ret;
    struct resource *res;

    pr_info("ISP probe called\n");

    dev->dev = &pdev->dev;
    platform_set_drvdata(pdev, dev);

    // Get memory resource from platform device
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res) {
        dev_err(&pdev->dev, "No memory resource\n");
        return -ENODEV;
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

    /* Initialize core state */
    pr_info("Starting core state init\n");
    ret = init_core_state(dev);
    if (ret)
        goto err_free_dev;

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
        goto err_unmap_regs;

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

    /* Setup I2C and sensor */
    pr_info("Starting I2C init\n");
    ret = setup_i2c_adapter(dev);
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
    struct frame_source_channel *fc;
    struct isp_framesource_state *fs;
    int i;

    dev_info(dev->dev, "ISP device removal started\n");

    if (!dev)
        return 0;

    /* First stop all active channels */
    for (i = 0; i < MAX_CHANNELS; i++) {
        fs = &dev->frame_sources[i];
        fc = fs->fc;

        if (fc && fs->is_open) {
            /* Stop streaming if active */
            if (fc->state == FC_STATE_STREAMING) {
                writel(0x0, dev->reg_base + ISP_STREAM_START + (i * 0x100));
                writel(0x0, dev->reg_base + ISP_STREAM_CTRL + (i * 0x100));
                wmb();
            }

            /* Free channel structure */
            kfree(fc);
            fs->fc = NULL;
            fs->is_open = 0;
        }
    }

    tx_isp_cleanup_resources(dev);

    /* Free IRQ resources */
    if (dev->irq > 0) {
        free_irq(dev->irq, dev);
        dev->irq = 0;
    }

    /* Clean up reserved memory */
    if (dev->dma_buf) {
        dma_free_coherent(dev->dev, dev->dma_size,
                         dev->dma_buf, dev->dma_addr);
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
    ret = platform_driver_register(&tx_isp_vic_driver);
    if (ret) {
        pr_err("Failed to register VIC driver: %d\n", ret);
        goto err_destroy_device;
    }

    ret = platform_driver_register(&tx_isp_csi_driver);
    if (ret) {
        pr_err("Failed to register CSI driver: %d\n", ret);
        goto err_unreg_vic;
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
            sd->inpads[i].links_type = TX_ISP_PADLINK_CSI;
        else
            sd->inpads[i].links_type = TX_ISP_PADLINK_ISP | TX_ISP_PADLINK_VIC;
    }

    // Initialize each output pad
    for (int i = 0; i < sd->num_outpads; i++) {
        sd->outpads[i].sd = sd;
        sd->outpads[i].index = i;
        sd->outpads[i].type = TX_ISP_PADTYPE_OUTPUT;
        sd->outpads[i].state = TX_ISP_PADSTATE_FREE;

        // Set appropriate link types based on device
        if (!strcmp(pdev->name, "tx-isp-vic"))
            sd->outpads[i].links_type = TX_ISP_PADLINK_DDR | TX_ISP_PADLINK_VIC;
        else if (!strcmp(pdev->name, "tx-isp-csi"))
            sd->outpads[i].links_type = TX_ISP_PADLINK_CSI;
        else
            sd->outpads[i].links_type = TX_ISP_PADLINK_ISP | TX_ISP_PADLINK_VIC;
    }

    // Map to appropriate register space
    if (dev->reg_base) {
        if (!strcmp(pdev->name, "tx-isp-vic")) {
            sd->base = dev->reg_base + VIC_BASE;
            // VIC typically has 1 input pad and 2 output pads
            sd->num_inpads = 1;
            sd->num_outpads = 2;
        }
        else if (!strcmp(pdev->name, "tx-isp-csi")) {
            if (!dev->csi_dev || !dev->csi_dev->csi_regs) {
                pr_err("CSI registers not ready\n");
                return -EINVAL;
            }
            sd->base = dev->csi_dev->csi_regs;
            // CSI typically has 1 input pad and 1 output pad
            sd->num_inpads = 1;
            sd->num_outpads = 1;
        }
        else {
            // Frame source or other platform devices
            sd->base = dev->reg_base;
            // Default to 1 input and 1 output pad
            sd->num_inpads = 1;
            sd->num_outpads = 1;
        }
        pr_info("Using register base: %p\n", sd->base);
    }

    // Allocate and initialize input pads
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

    // Allocate and initialize output pads
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

    // Initialize the module
    ret = tx_isp_module_init(pdev, sd);
    if (ret) {
        pr_err("Failed to init module: %d\n");
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