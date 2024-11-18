#include <linux/printk.h>
#include <linux/sysfs.h>
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

#include <tx-isp-main.h>
#include <tx-isp-device.h>
#include <tx-isp-common.h>
#include <tx-isp-debug.h>
#include <isp_driver_common.h>



// Add these to your global variables
static struct proc_data *proc_data_fs = NULL;
static struct proc_data *proc_data_m0 = NULL;
static struct proc_data *proc_data_w00 = NULL;
static struct proc_data *proc_data_w01 = NULL;
static struct proc_data *proc_data_w02 = NULL;


// Add these structure definitions
struct isp_buffer_info {
    uint32_t method;          // Allocation method
    uint32_t buffer_start;    // Physical address
    uint32_t virt_addr;       // Changed from vaddr
    uint32_t buffer_size;     // Changed from size
    uint32_t flags;           // Flags
    uint32_t frame_count;     // Number of frames processed
    uint8_t is_buffer_full;   // Buffer full indicator
};

// Add this structure to track open instances
struct isp_instance {
    int fd;
    struct file *file;
    struct isp_framesource_state *fs;
    struct list_head list;
};

// Add global list to track all open instances
static LIST_HEAD(isp_instances);
static DEFINE_SPINLOCK(instances_lock);

static void __iomem *reg_base;
static uint32_t soc_id = 0xFFFFFFFF;
static uint32_t cppsr = 0xFFFFFFFF;
static uint32_t subsoctype = 0xFFFFFFFF;
static uint32_t subremark = 0xFFFFFFFF;
struct IMPISPDev *ourISPdev = NULL;
uint32_t globe_ispdev = 0x0;
static struct resource *mem_region;

static struct class *tisp_class;
static struct device *tisp_device;
static dev_t tisp_dev_number;

static DEFINE_IDA(soc_ida);
static DEFINE_SPINLOCK(soc_lock);


static const char *device_names[] = {
    "tx-isp",
    "isp-fs",
    "isp-m0",
    "cgu_isp",
    "isp-device",
    "isp-w00",
    "isp-w01",
    "isp-w02"
};

static struct proc_dir_entry *isp_proc_dir;
static struct proc_dir_entry *jz_proc_dir;
static struct proc_dir_entry *isp_graph_entry;
static struct isp_graph_data *global_graph_data;
static struct isp_device_status *g_dev_status = NULL;


static struct sensor_info registered_sensors[MAX_SENSORS];
static int num_registered_sensors = 0;


// Add to frame_source_channel structure
struct frame_queue frame_queue;

//
//// Private wrapper functions (example)
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

// Export the private wrappers for kernel functions
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
 * Function declarations for ISP register access
 */
static inline u32 isp_reg_read(struct isp_reg_t *regs, size_t offset)
{
    return readl((void __iomem *)regs + offset);
}

static inline void isp_reg_write(struct isp_reg_t *regs, size_t offset, u32 val)
{
    writel(val, (void __iomem *)regs + offset);
}


/* Initialization helper */
static inline void isp_device_init(struct isp_device *isp)
{
    mutex_init(&isp->link_lock);
    spin_lock_init(&isp->buffer_lock);
    spin_lock_init(&isp->irq_lock);
    INIT_LIST_HEAD(&isp->buffer_queue);
}

/**
 * isp_get_subdev - Get subdevice by index
 * @isp: ISP device
 * @index: Subdevice index
 *
 * Helper function to safely access subdevice array
 */
static inline struct isp_subdev *isp_get_subdev(struct isp_device *isp, int index)
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


// This needs to be 0x308 bytes total based on the channel indexing
struct frame_entry {
    uint32_t flags;          // +0x00
    uint32_t timestamp;      // +0x04
    uint32_t frame_size;     // +0x08
    uint32_t frame_type;     // +0x0C
    uint32_t frame_num;      // +0x10
    uint32_t frame_rate;     // +0x14
    uint32_t num_slices;     // +0x18
    void *slice_data;        // +0x1C
    uint8_t reserved[0x308 - 0x20]; // Pad to full size
} __attribute__((aligned(8)));

struct frame_buffer {
    struct frame_entry *entries;    // At 0x1094d4
    uint32_t num_entries;          // At 0x1094c0
    uint32_t write_idx;            // At 0x1094d8
    struct semaphore frame_sem;     // At 0x109418
    struct mutex lock;              // At 0x109438
    uint32_t channel_offset;       // Add channel base offset
    uint8_t padding[0x308 - sizeof(struct frame_entry*)]; // Match expected size
};

static void handle_frame_complete(struct isp_framesource_state *fs)
{
    struct frame_buffer *fb = fs->private;
    struct frame_entry *entry;
    unsigned long flags;
    struct timespec ts;

    spin_lock_irqsave(&fs->lock, flags);

    // Get next entry
    entry = &fb->entries[fb->write_idx % fb->num_entries];

    // Fill frame info
    entry->flags = 0x1;  // Mark as filled
    ktime_get_ts(&ts);  // Use 3.10 time API
    entry->timestamp = timespec_to_ns(&ts);
    entry->frame_size = fs->buf_size;
    entry->frame_type = 1; // I-frame
    entry->frame_num = fs->frame_cnt;
    entry->frame_rate = 30; // From sensor config
    entry->num_slices = 1;
    entry->slice_data = fs->buf_base + (fs->buf_index * fs->buf_size);

    // Update indices
    fb->write_idx++;
    fs->frame_cnt++;

    spin_unlock_irqrestore(&fs->lock, flags);

    // Signal new frame
    up(&fb->frame_sem);
}

static struct isp_framesource_state *get_fs_from_fd(int fd)
{
    struct isp_instance *instance;
    struct isp_framesource_state *fs = NULL;

    spin_lock(&instances_lock);
    list_for_each_entry(instance, &isp_instances, list) {
        if (instance->fd == fd) {
            fs = instance->fs;
            break;
        }
    }
    spin_unlock(&instances_lock);

    return fs;
}



// Add helper functions for buffer state management
static inline void mark_buffer_filled(struct frame_source_channel *fc, int index) {
    spin_lock(&fc->state_lock);
    set_bit(index, &fc->buffer_states);
    atomic_inc(&fc->frames_completed);
    spin_unlock(&fc->state_lock);
}

static inline void mark_buffer_empty(struct frame_source_channel *fc, int index) {
    spin_lock(&fc->state_lock);
    clear_bit(index, &fc->buffer_states);
    spin_unlock(&fc->state_lock);
}

static inline bool is_buffer_filled(struct frame_source_channel *fc, int index) {
    return test_bit(index, &fc->buffer_states);
}


static irqreturn_t isp_irq_handler(int irq, void *dev_id)
{
    struct IMPISPDev *dev = dev_id;
    struct isp_framesource_state *fs;
    struct frame_source_channel *fc;
    unsigned long flags;
    u32 status;
    static u32 frame_count = 0;

    status = readl(dev->regs + ISP_STATUS_REG);
    if (!(status & ISP_INT_FRAME_DONE))
        return IRQ_NONE;

    fs = &dev->frame_sources[0];
    fc = fs->private;

    if (!fc || !fc->buf_base)
        return IRQ_HANDLED;

    // Debug every 32nd frame (using power of 2 to avoid division)
    if (!(frame_count & 0x1F)) {
        pr_info("ISP: frame=%u status=0x%x idx=%d\n",
                frame_count,
                status,
                atomic_read(&fc->buffer_index));

        // Sample first 16 bytes of both Y and UV planes
        u8 *y_buf = fc->buf_base + (atomic_read(&fc->buffer_index) * fc->buf_size);
        u8 *uv_buf = y_buf + (fc->buf_size * 2/3);  // UV starts at 2/3 point for NV12

        pr_info("Y data: %02x %02x %02x %02x %02x %02x %02x %02x\n",
                y_buf[0], y_buf[1], y_buf[2], y_buf[3],
                y_buf[4], y_buf[5], y_buf[6], y_buf[7]);

        pr_info("UV data: %02x %02x %02x %02x %02x %02x %02x %02x\n",
                uv_buf[0], uv_buf[1], uv_buf[2], uv_buf[3],
                uv_buf[4], uv_buf[5], uv_buf[6], uv_buf[7]);
    }

    spin_lock_irqsave(&fc->state_lock, flags);

    // Handle buffer index
    int buf_idx = atomic_read(&fc->buffer_index);
    mark_buffer_filled(fc, buf_idx);

    // Update for next frame
    atomic_inc(&fc->buffer_index);
    if (atomic_read(&fc->buffer_index) >= fc->buf_cnt)
        atomic_set(&fc->buffer_index, 0);

    // Configure next buffer address
    buf_idx = atomic_read(&fc->buffer_index);
    uint32_t next_buf = fc->dma_addr + (buf_idx * fc->buf_size);
    writel(next_buf, dev->regs + ISP_BUF0_OFFSET);
    writel(next_buf + (fc->buf_size * 2/3),
           dev->regs + ISP_BUF0_OFFSET + ISP_BUF_SIZE_STEP);
    wmb();

    spin_unlock_irqrestore(&fc->state_lock, flags);

    wake_up(&fc->wait);
    frame_count++;
    return IRQ_HANDLED;
}



static irqreturn_t isp_irq_thread_handle(int irq, void* interrupt_data)
{
    // Ensure interrupt_data is valid
    if (interrupt_data == NULL)
        return IRQ_NONE;

    struct irq_handler_data *handler_data = (struct irq_handler_data *)interrupt_data;
    struct irq_task *current_task = handler_data->task_list;
    struct irq_task *buffer_start = handler_data->task_list;  // Assume first task as start

    // Iterate over tasks, executing task functions
    while (current_task != NULL)
    {
        // Check if the task is valid and has a function to execute
        if (current_task->task_function != NULL && current_task->status != 0)
        {
            current_task->task_function();
        }

        // Move to next task, or break if we've reached the buffer start
        if (current_task == buffer_start)
            break;
        current_task++;
    }

    return IRQ_HANDLED;
}



int32_t tx_isp_enable_irq(struct IspDevice* arg1)
{
    unsigned long flags = 0;  // Use unsigned long for flags
    spin_lock_irqsave(&arg1->lock, flags);  // Lock and save interrupt flags

    // Check if irq_handler is set and call it if non-null
    if (arg1->irq_handler != NULL)
    {
        arg1->irq_handler(arg1, flags);  // Call the IRQ handler function with the device context
    }

    spin_unlock_irqrestore(&arg1->lock, flags);  // Restore interrupt flags and release lock
    return 0;  // Return 0 for success
}


int32_t tx_isp_disable_irq(struct IspDevice* arg1)
{
    unsigned long flags = 0;  // Use unsigned long for flags
    spin_lock_irqsave(&arg1->lock, flags);  // Lock and save interrupt flags

    // Check if IRQ is enabled and if irq_handler is set, then call it
    if (arg1->irq_enabled && arg1->irq_handler != NULL)
    {
        arg1->irq_handler(arg1, flags);  // Call the IRQ handler function
    }

    spin_unlock_irqrestore(&arg1->lock, flags);  // Restore interrupt flags and release lock
    return 0;  // Return 0 for success
}

int32_t tx_isp_request_irq(struct irq_info* irqInfo, struct irq_handler_data* irqHandlerData)
{
    // Check if either struct pointer is NULL
    if (irqInfo == NULL || irqHandlerData == NULL)
    {
        isp_printf(2, "%s[%d] the parameters are invalid...", "tx_isp_request_irq");
        return 0xffffffea;  // Return error code for invalid parameters
    }

    // Get IRQ number using the platform-specific function
    int32_t irqNumber = platform_get_irq(irqInfo, 0);

    // Check if the IRQ number is valid
    if (irqNumber >= 0)
    {
        // Initialize the spinlock for the handler data struct
        spin_lock_init(irqHandlerData);

        // Request the IRQ and set up the threading handlers
        unsigned long irq_flags = (unsigned long)irqInfo->irq_type;
		if (request_threaded_irq(irqNumber, isp_irq_handler, isp_irq_thread_handle, irq_flags, "isp_driver", irqHandlerData) != 0)
        {
            // If IRQ request failed, print an error message and reset the handler data struct
            isp_printf(2, "%s[%d] Failed to request irq(%d)...", "tx_isp_request_irq", irqNumber);
            irqHandlerData->irq_number = 0;
            return 0xfffffffc;  // Return error code for failed IRQ request
        }

        // Set up the IRQ handler data struct
        irqHandlerData->handler_function = tx_isp_enable_irq;
        irqHandlerData->irq_number = irqNumber;
        irqHandlerData->disable_function = tx_isp_disable_irq;

        // Disable the IRQ initially
        tx_isp_disable_irq(irqHandlerData);
    }
    else
    {
        // If IRQ number is invalid, reset the handler data struct
        irqHandlerData->irq_number = 0;
    }

    return 0;  // Return success
}

struct isp_i2c_board_info {
    char type[32];          // Sensor type/name
    u16 addr;               // I2C device address
    unsigned short flags;    // I2C flags
    void *platform_data;    // Platform specific data
    int irq;                // IRQ if needed
};

/* Initialize sensor as I2C device */
/* Initialize sensor as I2C device */
static struct i2c_client *isp_i2c_new_sensor_device(struct i2c_adapter *adapter,
                                                   struct isp_i2c_board_info *info)
{
    struct i2c_board_info board_info;
    int ret = 0;

    /* Initialize board_info properly */
    memset(&board_info, 0, sizeof(board_info));
    strlcpy(board_info.type, info->type, I2C_NAME_SIZE);
    board_info.addr = info->addr;
    board_info.platform_data = info->platform_data;
    board_info.irq = info->irq;
    board_info.flags = info->flags;

    /* Try to load the sensor module first */
    request_module(info->type);

    /* Create new I2C device */
    struct i2c_client *client = i2c_new_device(adapter, &board_info);
    if (!client) {
        pr_err("Failed to create I2C device for %s\n", info->type);
        return NULL;
    }

    /* Verify the driver bound successfully */
    if (!client->dev.driver) {
        pr_err("No driver bound to %s sensor\n", info->type);
        i2c_unregister_device(client);
        return NULL;
    }

    return client;
}

struct i2c_client *isp_i2c_new_subdev_board(struct i2c_adapter *adapter,
                                            struct isp_i2c_board_info *info,
                                            void *client_data)
{
    struct i2c_client *client;
    int ret;

    if (!adapter || !info) {
        pr_err("Invalid adapter or board info\n");
        return NULL;
    }

    /* Create the I2C device */
    client = isp_i2c_new_sensor_device(adapter, info);
    if (!client) {
        pr_err("Failed to create sensor I2C device\n");
        return NULL;
    }

    /* Store any private data */
    i2c_set_clientdata(client, client_data);

    /* Perform basic sensor validation by reading a chip ID register */
    struct i2c_msg msgs[2];
    u8 reg_addr = 0x30; /* Chip ID register - adjust based on your sensor */
    u16 chip_id = 0;  // Make sure to zero-initialize

	// Prepare I2C messages to read the chip ID
	msgs[0].addr = client->addr;
	msgs[0].flags = 0;
	msgs[0].len = 1;
	msgs[0].buf = &reg_addr;

	// Read 2 bytes for the 16-bit chip ID
	msgs[1].addr = client->addr;
	msgs[1].flags = I2C_M_RD;
	msgs[1].len = 2; // Correct length to read both bytes
	msgs[1].buf = (u8 *)&chip_id;


    /* Perform the I2C transfer */
    ret = i2c_transfer(adapter, msgs, 2);
    if (ret != 2) {
        pr_err("Failed to read sensor chip ID from address 0x%02x\n", client->addr);
        i2c_unregister_device(client);
        return NULL;
    }

    /* Handle endianness if necessary */
    chip_id = be16_to_cpu(chip_id); // Convert from big-endian to CPU endianness if needed

    /* Print the chip ID */
    pr_info("Sensor %s initialized successfully at address 0x%02x with chip ID 0x%04x\n",
            info->type, info->addr, chip_id);

    /* Return the initialized client */
    return client;
}


/* Helper function to configure sensor via I2C */
int isp_sensor_write_reg(struct i2c_client *client, u16 reg, u8 val)
{
    struct i2c_msg msg;
    u8 buf[3];
    int ret;

    buf[0] = reg >> 8;    // Register address high byte
    buf[1] = reg & 0xFF;  // Register address low byte
    buf[2] = val;         // Value to write

    msg.addr = client->addr;
    msg.flags = 0;
    msg.len = 3;
    msg.buf = buf;

    ret = i2c_transfer(client->adapter, &msg, 1);
    if (ret != 1) {
        pr_err("Failed to write sensor register 0x%04x\n", reg);
        return -EIO;
    }

    return 0;
}

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

int32_t tx_isp_module_init(int32_t* arg1, void* arg2) {
    // Validate the parameters
    if (arg2 == NULL) {
        isp_printf(2, "%s the parameters are invalid!\n", "tx_isp_module_init");
        return 0xffffffea; // Invalid parameters
    }

    // Check if arg1 is valid
    if (arg1 != NULL) {
        int32_t a1 = arg1[0x16]; // Fetch value from arg1[0x16]

        // If value at arg1[0x16] is non-zero, copy 4 bytes from that address into arg2
        if (a1 != 0) {
            memcpy(arg2, (void*)a1, 4); // Assuming `a1` points to valid memory
        }
    }

    // Initialize values in arg2
    *(uint32_t*)((char*)arg2 + 0x78) = 0; // Set 0 at offset 0x78
    memset((char*)arg2 + 0x38, 0, 0x40); // Zero 0x40 bytes starting at offset 0x38
    *(uint32_t*)((char*)arg2 + 0x08) = *(uint32_t*)arg1; // Copy value from arg1 to offset 0x08 of arg2
    *(uint32_t*)((char*)arg2 + 0x7c) = (uint32_t)tx_isp_notify; // Set function pointer at offset 0x7c
    *(uint32_t*)((char*)arg2 + 0x30) = 0; // Set 0 at offset 0x30
    *(uint32_t*)((char*)arg2 + 0x34) = 0; // Set 0 at offset 0x34
    *(uint32_t*)((char*)arg2 + 0x04) = (uint32_t)&arg1[4]; // Set the address of arg1[4] at offset 0x04 of arg2

    return 0; // Success
}

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

struct IspDeviceConfig {
    char field_00[0x4];           // 0x00-0x04
    uint32_t field_04;            // 0x04: Points to sub device offset_10
    char *field_08;               // 0x08: Points to device name
    char padding_0c[0x2C];        // 0x0C-0x38: padding
    char field_38[0x40];          // 0x38-0x78: Zeroed by memset
    uint32_t field_78;            // 0x78: Set to 0
    uint32_t field_7c;            // 0x7c: Set to tx_isp_notify
    uint32_t misc_deregister_flag;// 0x80: Init to 0
    uint32_t field_34;            // 0x84: Set to 0
    // Add proper register and irq fields
    void __iomem *register_mapped_address;  // Register mapping
    void *memory_region;                    // Memory region pointer
    int32_t irq_handle;                    // IRQ handle
};


static int tx_isp_create_graph_and_nodes(struct isp_graph_data *graph_data)
{
    struct platform_device **device_list;
    unsigned int device_count;
    int i, ret = 0;

    if (!graph_data) {
        pr_err("Invalid graph data pointer\n");
        return -EINVAL;
    }

    // Direct structure member access instead of pointer arithmetic
    device_count = graph_data->device_count;
    device_list = graph_data->devices;

    // Validate inputs
    if (!device_list) {
        pr_err("Invalid device list pointer\n");
        return -EINVAL;
    }

    if (device_count > 10) {  // Use defined maximum
        pr_err("Invalid device count: %u\n", device_count);
        return -EINVAL;
    }

    pr_info("Creating ISP graph with %u devices\n", device_count);

    // Process each device
    for (i = 0; i < device_count; i++) {
        struct platform_device *pdev = device_list[i];
        void *drvdata;

        if (!pdev) {
            pr_debug("Skipping NULL device at index %d\n", i);
            continue;
        }

        drvdata = platform_get_drvdata(pdev);
        if (!drvdata) {
            pr_debug("No driver data for device %d\n", i);
            continue;
        }

        // Setup device node
        pr_debug("Processing device %d: %s\n", i, pdev->name);

        // If the device needs a misc device registration
        if (drvdata) {
            struct miscdevice *misc_dev = drvdata + sizeof(struct platform_device);
            ret = misc_register(misc_dev);
            if (ret) {
                pr_warn("Failed to register misc device for %s\n", pdev->name);
            }
        }
    }

    pr_info("ISP graph creation completed successfully\n");
    return 0;
}


// Individual device proc handlers
static int isp_fs_show(struct seq_file *m, void *v)
{
    int i;

    if (!ourISPdev) {
        seq_puts(m, "Error: ISP device not initialized\n");
        return 0;
    }

    for (i = 0; i < MAX_FRAMESOURCE_CHANNELS; i++) {
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
    seq_printf(m, " %d, %d\n", ourISPdev->wdr_mode ? 1 : 0, 0);  // Example values

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

    // Store at offset 0x9c in ourISPdev (matches libimp)
    ourISPdev->tuning_data = tuning_data;
    ourISPdev->tuning_state = 2;  // Set at offset 0xa8

    file->private_data = ourISPdev;
    return 0;
}

static int isp_m0_chardev_release(struct inode *inode, struct file *file)
{
    if (ourISPdev && ourISPdev->tuning_data) {
        kfree(ourISPdev->tuning_data);
        ourISPdev->tuning_data = NULL;
    }
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

            ourISPdev->tuning_enabled = enable;
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

static void remove_isp_proc_entries(void)
{
    if (isp_proc_dir) {
        // Remove our entries in reverse order of creation
        remove_proc_entry("isp-fs", isp_proc_dir);
        remove_proc_entry("isp-m0", isp_proc_dir);
        remove_proc_entry("isp-w00", isp_proc_dir);
        remove_proc_entry("isp-w01", isp_proc_dir);
        remove_proc_entry("isp-w02", isp_proc_dir);

        // Remove our directory
        remove_proc_entry("isp", jz_proc_dir);
        isp_proc_dir = NULL;
    }

    // Free our allocated memory
    if (proc_data_fs) kfree(proc_data_fs);
    if (proc_data_m0) kfree(proc_data_m0);
    if (proc_data_w00) kfree(proc_data_w00);
    if (proc_data_w01) kfree(proc_data_w01);
    if (proc_data_w02) kfree(proc_data_w02);
    if (g_dev_status) kfree(g_dev_status);
}


static int create_isp_proc_entries(struct isp_graph_data *graph_data)
{
    int ret = 0;

    /* First allocate and initialize all proc_data structures */
    proc_data_fs = kmalloc(sizeof(struct proc_data), GFP_KERNEL);
    if (!proc_data_fs) {
        pr_err("Failed to allocate proc_data_fs\n");
        return -ENOMEM;
    }
    proc_data_fs->show_func = isp_fs_show;
    proc_data_fs->private_data = &graph_data->dev_status;

    proc_data_m0 = kmalloc(sizeof(struct proc_data), GFP_KERNEL);
    if (!proc_data_m0) {
        pr_err("Failed to allocate proc_data_m0\n");
        ret = -ENOMEM;
        goto cleanup;
    }
    proc_data_m0->show_func = isp_m0_show;
    proc_data_m0->private_data = &graph_data->dev_status;

    proc_data_w00 = kmalloc(sizeof(struct proc_data), GFP_KERNEL);
    if (!proc_data_w00) {
        pr_err("Failed to allocate proc_data_w00\n");
        ret = -ENOMEM;
        goto cleanup;
    }
    proc_data_w00->show_func = isp_w00_show;
    proc_data_w00->private_data = &graph_data->dev_status;

    proc_data_w01 = kmalloc(sizeof(struct proc_data), GFP_KERNEL);
    if (!proc_data_w01) {
        pr_err("Failed to allocate proc_data_w01\n");
        ret = -ENOMEM;
        goto cleanup;
    }
    proc_data_w01->show_func = isp_w00_show;  // Using w00 show function as per original code
    proc_data_w01->private_data = &graph_data->dev_status;

    proc_data_w02 = kmalloc(sizeof(struct proc_data), GFP_KERNEL);
    if (!proc_data_w02) {
        pr_err("Failed to allocate proc_data_w02\n");
        ret = -ENOMEM;
        goto cleanup;
    }
    proc_data_w02->show_func = isp_w02_show;
    proc_data_w02->private_data = &graph_data->dev_status;

    /* Create the /proc/jz/isp directory */
    isp_proc_dir = proc_mkdir("jz/isp", NULL);
    if (!isp_proc_dir) {
        pr_err("Failed to create /proc/jz/isp directory\n");
        ret = -ENOENT;
        goto cleanup;
    }

    /* Now create the entries with their associated data */
    if (!proc_create_data("isp-fs", 0444, isp_proc_dir, &isp_fs_fops, proc_data_fs)) {
        pr_err("Failed to create /proc/jz/isp/isp-fs\n");
        ret = -ENOMEM;
        goto cleanup;
    }

    if (!proc_create_data("isp-m0", 0444, isp_proc_dir, &isp_m0_fops, proc_data_m0)) {
        pr_err("Failed to create /proc/jz/isp/isp-m0\n");
        ret = -ENOMEM;
        goto cleanup;
    }

    if (!proc_create_data("isp-w00", 0444, isp_proc_dir, &isp_w00_fops, proc_data_w00)) {
        pr_err("Failed to create /proc/jz/isp/isp-w00\n");
        ret = -ENOMEM;
        goto cleanup;
    }

    if (!proc_create_data("isp-w01", 0444, isp_proc_dir, &isp_w00_fops, proc_data_w01)) {
        pr_err("Failed to create /proc/jz/isp/isp-w01\n");
        ret = -ENOMEM;
        goto cleanup;
    }

    if (!proc_create_data("isp-w02", 0444, isp_proc_dir, &isp_w02_fops, proc_data_w02)) {
        pr_err("Failed to create /proc/jz/isp/isp-w02\n");
        ret = -ENOMEM;
        goto cleanup;
    }

    return 0;

cleanup:
    /* Cleanup: free allocated memory and remove created entries */
    if (proc_data_fs) kfree(proc_data_fs);
    if (proc_data_m0) kfree(proc_data_m0);
    if (proc_data_w00) kfree(proc_data_w00);
    if (proc_data_w01) kfree(proc_data_w01);
    if (proc_data_w02) kfree(proc_data_w02);

    if (isp_proc_dir) {
        remove_proc_entry("isp-fs", isp_proc_dir);
        remove_proc_entry("isp-m0", isp_proc_dir);
        remove_proc_entry("isp-w00", isp_proc_dir);
        remove_proc_entry("isp-w01", isp_proc_dir);
        remove_proc_entry("isp-w02", isp_proc_dir);
        remove_proc_entry("jz/isp", NULL);
    }
    return ret;
}

int tx_isp_subdev_init(struct platform_device *pdev, struct tx_isp_subdev *sd,
                      struct tx_isp_subdev_ops *ops)
{
    struct resource *mem_region;
    void __iomem *mapped_regs;
    int ret;

    if (!pdev || !sd) {
        isp_printf(2, "%s the parameters are invalid!\n", __func__);
        return -EINVAL;
    }

    // Initialize module and IRQ device parts
    spin_lock_init(&sd->irqdev.slock);
    sd->irqdev.irq = -1;  // Will be set later

    // Initialize basic members
    sd->res = NULL;
    sd->base = NULL;
    sd->clks = NULL;
    sd->clk_num = 0;
    sd->ops = ops;

    // Initialize expanded members
    sd->num_outpads = 0;
    sd->num_inpads = 0;
    sd->outpads = NULL;
    sd->inpads = NULL;
    sd->dev_priv = NULL;
    sd->host_priv = NULL;

    // Initialize the ISP module
    ret = tx_isp_module_init(pdev, sd);
    if (ret) {
        isp_printf(2, "Failed to init isp module!\n");
        return -EFAULT;
    }

    // Handle memory regions if needed
    if (pdev->resource && pdev->resource[0x16].start) {
        ret = tx_isp_request_irq(pdev, &sd->irqdev);
        if (ret) {
            isp_printf(2, "Failed to request irq!\n");
            tx_isp_module_deinit(sd);
            return ret;
        }

        mem_region = platform_get_resource(pdev, IORESOURCE_MEM, 0);
        if (!mem_region) {
            isp_printf(2, "Unable to get memory resource\n");
            ret = -ENODEV;
            goto err_free_irq;
        }

        mapped_regs = ioremap(mem_region->start, resource_size(mem_region));
        if (!mapped_regs) {
            isp_printf(2, "Unable to map registers\n");
            ret = -ENOMEM;
            goto err_free_irq;
        }

        // Store mapped registers and resource
        sd->base = mapped_regs;
        sd->res = mem_region;
    }

    return 0;

err_unmap:
    if (sd->base) {
        iounmap(sd->base);
        sd->base = NULL;
    }
    if (sd->res) {
        release_mem_region(sd->res->start, resource_size(sd->res));
        sd->res = NULL;
    }
err_free_irq:
    tx_isp_free_irq(&sd->irqdev);
    tx_isp_module_deinit(sd);
    return ret;
}
EXPORT_SYMBOL(tx_isp_subdev_init);


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

// Define the IRQHandler structure
struct IRQHandler
{
    int32_t irq_number;  // The IRQ number to be freed
};

void tx_isp_subdev_deinit(struct tx_isp_subdev *sd)
{
    int ret;
    void *memory_region;
    int32_t irq_number;
    struct isp_device_config *dev_config = NULL;

    if (!sd) {
        pr_err("NULL subdev in deinit\n");
        return;
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
        kfree(sd->inpads);
        sd->inpads = NULL;
    }

    if (sd->outpads) {
        kfree(sd->outpads);
        sd->outpads = NULL;
    }

    pr_info("Subdev %p cleanup complete\n", sd);
}
EXPORT_SYMBOL(tx_isp_subdev_deinit);

static void populate_device_list(struct isp_graph_data *graph_data, unsigned int max_devices)
{
    unsigned int i;
    struct platform_device *pdev;
    const int num_devices = sizeof(device_names) / sizeof(device_names[0]);
    unsigned int device_limit = max_devices < num_devices ? max_devices : num_devices;

    for (i = 0; i < device_limit; i++) {
        pdev = platform_device_register_simple(device_names[i], -1, NULL, 0);
        if (IS_ERR(pdev)) {
            dev_warn(graph_data->dev->dev, "Failed to register device: %s\n", device_names[i]);
            graph_data->devices[i] = NULL;
            continue;
        }
        graph_data->devices[i] = pdev;
        dev_info(graph_data->dev->dev, "Registered device: %s\n", device_names[i]);
    }
    graph_data->device_count = i;
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

// TODO: Define the AWB statistics structure
/* Function declarations for hardware-specific operations */
// int tisp_ae_algo_deinit(void);
//int tisp_awb_algo_deinit(void);



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
            pr_debug("%s: %s", func, buf);
            break;
        default:
            pr_info("%s: %s", func, buf);
    }
}

// Mock function for IMP_Log_Get_Option
static int IMP_Log_Get_Option(void) {

err_module_deinit:
    tx_isp_module_deinit(tisp_dev);
    kfree(tisp_dev);
    return -1;

return 0;
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

/* Read SoC information from hardware registers */
int read_soc_info(void)
{
    if (!reg_base)
        return -EINVAL;

    /* Read values from hardware registers */
    soc_id = readl(reg_base + (CPU_ID_ADDR - ISP_BASE_ADDR));
    cppsr = readl(reg_base + (CPPSR_ADDR - ISP_BASE_ADDR));
    subsoctype = readl(reg_base + (SUBTYPE_ADDR - ISP_BASE_ADDR));
    subremark = readl(reg_base + (SUBREMARK_ADDR - ISP_BASE_ADDR));

    /* Validate readings */
    if (soc_id == 0xFFFFFFFF || cppsr == 0xFFFFFFFF)
        return -EIO;

    pr_info("SoC ID: 0x%08x, CPPSR: 0x%08x\n", soc_id, cppsr);
    pr_info("Subtype: 0x%08x, Subremark: 0x%08x\n", subsoctype, subremark);

    return 0;  // Remove incorrect error path
}

void write_register(void *base, unsigned long reg, unsigned long value)
{
    writel(value, base + reg);
}

static int isp_device_registers_configure(struct IMPISPDev *dev)
{
    if (!dev || !dev->regs) {
        pr_err("Invalid hardware register base\n");
        return -EINVAL;
    }

    // Example of configuring hardware registers (adjust based on your ISP)
    // Set the ISP mode (capture, preview, etc.)
    write_register(dev->regs, ISP_MODE_REG, ISP_MODE_CAPTURE);

    // Configure input format (YUV420, RGB, etc.)
    write_register(dev->regs, ISP_INPUT_FORMAT_REG, ISP_FORMAT_YUV420);

    // Configure output format (YUV420, RGB, etc.)
    write_register(dev->regs, ISP_OUTPUT_FORMAT_REG, ISP_FORMAT_YUV420);

    // Enable hardware features (e.g., noise reduction, lens distortion correction)
    write_register(dev->regs, ISP_FEATURES_REG, ISP_FEATURE_NR | ISP_FEATURE_LDC);

    pr_info("ISP hardware registers configured successfully\n");

    return 0;
}

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


static int isp_device_pipeline_setup(struct isp_device *dev)
{
    if (!dev) {
        pr_err("ISP device is NULL\n");
        return -EINVAL;
    }

    // Set up the input and output pipelines
    dev->pipeline.input_format = ISP_FORMAT_YUV420;  // Input from sensor
    dev->pipeline.output_format = ISP_FMT_NV12;      // Output in NV12
    dev->pipeline.scaling_enabled = true;

    // Configure pipeline parameters
    dev->pipeline.input_width = 1920;
    dev->pipeline.input_height = 1080;
    dev->pipeline.output_width = 1920;
    dev->pipeline.output_height = 1080;

    pr_info("ISP pipeline configured: NV12 output format\n");

    return 0;
}

static int isp_device_interrupts_setup(struct IMPISPDev *dev)
{
    void __iomem *regs = dev->regs;
    u32 val;
    int ret;

    pr_info("Configuring ISP interrupts...\n");

    // 1. Clear any pending interrupts first
    writel(0xffffffff, regs + ISP_INT_CLEAR_REG);
    wmb();

    // 2. Enable the interrupts we want
    val = ISP_INT_FRAME_DONE;  // Frame complete interrupt
    writel(val, regs + ISP_INT_MASK_REG);
    wmb();

    // 3. Request IRQ with proper flags
    ret = request_threaded_irq(dev->irq,
                             isp_irq_handler,
                             NULL,
                             IRQF_SHARED,
                             "isp-irq",
                             dev);
    if (ret) {
        pr_err("Failed to request IRQ %d: %d\n", dev->irq, ret);
        return ret;
    }

    // 4. Read back and verify mask
    val = readl(regs + ISP_INT_MASK_REG);
    pr_info("IRQ configured: irq=%d mask=0x%x\n", dev->irq, val);

    // 5. Debug status
    val = readl(regs + ISP_STATUS_REG);
    pr_info("Initial status: 0x%x\n", val);

    return 0;
}


// In isp_device_memory_setup, remove buffer allocation:
static int isp_device_memory_setup(struct IMPISPDev *dev)
{
    if (!dev) {
        pr_err("ISP device is NULL\n");
        return -EINVAL;
    }

    // Only need to check if buffer info structure exists - actual buffer allocation
    // will be handled by userspace through IMP_Alloc()
    if (!dev->buf_info) {
        pr_err("Buffer info structure not initialized\n");
        return -EINVAL;
    }

    pr_info("ISP device memory setup ready for userspace buffers\n");
    return 0;
}



static int isp_device_configure(struct IMPISPDev *dev)
{
    int ret = 0;

    if (!dev) {
        pr_err("ISP device is NULL\n");
        return -EINVAL;
    }

    pr_info("Configuring ISP device...\n");

    // TODO
    // Set default width and height based on sensor specifications
    dev->width = 1920;  // Set this based on your sensor configuration
    dev->height = 1080; // Set this based on your sensor configuration

    // Step 1: Allocate and initialize buffer info structures
    if (!dev->buf_info) {
        dev->buf_info = kzalloc(sizeof(struct sensor_buffer_info), GFP_KERNEL);
        if (!dev->buf_info) {
            pr_err("Failed to allocate memory for buf_info\n");
            return -ENOMEM;
        }
        pr_info("Allocated buf_info at %p\n", dev->buf_info);
    }

    if (!dev->wdr_buf_info) {
        dev->wdr_buf_info = kzalloc(sizeof(struct sensor_control_info), GFP_KERNEL);
        if (!dev->wdr_buf_info) {
            pr_err("Failed to allocate memory for wdr_buf_info\n");
            kfree(dev->buf_info);
            return -ENOMEM;
        }
        pr_info("Allocated wdr_buf_info at %p\n", dev->wdr_buf_info);
    }

    // Step 2: Configure hardware registers
    ret = isp_device_registers_configure(dev);
    if (ret) {
        pr_err("Failed to configure ISP hardware registers\n");
        goto cleanup_buffers;
    }

    // Step 3: Set up memory buffers (e.g., DMA buffers)
    ret = isp_device_memory_setup(dev);
    if (ret) {
        pr_err("Failed to set up memory buffers for ISP device\n");
        goto cleanup_buffers;
    }

    // Step 4: Configure ISP pipeline (input/output configuration, scaling, etc.)
    ret = isp_device_pipeline_setup(dev);
    if (ret) {
        pr_err("Failed to set up ISP pipeline\n");
        goto cleanup_buffers;
    }

    // Step 5: Set up interrupts, if required
    ret = isp_device_interrupts_setup(dev);
    if (ret) {
        pr_err("Failed to set up interrupts for ISP device\n");
        goto cleanup_buffers;
    }

    // Mark the device as initialized
    pr_info("ISP device successfully configured\n");
    return 0; // Success

cleanup_buffers:
    kfree(dev->buf_info);
    kfree(dev->wdr_buf_info);
    return ret;
}


#define ISP_CTRL_REG        0x100
#define ISP_STATUS_REG      0x104
#define ISP_READY_BIT       0x01

/* Clock and power control registers based on T31 */
#define CPM_BASE            0x10000000
#define CPM_CLKGR           0x00000020
#define CPM_CLKGR1          0x00000028
#define CPM_ISP_CLK_GATE    BIT(3)
#define CPM_MIPI_CLK_GATE   BIT(4)

/* ISP specific registers */
#define ISP_CLK_CTRL        0x08
#define ISP_RST_CTRL        0x04
#define ISP_POWER_CTRL      0x0C
#define ISP_MIPI_CTRL       0x30

static int isp_power_on(struct IMPISPDev *dev)
{
    void __iomem *cpm_base;
    u32 val;

    /* Map CPM registers */
    cpm_base = ioremap(CPM_BASE, 0x100);
    if (!cpm_base) {
        pr_err("Failed to map CPM registers\n");
        return -ENOMEM;
    }

    /* Enable clocks */
    val = readl(cpm_base + CPM_CLKGR);
    val &= ~(CPM_ISP_CLK_GATE | CPM_MIPI_CLK_GATE);
    writel(val, cpm_base + CPM_CLKGR);

    /* Enable ISP power domain if needed */
    val = readl(cpm_base + CPM_CLKGR1);
    val &= ~BIT(2);  // ISP power domain bit
    writel(val, cpm_base + CPM_CLKGR1);

    /* Wait for power stabilization */
    usleep_range(1000, 2000);

    iounmap(cpm_base);
    return 0;
}

static int isp_reset_hw(struct IMPISPDev *dev)
{
    u32 val;

    /* Assert reset */
    writel(0x1, dev->regs + ISP_RST_CTRL);
    usleep_range(1000, 2000);

    /* De-assert reset */
    writel(0x0, dev->regs + ISP_RST_CTRL);
    usleep_range(1000, 2000);

    /* Enable core clock */
    val = readl(dev->regs + ISP_CLK_CTRL);
    val |= 0x1;
    writel(val, dev->regs + ISP_CLK_CTRL);

    /* Enable power */
    val = readl(dev->regs + ISP_POWER_CTRL);
    val |= 0x1;
    writel(val, dev->regs + ISP_POWER_CTRL);

    /* Wait for power stabilization */
    usleep_range(5000, 10000);

    return 0;
}

static int isp_hw_init(struct IMPISPDev *dev)
{
    void __iomem *ctrl_reg;
    void __iomem *status_reg;
    unsigned long timeout;
    u32 val;
    int ret;

    if (!dev || !dev->regs) {
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

    ctrl_reg = dev->regs + ISP_CTRL_REG;
    status_reg = dev->regs + ISP_STATUS_REG;

    /* Read initial values */
    val = readl(ctrl_reg);
    pr_info("Initial ctrl reg value: 0x%08x\n", val);

    /* Enable ISP core */
    writel(0x1, ctrl_reg);
    val = readl(ctrl_reg);
    pr_info("Read back value after enable: 0x%08x\n", val);

    /* Configure MIPI interface */
    val = readl(dev->regs + ISP_MIPI_CTRL);
    val |= 0x1;  // Enable MIPI interface
    writel(val, dev->regs + ISP_MIPI_CTRL);

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


static struct IMPISPDev *ourISPdev_bak = NULL;  // Backup for reopen

#define VBM_POOL_SIZE 0x99888
#define VBM_ALLOC_METHOD 0x3  // Seen in decompiled code

static int setup_frame_source(struct IMPISPDev *dev, int channel) {
    struct isp_framesource_state *fs = &dev->frame_sources[channel];
    struct frame_source_channel *fc;
    int ret;

    fc = kzalloc(sizeof(*fc), GFP_KERNEL);
    if (!fc)
        return -ENOMEM;

    pr_info("Frame source setup: dev=%p channel=%d\n", dev, channel);
    // Debug print of critical offset - matches libimp expectation
    pr_info("dev offset 0x20=0x%x\n",
            *((unsigned int*)((char*)dev + 0x20)));

    // Initialize channel state first
    fc->state = 1;  // Ready state
    fc->buf_cnt = 4;  // Fixed buffer count matching libimp
    fc->channel_offset = channel * 0x2e8;  // Critical offset from decompiled
    fc->write_idx = 0;

    // Get dimensions from sensor window size
    if (!dev->sensor_window_size) {
        pr_err("No sensor window size information\n");
        kfree(fc);
        return -EINVAL;
    }

    // Set format and dimensions from sensor
    fs->width = dev->sensor_window_size->width;
    fs->height = dev->sensor_window_size->height;
    fs->fmt = V4L2_PIX_FMT_NV12;  // Force NV12 format

    // Calculate NV12 buffer layout
    uint32_t y_stride = ALIGN(fs->width, 32);  // Y plane stride
    uint32_t uv_stride = ALIGN(fs->width/2, 32); // UV plane stride (half width)
    uint32_t y_size = y_stride * fs->height;
    uint32_t uv_size = (uv_stride * fs->height) / 2;  // UV is half height
    fs->buf_size = y_size + uv_size;

    // Critical: Use exact base offset from decompiled code
    uint32_t base_offset = 0x1094d4;

    // Calculate buffer addresses maintaining libimp layout
    fc->buf_base = dev->dma_buf + base_offset +
                   (channel * fc->buf_size * fc->buf_cnt);
    fc->dma_addr = dev->dma_addr + base_offset +
                   (channel * fc->buf_size * fc->buf_cnt);

    // Initialize synchronization - maintain order from original
    spin_lock_init(&fc->state_lock);
    atomic_set(&fc->frames_completed, 0);
    atomic_set(&fc->buffer_index, 0);
    fc->buffer_states = 0;
    init_waitqueue_head(&fc->wait);
    sema_init(&fc->sem, 0);  // Start with no frames
    mutex_init(&fc->lock);

    // Initialize frame source state
    fs->frame_cnt = 0;
    fs->buf_index = 0;
    fs->state = 1;  // Ready state
    fs->private = fc;

    pr_info("Frame source channel %d initialized:\n", channel);
    pr_info("  Resolution: %dx%d format=NV12\n", fs->width, fs->height);
    pr_info("  Buffer layout: y_stride=%d uv_stride=%d\n",
            y_stride, uv_stride);
    pr_info("  Buffer config: size=%u count=%d\n",
            fs->buf_size, fc->buf_cnt);
    pr_info("  Memory: base=%p dma=0x%x offset=0x%x\n",
            fc->buf_base, (uint32_t)fc->dma_addr, fc->channel_offset);
    pr_info("  Frame address calc: base+0x%x\n",
            base_offset + (channel * fc->buf_size * fc->buf_cnt));

    // Add validation check
    if ((uint32_t)fc->buf_base & 0x1F) {
        pr_warn("Warning: Buffer base 0x%p not 32-byte aligned\n",
                fc->buf_base);
    }

    return 0;
}

static int isp_frame_thread(void *data)
{
    struct isp_framesource_state *fs = data;
    struct frame_source_channel *fc;
    struct isp_buffer_info *curr_buf;
    int ret;

    if (!fs || !fs->private) {
        pr_err("Invalid frame source state\n");
        return -EINVAL;
    }

    fc = fs->private;

    while (!kthread_should_stop()) {
        // Wait for frame with timeout
        ret = wait_event_interruptible_timeout(
            fs->wait,
            fs->frame_cnt > fc->write_idx || kthread_should_stop(),
            msecs_to_jiffies(100));

        if (ret == 0)  // Timeout
            continue;
        if (ret == -ERESTARTSYS)
            break;

        // Lock access
        if (down_interruptible(&fc->sem))
            continue;

        mutex_lock(&fc->lock);

        // Get current buffer
        curr_buf = &fs->bufs[fs->buf_index];
        if (!curr_buf || !curr_buf->virt_addr) {
            mutex_unlock(&fc->lock);
            up(&fc->sem);
            continue;
        }

        // Update buffer state
        curr_buf->frame_count = fs->frame_cnt;
        curr_buf->is_buffer_full = 1;

        // Calculate addresses for current buffer
        void *src = fc->buf_base + (fs->buf_index * fc->buf_size);
        void *dst = (void *)curr_buf->virt_addr;

	    // Copy frame data based on format
	    if (fs->fmt == ISP_FMT_NV12) {
	        // NV12: Copy Y plane then UV plane
	        uint32_t y_size = fs->width * fs->height;
	        uint32_t uv_size = y_size / 2;

	        // Copy Y plane
	        memcpy(dst, src, y_size);
	        // Copy UV plane
	        memcpy(dst + y_size, src + y_size, uv_size);
	    } else if (fs->fmt == ISP_FMT_YUV422) {
	        // Keep YUV422 handling for backward compatibility
	        memcpy(dst, src, fc->buf_size);
	    }

        mutex_unlock(&fc->lock);
        up(&fc->sem);

        // Signal frame completion
        wake_up(&fs->wait);
    }

    return 0;
}


// Add these structures to match sensor expectations
struct sensor_win_size {
    uint32_t width;          // 0x00: Frame width
    uint32_t height;         // 0x04: Frame height
    uint32_t fps;           // 0x08: Frame rate
    uint32_t max_fps;       // 0x0C: Maximum frame rate
    uint32_t format;        // 0x10: Image format
};

static int setup_isp_buffers(struct IMPISPDev *dev)
{
    struct sensor_win_size *wsize = dev->sensor_window_size;
    struct isp_framesource_state *fs = &dev->frame_sources[0];
    struct frame_source_channel *fc;
    void __iomem *regs = dev->regs;

    if (!regs || !wsize || !fs) {
        pr_err("Invalid state for buffer setup\n");
        return -EINVAL;
    }

    fc = fs->private;
    if (!fc) {
        pr_err("No frame channel\n");
        return -EINVAL;
    }

    // Use frame channel configuration
    uint32_t width = ALIGN(fs->width, 32);
    uint32_t height = fs->height;
    uint32_t y_size = width * height;
    uint32_t y_stride = width;
    uint32_t uv_stride = width / 2;

    // Calculate exact buffer addresses
    uint32_t y_addr = fc->dma_addr;
    uint32_t uv_addr = y_addr + y_size;

    pr_info("Setting up ISP NV12 buffers ch%d:\n"
            "  Dimensions: %dx%d\n"
            "  Y: addr=0x%x stride=%d\n"
            "  UV: addr=0x%x stride=%d\n",
            fs->chn_num, width, height,
            y_addr, y_stride,
            uv_addr, uv_stride);

    // Configure Y plane DMA
    writel(y_addr, regs + ISP_BUF0_OFFSET);
    writel(y_stride, regs + ISP_BUF0_OFFSET + 0x4);
    wmb();

    // Configure UV plane DMA
    writel(uv_addr, regs + ISP_BUF0_OFFSET + 0x8);
    writel(uv_stride, regs + ISP_BUF0_OFFSET + 0xc);
    wmb();

    return 0;
}


static int start_frame_source(struct IMPISPDev *dev, int channel) {
    struct isp_framesource_state *fs = &dev->frame_sources[channel];
    struct frame_source_channel *fc = fs->private;
    void __iomem *regs = dev->regs;
    int ret;

    if (!fs || !fc || !fs->is_open || !regs) {
        pr_err("Invalid frame source state\n");
        return -EINVAL;
    }

    pr_info("Starting frame source on channel %d:\n"
            "  fs=%p fc=%p regs=%p\n"
            "  dma_addr=0x%x buf_base=%p\n"
            "  buf_size=%u count=%d\n",
            channel, fs, fc, regs,
            (unsigned int)fc->dma_addr, fc->buf_base,
            fc->buf_size, fc->buf_cnt);

    // Validate buffer setup
    if (!fc->dma_addr || !fc->buf_base || !fc->buf_size) {
        pr_err("Invalid buffer configuration\n");
        return -EINVAL;
    }

    // Configure base buffer register
    writel(fc->dma_addr, regs + ISP_BUF0_OFFSET);
    writel(fc->buf_size, regs + ISP_BUF0_OFFSET + 0x4);
    wmb();

    // Debug print register values
    pr_info("Buffer registers:\n"
            "  BUF0: addr=0x%x size=0x%x\n",
            readl(regs + ISP_BUF0_OFFSET),
            readl(regs + ISP_BUF0_OFFSET + 0x4));

    // Configure additional buffers
    for (int i = 1; i < fc->buf_cnt; i++) {
        unsigned int reg_offset = ISP_BUF0_OFFSET + (i * ISP_BUF_SIZE_STEP);
        writel(fc->dma_addr + (i * fc->buf_size), regs + reg_offset);
        writel(fc->buf_size, regs + reg_offset + 0x4);
    }
    wmb();

    // Enable frame processing
    writel(0x1, regs + ISP_CTRL_REG);

    // Initialize buffer queue
    INIT_LIST_HEAD(&fs->ready_queue);
    INIT_LIST_HEAD(&fs->done_queue);
    fs->fifo_depth = 0;
    fs->frame_depth = 1;
    fs->fifo_initialized = true;

    // Initialize buffer descriptors
    for (int i = 0; i < fs->buf_cnt; i++) {
        fs->bufs[i].frame_count = 0;
        fs->bufs[i].is_buffer_full = 0;
    }

    // Update state
    fs->state = 2;  // Running state
    fs->flags |= 0x2;
    fs->frame_cnt = 0;
    fs->buf_index = 0;

    pr_info("Frame source started successfully\n"
            "  state=%d flags=0x%x\n"
            "  ctrl_reg=0x%x status=0x%x\n",
            fs->state, fs->flags,
            readl(regs + ISP_CTRL_REG),
            readl(regs + ISP_STATUS_REG));

    return 0;
}

static void cleanup_frame_source(struct IMPISPDev *dev, int channel)
{
    struct isp_framesource_state *fs = &dev->frame_sources[channel];
    struct frame_source_channel *fc;

    if (!fs || !fs->is_open)
        return;

    // Get private data
    fc = fs->private;
    if (!fc)
        return;

    // Stop thread first if running
    if (fs->thread) {
        kthread_stop(fs->thread);
        fs->thread = NULL;
    }

    // Wait for any pending operations
    if (fs->state == 2) {
        fs->state = 1;
        msleep(50);  // Allow operations to complete
    }

    mutex_lock(&fc->lock);

    // Clear buffer states
    if (fs->bufs) {
        int i;
        for (i = 0; i < fs->buf_cnt; i++) {
            fs->bufs[i].frame_count = 0;
            fs->bufs[i].is_buffer_full = 0;
            fs->bufs[i].buffer_start = 0;
            fs->bufs[i].virt_addr = 0;
        }
    }

    // Reset frame source state
    fs->frame_cnt = 0;
    fs->buf_index = 0;
    fs->state = 0;

    mutex_unlock(&fc->lock);

    // Free resources
    if (fs->bufs) {
        kfree(fs->bufs);
        fs->bufs = NULL;
    }

    kfree(fc);
    fs->private = NULL;
    fs->is_open = 0;
}


static int init_frame_source(struct IMPISPDev *dev, int channel) {
    struct isp_framesource_state *fs = &dev->frame_sources[channel];
    struct frame_source_channel *fc;
    uint32_t type_flags = 0;

    pr_info("Initializing frame source channel %d\n", channel);

    // Clear structure
    memset(fs, 0, sizeof(*fs));

    // Important: Set type flags at 0x58 offset
    // For channel 0, this should be 0
    // For other channels, set a flag
    if (channel != 0) {
        type_flags = 1;  // Non-zero for other channels
    }
    *(uint32_t *)((char *)dev + 0x58 + (channel * sizeof(uint32_t))) = type_flags;

    // Allocate aligned channel data
    fc = kzalloc(sizeof(*fc), GFP_KERNEL | GFP_DMA);
    if (!fc) {
        pr_err("Failed to allocate channel data\n");
        return -ENOMEM;
    }

    fs->chn_num = channel;
    if (channel == 0) {
        fs->width = 1920;
        fs->height = 1080;
    } else {
        fs->width = 640;  // Default for non-zero channels
        fs->height = 360;
    }

    // Important: Different buffer setup for different channels
    fs->buf_cnt = (channel == 0) ? 4 : 2;  // Fewer buffers for non-zero channels
    fs->buf_size = fs->width * fs->height * 2;  // YUV422

    // Calculate proper offsets based on channel
    uint32_t aligned_offset = ALIGN(ISP_FRAME_BUFFER_OFFSET +
                                  (channel * fs->buf_size * fs->buf_cnt), 8);

    fs->buf_base = dev->dma_buf + aligned_offset;
    fs->dma_addr = dev->dma_addr + aligned_offset;

    // Initialize channel data
    fc->buf_base = fs->buf_base;
    fc->dma_addr = fs->dma_addr;
    fc->buf_size = fs->buf_size;
    fc->buf_cnt = fs->buf_cnt;
    fc->state = 1;  // Ready state

    // Initialize synchronization primitives
    mutex_init(&fc->lock);
    spin_lock_init(&fc->queue_lock);
    init_waitqueue_head(&fc->wait);
    atomic_set(&fc->frame_count, 0);

    fs->private = fc;
    fs->is_open = 1;

    // Debug print to see our type flags
    pr_info("Channel %d initialized: %dx%d, buffers=%d size=%d type_flags=0x%x\n",
            channel, fs->width, fs->height, fs->buf_cnt, fs->buf_size, type_flags);

    // Also dump the actual value at the offset
    pr_info("Channel %d: value at 0x58 offset: 0x%x\n", channel,
            *(uint32_t *)((char *)dev + 0x58 + (channel * sizeof(uint32_t))));

    return 0;
}

static atomic_t isp_instance_counter = ATOMIC_INIT(0);

static int tisp_open(struct file *file)
{
    struct isp_instance *instance;
    int ret = 0;
    int fd;

    pr_info("ISP device open called from pid %d\n", current->pid);

    if (!ourISPdev) {
        pr_err("ISP device not initialized\n");
        return -ENODEV;
    }

    // Reset stream count on open
    ourISPdev->stream_count = 0;

    // Initialize frame source first
    ret = init_frame_source(ourISPdev, 0);
    if (ret) {
        pr_err("Failed to initialize frame source: %d\n", ret);
        return ret;
    }

    // Allocate instance tracking
    instance = kzalloc(sizeof(*instance), GFP_KERNEL);
    if (!instance)
        return -ENOMEM;

    // Get frame source pointer
    struct isp_framesource_state *fs = &ourISPdev->frame_sources[0];
    if (!fs || !fs->is_open) {
        pr_err("Frame source not properly initialized\n");
        kfree(instance);
        return -EINVAL;
    }

    // Initialize instance data
    instance->fd = fd = get_unused_fd_flags(O_CLOEXEC);
    if (fd < 0) {
        kfree(instance);
        return fd;
    }

    instance->file = file;
    instance->fs = fs;

    // IMPORTANT: Set file->private_data to instance, not fs
    file->private_data = instance;  // Change this line

    // Add to global list with lock protection
    spin_lock(&instances_lock);
    list_add_tail(&instance->list, &isp_instances);
    spin_unlock(&instances_lock);

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
        if (dev->buf_info) {
            // Just free the info structure - buffer is managed by IMP_Alloc
            kfree(dev->buf_info);
            dev->buf_info = NULL;
        }

        if (dev->wdr_buf_info) {
            kfree(dev->wdr_buf_info);
            dev->wdr_buf_info = NULL;
        }
    }
}



static int stop_streaming(struct IMPISPDev *dev)
{
    struct i2c_client *client = dev->sensor_i2c_client;

    // Stop sensor streaming
    if (client)
        isp_sensor_write_reg(client, SENSOR_REG_MODE, 0x00);

    // Disable ISP streaming
    writel(0x00, dev->regs + ISP_CTRL_REG);

    // Disable MIPI receiver
    writel(0x00, dev->regs + ISP_MIPI_CTRL);

    pr_info("Streaming stopped\n");
    return 0;
}

static void cleanup_isp_memory(struct IMPISPDev *dev) {
    if (dev->dma_buf) {
        iounmap(dev->dma_buf);
        dev->dma_buf = NULL;
    }

    if (dev->buf_info) {
        kfree(dev->buf_info);
        dev->buf_info = NULL;
    }

    dev->dma_addr = 0;
    dev->dma_size = 0;

    isp_mem.initialized = false;
    isp_mem.virt_addr = NULL;
    isp_mem.phys_addr = 0;
    isp_mem.size = 0;
}
// Add cleanup for registers
static void cleanup_isp_registers(struct IMPISPDev *dev)
{
    if (dev && dev->regs) {
        // Disable streaming
        writel(0x0, dev->regs + ISP_CTRL_OFFSET);

        // Unmap registers
        iounmap(dev->regs);
        dev->regs = NULL;
    }
}

static void cleanup_frame_buffers(struct isp_framesource_state *fs)
{
    if (!fs)
        return;

    if (fs->buf_base) {
        iounmap(fs->buf_base);
        fs->buf_base = NULL;
    }

    if (fs->bufs) {
        kfree(fs->bufs);
        fs->bufs = NULL;
    }

    fs->dma_addr = 0;
    fs->buf_cnt = 0;
    fs->buf_size = 0;
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

static int setup_isp_subdevs(struct IMPISPDev *dev)
{
    struct isp_subdev_state *sd;
    struct isp_pad_desc *src_pad, *sink_pad;
    int ret;

    pr_info("Setting up ISP subdevices\n");

    // Allocate subdev state
    sd = kzalloc(sizeof(*sd), GFP_KERNEL);
    if (!sd) {
        pr_err("Failed to allocate subdev\n");
        return -ENOMEM;
    }

    // Initialize subdev
    snprintf(sd->name, sizeof(sd->name), "isp-subdev");
    sd->regs = dev->regs;
    mutex_init(&sd->lock);

    // Allocate pads
    sd->num_src_pads = 1;
    sd->num_sink_pads = 1;

    src_pad = kzalloc(sizeof(*src_pad), GFP_KERNEL);
    sink_pad = kzalloc(sizeof(*sink_pad), GFP_KERNEL);
    if (!src_pad || !sink_pad) {
        ret = -ENOMEM;
        goto err_free;
    }

    // Initialize source pad
    src_pad->type = 1;  // Source
    src_pad->flags = PAD_FL_SOURCE;
    src_pad->link_state = LINK_STATE_INACTIVE;
    src_pad->entity = dev;

    // Initialize sink pad
    sink_pad->type = 2;  // Sink
    sink_pad->flags = PAD_FL_SINK;
    sink_pad->link_state = LINK_STATE_INACTIVE;
    sink_pad->entity = dev;

    sd->src_pads = src_pad;
    sd->sink_pads = sink_pad;

    // Store in device
    dev->subdevs = kzalloc(sizeof(struct isp_subdev *), GFP_KERNEL);
    if (!dev->subdevs) {
        ret = -ENOMEM;
        goto err_free_pads;
    }
    dev->subdevs[0] = (struct isp_subdev *)sd;

    pr_info("Subdev setup complete:\n");
    pr_info("  sd=%p src_pad=%p sink_pad=%p\n", sd, src_pad, sink_pad);

    return 0;

err_free_pads:
    kfree(src_pad);
    kfree(sink_pad);
err_free:
    kfree(sd);
    return ret;
}

static struct isp_link_config link_configs[] = {
    [0] = {
        .pads = &(struct isp_pad_desc){
            .type = 1,  // Source pad
            .link_state = LINK_STATE_SOURCE,
            .flags = PAD_FL_SOURCE
        },
        .num_pads = 2  // Matches 0x6ad80 entry
    },
    [1] = {
        .pads = &(struct isp_pad_desc){
            .type = 2,  // Sink pad
            .link_state = LINK_STATE_SOURCE,
            .flags = PAD_FL_SINK
        },
        .num_pads = 2  // Matches 0x6ad80 entry
    }
};

struct isp_link_type {
    uint32_t type;      // Link type flags
    uint32_t pad_count;
};

static const struct isp_link_type link_types[] = {
    [0] = { .type = 0x3, .pad_count = 2 },  // Direct path - both source/sink flags
    [1] = { .type = 0x3, .pad_count = 2 }   // Bypass path - both source/sink flags
};




/**
 * setup_video_link - Configure video link routing
 * @dev: IMPISPDev structure
 * @link: Link identifier from userspace
 *
 * Based on decompiled code at case 0x800456d0
 */
static int setup_video_link(struct IMPISPDev *dev, int link_num)
{
    struct isp_subdev_state *sd;
    struct isp_pad_desc *src_pad, *sink_pad;
    int ret = 0;

    pr_info("Setting up video link %d\n", link_num);

    if (!dev || !dev->subdevs || !dev->subdevs[0]) {
        pr_err("No subdevs initialized\n");
        return -EINVAL;
    }

    sd = (struct isp_subdev_state *)dev->subdevs[0];
    src_pad = sd->src_pads;
    sink_pad = sd->sink_pads;

    if (!src_pad || !sink_pad) {
        pr_err("Invalid pad configuration\n");
        return -EINVAL;
    }

    pr_info("Configuring link:\n");
    pr_info("  sd=%p src=%p sink=%p\n", sd, src_pad, sink_pad);
    pr_info("  src flags=0x%x sink flags=0x%x\n",
            src_pad->flags, sink_pad->flags);

    mutex_lock(&sd->lock);

    // Reset existing links
    src_pad->link_state = LINK_STATE_INACTIVE;
    sink_pad->link_state = LINK_STATE_INACTIVE;
    src_pad->source = NULL;
    src_pad->sink = NULL;
    sink_pad->source = NULL;
    sink_pad->sink = NULL;

    // Setup new link
    if (link_num == 0) {
        src_pad->sink = sink_pad;
        sink_pad->source = src_pad;
        src_pad->link_state = LINK_STATE_SOURCE;
        sink_pad->link_state = LINK_STATE_SOURCE;

        // Configure hardware path
        writel(1, dev->regs + 0x140);  // Direct path
        writel(0, dev->regs + 0x144);  // Disable bypass
    } else {
        src_pad->sink = NULL;
        sink_pad->source = NULL;

        // Configure bypass path
        writel(0, dev->regs + 0x140);  // Disable direct
        writel(1, dev->regs + 0x144);  // Enable bypass
    }
    wmb();

    dev->current_link = link_num;

    mutex_unlock(&sd->lock);

    pr_info("Video link %d configured successfully\n", link_num);
    return 0;
}

struct isp_subdev_link {
    struct isp_subdev *subdev;
    uint32_t flags;
    uint32_t state;
    void *source;
    void *sink;
};

// Initialize frame queue in init_frame_source
static int init_frame_queue(struct frame_source_channel *fc)
{
    spin_lock_init(&fc->queue_lock);
    INIT_LIST_HEAD(&fc->ready_list);
    INIT_LIST_HEAD(&fc->done_list);
    init_waitqueue_head(&fc->wait);
    atomic_set(&fc->frame_count, 0);
    fc->max_frames = fc->buf_cnt;

    return 0;
}

// Add buffer management functions
static struct frame_node *alloc_frame_node(struct frame_source_channel *fc)
{
    struct frame_node *node;

    node = kzalloc(sizeof(*node), GFP_KERNEL);
    if (!node)
        return NULL;

    node->magic = 0x336ac;
    node->frame_size = fc->buf_size;
    node->data = fc->buf_base + (fc->write_idx * fc->buf_size);
    INIT_LIST_HEAD(&node->list);

    return node;
}

static void free_frame_node(struct frame_node *node)
{
    if (node) {
        list_del(&node->list);
        kfree(node);
    }
}


int isp_sensor_read_reg(struct i2c_client *client, u16 reg, u8 *val)
{
    struct i2c_msg msgs[2];
    u8 buf[2];
    int ret;

    buf[0] = reg >> 8;
    buf[1] = reg & 0xFF;

    msgs[0].addr = client->addr;
    msgs[0].flags = 0;
    msgs[0].len = 2;
    msgs[0].buf = buf;

    msgs[1].addr = client->addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = 1;
    msgs[1].buf = val;

    ret = i2c_transfer(client->adapter, msgs, 2);
    if (ret != 2) {
        pr_err("Failed to read sensor register 0x%04x\n", reg);
        return -EIO;
    }

    return 0;
}


static int debug_sensor_registers(struct IMPISPDev *dev)
{
    struct i2c_client *client = dev->sensor_i2c_client;
    u8 val;
    int ret;

    pr_info("SC2336 Register Dump:\n");

    // Read critical registers
    ret = isp_sensor_read_reg(client, 0x0100, &val);
    pr_info("Streaming (0x0100): 0x%02x\n", val);

    ret |= isp_sensor_read_reg(client, 0x3031, &val);
    pr_info("Format control (0x3031): 0x%02x\n", val);

    // Test pattern control - IMPORTANT
    ret |= isp_sensor_read_reg(client, 0x4501, &val);
    pr_info("Test pattern (0x4501): 0x%02x\n", val);

    // Output format & timing
    ret |= isp_sensor_read_reg(client, 0x3108, &val);
    pr_info("Output format (0x3108): 0x%02x\n", val);

    ret |= isp_sensor_read_reg(client, 0x3200, &val);
    pr_info("Output width high: 0x%02x\n", val);

    ret |= isp_sensor_read_reg(client, 0x3201, &val);
    pr_info("Output width low: 0x%02x\n", val);

    return ret;
}

static void debug_isp_registers(struct IMPISPDev *dev)
{
    void __iomem *regs = dev->regs;

    pr_info("ISP Register State:\n");
    pr_info("CTRL: 0x%08x\n", readl(regs + ISP_CTRL_REG));
    pr_info("STATUS: 0x%08x\n", readl(regs + ISP_STATUS_REG));
    pr_info("Stream Control: 0x%08x\n", readl(regs + ISP_STREAM_CTRL));
    pr_info("Stream Start: 0x%08x\n", readl(regs + ISP_STREAM_START));

    // Buffer configuration
    pr_info("Buffer 0: addr=0x%08x size=0x%08x\n",
            readl(regs + ISP_BUF0_OFFSET),
            readl(regs + ISP_BUF0_OFFSET + 0x4));

    // Format configuration
    pr_info("Input format: 0x%08x\n", readl(regs + ISP_INPUT_FORMAT_REG));
    pr_info("Output format: 0x%08x\n", readl(regs + ISP_OUTPUT_FORMAT_REG));
}


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

    if (fs->private) {
        struct frame_source_channel *fc = fs->private;
        pr_info("  DMA: addr=0x%08x base=%p\n",
                (u32)fc->dma_addr, fc->buf_base);
    }
}


static int enable_isp_streaming(struct IMPISPDev *dev, struct file *file, int channel, bool enable) {
    struct isp_framesource_state *fs;
    struct frame_source_channel *fc;
    void __iomem *regs = dev->regs;
    int ret = 0;

    pr_info("Stream control: channel=%d enable=%d\n", channel, enable);

    if (!dev || !regs) {
        pr_err("Invalid device state\n");
        return -EINVAL;
    }

    fs = file->private_data;
    if (!fs || !fs->is_open) {
        pr_err("No frame source state\n");
        return -EINVAL;
    }
    fc = fs->private;

    if (enable) {
        // 1. Configure sensor first
        struct i2c_client *client = dev->sensor_i2c_client;

        // Before starting stream
        pr_info("Pre-stream state:\n");
        pr_info("  IRQ mask: 0x%08x\n", readl(regs + ISP_INT_MASK_REG));
        pr_info("  Status: 0x%08x\n", readl(regs + ISP_STATUS_REG));
        pr_info("  MIPI status: 0x%08x\n", readl(regs + ISP_MIPI_STATUS));
        pr_info("  MIPI timing: 0x%08x\n", readl(regs + ISP_MIPI_TIMING));

        // Force RAW10 mode and specific timing
        isp_sensor_write_reg(client, 0x3031, 0x0a); // RAW10
        isp_sensor_write_reg(client, 0x3018, 0x72); // MIPI timing
        isp_sensor_write_reg(client, 0x3019, 0x00);
        isp_sensor_write_reg(client, 0x301a, 0xf0);

        // Configure resolution
        isp_sensor_write_reg(client, 0x3200, 0x07); // Width high
        isp_sensor_write_reg(client, 0x3201, 0x80); // Width low (1920)
        isp_sensor_write_reg(client, 0x3202, 0x04); // Height high
        isp_sensor_write_reg(client, 0x3203, 0x38); // Height low (1080)

        // 2. Configure MIPI CSI
        writel(0x0, regs + ISP_MIPI_CTRL); // Reset
        wmb();
        msleep(10);

        // MIPI timing - critical for proper data capture
        writel((0x8 << 24) | (0x10 << 16) | (0x8 << 8) | 0x3,
               regs + ISP_MIPI_TIMING);

        // Enable MIPI with proper configuration
        writel(0x80000103, regs + ISP_MIPI_CTRL); // Enable, 2 lanes
        wmb();
        msleep(10);

        // 3. Configure DMA with proper NV12 layout
        uint32_t y_stride = ALIGN(fs->width, 32);
        uint32_t uv_stride = ALIGN(fs->width/2, 32);
        uint32_t y_size = y_stride * fs->height;
        uint32_t uv_size = (uv_stride * fs->height) / 2;

        // Set DMA addresses for Y and UV planes
        writel(fc->dma_addr, regs + ISP_BUF0_OFFSET);
        writel(y_stride, regs + ISP_BUF0_OFFSET + 0x4);
        writel(fc->dma_addr + y_size, regs + ISP_BUF0_OFFSET + 0x8);
        writel(uv_stride, regs + ISP_BUF0_OFFSET + 0xc);
        wmb();

        // 4. Enable ISP processing - sequence matters
        writel(0x1, regs + ISP_CTRL_REG); // Enable core
        wmb();
        msleep(10);

        writel(0x1, regs + ISP_STREAM_CTRL + (channel * 0x100));
        wmb();
        msleep(10);

        writel(0x1, regs + ISP_STREAM_START + (channel * 0x100));
        wmb();

                // Clear any pending interrupts first
        writel(0xFFFFFFFF, regs + ISP_INT_CLEAR_REG);
        wmb();

        // 5. Finally start sensor streaming
        isp_sensor_write_reg(client, 0x0100, 0x01);
        msleep(20); // Let sensor stabilize

        // Update state
        fs->state = 2;
        fs->flags |= 0x2;


        // After enabling stream
        writel(0x1, regs + ISP_CTRL_REG);
        wmb();

        // Debug after enable
        pr_info("Post-stream state:\n");
        pr_info("  IRQ mask: 0x%08x\n", readl(regs + ISP_INT_MASK_REG));
        pr_info("  Status: 0x%08x\n", readl(regs + ISP_STATUS_REG));
        pr_info("  MIPI status: 0x%08x\n", readl(regs + ISP_MIPI_STATUS));

        // Debug register state
        pr_info("Stream enabled:\n"
                "  MIPI ctrl=0x%x status=0x%x\n"
                "  ISP ctrl=0x%x stream=0x%x\n"
                "  DMA Y: addr=0x%x stride=%d\n"
                "  DMA UV: addr=0x%x stride=%d\n",
                readl(regs + ISP_MIPI_CTRL),
                readl(regs + ISP_MIPI_STATUS),
                readl(regs + ISP_CTRL_REG),
                readl(regs + ISP_STREAM_CTRL),
                readl(regs + ISP_BUF0_OFFSET),
                readl(regs + ISP_BUF0_OFFSET + 0x4),
                readl(regs + ISP_BUF0_OFFSET + 0x8),
                readl(regs + ISP_BUF0_OFFSET + 0xc));

    } else {
        // Disable stream in reverse order
        isp_sensor_write_reg(dev->sensor_i2c_client, 0x0100, 0x00);

        writel(0x0, regs + ISP_STREAM_START + (channel * 0x100));
        writel(0x0, regs + ISP_STREAM_CTRL + (channel * 0x100));
        writel(0x0, regs + ISP_CTRL_REG);
        writel(0x0, regs + ISP_MIPI_CTRL);
        wmb();

        fs->state = 1;
        fs->flags &= ~0x2;
    }

    return 0;
}

static int tisp_release(struct inode *inode, struct file *file)
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

        // Remove from global list
        spin_lock(&instances_lock);
        list_del(&instance->list);
        spin_unlock(&instances_lock);

        // Clean up fd
        if (instance->fd >= 0) {
            put_unused_fd(instance->fd);
        }

        kfree(instance);
    }

    // Clean up frame source state
    if (ourISPdev) {
        for (int channel = 0; channel < MAX_FRAMESOURCE_CHANNELS; channel++) {
            cleanup_frame_source(ourISPdev, channel);
        }
    }

    file->private_data = NULL;
    return 0;
}



// Update tisp_read function
static ssize_t tisp_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
    pr_debug("Read operation\n");
    return 0;  // Remove incorrect error path
}

static ssize_t tisp_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
    pr_debug("Write operation\n");
    return count;
}


/**
 * find_subdev_link_pad - Helper to find a pad by type and index
 * @dev: IMPISPDev structure
 * @pad: Pad descriptor to match against
 *
 * Based on decompiled implementation at 0xd368
 */
static struct isp_pad_desc *find_subdev_link_pad(struct IMPISPDev *dev,
                                                struct isp_pad_desc *pad)
{
    struct isp_subdev **subdev_list;
    struct isp_subdev *subdev;
    uint8_t *name1, *name2;
    uint8_t pad_type, pad_index;

    if (!dev || !pad)
        return NULL;

    // Start at subdev list offset 0x38 from device base
    subdev_list = (struct isp_subdev **)(dev + 0x38);

    // Iterate through subdev list until offset 0x78
    while (subdev_list < (struct isp_subdev **)(dev + 0x78)) {
        subdev = *subdev_list;

        if (subdev) {
            // Compare subdev names - offset 0x8 contains name
            name1 = *(uint8_t **)(subdev + 0x8);
            name2 = *(uint8_t **)pad;

            // String comparison loop
            while (*name1 && *name2 && (*name1 == *name2)) {
                name1++;
                name2++;
            }

            // If names match
            if (*name1 == *name2) {
                pad_type = *(uint8_t *)(pad + 4); // offset from decompiled +4
                pad_index = *(uint8_t *)(pad + 5); // offset from decompiled +5

                if (pad_type == 1) {
                    // Source pad lookup
                    uint16_t num_src_pads = *(uint16_t *)(subdev + 0xca);
                    if (pad_index < num_src_pads) {
                        uint32_t pad_base = *(uint32_t *)(subdev + 0xd0);
                        return (struct isp_pad_desc *)(pad_base + (pad_index * 0x24));
                    }
                }
                else if (pad_type == 2) {
                    // Sink pad lookup
                    uint16_t num_sink_pads = *(uint16_t *)(subdev + 0xc8);
                    if (pad_index < num_sink_pads) {
                        uint32_t pad_base = *(uint32_t *)(subdev + 0xcc);
                        return (struct isp_pad_desc *)(pad_base + (pad_index * 0x24));
                    }
                }

                pr_err("Can't find the matched pad!\n");
                return NULL;
            }
        }

        subdev_list++;
    }

    return NULL;
}

/**
 * subdev_video_destroy_link - Destroy an existing video link
 * Implementation of function called at 0xe3dc in decompiled
 */
static int subdev_video_destroy_link(struct isp_pad_desc *pad)
{
    if (!pad)
        return -EINVAL;

    /* Clear link state */
    pad->link_state = LINK_STATE_INACTIVE;
    pad->source = NULL;
    pad->sink = NULL;

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
 * struct isp_wdr_buf_size - WDR buffer size information
 * Used for TW_ISP_WDR_GET_BUF ioctl
 */
struct isp_wdr_buf_size {
    __u32 size;        /* Required buffer size */
    __u32 tsize;       /* Total size needed */
};

struct wdr_reg_info {
    uint32_t width;      // offset 0x124 in decompiled
    uint32_t height;     // offset 0x128 in decompiled
    uint32_t wdr_mode;   // offset 0x90 in decompiled
    uint32_t frame_size; // offset 0xe8 in decompiled
};

/* Magic number from decompiled code at 0xef64 */
#define AE_ALGO_MAGIC   0x336ac

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
 */
static void isp_disable_wdr(struct isp_device *isp)
{
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
    regs = (struct isp_reg_t *)dev->regs;  // Cast the void* regs to proper type

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


/* WDR register offsets within main register block */
#define ISP_WDR_WIDTH_OFFSET    0x124  /* From decompiled code */
#define ISP_WDR_HEIGHT_OFFSET   0x128
#define ISP_WDR_MODE_OFFSET     0x90
#define ISP_WDR_FRAME_OFFSET    0xe8

/**
 * setup_wdr_buffers - Configure WDR buffer memory regions
 * @isp: ISP device structure
 * @buf_info: WDR Buffer information from userspace
 *
 * Based on decompiled code at case 0x800856d6
 */
int setup_wdr_buffers(struct isp_device *isp, struct isp_wdr_buf_info *buf_info)
{
    uint32_t required_size;
    uint32_t line_width;
    uint32_t height;
    struct isp_reg_t *regs = isp->regs;  // Use main register block

    /* Debug print matching decompiled code */
    dev_dbg(isp->dev, "%s:%d::tsize is %d, buf info size: %d\n",
            __func__, __LINE__, required_size, buf_info->size);

    /* Get WDR mode from registers */
    uint32_t wdr_mode = isp_reg_read(regs, ISP_WDR_MODE_OFFSET);

    /* Calculate required buffer size based on WDR mode */
    switch (wdr_mode) {
    case WDR_MODE_LINE:
        /* Line-interleaved WDR mode */
        line_width = isp_reg_read(regs, ISP_WDR_WIDTH_OFFSET) << 1;
        height = isp_reg_read(regs, ISP_WDR_HEIGHT_OFFSET);
        required_size = line_width * height;
        break;

    case WDR_MODE_FRAME:
        /* Frame-based WDR mode */
        required_size = isp_reg_read(regs, ISP_WDR_FRAME_OFFSET);
        line_width = isp_reg_read(regs, ISP_WDR_WIDTH_OFFSET) << 1;
        height = required_size / line_width;
        break;

    default:
        /* Not in WDR mode */
        dev_err(isp->dev, "Not in WDR mode, buffer setup not needed\n");
        return -EINVAL;
    }

    /* Verify buffer size is sufficient */
    if (buf_info->size < required_size) {
        dev_err(isp->dev, "%s,%d: buf size too small\n", __func__, __LINE__);

        /* Clear WDR mode and disable WDR */
        isp_reg_write(regs, ISP_WDR_MODE_OFFSET, WDR_MODE_NONE);
        isp_disable_wdr(isp);

        return -EINVAL;
    }

    /* Program WDR buffer registers */
    system_reg_write(WDR_REG_BUF, buf_info->addr);
    system_reg_write(WDR_REG_LINE, line_width);
    system_reg_write(WDR_REG_HEIGHT, height);

    dev_dbg(isp->dev, "WDR buffer setup complete: addr=0x%08x, line=%d, height=%d\n",
            buf_info->addr, line_width, height);

    return 0;
}



static int validate_buffer_setup(struct IMPISPDev *dev) {
    if (!dev || !dev->buf_info) {
        pr_err("Invalid buffer setup\n");
        return -EINVAL;
    }

    if (!dev->buf_info->buffer_start || !dev->buf_info->buffer_size) {
        pr_err("Buffer not initialized\n");
        return -EINVAL;
    }

    return 0;
}

/* Global state tracking */
static uint32_t g_wdr_en;           /* Global WDR enable state at 0xa2ea4 */
static uint32_t data_84b6c = 0;     /* Constant from decompiled code */

/**
 * tisp_s_wdr_en - Enable/disable WDR mode and configure related modules
 * Based on decompiled function at 0x64824
 *
 * @arg1: WDR enable flag (1=enable, 0=disable)
 */
static int tisp_s_wdr_en(uint32_t arg1)
{
    u32 reg_val;

    /* Set up initial registers - matches 0x6486c */
    system_reg_write(0x24, system_reg_read(0x24) | 1);

    /* Wait for hardware ready - matches do-while at 0x64880 */
    while (!(system_reg_read(0x28) & 1))
        ;

    /* Toggle control bit - matches 0x64888-0x648a8 */
    reg_val = system_reg_read(0x20);
    system_reg_write(0x20, reg_val | 4);
    system_reg_write(0x20, reg_val & ~4);

    /* Get and modify top register - matches 0x648b0 */
    reg_val = system_reg_read(0xc);

    /* Debug print matching decompiled at 0x648e4 */
    isp_printf(0, "%s:%d::wdr en is %d,top is 0x%x\n",
            "tisp_s_wdr_en", __LINE__, arg1, reg_val);

    u32 new_val;
    u32 addr_val;

    if (arg1 != 1) {
        /* Disable WDR - matches 0x64958-0x64968 */
        new_val = (reg_val & 0xb577ff7d) | 0x34000009;
        new_val |= (data_84b6c << 7);  // data_84b6c would need to be defined
        addr_val = 0x1c;
        g_wdr_en = 0;  // global variable seen at 0x6495c
    } else {
        /* Enable WDR - matches 0x6490c-0x64934 */
        new_val = (reg_val & 0xa1ffdf76) | 0x880002;
        addr_val = 0x10;
        g_wdr_en = arg1;
    }

    /* Program control registers - matches 0x6496c-0x64978 */
    system_reg_write(0x804, addr_val);
    system_reg_write(0xc, new_val);

    /* Configure all subsystems - matches 0x64988-0x64a48 */
//    tisp_dpc_wdr_en(arg1);
//    tisp_lsc_wdr_en(arg1);
//    tisp_gamma_wdr_en(arg1);
//    tisp_sharpen_wdr_en(arg1);
//    tisp_ccm_wdr_en(arg1);
//    tisp_bcsh_wdr_en(arg1);
//    tisp_rdns_wdr_en(arg1);
//    tisp_adr_wdr_en(arg1);
//    tisp_defog_wdr_en(arg1);
//    tisp_mdns_wdr_en(arg1);
//    tisp_dmsc_wdr_en(arg1);
//    tisp_ae_wdr_en(arg1);
//    tisp_sdns_wdr_en(arg1);

    /* Initialize additional modules - matches 0x64a58-0x64a68 */
//    tiziano_clm_init();
//    tiziano_ydns_init();

    /* Final control register - matches 0x64a74 */
    system_reg_write(0x800, 1);

    /* Final debug print matching decompiled at 0x64a90 */
    isp_printf(0, "%s:%d::wdr en is %d,top is 0x%x\n",
            "tisp_s_wdr_en", __LINE__, arg1, new_val);

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


static int sensor_setup_streaming(struct IMPISPDev *dev)
{
    struct i2c_client *client = dev->sensor_i2c_client;
    u8 val;
    int ret = 0;

    pr_info("SC2336: Setting up sensor streaming...\n");

    if (!client) {
        pr_err("No sensor I2C client\n");
        return -ENODEV;
    }

    // Read sensor ID again to verify communication
    ret = isp_sensor_read_reg(client, 0x3107, &val);
    pr_info("SC2336: ID high byte: 0x%02x\n", val);
    ret |= isp_sensor_read_reg(client, 0x3108, &val);
    pr_info("SC2336: ID low byte: 0x%02x\n", val);

    // Add detailed sensor register setup with verification
    const struct {
        u16 reg;
        u8 val;
        const char *desc;
    } sensor_init_regs[] = {
        {0x0103, 0x01, "Software reset"},
        {0x0100, 0x00, "Stream off for init"},
        {0x3018, 0x72, "Timing control"},
        {0x3019, 0x00, "Frame length control"},
        {0x301a, 0xf0, "Line length control"},
        {0x301c, 0x30, "Clock settings"},
        {0x3032, 0xa0, "Digital control"},
        {0x3106, 0x05, "Output control"},
        {0x3600, 0x00, "Analog control"},
        {0x3601, 0x55, "Analog control"},
        // Add format control registers
        {0x3031, 0x0a, "Output format control - RAW10"},
        {0x3820, 0x00, "Sensor timing"},
        {0x3821, 0x00, "Sensor timing"},
        // Add more registers based on datasheet
    };

    for (int i = 0; i < ARRAY_SIZE(sensor_init_regs); i++) {
        ret = isp_sensor_write_reg(client, sensor_init_regs[i].reg,
                                 sensor_init_regs[i].val);
        if (ret) {
            pr_err("Failed to set %s (0x%04x)\n",
                   sensor_init_regs[i].desc, sensor_init_regs[i].reg);
            return ret;
        }

        // Verify write
        ret = isp_sensor_read_reg(client, sensor_init_regs[i].reg, &val);
        if (ret || val != sensor_init_regs[i].val) {
            pr_err("Register verify failed: %s (0x%04x) expected 0x%02x got 0x%02x\n",
                   sensor_init_regs[i].desc, sensor_init_regs[i].reg,
                   sensor_init_regs[i].val, val);
            return -EIO;
        }
    }

    return ret;
}


// Add these defines for consistent memory values
#define ISP_MAGIC_METHOD    0x203a726f
#define ISP_MAGIC_PHYS      0x33326373
#define ISP_INIT_PHYS       0x1
#define ISP_FINAL_PHYS      0x02a80000
#define ISP_ALLOC_SIZE      0x2a80000
#define ISP_ALLOC_KMALLOC   1

// Add these debug helper functions
static void dump_memory_info(struct IMPISPDev *dev) {
    pr_info("ISP Memory Info:\n");
    pr_info("  DMA Buffer: %p\n", dev->dma_buf);
    pr_info("  DMA Physical: 0x%08x\n", (uint32_t)dev->dma_addr);
    pr_info("  DMA Size: %zu\n", dev->dma_size);
    if (dev->buf_info) {
        pr_info("  Buffer Info:\n");
        pr_info("    Method: 0x%x\n", dev->buf_info->method);
        pr_info("    Physical: 0x%08x\n", dev->buf_info->buffer_start);
        pr_info("    Virtual: 0x%08x\n", (uint32_t)dev->buf_info->virt_addr);
        pr_info("    Size: %u\n", dev->buf_info->buffer_size);
    }
}



static int init_isp_registers(struct IMPISPDev *dev)
{
    void __iomem *base;

    pr_info("Initializing ISP registers\n");

    // Map ISP registers
    base = ioremap(ISP_REG_BASE, ISP_REG_SIZE);
    if (!base) {
        pr_err("Failed to map ISP registers\n");
        return -ENOMEM;
    }

    dev->regs = base;

    // Initialize control registers
    writel(0x0, dev->regs + ISP_CTRL_OFFSET);  // Reset control
    writel(0x1, dev->regs + ISP_INIT_OFFSET);  // Initialize

    // Set up configuration
    writel(0x0, dev->regs + ISP_CONF_OFFSET);

    // Read back status to verify
    u32 status = readl(dev->regs + ISP_STAT_OFFSET);
    pr_info("ISP status after init: 0x%08x\n", status);

    // Configure DMA addresses
    if (dev->dma_addr) {
        writel(dev->dma_addr, dev->regs + ISP_BUF0_REG);
        writel(dev->dma_addr + dev->dma_size/2, dev->regs + ISP_BUF1_REG);
        pr_info("Configured DMA buffers:\n");
        pr_info("  Buffer 0: 0x%08x\n", dev->dma_addr);
        pr_info("  Buffer 1: 0x%08x\n", dev->dma_addr + dev->dma_size/2);
    }

    return 0;
}

static int start_streaming(struct IMPISPDev *dev)
{
    int ret;

    // Validate device state
    if (!dev || !dev->regs || !dev->dma_buf) {
        pr_err("Invalid device state for streaming\n");
        return -EINVAL;
    }

    // Initialize ISP registers
    ret = init_isp_registers(dev);
    if (ret) {
        pr_err("Failed to initialize ISP registers\n");
        return ret;
    }

    // Setup sensor
    ret = sensor_setup_streaming(dev);
    if (ret) {
        pr_err("Failed to setup sensor streaming\n");
        return ret;
    }

    // Enable streaming - write to critical registers
    writel(0x1, dev->regs + ISP_CTRL_OFFSET);

    pr_info("Streaming started successfully\n");
    return 0;
}



static int configure_sensor_streaming(struct IMPISPDev *dev)
{
    struct i2c_client *client = dev->sensor_i2c_client;
    u8 val;
    int ret;

    pr_info("SC2336: Full sensor initialization sequence...\n");

    // Software reset
    ret = isp_sensor_write_reg(client, 0x0103, 0x01);
    msleep(20);  // Important delay after reset

    // Core initialization sequence for SC2336
    const struct {
        u16 reg;
        u8 val;
    } init_regs[] = {
        // Timing control & PLL settings
        {0x301f, 0x01},  // Framing mode
        {0x3038, 0x44},  // SC2336 specific timing
        {0x3253, 0x0c},  // Control register
        {0x3301, 0x04},  // AEC settings
        {0x3304, 0x60},  // Manual gain control
        {0x3306, 0x70},  // Analog control
        {0x330b, 0xc0},  // Exposure control
        {0x3333, 0x10},  // Fine integration time
        {0x3364, 0x56},  // Analog control

        // Format & Resolution (1920x1080)
        {0x3200, 0x07},  // Output width high byte
        {0x3201, 0x80},  // Output width low byte (1920)
        {0x3202, 0x04},  // Output height high byte
        {0x3203, 0x38},  // Output height low byte (1080)

        // Data format - RAW10
        {0x3031, 0x0a},  // RAW10 format
        {0x3018, 0x72},  // MIPI configuration

        // Frame timing
        {0x320c, 0x08},  // HTS high byte
        {0x320d, 0xca},  // HTS low byte
        {0x320e, 0x05},  // VTS high byte
        {0x320f, 0xa0},  // VTS low byte

        // Initial exposure & gain
        {0x3e01, 0x8c},  // Exposure high byte
        {0x3e02, 0x60},  // Exposure low byte
        {0x3e08, 0x03},  // Gain high byte
        {0x3e09, 0x10},  // Gain low byte
    };

    // Apply initialization sequence
    for (int i = 0; i < ARRAY_SIZE(init_regs); i++) {
        ret = isp_sensor_write_reg(client, init_regs[i].reg, init_regs[i].val);
        if (ret) {
            pr_err("Failed to write reg 0x%04x: %d\n", init_regs[i].reg, ret);
            return ret;
        }
        // Add small delay between writes
        udelay(10);
    }

    // Verify key registers
    isp_sensor_read_reg(client, 0x3031, &val);
    pr_info("Format control: 0x%02x\n", val);

    isp_sensor_read_reg(client, 0x320e, &val);
    pr_info("VTS high: 0x%02x\n", val);

    isp_sensor_read_reg(client, 0x3e08, &val);
    pr_info("Gain high: 0x%02x\n", val);

    // Finally enable streaming
    msleep(10);  // Delay before stream on
    ret = isp_sensor_write_reg(client, 0x0100, 0x01);
    msleep(20);  // Let sensor start up

    pr_info("SC2336: Sensor initialization complete\n");
    return ret;
}

static int configure_mipi_csi(struct IMPISPDev *dev) {
    void __iomem *regs = dev->regs;
    void __iomem *cpm_base;
    u32 val;
    int timeout = 100;
    int ret = 0;

    pr_info("Starting MIPI CSI configuration...\n");

    // 1. First disable MIPI
    writel(0x0, regs + ISP_MIPI_CTRL);
    wmb();
    msleep(10);

    // 2. Map and configure CPM
    cpm_base = ioremap(T31_CPM_BASE, 0x100);
    if (!cpm_base) {
        pr_err("Failed to map CPM registers\n");
        return -ENOMEM;
    }

    // Read initial state
    pr_info("Initial CPM state:\n"
            "  CLKGATE: 0x%08x\n"
            "  MIPI_CTRL: 0x%08x\n",
            readl(cpm_base + T31_CPM_CLKGATE),
            readl(cpm_base + T31_CPM_MIPI_CTRL));

    // Enable MIPI clocks
    val = readl(cpm_base + T31_CPM_MIPI_CTRL);
    val |= CPM_MIPI_CTRL_EN | CPM_MIPI_CLK_EN | CPM_MIPI_CLK_SEL;
    writel(val, cpm_base + T31_CPM_MIPI_CTRL);
    wmb();
    msleep(10);

    // 3. Configure MIPI timing first
    val = (0x8 << 24) |   // HS settle
          (0x10 << 16) |  // CLK settle
          (0x8 << 8) |    // LP settle
          0x3;            // 2 lanes

    writel(val, regs + ISP_MIPI_TIMING);
    wmb();
    msleep(5);

    // 4. Configure DMA
    uint32_t line_size = ((1920 + 7) >> 3) << 3;  // Original alignment
    uint32_t second_buf_offset = line_size * 1080;

    // Configure base registers
    writel(dev->dma_addr, regs + 0x7820);
    writel(line_size, regs + 0x7824);
    writel(dev->dma_addr + second_buf_offset, regs + 0x7828);
    writel(line_size >> 1, regs + 0x782c);
    wmb();
    msleep(5);

    // 5. Enable MIPI with proper configuration
    val = (1 << 31) |    // Global enable
          (1 << 8) |     // Clock lane
          0x3;           // 2 data lanes

    writel(val, regs + ISP_MIPI_CTRL);
    wmb();
    msleep(5);

    // 6. Enable stream
    writel(0x1, regs + 0x783c);
    wmb();

    // 7. Wait for ready with better debug
    while (timeout--) {
        val = readl(regs + ISP_MIPI_STATUS);
        if (val & BIT(0)) {
            pr_info("MIPI ready: status=0x%x\n", val);
            ret = 0;
            goto cleanup;
        }
        if ((timeout % 10) == 0)
            pr_info("Waiting... status=0x%x ctrl=0x%x\n",
                   val, readl(regs + ISP_MIPI_CTRL));
        msleep(1);
    }

    ret = -ETIMEDOUT;
    pr_err("MIPI CSI timeout!\n");

cleanup:
    iounmap(cpm_base);
    return ret;
}



static int configure_streaming_hardware(struct IMPISPDev *dev)
{
    void __iomem *regs = dev->regs;
    u32 val;
    int ret = 0;

    pr_info("Configuring ISP hardware...\n");

    // Full reset sequence
    writel(0x0, regs + ISP_CTRL_REG);
    writel(0x0, regs + ISP_INT_MASK_REG);
    writel(0xFFFFFFFF, regs + ISP_INT_CLEAR_REG);
    wmb();
    msleep(10);

    // MIPI configuration first
    writel(0x0, regs + ISP_MIPI_CTRL);
    wmb();
    msleep(10);

    // MIPI timing
    val = (0x8 << 24) |    // HS settle
          (0x10 << 16) |   // CLK settle
          (0x8 << 8);      // LP settle
    writel(val, regs + ISP_MIPI_TIMING);
    wmb();
    msleep(10);

    // Enable MIPI with 2 lanes
    val = (1 << 31) |    // Global enable
          (1 << 8) |     // Clock lane
          0x3;           // 2 data lanes
    writel(val, regs + ISP_MIPI_CTRL);
    wmb();
    msleep(10);

    // Wait for MIPI ready
    int timeout = 100;
    while (timeout--) {
        if (readl(regs + ISP_MIPI_STATUS) & 0x1)
            break;
        msleep(1);
    }
    if (timeout <= 0) {
        pr_err("MIPI interface failed to become ready\n");
        return -ETIMEDOUT;
    }

    // Format configuration - be explicit about NV12
    writel(0x3231564e, regs + ISP_INPUT_FORMAT_REG);   // NV12 in
    writel(0x3231564e, regs + ISP_OUTPUT_FORMAT_REG);  // NV12 out
    wmb();
    msleep(10);

    // Configure IPU first if needed
    val = readl(regs + ISP_IPU_CTRL);
    val |= (1 << 0);     // Enable IPU
    writel(val, regs + ISP_IPU_CTRL);
    wmb();
    msleep(10);

    // Enable core with interrupts
    val = ISP_CTRL_ENABLE |       // Core enable
          (1 << 1) |              // Frame processing
          (1 << 2);               // Enable IPU path
    writel(val, regs + ISP_CTRL_REG);

    // Enable frame interrupts
    writel(ISP_INT_FRAME_DONE, regs + ISP_INT_MASK_REG);
    wmb();
    msleep(10);

    // Verify state
    pr_info("Hardware config:\n");
    pr_info("MIPI: ctrl=0x%08x status=0x%08x timing=0x%08x\n",
            readl(regs + ISP_MIPI_CTRL),
            readl(regs + ISP_MIPI_STATUS),
            readl(regs + ISP_MIPI_TIMING));
    pr_info("Format: in=0x%08x out=0x%08x\n",
            readl(regs + ISP_INPUT_FORMAT_REG),
            readl(regs + ISP_OUTPUT_FORMAT_REG));
    pr_info("IPU: ctrl=0x%08x status=0x%08x\n",
            readl(regs + ISP_IPU_CTRL),
            readl(regs + ISP_IPU_STATUS));
    pr_info("Control: ctrl=0x%08x mask=0x%08x status=0x%08x\n",
            readl(regs + ISP_CTRL_REG),
            readl(regs + ISP_INT_MASK_REG),
            readl(regs + ISP_STATUS_REG));

    return 0;
}

static int handle_stream_enable(struct IMPISPDev *dev, bool enable)
{
    void __iomem *regs = dev->regs;

    if (!regs)
        return -EINVAL;

    pr_info("Configuring stream enable=%d\n", enable);

    // Add before enabling stream:
    struct i2c_client *client = dev->sensor_i2c_client;

    // Force sensor to output data
    isp_sensor_write_reg(client, 0x0100, 0x01); // Streaming
    isp_sensor_write_reg(client, 0x3f00, 0x00); // Format control
    isp_sensor_write_reg(client, 0x3f04, 0x03); // Raw10

    msleep(30); // Wait for sensor

    // Proper sequence:
    writel(0x1, regs + ISP_CTRL_REG);  // Enable core first
    wmb();
    udelay(100);

    writel(0x1, regs + ISP_STREAM_CTRL);  // Enable stream control
    wmb();
    udelay(100);

    writel(0x1, regs + ISP_STREAM_START);  // Start streaming
    wmb();

    // Enable sensor streaming
    if (dev->sensor_i2c_client) {
        int ret = isp_sensor_write_reg(dev->sensor_i2c_client,
                                     SENSOR_REG_MODE, 0x01);
        if (ret) {
            pr_err("Failed to start sensor streaming\n");
            return ret;
        }
    }

    pr_info("Stream enabled successfully\n");
    return 0;
}

static int prepare_streaming(struct IMPISPDev *dev) {
    int ret;

    pr_info("Preparing streaming configuration...\n");

    // Configure initial buffer locations
    ret = setup_isp_buffers(dev);
    if (ret) {
        pr_err("Failed to setup ISP buffers\n");
        return ret;
    }

    // Configure basic hardware - but don't enable streaming yet
    ret = configure_streaming_hardware(dev);
    if (ret) {
        pr_err("Failed to configure streaming hardware\n");
        goto cleanup_buffers;
    }

    return 0;

cleanup_buffers:
    cleanup_frame_buffers(&dev->frame_sources[0]);
    return ret;
}



/* Handle sensor-specific IOCTL commands */
static int handle_sensor_ioctl(struct IMPISPDev *dev, unsigned int cmd, void __user *arg)
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
            // This is the initial stream enable from libimp
            // Set up internal state but keep external interface clean
            ret = prepare_streaming(ourISPdev);
            return ret;
        }

        case SENSOR_CMD_STREAM_OFF: {
            ret = isp_sensor_write_reg(client, 0x0100, 0x00); // Stream off
            pr_info("Sensor streaming stopped\n");
            break;
        }

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
		    struct sensor_buffer_info wdr_info = {0};

		    pr_info("tx_isp: Handling WDR buffer info request\n");

		    if (!ourISPdev || !ourISPdev->wdr_mode) {
		        pr_err("tx_isp: WDR not enabled\n");
		        return -EINVAL;
		    }

		    // Ensure WDR control buffer is allocated
		    if (!ourISPdev->wdr_buf_info) {
		        ourISPdev->wdr_buf_info = kzalloc(sizeof(struct sensor_buffer_info), GFP_KERNEL);
		        if (!ourISPdev->wdr_buf_info) {
		            pr_err("tx_isp: Failed to allocate WDR buffer info\n");
		            return -ENOMEM;
		        }
		    }

		    // Calculate WDR buffer size for NV12
		    uint32_t y_size = ourISPdev->width * ourISPdev->height;
		    uint32_t uv_size = y_size / 2;  // NV12 has half-size UV plane
		    uint32_t wdr_size = y_size + uv_size;  // Total 1.5 bytes per pixel
		    wdr_info.buffer_size = wdr_size;
		    wdr_info.buffer_start = 0;  // Will be allocated by userspace if needed
		    wdr_info.frame_count = 0;
		    wdr_info.is_buffer_full = 0;

		    pr_info("tx_isp: Calculated WDR buffer size: %u\n", wdr_size);

		    // Copy the WDR buffer info to userspace
		    if (copy_to_user((void __user *)arg, &wdr_info, sizeof(wdr_info))) {
		        pr_err("tx_isp: Failed to copy WDR buffer info to userspace\n");
		        return -EFAULT;
		    }

		    pr_info("tx_isp: WDR buffer requirements sent to userspace\n");
		    return 0;
		}

        default:
            pr_err("Unknown sensor command: 0x%x\n", cmd);
            return -EINVAL;
    }

    return ret;
}


// Initialize reserved memory
static int isp_init_reserved_memory(struct device *dev) {
    pr_info("tx_isp: Mapping pre-reserved memory using ioremap\n");

    // Map the reserved memory region using ioremap
    reserved_buffer_virt = ioremap(RMEM_START, DRIVER_RESERVED_SIZE);
    if (!reserved_buffer_virt) {
        pr_err("tx_isp: Failed to map reserved memory\n");
        return -ENOMEM;
    }

    pr_info("Reserved memory mapped successfully:\n");
    pr_info("  Physical address: 0x%08x\n", RMEM_START);
    pr_info("  Virtual address: %p\n", reserved_buffer_virt);
    pr_info("  Size: %zu bytes\n", reserved_buffer_size);

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


// Add these defines at the top
#define RMEM_BASE     0x2A80000   // Reserved memory base - matches binary

#define ISP_DEFAULT_SIZE   (44 * 1024 * 1024)  // 44MB - matches libimp expectation
#define ISP_INIT_MAGIC    0x00000001
#define ISP_ALLOC_METHOD  0x203a726f
#define ISP_ALLOC_MAGIC   0x33326373

// Structure to match libimp's expectation
struct imp_buffer_info {
    uint32_t method;
    uint32_t phys_addr;
    uint32_t virt_addr;
    uint32_t size;
    uint32_t flags;
};

// Structure for managing physical/virtual memory addresses
// Add at the top of the file
#define ISP_ALLOC_MAGIC1   0x203a726f
#define ISP_ALLOC_MAGIC2   0x33326373


static int init_isp_reserved_memory(struct platform_device *pdev)
{
    pr_info("tx_isp: Initializing reserved memory\n");

    if (!ourISPdev) {
        dev_err(&pdev->dev, "ISP device not initialized\n");
        return -EINVAL;
    }

    // Store the base address
    ourISPdev->dma_addr = RMEM_BASE;
    ourISPdev->dma_size = RMEM_SIZE;

    // Map the reserved memory region
    ourISPdev->dma_buf = ioremap(RMEM_BASE, RMEM_SIZE);
    if (!ourISPdev->dma_buf) {
        dev_err(&pdev->dev, "Failed to map reserved memory\n");
        return -ENOMEM;
    }

    pr_info("tx_isp: Reserved memory initialized:\n");
    pr_info("  Physical address: 0x%08x\n", (uint32_t)ourISPdev->dma_addr);
    pr_info("  Virtual address: %p\n", ourISPdev->dma_buf);
    pr_info("  Size: %zu bytes\n", ourISPdev->dma_size);

    // Add validation print
    pr_info("tx_isp: Validating memory setup:\n");
    pr_info("  ourISPdev = %p\n", ourISPdev);
    pr_info("  dma_addr = 0x%08x\n", (uint32_t)ourISPdev->dma_addr);
    pr_info("  dma_buf = %p\n", ourISPdev->dma_buf);

    return 0;
}

// Add to probe:
#define ISP_PARAM_OFFSET 0x1000
#define ISP_WDR_OFFSET   0x2000

static int setup_isp_memory_regions(struct IMPISPDev *dev) {
    // Parameters region
    dev->isp_params = dev->dma_buf + ISP_PARAM_OFFSET;
    dev->wdr_params = dev->dma_buf + ISP_WDR_OFFSET;

    pr_info("Memory regions:\n");
    pr_info("  Params: %p\n", dev->isp_params);
    pr_info("  WDR: %p\n", dev->wdr_params);

    return 0;
}

// Add at the top of the file
#define ISP_ALLOC_MAGIC1   0x203a726f
#define ISP_ALLOC_MAGIC2   0x33326373
#define ISP_DEFAULT_SIZE   0x2a80000  // Match the size from logs


// Function to allocate memory
// Function to allocate memory using existing structure
static int alloc_isp_memory(size_t size)
{
    if (isp_memory.initialized) {
        pr_info("tx_isp: Memory already allocated\n");
        return 0;
    }

    // Use our reserved memory
    isp_memory.phys_addr = reserved_buffer_phys;
    isp_memory.virt_addr = reserved_buffer_virt;
    isp_memory.size = min_t(size_t, size, reserved_buffer_size);
    isp_memory.initialized = true;

    pr_info("tx_isp: Memory allocated:\n");
    pr_info("  Physical: 0x%08x\n", isp_memory.phys_addr);
    pr_info("  Virtual: %p\n", isp_memory.virt_addr);
    pr_info("  Size: %zu\n", isp_memory.size);

    return 0;
}


struct isp_mem_request {
    uint32_t method;        // 0x80: Should match ISP_ALLOC_KMALLOC
    uint32_t phys_addr;     // 0x84: Physical address from IMP_Alloc
    uint32_t virt_addr;     // 0x88: Virtual address
    uint32_t size;          // 0x8C: Buffer size
    uint32_t flags;         // 0x90: Flags/attributes
};

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
        if (!dev->buf_info) {
            dev->buf_info = kzalloc(sizeof(struct sensor_buffer_info), GFP_KERNEL);
            if (!dev->buf_info)
                return -ENOMEM;
        }

        // Maintain exact same values for initial request
        req.method = ISP_ALLOC_KMALLOC;
        req.phys_addr = 0x1;  // Initial magic value
        req.size = 0x1;       // Initial size
        req.virt_addr = (unsigned long)dev->dma_buf;
        req.flags = 0;

        // Store actual values in our buffer info
        dev->buf_info->method = req.method;
        dev->buf_info->buffer_start = dev->dma_addr;
        dev->buf_info->buffer_size = dev->dma_size;
        dev->buf_info->virt_addr = (unsigned long)dev->dma_buf;
        dev->buf_info->flags = 0;

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
        // Return actual memory info - keep same sequence
        req.method = ISP_ALLOC_KMALLOC;
        req.phys_addr = dev->dma_addr;
        req.size = dev->dma_size;
        req.virt_addr = (unsigned long)dev->dma_buf;
        req.flags = 0;

        pr_info("tx_isp: Memory allocation: phys=0x%x virt=%p size=0x%x\n",
                (unsigned int)dev->dma_addr, dev->dma_buf, dev->dma_size);

        if (copy_to_user(argp, &req, sizeof(req)))
            return -EFAULT;
        return 0;
    }

    pr_err("tx_isp: Unhandled request\n");
    return -EINVAL;
}



struct isp_sensor_info {
    int is_initialized;
    int chip_id;
    int width;
    int height;
};

static int reset_gpio = GPIO_PA(18);  // Default reset GPIO
static int pwdn_gpio = -1;  // Default power down GPIO disabled


static struct isp_instance *find_instance_by_fd(int fd)
{
    struct isp_instance *instance;

    spin_lock(&instances_lock);
    list_for_each_entry(instance, &isp_instances, list) {
        if (instance->fd == fd) {
            spin_unlock(&instances_lock);
            return instance;
        }
    }
    spin_unlock(&instances_lock);
    return NULL;
}

static long handle_set_buf_ioctl(struct IMPISPDev *dev, unsigned long arg) {
    struct sensor_buffer_info buf_info = {0};
    void __user *argp = (void __user *)arg;
    struct isp_framesource_state *fs;

    pr_info("tx_isp: Handling ioctl VIDIOC_SET_BUF_INFO\n");

    if (!dev || !dev->buf_info || !dev->dma_buf) {
        pr_err("tx_isp: Invalid device state\n");
        return -EINVAL;
    }

    fs = dev->fs_info;
    if (!fs) {
        pr_err("tx_isp: No frame source initialized\n");
        return -EINVAL;
    }

    if (copy_from_user(&buf_info, argp, sizeof(buf_info))) {
        pr_err("tx_isp: Failed to copy from user\n");
        return -EFAULT;
    }

    // Store consistent info across both buf_info and frame source
    buf_info.method = ISP_ALLOC_KMALLOC;
    buf_info.buffer_start = dev->dma_addr;
    buf_info.buffer_size = fs->buf_size * fs->buf_cnt; // Match frame source size
    buf_info.virt_addr = (unsigned long)dev->dma_buf;
    buf_info.flags = 1;
    buf_info.frame_count = 0;
    buf_info.is_buffer_full = 0;

    // Update frame source buffer info
    fs->dma_addr = dev->dma_addr;
    fs->buf_base = dev->dma_buf;

    // Make sure buf_info array is allocated
    if (!fs->bufs) {
        fs->bufs = kzalloc(sizeof(struct isp_buffer_info) * fs->buf_cnt, GFP_KERNEL);
        if (!fs->bufs) {
            return -ENOMEM;
        }
    }

	// Initialize each buffer in the array
	int i;
	for (i = 0; i < fs->buf_cnt; i++) {
	    fs->bufs[i].buffer_start = fs->dma_addr + (i * fs->buf_size);
	    fs->bufs[i].virt_addr = (unsigned long)fs->buf_base + (i * fs->buf_size); // Cast to unsigned long
	    fs->bufs[i].buffer_size = fs->buf_size;
	    fs->bufs[i].method = ISP_ALLOC_KMALLOC;
	    fs->bufs[i].flags = 1;
	    fs->bufs[i].frame_count = 0;
	    fs->bufs[i].is_buffer_full = 0;
	}

    // Store consistent info
    memcpy(dev->buf_info, &buf_info, sizeof(buf_info));

    pr_info("tx_isp: Buffer info configured:\n");
    pr_info("  phys=0x%x virt=%p size=%u\n",
            (unsigned int)buf_info.buffer_start,
            (void *)buf_info.virt_addr,
            buf_info.buffer_size);
    pr_info("  Frame buffer size=%u count=%u\n",
            fs->buf_size, fs->buf_cnt);

    if (copy_to_user(argp, &buf_info, sizeof(buf_info)))
        return -EFAULT;

    return 0;
}

// External structures for libimp interface
struct sensor_stream_info {
    uint32_t state;          // 0 = disabled, 1 = enabled
    uint32_t format;         // Frame format
    uint32_t width;
    uint32_t height;
    uint32_t flags;
};

struct sensor_enable_param {
    uint32_t channel;       // Channel number
    uint32_t enable;        // 0 = disable, 1 = enable
    uint32_t flags;         // Reserved flags
};


/**
 * isp_driver_ioctl - IOCTL handler for ISP driver
 * @file: File structure
 * @cmd: IOCTL command
 * @arg: Command argument
 *
 * Based on decompiled tx_isp_unlocked_ioctl function
 */
static long isp_driver_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    void __user *argp = (void __user *)arg;
    struct sensor_list_info sensor_list[MAX_SENSORS];
    int fd = (int)(unsigned long)file->private_data;
    struct isp_framesource_state *fs = NULL;
    struct isp_instance *instance;
    int ret = 0;
    int channel = 0;

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
    case VIDIOC_REGISTER_SENSOR: {
        struct i2c_client *client = ourISPdev->sensor_i2c_client;
        unsigned char val_h = 0, val_l = 0;
        int i;

        // Add from sensor_init_regs_1920_1080_30fps_mipi[]
        isp_sensor_write_reg(client, 0x0103, 0x01); // Reset
        msleep(20);

        isp_sensor_write_reg(client, 0x36e9, 0x80);
        isp_sensor_write_reg(client, 0x37f9, 0x80);
        isp_sensor_write_reg(client, 0x301f, 0x02);
        isp_sensor_write_reg(client, 0x3106, 0x05);
        isp_sensor_write_reg(client, 0x320c, 0x08);
        isp_sensor_write_reg(client, 0x320d, 0xca);
        isp_sensor_write_reg(client, 0x320e, 0x05);
        isp_sensor_write_reg(client, 0x320f, 0xa0);

        // Core sensor settings
        isp_sensor_write_reg(client, 0x3301, 0x09);
        isp_sensor_write_reg(client, 0x3306, 0x60);
        isp_sensor_write_reg(client, 0x3633, 0x22);
        isp_sensor_write_reg(client, 0x3639, 0xf4);

        // AE/AG settings
        isp_sensor_write_reg(client, 0x3e01, 0x4a);
        isp_sensor_write_reg(client, 0x3e02, 0xb0);
        isp_sensor_write_reg(client, 0x3e08, 0x1f);
        isp_sensor_write_reg(client, 0x3e09, 0x20);

        // Read ID registers
        ret = isp_sensor_read_reg(client, 0x3107, &val_h);
        ret |= isp_sensor_read_reg(client, 0x3108, &val_l);
        pr_info("SC2336: ID = 0x%02x%02x\n", val_h, val_l);

        // Enable output
        isp_sensor_write_reg(client, 0x0100, 0x01);
        msleep(20); // Let sensor stabilize

        return ret;
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

        // The decompiled code shows it's passing 0 directly as arg
        // We shouldn't try to copy from user here since arg is the value
        if (arg != 0) {
            pr_err("Invalid enable stream argument\n");
            return -EINVAL;
        }

        // Don't try to copy param, just enable
        ret = handle_stream_enable(ourISPdev, true);
        if (ret == 0) {
            ourISPdev->stream_count += 2;
            pr_info("Stream enabled, count=%d\n", ourISPdev->stream_count);
        }

        return ret;
    }
    case ISP_TUNING_ENABLE: {
        int enable;
        if (copy_from_user(&enable, (void __user *)arg, sizeof(enable))) {
            pr_err("Failed to copy tuning enable\n");
            return -EFAULT;
        }

        pr_info("ISP tuning %s requested\n", enable ? "enable" : "disable");

        // Store tuning state
        ourISPdev->tuning_enabled = enable;

        // Set up tuning registers
        if (enable) {
            // Map correct ISP tuning regions
            // Base at 0x13380000
            void __iomem *tuning_base = ioremap(0x13380000, 0x1b000);
            if (!tuning_base) {
                pr_err("Failed to map tuning registers\n");
                return -ENOMEM;
            }
            ourISPdev->tuning_regs = tuning_base;

            // Initialize tuning parameters
            writel(0x1, tuning_base + 0x100); // Enable tuning
            wmb();
        }

        return 0;
    }
    case TX_ISP_SET_BUF: {  // 0x800856d5
        struct imp_buffer_info *buf_info = (struct imp_buffer_info *)arg;
        pr_info("tx_isp: SET_BUF request: method=0x%x phys=0x%x size=0x%x\n",
                buf_info->method, buf_info->phys_addr, buf_info->size);

        if (ourISPdev && ourISPdev->buf_info) {
            pr_info("tx_isp: Magic allocation setup: phys=0x%x virt=%p size=0x%x\n",
                    (unsigned int)ourISPdev->dma_addr, ourISPdev->dma_buf, ourISPdev->dma_size);

            struct isp_framesource_state *fs = &ourISPdev->frame_sources[0];
            pr_info("Frame source state: is_open=%d flags=0x%x dma=0x%x size=%u\n",
                    fs->is_open, fs->flags, (unsigned int)fs->dma_addr, fs->buf_size);
        }

        return handle_get_buf_ioctl(ourISPdev, arg);
    }
    case VIDIOC_SET_BUF_INFO: {
        struct sensor_buffer_info buf_info = {0};
        void __user *argp = (void __user *)arg;

        pr_info("tx_isp: Handling ioctl VIDIOC_SET_BUF_INFO\n");

        if (!ourISPdev) {
            pr_err("No ISP device\n");
            return -EINVAL;
        }

        // Allocate buf_info if not already done
        if (!ourISPdev->buf_info) {
            ourISPdev->buf_info = kzalloc(sizeof(struct sensor_buffer_info), GFP_KERNEL);
            if (!ourISPdev->buf_info) {
                pr_err("Failed to allocate buffer info\n");
                return -ENOMEM;
            }
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

        // Store consistent info
        memcpy(ourISPdev->buf_info, &buf_info, sizeof(buf_info));

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

            if (copy_from_user(&ctrl, (void __user *)arg, sizeof(ctrl)))
                return -EFAULT;

            pr_info("Setting control: id=0x%x value=%d\n", ctrl.id, ctrl.value);

            switch(ctrl.id) {
                case V4L2_CID_BRIGHTNESS:
                    writel(ctrl.value, ourISPdev->regs + 0x1100);
                    break;
                case V4L2_CID_CONTRAST:
                    writel(ctrl.value, ourISPdev->regs + 0x1104);
                    break;
                case V4L2_CID_SATURATION:
                    writel(ctrl.value, ourISPdev->regs + 0x1108);
                    break;
                case V4L2_CID_SHARPNESS:
                    writel(ctrl.value, ourISPdev->regs + 0x110c);
                    break;
                default:
                    pr_info("Unhandled control: id=0x%x\n", ctrl.id);
                    return -EINVAL;
            }
            wmb();
            return 0;
        }
    case 0x800456d0: {  // Create/setup links
        int __user *enable = (int __user *)arg;
        int enable_val;

        if (get_user(enable_val, enable)) {
            pr_err("Failed to get link enable value\n");
            return -EFAULT;
        }

        pr_info("Creating ISP links with enable=%d\n", enable_val);

        struct isp_subdev_state *sd = (struct isp_subdev_state *)ourISPdev->subdevs[0];
        if (sd) {
            mutex_lock(&sd->lock);

            // Setup link based on bypass mode
            if (sd->bypass_mode) {
                // Bypass mode - direct link between source and sink
                if (sd->src_pads && sd->sink_pads) {
                    sd->src_pads->sink = sd->sink_pads;
                    sd->sink_pads->source = sd->src_pads;
                    sd->src_pads->link_state = enable_val ? LINK_STATE_ENABLED : LINK_STATE_INACTIVE;
                    sd->sink_pads->link_state = enable_val ? LINK_STATE_ENABLED : LINK_STATE_INACTIVE;
                }
            } else {
                // Normal mode - process through ISP
                if (sd->src_pads && sd->sink_pads) {
                    sd->src_pads->sink = sd->sink_pads;
                    sd->sink_pads->source = sd->src_pads;
                    sd->src_pads->link_state = enable_val ? LINK_STATE_SOURCE : LINK_STATE_INACTIVE;
                    sd->sink_pads->link_state = enable_val ? LINK_STATE_SOURCE : LINK_STATE_INACTIVE;
                }
            }

            mutex_unlock(&sd->lock);
        }

        // Write back link status
        if (put_user(enable_val, enable))
            return -EFAULT;

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

        // Disable all active links
        struct isp_subdev_state *sd = (struct isp_subdev_state *)ourISPdev->subdevs[0];
        if (sd) {
            mutex_lock(&sd->lock);
            if (sd->src_pads)
                sd->src_pads->link_state = LINK_STATE_INACTIVE;
            if (sd->sink_pads)
                sd->sink_pads->link_state = LINK_STATE_INACTIVE;
            mutex_unlock(&sd->lock);
        }

        return 0;
    }
    case 0x800456d1: {  // Destroy links
        pr_info("Destroying ISP links\n");
        int __user *result = (int __user *)arg;

        struct isp_subdev_state *sd = (struct isp_subdev_state *)ourISPdev->subdevs[0];
        if (sd) {
            mutex_lock(&sd->lock);
            // Clear link state
            if (sd->src_pads) {
                sd->src_pads->source = NULL;
                sd->src_pads->sink = NULL;
            }
            if (sd->sink_pads) {
                sd->sink_pads->source = NULL;
                sd->sink_pads->sink = NULL;
            }
            mutex_unlock(&sd->lock);
        }

        // Write back -1 as expected
        if (put_user(-1, result))
            return -EFAULT;

        return 0;
    }
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
	    pr_info("Stream OFF requested\n");
	    return enable_isp_streaming(ourISPdev, file, channel, false);
	    break;
	case VIDIOC_S_FMT: {
	    struct v4l2_format fmt;
	    struct isp_framesource_state *fs = file->private_data;

	    if (copy_from_user(&fmt, argp, sizeof(fmt)))
	        return -EFAULT;

	    if (!fs) {
	        pr_err("No frame source state\n");
	        return -EINVAL;
	    }

	    pr_info("Channel %d format request: %dx%d format: %d\n",
	            fs->chn_num, fmt.fmt.pix.width, fmt.fmt.pix.height,
	            fmt.fmt.pix.pixelformat);

	    // Store new format
	    fs->width = fmt.fmt.pix.width;
	    fs->height = fmt.fmt.pix.height;

	    // Only allow format change if not streaming
	    if (fs->state == 2) {
	        pr_err("Cannot change format while streaming\n");
	        return -EBUSY;
	    }

	    // Recalculate buffer sizes but don't allocate yet
	    fs->buf_size = fs->width * fs->height * 2;  // YUV422

	    // Return negotiated format
	    if (copy_to_user(argp, &fmt, sizeof(fmt)))
	        return -EFAULT;

	    pr_info("Channel %d format set to %dx%d\n",
	            fs->chn_num, fs->width, fs->height);
	    break;
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
    case VIDIOC_STREAMON:  // 0x80045612
        pr_info("Sensor command: 0x%x\n", cmd);
        ret = handle_sensor_ioctl(ourISPdev, cmd, argp);
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
    .open           = tisp_open,
    .release        = tisp_release,
    .unlocked_ioctl = isp_driver_ioctl,
    .mmap = tx_isp_mmap,
};


// Update file operations structure
static const struct file_operations isp_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = isp_driver_ioctl,
    .open = tisp_open,
    .release = tisp_release,
    .read = tisp_read,
    .write = tisp_write,
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

// Add these at the top of the file with other defines
// Update the CPM register definitions for T31
#define CPM_PHYS_BASE   0x10000000  // T31 CPM physical base
#define CPM_REG_SIZE    0x1000      // Cover full register range
#define CPM_ISPCDR      0x88        // Offset from CPM base
#define CPM_CLKGR       0x20        // Offset from CPM base
#define CPM_CLKGR1      0x28        // Additional control register


#define ISP_OFFSET_PARAMS 159736  // Update to match actual parameter file size

// Add these at top of file
#define CPM_OPCR      0x24    // Operating Parameter Control Register offset
#define CPM_LCR       0x04    // Low power control register offset

// Add this structure if not already present
struct isp_platform_data {
    unsigned long clock_rate;
};

#define T31_ISP_FREQ     250000000   // 250MHz ISP core clock (matches AHB)
#define T31_CGU_ISP_FREQ 125000000   // 125MHz CGU_ISP clock (matches APB)

// Add CGU ISP bit positions
#define ISP_CLKGR_BIT    (1 << 23)   // ISP clock gate bit in CLKGR
#define CGU_ISP_BIT      (1 << 2)    // CGU_ISP clock gate bit in CLKGR1

static struct isp_platform_data isp_pdata = {
    .clock_rate = T31_ISP_FREQ,
};

static int configure_isp_clocks(struct IMPISPDev *dev)
{
    int ret;

    pr_info("Configuring ISP clocks using standard API\n");

    // Step 1: Get CSI clock
    dev->csi_clk = clk_get(dev->dev, "csi");
    if (!IS_ERR(dev->csi_clk)) {
        ret = clk_prepare_enable(dev->csi_clk);
        if (ret) {
            pr_err("Failed to enable CSI clock\n");
            goto err_put_csi;
        }
    }

    // Step 2: Get IPU clock
    dev->ipu_clk = clk_get(dev->dev, "ipu");
    if (IS_ERR(dev->ipu_clk)) {
        pr_warn("IPU clock not available\n");
        dev->ipu_clk = NULL;
    }

    // Step 3: Get ISP core clock
    dev->isp_clk = clk_get(dev->dev, "isp");
    if (IS_ERR(dev->isp_clk)) {
        ret = PTR_ERR(dev->isp_clk);
        pr_err("Failed to get ISP clock: %d\n", ret);
        dev->isp_clk = NULL;
        return ret;
    }

    // Step 4: Get CGU ISP clock
    dev->cgu_isp_clk = clk_get(dev->dev, "cgu_isp");
    if (IS_ERR(dev->cgu_isp_clk)) {
        ret = PTR_ERR(dev->cgu_isp_clk);
        pr_err("Failed to get CGU ISP clock: %d\n", ret);
        dev->cgu_isp_clk = NULL;
        goto err_put_isp;
    }

    // Step 5: Enable CGU ISP clock first
    ret = clk_set_rate(dev->cgu_isp_clk, T31_CGU_ISP_FREQ);
    if (ret) {
        pr_err("Failed to set CGU ISP clock rate: %d\n", ret);
        goto err_put_cgu;
    }
    ret = clk_prepare_enable(dev->cgu_isp_clk);
    if (ret) {
        pr_err("Failed to enable CGU ISP clock: %d\n", ret);
        goto err_put_cgu;
    }

    // Step 6: Enable ISP core clock
    ret = clk_prepare_enable(dev->isp_clk);
    if (ret) {
        pr_err("Failed to enable ISP clock: %d\n", ret);
        goto err_disable_cgu;
    }

    // Step 7: Enable IPU clock if available
    if (dev->ipu_clk) {
        ret = clk_prepare_enable(dev->ipu_clk);
        if (ret) {
            pr_warn("Failed to enable IPU clock\n");
        }
    }

    // Step 8: Enable additional peripheral clocks (e.g., LCD, AES)
    struct clk *lcd_clk = clk_get(dev->dev, "lcd");
    if (!IS_ERR(lcd_clk)) {
        clk_prepare_enable(lcd_clk);
        clk_set_rate(lcd_clk, 250000000);
    }

    struct clk *aes_clk = clk_get(dev->dev, "aes");
    if (!IS_ERR(aes_clk)) {
        clk_prepare_enable(aes_clk);
        clk_set_rate(aes_clk, 250000000);
    }

    // Step 9: Verify clock rates
    pr_info("Clock rates after configuration:\n");
    pr_info("  CSI: %lu Hz\n", dev->csi_clk ? clk_get_rate(dev->csi_clk) : 0);
    pr_info("  ISP Core: %lu Hz\n", clk_get_rate(dev->isp_clk));
    pr_info("  CGU ISP: %lu Hz\n", clk_get_rate(dev->cgu_isp_clk));
    pr_info("  IPU: %lu Hz\n", dev->ipu_clk ? clk_get_rate(dev->ipu_clk) : 0);

    return 0;

err_disable_cgu:
    clk_disable_unprepare(dev->cgu_isp_clk);
err_put_cgu:
    clk_put(dev->cgu_isp_clk);
    dev->cgu_isp_clk = NULL;
err_put_isp:
    clk_put(dev->isp_clk);
    dev->isp_clk = NULL;
err_put_csi:
    if (dev->csi_clk) {
        clk_disable_unprepare(dev->csi_clk);
        clk_put(dev->csi_clk);
        dev->csi_clk = NULL;
    }
    return ret;
}

static void cleanup_isp_clocks(struct IMPISPDev *dev)
{
    if (dev->cgu_isp_clk) {
        clk_disable_unprepare(dev->cgu_isp_clk);
        clk_put(dev->cgu_isp_clk);
        dev->cgu_isp_clk = NULL;
    }

    if (dev->isp_clk) {
        clk_disable_unprepare(dev->isp_clk);
        clk_put(dev->isp_clk);
        dev->isp_clk = NULL;
    }
}

#define ISP_SOFT_RESET     (1 << 31)
#define ISP_POWER_ON       (1 << 23)

#define ISP_CLK_CTRL 0x08

static int tisp_init(struct device *dev)
{
    int ret;
    const char *param_file = "/etc/sensor/sc2336-t31.bin";
    uint32_t reg_val;

    pr_info("Starting tisp_init...\n");

    // Verify registers are mapped using global ourISPdev
    if (!ourISPdev || !ourISPdev->regs) {
        dev_err(dev, "ISP registers not mapped!\n");
        return -EFAULT;
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

    if (pwdn_gpio != -1) {
        ret = gpio_request(pwdn_gpio, "sensor_pwdn");
        if (!ret) {
            gpio_direction_output(pwdn_gpio, 0);
            msleep(10);
        }
    }

    // Enable sensor clock
    writel(0x1, ourISPdev->regs + ISP_CLK_CTRL);
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

    // Step 4: Write the parameters to hardware registers using global ourISPdev->regs
    writel((unsigned long)tparams_day, ourISPdev->regs + 0x84b50);
    dev_info(dev, "tparams_day written to register successfully\n");

    return 0;

cleanup:
    if (tparams_night)
        vfree(tparams_night);
    if (tparams_day)
        vfree(tparams_day);
    return ret;
}


static void __iomem *map_isp_registers(struct platform_device *pdev)
{
    void __iomem *base;
    struct resource *res;

    pr_info("Starting ISP register mapping...\n");

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res) {
        dev_err(&pdev->dev, "No memory resource found\n");
        return ERR_PTR(-ENODEV);
    }

    pr_info("Found memory resource: start=0x%lx, end=0x%lx\n",
            (unsigned long)res->start, (unsigned long)res->end);


    base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(base)) {
        dev_err(&pdev->dev, "Failed to map ISP registers\n");
        return base;
    }

    pr_info("Successfully mapped ISP registers to virtual address %p\n", base);
    return base;
}


// And add buffer setup in open
static int framechan_open(struct inode *inode, struct file *file) {
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
        fc->buf_cnt = fs->buf_cnt;
        fc->state = 1;  // Ready state
        fc->channel_offset = minor * (fs->buf_size * fs->buf_cnt);

        mutex_init(&fc->lock);
        spin_lock_init(&fc->queue_lock);
        init_waitqueue_head(&fc->wait);
        atomic_set(&fc->frame_count, 0);

        fs->private = fc;
        fs->is_open = 1;
    }

    // Important: Save fs pointer
    file->private_data = fs;

    pr_info("Opened framechan%d: fs=%p base=%p\n",
            minor, fs, fs->buf_base);

    return 0;
}

static long framechan_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct IMPISPDev *dev = ourISPdev;
    struct isp_framesource_state *fs = file->private_data;
    struct frame_source_channel *fc;
    u32 buf_index;
    int ret = 0;

    if ((cmd != VIDIOC_GET_BUFFER_INFO) && (cmd != VIDIOC_SET_FRAME_MODE) && (cmd != VIDIOC_SET_FIFO_ATTR)) {
    	pr_info("Frame channel IOCTL: cmd=0x%x arg=0x%lx\n", cmd, arg);
//	    pr_info("Memory state:\n"
//	            "  fs=%p private=%p\n"
//	            "  buf_base=%p dma_addr=0x%x\n"
//	            "  dev buf_info=%p\n",
//	            fs, fs->private,
//	            fs->buf_base, (unsigned int)fs->dma_addr,
//	            ourISPdev ? ourISPdev->buf_info : NULL);
    }

    if (!fs || !fs->is_open) {
        pr_err("Invalid frame source state\n");
        return -EINVAL;
    }

    fc = fs->private;
    if (!fc) {
        pr_err("No channel data\n");
        return -EINVAL;
    }

    switch(cmd) {
		case VIDIOC_SET_CHANNEL_ATTR: {
		    struct channel_attr attr;
		    if (copy_from_user(&attr, (void __user *)arg, sizeof(attr))) {
		        pr_err("Failed to copy channel attributes\n");
		        return -EFAULT;
		    }

		    pr_info("ISP-DBG: Channel attr request:\n"
		           "  enable=%d format=0x%x\n"
		           "  size=%dx%d\n"
		           "  crop=%d (%d,%d) %dx%d\n"
		           "  scale=%d %dx%d\n"
		           "  pic=%dx%d\n",
		           attr.enable, attr.format,
		           attr.width, attr.height,
		           attr.crop_enable,
		           attr.crop.x, attr.crop.y,
		           attr.crop.width, attr.crop.height,
		           attr.scaler_enable,
		           attr.scaler_outwidth, attr.scaler_outheight,
		           attr.picwidth, attr.picheight);

		    // Store format info
		    fs->width = attr.width;
		    fs->height = attr.height;
		    fs->fmt = ISP_FMT_NV12;  // Force NV12 format

		    // Update buffer configuration if needed
		    if (attr.enable) {
                // Calculate aligned buffer size for NV12
                uint32_t aligned_width = ALIGN(attr.width, 8);
                fs->buf_size = (aligned_width / 2) * attr.height * 3;  // NV12, avoid 64-bit math
                fc->buf_size = fs->buf_size;

		        // Set channel state
		        fs->state = 1;  // Ready state
		        fs->flags |= 0x1;  // Enable flag
		        fc->state = 1;  // Channel ready

		        // Configure channel format info if scaling enabled
		        if (attr.scaler_enable) {
		            fs->width = attr.scaler_outwidth;
		            fs->height = attr.scaler_outheight;
		        }

		        // If we have picture dimensions, use those
		        if (attr.picwidth && attr.picheight) {
		            fs->width = attr.picwidth;
		            fs->height = attr.picheight;
		        }

		    } else {
		        // Channel disable
		        fs->state = 0;
		        fs->flags &= ~0x1;
		        fc->state = 0;
		    }

		    // Important: Update channel parameters
		    if (!fs->buf_cnt) {
		        fs->buf_cnt = 4;  // Default buffer count
		    }

		    pr_info("Channel %d configured: %dx%d fmt=0x%x size=%d state=%d\n",
		            fs->chn_num, fs->width, fs->height, fs->fmt,
		            fs->buf_size, fs->state);

            dump_frame_format_state(fs);
		    return 0;
		}
        // Add more detailed buffer state handling
        case VIDIOC_QUERYBUF: {
            struct v4l2_buffer buf;
            if (copy_from_user(&buf, (void __user *)arg, sizeof(buf)))
                return -EFAULT;

            buf_index = buf.index;
            if (buf_index >= fs->buf_cnt)
                return -EINVAL;

            pr_info("Query buffer %d: offset=0x%x\n",
                   buf_index, buf_index * fs->buf_size);

            // Fill in buffer info
            buf.length = fs->buf_size;
            buf.m.offset = buf_index * fs->buf_size;

            if (copy_to_user((void __user *)arg, &buf, sizeof(buf)))
                return -EFAULT;
            return 0;
        }
        case VIDIOC_SET_FRAME_DEPTH: {
            struct frame_depth_config config;
            struct frame_source_channel *fc = fs->private;

            if (copy_from_user(&config, (void __user *)arg, sizeof(config))) {
                pr_err("Failed to copy frame depth config\n");
                return -EFAULT;
            }

            pr_info("Set frame depth: channel=%d depth=%d\n",
                    config.channel, config.depth);

            // Store frame depth
            fs->frame_depth = config.depth;

            // Initialize frame queue if needed
            if (!fs->fifo_initialized) {
                INIT_LIST_HEAD(&fs->ready_queue);
                INIT_LIST_HEAD(&fs->done_queue);
                fs->fifo_depth = 0;
                fs->fifo_initialized = true;
            }

            // Allocate frame descriptors
            if (fs->bufs) {
                kfree(fs->bufs);
            }
            fs->bufs = kzalloc(sizeof(struct isp_buffer_info) * config.depth,
                               GFP_KERNEL);
            if (!fs->bufs) {
                return -ENOMEM;
            }

            // Initialize buffers
            for (int i = 0; i < config.depth; i++) {
                fs->bufs[i].method = ISP_ALLOC_KMALLOC;
                fs->bufs[i].buffer_start = fs->dma_addr + (i * fs->buf_size);
                fs->bufs[i].virt_addr = (unsigned long)(fs->buf_base +
                                                      (i * fs->buf_size));
                fs->bufs[i].buffer_size = fs->buf_size;
                fs->bufs[i].flags = 1;
                fs->bufs[i].frame_count = 0;
                fs->bufs[i].is_buffer_full = 0;
            }

            pr_info("Frame depth set: channel=%d depth=%d buf_size=%d\n",
                    config.channel, config.depth, fs->buf_size);

           	// Validate buffer size is set
		    if (fs->buf_size == 0) {
		        pr_err("Buffer size not configured\n");
		        return -EINVAL;
		    }

		    pr_info("Channel %d configured: %dx%d fmt=0x%x size=%d state=%d\n",
		            fs->chn_num, fs->width, fs->height, fs->fmt,
		            fs->buf_size, fs->state);

		    // Add DMA configuration debug info
		    pr_info("DMA config: addr=0x%08x base=%p size=%u flags=0x%x\n",
		            (unsigned int)fc->dma_addr, fc->buf_base, fc->buf_size, fs->flags);

            return 0;
        }
        // Add to framechan_ioctl:
        case VIDIOC_SET_FIFO_ATTR: {
            struct fifo_attr attr;
            struct frame_source_channel *fc = fs->private;

            if (copy_from_user(&attr, (void __user *)arg, sizeof(attr))) {
                pr_err("Failed to copy FIFO attributes\n");
                return -EFAULT;
            }

//            pr_info("Set FIFO attr: channel=%d depth=%d thresh=%d flags=0x%x\n",
//                    attr.channel, attr.depth, attr.thresh, attr.flags);
//            pr_info("Frame info: %dx%d format=0x%x\n",
//                    attr.frame_info.width, attr.frame_info.height,
//                    attr.frame_info.format);

            // Store FIFO configuration in channel
            fc->fifo_depth = attr.depth;
            fc->fifo_thresh = attr.thresh;
            fc->fifo_flags = attr.flags;

            // Initialize FIFO if not done
            spin_lock_init(&fc->queue_lock);
            INIT_LIST_HEAD(&fc->ready_list);
            INIT_LIST_HEAD(&fc->done_list);
            init_waitqueue_head(&fc->wait);
            atomic_set(&fc->frame_count, 0);

//            pr_info("FIFO initialized: depth=%d thresh=%d flags=0x%x\n",
//                    fc->fifo_depth, fc->fifo_thresh, fc->fifo_flags);

            return 0;
        }
        case VIDIOC_STREAM_ON: {
            struct frame_source_channel *fc;
            void __iomem *regs;
            int ret;

            pr_info("Stream ON request for channel %d\n", fs->chn_num);

            // Basic validation
            if (!fs || !ourISPdev || !ourISPdev->regs) {
                pr_err("Invalid device state\n");
                return -EINVAL;
            }

            fc = fs->private;
            if (!fc || !fs->buf_base) {
                pr_err("Invalid channel state\n");
                return -EINVAL;
            }

            regs = ourISPdev->regs;

            // State checks
            if (fs->state == 2) {
                pr_info("Channel %d already streaming\n", fs->chn_num);
                return 0;
            }

            // Set initial state
            fs->state = 1;  // Ready state first

            // Configure buffer sequence
            uint32_t buf_offset = ISP_BUF0_OFFSET + (fs->chn_num * ISP_BUF_SIZE_STEP);

            // Set up main buffer - important offset calculation
            writel(fc->dma_addr + (fc->channel_offset), regs + buf_offset);
            writel(fc->buf_size, regs + buf_offset + 0x4);
            wmb();

            // Additional buffer setup if needed
            if (fc->buf_cnt > 1) {
                for (int i = 1; i < fc->buf_cnt; i++) {
                    uint32_t offset = buf_offset + (i * ISP_BUF_SIZE_STEP);
                    writel(fc->dma_addr + (fc->channel_offset) + (i * fc->buf_size),
                           regs + offset);
                    writel(fc->buf_size, regs + offset + 0x4);
                }
                wmb();
            }

            // Reset counters
            fs->frame_cnt = 0;
            fs->write_idx = 0;

            // Important: Channel-specific control registers
            uint32_t ctrl_offset = ISP_STREAM_CTRL + (fs->chn_num * 0x100);
            uint32_t start_offset = ISP_STREAM_START + (fs->chn_num * 0x100);

            // Enable frame processing - sequence matters
            writel(0x1, regs + ctrl_offset);
            wmb();
            udelay(100);  // Required delay between control writes
            writel(0x1, regs + start_offset);
            wmb();

            // Set final streaming state
            fs->state = 2;  // Streaming state
            fs->flags |= 0x2;  // Set streaming flag

            pr_info("Channel %d streaming started:\n"
                    "  base=0x%x offset=0x%x\n"
                    "  buf_size=%u count=%d\n"
                    "  ctrl=0x%x start=0x%x\n",
                    fs->chn_num,
                    (unsigned int)fc->dma_addr, fc->channel_offset,
                    fc->buf_size, fc->buf_cnt,
                    readl(regs + ctrl_offset),
                    readl(regs + start_offset));

            return 0;
        }
        case VIDIOC_GET_BUFFER_INFO: {
            struct buffer_info info;
            struct frame_source_channel *fc = fs->private;

            if (!fc) {
                pr_err("No channel data for buffer info\n");
                return -EINVAL;
            }

            // Initialize with zeros
            memset(&info, 0, sizeof(info));

            // Set basic info - channel number is important
            info.channel = fs->chn_num;

            // Use actual buffer size from frame source
            info.buffer_size = fs->buf_size;
            if (info.buffer_size == 0) {
                info.buffer_size = fs->width * fs->height * 2;  // YUV422
            }

            // Buffer count should match frame source
            info.count = fs->buf_cnt;
            if (info.count == 0) {
                info.count = 4;  // Default to 4 buffers as seen in LIBIMP
            }

            // Status matches frame source state
            info.status = fs->state;

            // Flags should reflect current state
            info.flags = fs->flags;

            pr_debug("Buffer info for channel %d: size=%u count=%u state=%d flags=0x%x\n",
                     info.channel, info.buffer_size, info.count, info.status, info.flags);

            if (copy_to_user((void __user *)arg, &info, sizeof(info))) {
                pr_err("Failed to copy buffer info to user\n");
                return -EFAULT;
            }

            return 0;
        }
        case VIDIOC_SET_FRAME_MODE: {
            struct frame_mode mode;
            struct frame_source_channel *fc = fs->private;

            if (!fc) {
                pr_err("No channel data for frame mode\n");
                return -EINVAL;
            }

            if (copy_from_user(&mode, (void __user *)arg, sizeof(mode))) {
                pr_err("Failed to copy frame mode\n");
                return -EFAULT;
            }

//            pr_info("Set frame mode: ch=%d mode=%d size=%dx%d format=0x%x flags=0x%x\n",
//                    mode.channel, mode.mode, mode.width, mode.height,
//                    mode.format, mode.flags);

            // If width/height are 0, keep existing values
            if (mode.width == 0 || mode.height == 0) {
                mode.width = fs->width;
                mode.height = fs->height;
                // pr_info("Using existing dimensions: %dx%d\n", mode.width, mode.height);
            }

            // If format is 0, keep existing format
            if (mode.format == 0) {
                mode.format = fs->fmt ? fs->fmt : 0x3231564e; // Default to YUV422
                // pr_info("Using format: 0x%x\n", mode.format);
            }

            // Update frame source configuration
            fs->width = mode.width;
            fs->height = mode.height;
            fs->fmt = mode.format;

            // Calculate buffer size if not provided
            if (mode.buf_size == 0) {
                mode.buf_size = mode.width * mode.height * 2; // YUV422
                //pr_info("Calculated buffer size: %d\n", mode.buf_size);
            }

            // Update buffer size
            if (mode.buf_size != fc->buf_size) {
                fc->buf_size = mode.buf_size;
                fs->buf_size = mode.buf_size;

                //pr_info("Updated buffer size: %d bytes\n", mode.buf_size);

                // Reconfigure DMA if streaming
                if (fs->state == 2) {
                    writel(fc->dma_addr, ourISPdev->regs + ISP_BUF0_OFFSET);
                    writel(fc->buf_size, ourISPdev->regs + ISP_BUF0_OFFSET + 0x4);
                    wmb();
                }
            }

            return 0;
        }
        default:
            return -ENOTTY;
    }
}

static int framechan_release(struct inode *inode, struct file *file)
{
    struct isp_framesource_state *fs = file->private_data;
    struct frame_source_channel *fc;

    if (!fs)
        return 0;

    fc = fs->private;
    if (fc) {
        // Clean up channel data
        if (fs->state == 2) {
            // Disable streaming first
            writel(0, ourISPdev->regs + ISP_STREAM_START);
            writel(0, ourISPdev->regs + ISP_STREAM_CTRL);
            wmb();
            fs->state = 1;
        }

        kfree(fc);
        fs->private = NULL;
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

static int create_framechan_devices(struct device *dev)
{
    int i, ret;

    // Allocate device numbers
    ret = alloc_chrdev_region(&framechan_dev, 0, MAX_CHANNELS, "framechan");
    if (ret) {
        dev_err(dev, "Failed to allocate char device region\n");
        return ret;
    }

    // Create device class
    framechan_class = class_create(THIS_MODULE, "framechan_class");
    if (IS_ERR(framechan_class)) {
        unregister_chrdev_region(framechan_dev, MAX_CHANNELS);
        dev_err(dev, "Failed to create device class\n");
        return PTR_ERR(framechan_class);
    }

    // Create each device node
    for (i = 0; i < MAX_CHANNELS; i++) {
        cdev_init(&framechan_cdev, &framechan_fops);
        framechan_cdev.owner = THIS_MODULE;
        ret = cdev_add(&framechan_cdev, MKDEV(MAJOR(framechan_dev), i), 1);
        if (ret) {
            pr_err("Failed to add cdev for /dev/framechan%d\n", i);
            return ret;
        }

        device_create(framechan_class, NULL, MKDEV(MAJOR(framechan_dev), i), NULL, "framechan%d", i);
        pr_info("Created device /dev/framechan%d\n", i);
    }



    return 0;
}


static void remove_framechan_devices(void)
{
    int i;

    if (framechan_class) {
        for (i = 0; i < MAX_CHANNELS; i++) {
            device_destroy(framechan_class, MKDEV(MAJOR(framechan_dev), i));
        }
        class_destroy(framechan_class);
    }
    unregister_chrdev_region(framechan_dev, MAX_CHANNELS);
}


static int init_sensor_parameters(struct IMPISPDev *dev)
{
    struct i2c_client *client = dev->sensor_i2c_client;

    // Set initial analog gain (adjust values based on sensor datasheet)
    isp_sensor_write_reg(client, 0x3e08, 0x03); // Gain high byte
    isp_sensor_write_reg(client, 0x3e09, 0x10); // Gain low byte

    // Set initial exposure
    isp_sensor_write_reg(client, 0x3e01, 0x0a); // Exposure high byte
    isp_sensor_write_reg(client, 0x3e02, 0x00); // Exposure low byte

    return 0;
}

static int setup_i2c_sensor(struct IMPISPDev *dev)
{
    struct i2c_client *client;
    struct i2c_adapter *adapter;
    struct tx_isp_sensor_win_setting *win_size;

    pr_info("Setting up I2C infrastructure for SC2336...\n");

    // Allocate window size structure
    win_size = kzalloc(sizeof(*win_size), GFP_KERNEL);
    if (!win_size) {
        pr_err("Failed to allocate window size info\n");
        return -ENOMEM;
    }

    // Fill in window settings from driver
    win_size->width = 1920;
    win_size->height = 1080;
    win_size->fps = 25 << 16 | 1;  // Match driver fps format
    win_size->mbus_code = V4L2_MBUS_FMT_SBGGR10_1X10;
    win_size->colorspace = V4L2_COLORSPACE_SRGB;

    // Get I2C adapter
    adapter = i2c_get_adapter(0);  // Make sure this matches your hardware
    if (!adapter) {
        pr_err("Failed to get I2C adapter\n");
        kfree(win_size);
        return -ENODEV;
    }

    struct i2c_board_info board_info = {
        .type = "sc2336",
        .addr = 0x30,  // SC2336 I2C address
        .platform_data = win_size,
    };

    client = i2c_new_device(adapter, &board_info);
    if (!client) {
        pr_err("Failed to create I2C device\n");
        i2c_put_adapter(adapter);
        kfree(win_size);
        return -ENODEV;
    }

    // Store client and window size in device structure
    dev->sensor_i2c_client = client;
    dev->sensor_window_size = win_size;

    pr_info("I2C infrastructure ready for SC2336 sensor at address 0x%02x\n",
            client->addr);
    return 0;
}

static int tisp_probe(struct platform_device *pdev)
{
    struct resource *res;
    void __iomem *base;
    struct isp_graph_data *graph_data;
    int ret;

    pr_info("Starting ISP probe...\n");

    // Validate ourISPdev
    if (!ourISPdev) {
        dev_err(&pdev->dev, "Global ISP device structure not allocated\n");
        return -ENOMEM;
    }

    // Allocate and initialize graph data
    graph_data = kzalloc(sizeof(*graph_data), GFP_KERNEL);
    if (!graph_data) {
        dev_err(&pdev->dev, "Failed to allocate graph data\n");
        return -ENOMEM;
    }

    // Initialize graph data
    graph_data->dev = ourISPdev;
    graph_data->dev_status.sensor_enabled = 0;
    memset(&graph_data->dev_status.fs_status, 0,
           sizeof(struct isp_framesource_status));

    // Store globally for proc entries to use
    global_graph_data = graph_data;

    // Initialize device structure properly
    ourISPdev->dev = &pdev->dev;
    if (!ourISPdev->dev) {
        dev_err(&pdev->dev, "Failed to set device pointer\n");
        ret = -EINVAL;
        goto err_free_graph;
    }

    // Map registers with proper error checking
    base = ioremap(ISP_PHYS_BASE, ISP_REG_SIZE);
    if (!base) {
        dev_err(&pdev->dev, "Failed to map ISP registers\n");
        ret = -ENOMEM;
        goto err_free_graph;
    }

    // Store mapped registers
    ourISPdev->regs = base;

    // Set the IRQ value to 36
    ourISPdev->irq = 36;
    pdev->dev.platform_data = &isp_pdata;
    pr_info("Setting up ISP interrupts...\n");

    // Test register access
    pr_info("Testing register access at base %p\n", base);
    u32 test_val = readl(base + ISP_CTRL_OFFSET);
    pr_info("Initial control register: 0x%08x\n", test_val);

    // Set the IRQ value to 36
    ourISPdev->irq = 36;
    pdev->dev.platform_data = &isp_pdata;

    pr_info("Setting up ISP interrupts... dev=%p\n", ourISPdev);

    // First allocate an IspDevice for IRQ handling
    struct IspDevice *isp_dev = kzalloc(sizeof(*isp_dev), GFP_KERNEL);
    if (!isp_dev) {
        dev_err(&pdev->dev, "Failed to allocate ISP device\n");
        ret = -ENOMEM;
        goto err_unmap_regs;
    }

    // Initialize IspDevice carefully
    memset(isp_dev, 0, sizeof(*isp_dev));
    spin_lock_init(&isp_dev->lock);
    isp_dev->irq_enabled = 0;

    // Store in ourISPdev
    ourISPdev->isp_dev = isp_dev;

    // Allocate and initialize IRQ data
    struct irq_handler_data *irq_data = kzalloc(sizeof(*irq_data), GFP_KERNEL);
    if (!irq_data) {
        dev_err(&pdev->dev, "Failed to allocate IRQ data\n");
        ret = -ENOMEM;
        kfree(isp_dev);
        goto err_unmap_regs;
    }

    // Zero initialize
    memset(irq_data, 0, sizeof(*irq_data));
    raw_spin_lock_init(&irq_data->rlock);
    irq_data->irq_number = ourISPdev->irq;

    pr_info("IRQ structures allocated and initialized: isp_dev=%p irq_data=%p\n",
            isp_dev, irq_data);

    // Set up minimal IRQ info
    struct irq_info irq_info;
    memset(&irq_info, 0, sizeof(irq_info));
    irq_info.irq_type = IRQF_SHARED;

    pr_info("Requesting ISP IRQ with type=0x%x num=%d\n",
            irq_info.irq_type, irq_data->irq_number);

    // Handle IRQ setup directly instead of using tx_isp_request_irq for now
    ret = request_irq(irq_data->irq_number,
                     isp_irq_handler,  // Just use primary handler for now
                     irq_info.irq_type,
                     "isp_driver",
                     irq_data);
    if (ret) {
        pr_err("Failed to request IRQ: %d\n", ret);
        kfree(irq_data);
        kfree(isp_dev);
        goto err_unmap_regs;
    }

    // Store IRQ data after successful setup
    ourISPdev->irq_data = irq_data;

    pr_info("ISP IRQ setup complete: num=%d\n", irq_data->irq_number);
    // Configure clocks
    ret = configure_isp_clocks(ourISPdev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to configure ISP clocks: %d\n", ret);
        goto err_unmap_regs;
    }

    // Initialize reserved memory
    ret = init_isp_reserved_memory(pdev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to initialize reserved memory\n");
        goto err_cleanup_clocks;
    }

    // Set up memory regions
    ret = setup_isp_memory_regions(ourISPdev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to setup memory regions\n");
        goto err_cleanup_memory;
    }

    // Setup I2C sensor
    ret = setup_i2c_sensor(ourISPdev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to setup I2C sensor\n");
        goto err_cleanup_memory;
    }

    // Create proc entries - using graph_data
    ret = create_isp_proc_entries(graph_data);
    if (ret) {
        dev_err(&pdev->dev, "Failed to create proc entries: %d\n", ret);
        goto err_cleanup_memory;
    }

    // Initialize ISP
    ret = tisp_init(&pdev->dev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to initialize ISP subsystem\n");
        goto err_cleanup_proc;
    }

    ret = setup_isp_subdevs(ourISPdev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to setup subdevices\n");
        goto err_cleanup_proc;
    }

    // Keep links enabled
    if (ourISPdev->subdevs && ourISPdev->subdevs[0]) {
        struct isp_subdev_state *sd = ourISPdev->subdevs[0];
        if (sd->src_pads && sd->sink_pads) {
            sd->src_pads->link_state = LINK_STATE_ENABLED;
            sd->sink_pads->link_state = LINK_STATE_ENABLED;
        }
    }

    ret = create_framechan_devices(&pdev->dev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to create frame channels\n");
        goto err_cleanup_proc;
    }

    // Register /dev/isp-m0 device
    ret = misc_register(&isp_m0_miscdev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to register isp-m0 device\n");
        goto err_cleanup_proc;
    }

    platform_set_drvdata(pdev, ourISPdev);

    pr_info("ISP probe completed successfully\n");
    return 0;

err_cleanup_proc:
    remove_isp_proc_entries();
err_cleanup_memory:
    cleanup_isp_memory(ourISPdev);
err_cleanup_clocks:
    cleanup_isp_clocks(ourISPdev);
err_free_irq_data:
    tx_isp_free_irq(ourISPdev->irq_data);
err_unmap_regs:
    if (ourISPdev->regs)
        iounmap(ourISPdev->regs);
err_free_graph:
    if (graph_data) {
        kfree(graph_data);
        global_graph_data = NULL;
    }
    return ret;
}


static int tisp_remove(struct platform_device *pdev) {
    struct IMPISPDev *dev = platform_get_drvdata(pdev);

    pr_info("ISP device remove called\n");

    if (dev) {
        if (dev->sensor_window_size) {
            kfree(dev->sensor_window_size);
            dev->sensor_window_size = NULL;
        }
     	// Unregister /dev/isp-m0 device
       	misc_deregister(&isp_m0_miscdev);

        // Remove frame channel devices first
        remove_framechan_devices();

        // Remove proc entries before cleaning up their data source
        remove_isp_proc_entries();

        // Clean up graph data
        if (global_graph_data) {
            kfree(global_graph_data);
            global_graph_data = NULL;
        }

        // Clean up device subdevices
        // This should be done before memory cleanup
        if (dev->subdevs) {
            int i;
            for (i = 0; dev->subdevs[i]; i++) {
                kfree(dev->subdevs[i]);
            }
            kfree(dev->subdevs);
            dev->subdevs = NULL;
        }

        // Clean up memory regions
        cleanup_isp_memory(dev);

        // Clean up device clocks
        cleanup_isp_clocks(dev);

        // Clean up I2C
        if (dev->sensor_i2c_client) {
            i2c_unregister_device(dev->sensor_i2c_client);
            dev->sensor_i2c_client = NULL;
        }

        // Unmap registers
        if (dev->regs) {
            iounmap(dev->regs);
            dev->regs = NULL;
        }

        // Free tuning data if it exists
        if (dev->tuning_data) {
            kfree(dev->tuning_data);
            dev->tuning_data = NULL;
        }

        // Finally free the device
        kfree(dev);
    }

    pr_info("ISP device removed\n");
    return 0;
}


/* Platform device ID matching table */
static struct platform_device_id tisp_platform_ids[] = {
    {
        .name = "tisp-driver",
        .driver_data = 0,
    },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(platform, tisp_platform_ids);

/* Platform driver structure */
static struct platform_driver tisp_platform_driver = {
    .probe = tisp_probe,
    .remove = tisp_remove,
    .driver = {
        .name = "tisp-driver",
        .owner = THIS_MODULE,
    },
    .id_table = tisp_platform_ids,  // Add this
};

// Define the attribute show function
static ssize_t isp_status_show(struct device *dev,
                              struct device_attribute *attr, char *buf)
{
    if (!ourISPdev)
        return sprintf(buf, "ISP device not initialized\n");

    return sprintf(buf, "ISP Status:\n"
                       "  Open: %s\n"
                       "  Sensor: %s\n"
                       "  WDR Mode: %d\n",
                       ourISPdev->is_open ? "yes" : "no",
                       ourISPdev->sensor_name[0] ? ourISPdev->sensor_name : "none",
                       ourISPdev->wdr_mode);
}

// Create the device attribute
static DEVICE_ATTR(status, S_IRUGO, isp_status_show, NULL);

static int isp_dev_uevent(struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}

/* Module initialization function */
static int __init isp_driver_init(void)
{
    int ret;
    struct platform_device *pdev;
    struct resource res[] = {
        {
            .start  = ISP_BASE_ADDR,
            .end    = ISP_BASE_ADDR + ISP_MAP_SIZE - 1,
            .flags  = IORESOURCE_MEM,
            .name   = "tisp-regs",
        }
    };

    pr_info("Loading ISP driver...\n");

    // 1. Create the tx-isp character device first
    ret = alloc_chrdev_region(&tisp_dev_number, 0, 1, "tx-isp");
    if (ret < 0) {
        pr_err("Failed to allocate device number\n");
        return ret;
    }

    // 2. Create device class
    tisp_class = class_create(THIS_MODULE, "tx-isp");
    if (IS_ERR(tisp_class)) {
        unregister_chrdev_region(tisp_dev_number, 1);
        return PTR_ERR(tisp_class);
    }

    // Set class permissions
    tisp_class->dev_uevent = isp_dev_uevent;

    // 3. Allocate device structure
    ourISPdev = kzalloc(sizeof(struct IMPISPDev), GFP_KERNEL);
    if (!ourISPdev) {
        class_destroy(tisp_class);
        unregister_chrdev_region(tisp_dev_number, 1);
        return -ENOMEM;
    }
    pr_info("ourISPdev allocated at %p\n", ourISPdev);

    // 4. Initialize character device
    cdev_init(&ourISPdev->cdev, &isp_fops);
    ourISPdev->cdev.owner = THIS_MODULE;

    ret = cdev_add(&ourISPdev->cdev, tisp_dev_number, 1);
    if (ret) {
        kfree(ourISPdev);
        class_destroy(tisp_class);
        unregister_chrdev_region(tisp_dev_number, 1);
        return ret;
    }

    // 5. Create device node
    tisp_device = device_create(tisp_class, NULL, tisp_dev_number, NULL, "tx-isp");
    if (IS_ERR(tisp_device)) {
        pr_err("Failed to create tx-isp device\n");
        cdev_del(&ourISPdev->cdev);
        kfree(ourISPdev);
        class_destroy(tisp_class);
        unregister_chrdev_region(tisp_dev_number, 1);
        return PTR_ERR(tisp_device);
    }

    // 6. Create device attributes
    if (tisp_device) {
        if (device_create_file(tisp_device, &dev_attr_status) < 0) {
            pr_err("Failed to create device attributes\n");
        }
    }

    // 7. Register platform driver
    pr_info("Registering platform driver\n");
    ret = platform_driver_register(&tisp_platform_driver);
    if (ret) {
        pr_err("Failed to register platform driver: %d\n", ret);
        goto err_destroy_device;
    }
    pr_info("Platform driver registered successfully\n");

    // 8. Create and register platform device
    pr_info("Registering platform device with name 'tisp-driver'\n");
    pdev = platform_device_register_simple("tisp-driver", -1, res, ARRAY_SIZE(res));
    if (IS_ERR(pdev)) {
        ret = PTR_ERR(pdev);
        pr_err("Failed to register platform device: %d\n", ret);
        platform_driver_unregister(&tisp_platform_driver);
        goto err_destroy_device;
    }
    pr_info("Platform device registered successfully\n");

    pr_info("ISP driver loaded successfully\n");
    return 0;

err_destroy_device:
    device_destroy(tisp_class, tisp_dev_number);
    cdev_del(&ourISPdev->cdev);
    kfree(ourISPdev);
    class_destroy(tisp_class);
    unregister_chrdev_region(tisp_dev_number, 1);
    return ret;
}

/* Module cleanup function */
static void __exit isp_driver_exit(void)
{
    struct platform_device *pdev = NULL;
    struct device *dev = NULL;

    // Add this block
    if (ourISPdev && ourISPdev->fs_info) {
        cleanup_frame_buffers(ourISPdev->fs_info);
    }

    remove_isp_proc_entries();

    /* Find our platform device */
    dev = bus_find_device_by_name(&platform_bus_type, NULL, "tisp-driver");
    if (dev) {
        pdev = to_platform_device(dev);
        platform_device_unregister(pdev);
        put_device(dev);
    }

    // Rest of existing cleanup...
    platform_driver_unregister(&tisp_platform_driver);
    if (ourISPdev) {
        cleanup_isp_memory(ourISPdev);
        if (ourISPdev->buf_info) {
            kfree(ourISPdev->buf_info);
            ourISPdev->buf_info = NULL;
        }
        if (ourISPdev->wdr_buf_info) {
            kfree(ourISPdev->wdr_buf_info);
            ourISPdev->wdr_buf_info = NULL;
        }
        kfree(ourISPdev);
        ourISPdev = NULL;
    }

    platform_driver_unregister(&tisp_platform_driver);
    unregister_chrdev_region(tisp_dev_number, 1);
    class_destroy(tisp_class);

    pr_info("ISP driver unloaded\n");
}

module_init(isp_driver_init);
module_exit(isp_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Custom ISP Driver with SoC Detection");
