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
#include <linux/printk.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/capability.h>
#include <linux/videodev2.h>
#include <linux/errno.h>
#include <linux/wait.h>
#include <media/videobuf2-core.h>
#include <media/videobuf2-dma-contig.h>
#include <media/v4l2-device.h>

#include <tx-isp-device.h>
#include <tx-isp-common.h>
#include <tx-isp-debug.h>
#include <isp_driver_common.h>

// Add these at the top after includes
static int __init_or_module early_init(void) __attribute__((constructor));
static int early_init(void)
{
    printk(KERN_INFO "ISP: Early initialization starting\n");
    return 0;
}

#define ISP_BASE_ADDR        0x13300000
#define ISP_MAP_SIZE         0x1000
#define ISP_OFFSET_PARAMS    0x1000
#define REG_CONTROL          0x100

// CPU info register addresses
#define CPU_ID_ADDR          0x1300002c
#define CPPSR_ADDR           0x10000034
#define SUBTYPE_ADDR         0x13540238
#define SUBREMARK_ADDR       0x13540231

#define VIDIOC_GET_SENSOR_LIST 0xc050561a

// Log level definitions
#define IMP_LOG_ERROR   6
#define IMP_LOG_INFO    3
#define IMP_LOG_DEBUG   4

/* Hardware interface helper macros */
#define ISP_REG_WDR_GAIN0    0x1000  /* From decompiled code */
#define ISP_REG_WDR_GAIN1    0x100c  /* From decompiled code */
#define ISP_REG_STD_GAIN     0x1030  /* Standard gain register */


// Register offsets (these are just examples; they should match your ISP hardware specification)
#define ISP_MODE_REG            0x00
#define ISP_INPUT_FORMAT_REG    0x04
#define ISP_OUTPUT_FORMAT_REG   0x08
#define ISP_FEATURES_REG        0x0C

// Format and feature values (again, these should be based on the hardware spec)
#define ISP_MODE_CAPTURE        0x01  // Capture mode
#define ISP_FORMAT_YUV420       0x10  // YUV420 input format
#define ISP_FEATURE_NR          0x01  // Noise reduction enabled
#define ISP_FEATURE_LDC         0x02  // Lens distortion correction enabled


// Device structure for ISP
struct IMPISPDev {
    int result;
    char dev_name[32];     // Device name/path
    struct cdev cdev;      // Character device structure
    int major;             // Major number
    int minor;             // Minor number
    int fd;                // File descriptor
    int is_open;           // Device open status
    char sensor_name[80];  // Sensor name buffer
    char padding1[0x60];   // Padding to 0xac
    void* buf_info;        // Buffer info pointer
    int wdr_mode;          // WDR mode flag
    void* wdr_buf_info;    // WDR buffer info pointer
    unsigned int current_link;  // Track current link configuration
    char padding2[0x24];   // Adjusted padding (reduced by 4 bytes for current_link)
    struct device *dev;    // Device pointer

    // Fields for hardware-level operations
    void *regs;            // Register base (mapped registers)
    int irq;               // Interrupt line
};

static void __iomem *reg_base;
static uint32_t soc_id = 0xFFFFFFFF;
static uint32_t cppsr = 0xFFFFFFFF;
static uint32_t subsoctype = 0xFFFFFFFF;
static uint32_t subremark = 0xFFFFFFFF;
struct IMPISPDev *gISPdev = NULL;
uint32_t globe_ispdev = 0x0;
static struct resource *mem_region;

static struct class *tisp_class;
static struct device *tisp_device;
static dev_t tisp_dev_number;

static DEFINE_IDA(soc_ida);
static DEFINE_SPINLOCK(soc_lock);

struct tisp_param_info {
    uint32_t data[8];  // Array size can be adjusted based on needs
};

/* Private data structure for our device */
struct tisp_device {
    struct device *dev;
    struct tisp_param_info params;
    void __iomem *reg_base;
};

struct tisp_device *tisp_dev = NULL;

// Add these definitions for device status
#define MAX_FRAMESOURCE_CHANNELS 3

struct isp_framesource_status {
    int chan_status[MAX_FRAMESOURCE_CHANNELS];  // 0 = stop, 1 = running
};

struct isp_device_status {
    int sensor_enabled;
    struct isp_framesource_status fs_status;
    bool memory_initialized;
    int w02_values[13];  // For isp-w02 specific values
};

// Add status tracking to the graph data structure
struct isp_graph_data {
    struct tisp_device *dev;
    unsigned int device_count;
    struct platform_device **devices;
    struct isp_device_status dev_status;
    void *node_data[];
} __attribute__((aligned(8)));

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

// Define a generic callback type for show functions
typedef int (*show_func_t)(struct seq_file *, void *);

// Structure to pass both function and data
struct proc_data {
    show_func_t show_func;
    void *private_data;
};

typedef struct {
    uint32_t handler_status;
    // Other fields related to interrupt status
} InterruptStatus;

typedef struct {
    void* current_handler;
    // Pointer to next handler in the list
    struct HandlerList* next;
} HandlerList;

typedef struct {
    int32_t result;
    // Other handler data fields
} HandlerData;

typedef struct {
    InterruptStatus* interrupt_status;
    HandlerList* handler_list;
    HandlerList* handler_list_end;
} DeviceContext;


// Structs for handling interrupt-related data and configurations
struct irq_task {
    void (*task_function)(void);
    uint32_t status;
    void* task_data;
};

struct irq_handler_data {
    struct irq_task *task_list;
    int task_count;
    int32_t irq_number;  // IRQ number
    void* handler_function;  // Pointer to the interrupt handler function
    void* disable_function;  // Pointer to the function to disable the IRQ
    raw_spinlock_t rlock;    // Add the raw spinlock here
};

// Struct for passing IRQ-related information to functions
struct irq_info
{
    int32_t irq_type;  // IRQ type or identifier
};

struct ClockConfig
{
    const char *clock_name;  // Change to const char* to hold the clock name
    int32_t clock_rate;      // Clock rate
};


struct IspDevice
{
    char _0[8];              // Reserved/unused padding
    char _8[0x30];           // Reserved padding (likely for alignment)
    char _38[8];             // Reserved padding
    char _40[0x38];          // Reserved padding
    char _78[8];             // Reserved padding
    spinlock_t lock;         // Spinlock for synchronization
    void (*irq_handler)(struct IspDevice*, unsigned long flags);  // Function pointer for the IRQ handler
    int32_t irq_enabled;     // Flag indicating whether IRQs are enabled
};

struct IspSubdev
{
    char _0[4];              // Reserved/unused padding
    int32_t clock_source;    // The clock source (integer value)
    char _8[0x38];           // Reserved padding (likely for alignment)
    char _40[0x40];          // Reserved padding (likely for alignment)
    char _80[0x3c];          // Reserved padding (likely for alignment)
    void* allocated_clocks;  // Pointer to allocated clocks (generic pointer)
    int32_t num_clocks;      // Number of clocks allocated (integer value)
    struct device *dev;      // Device associated with this sub-device (for clock management)
};

struct sensor_list {
    char sensor_name[32];  // Can hold the "sc2336" sensor name
    int num_sensors;       // Number of currently registered sensors
};

#define MAX_SENSORS 4

struct sensor_info {
    char name[32];
    bool registered;
};

static struct sensor_info registered_sensors[MAX_SENSORS];
static int num_registered_sensors = 0;

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


struct DriverInterface {
    void* field_00;
};


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

#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/mutex.h>
#include <linux/videodev2.h>

/* Register block structure based on decompiled access patterns */
struct isp_reg_block {
    u32 width;          /* 0xec offset in decompiled */
    u32 height;         /* 0xf0 offset */
    u32 gain;
    u32 exposure;
    u32 gain_factor;
    u32 wdr_enable;     /* 0x17c offset seen in decompiled */
    u32 reserved[50];   /* Padding for other registers */
    u32 af_stats[16];   /* AF statistics registers */
    u32 ae_stats[16];   /* AE statistics registers */
};

struct isp_pad_desc {
    char *name;                // 0x00: Pad name
    uint8_t type;             // 0x04: Pad type (1=source, 2=sink)
    uint8_t index;            // 0x05: Pad index
    uint32_t flags;           // Flags for pad capabilities
    uint32_t link_type;       // Type of link
    uint32_t link_state;      // State of link
    void *source;             // Source pad pointer
    void *sink;               // Sink pad pointer
    void *entity;             // Entity pointer
};

/**
 * struct isp_subdev - ISP sub-device structure
 * Referenced in decompiled at multiple offsets
 */
struct isp_subdev {
    char name[32];             // 0x00: Device name
    char *name_ptr;            // 0x08: Pointer to name
    struct v4l2_subdev sd;
    struct mutex lock;
    u32 index;
    void *priv;
    void __iomem *regs;        // Add this for register mapping
    struct tx_isp_subdev_ops *ops;  // Add this for ops
    uint16_t num_sink_pads;    // 0xc8: Number of sink pads
    uint16_t num_src_pads;     // 0xca: Number of source pads
    uint32_t sink_pads;        // 0xcc: Base address of sink pads array
    uint32_t src_pads;         // 0xd0: Base address of source pads array
};

struct isp_pipeline {
    unsigned int input_format;
    unsigned int output_format;
    bool scaling_enabled;
    unsigned int input_width;
    unsigned int input_height;
    unsigned int output_width;
    unsigned int output_height;
};

/**
 * struct isp_device - Main ISP device structure
 * Based on decompiled code access patterns
 */
struct isp_device {
    /* Device infrastructure */
    struct device *dev;  // Parent device
    struct cdev cdev;               /* 0x04: Character device */
    struct v4l2_device v4l2_dev;    /* 0x08: V4L2 device */

    /* Hardware interface */
    void __iomem *base;             /* Base address for register access */
    struct isp_reg_block __iomem *regs;  /* 0x2c: Register block - heavily accessed */

    /* Video device handling */
    struct video_device *video_dev;  /* Video device structure */
    struct vb2_queue vb2_queue;      /* Video buffer queue */

    /* Sub-devices - array seen at 0x2c with multiple entries */
    struct isp_subdev *subdevs[16];  /* Array of subdevices */
    int num_subdevs;

    /* Link configuration */
    unsigned int current_link;       /* 0x10c: Current link configuration */
    struct mutex link_lock;          /* Protect link configuration */

    /* Buffer management */
    struct list_head buffer_queue;    /* Queue of buffers */
    spinlock_t buffer_lock;          /* Protect buffer queue */

    /* Algorithm state */
    struct {
        bool ae_enabled;
        bool awb_enabled;
        bool af_enabled;
        bool wdr_enabled;            /* WDR mode state */
    } algo_state;

    /* IRQ handling */
    int irq;                         /* IRQ number */
    spinlock_t irq_lock;            /* Protect IRQ handling */

    /* DMA handling */
    struct {
        dma_addr_t addr;            /* DMA address */
        size_t size;                /* DMA buffer size */
    } dma_buf;

    /* Memory optimization */
    bool memopt_enabled;            /* Memory optimization enabled flag */

    /* Pad configuration - based on decompiled pad handling */
    struct isp_pad_desc *pads;      /* Array of pad descriptors */
    int num_pads;                   /* Number of configured pads */
    struct isp_pipeline pipeline;  /* Pipeline configuration */
    /* Private data */
    void *priv;                     /* Driver private data */
};

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

int32_t isp_irq_handler(int32_t irq, void* device_context)
{
    int32_t result = 1;

    // Cast the device_context to the correct type
    DeviceContext* ctx = (DeviceContext*)device_context;

    // Ensure device_context is valid
    if (ctx != NULL && ctx->interrupt_status != NULL)
    {
        InterruptStatus* interrupt_status = ctx->interrupt_status;

        // Check interrupt status and handler status
        if (interrupt_status != NULL)
        {
            int32_t handler_status = interrupt_status->handler_status;

            if (handler_status == 0)
            {
                result = 1;
            }
            else
            {
                result = 1;

                // Call the handler function if it is valid
                if (handler_status == 2)
                {
                    result = 2;
                }
            }
        }
    }
    else
    {
        result = 1;
    }

    // Process the handler list
    HandlerList* handler_list = ctx->handler_list;
    void* current_handler = handler_list ? handler_list->current_handler : NULL;

    while (current_handler != NULL)
    {
        HandlerData* handler_data = (HandlerData*)current_handler;

        if (handler_data != NULL)
        {
            int32_t handler_result = handler_data->result;

            if (handler_result != 0 && handler_result == 2)
            {
                result = 2;
            }
        }

        // Move to the next handler in the list
        handler_list = handler_list->next;
        current_handler = handler_list ? handler_list->current_handler : NULL;

        if (handler_list == ctx->handler_list_end)
        {
            break;
        }
    }

    return result;
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

int32_t isp_subdev_init_clks(struct IspSubdev* ispSubdev, struct ClockConfig* clockConfig)
{
    // Get the number of clocks from the IspSubdev struct
    int32_t num_clocks = ispSubdev->num_clocks;

    // Calculate memory size needed for the clocks array
    int32_t memorySize = num_clocks * sizeof(struct clk *);  // Use size of struct clk*

    // If no clocks are present, return null for allocated clocks
    if (num_clocks != 0)
    {
        // Allocate memory for the clocks array
        struct clk **clocksArray = kmalloc(memorySize, GFP_KERNEL);

        // Check if memory allocation failed
        if (!clocksArray)
        {
            printk(KERN_ERR "Failed to allocate memory for clocks\n");
            return -ENOMEM;  // Return error code for allocation failure
        }

        // Initialize the allocated memory to zero
        memset(clocksArray, 0, memorySize);

        // Set up pointers to iterate over the ClockConfig struct and allocated memory
        struct ClockConfig* currentClockConfig = clockConfig;
        struct clk** currentClock = clocksArray;
        int32_t clockIndex = 0;

        // Iterate through all the clocks
        while (clockIndex < num_clocks)
        {
            // Get the clock handle using the clock source and clock name from the struct
            struct clk *clockHandle = clk_get(ispSubdev->dev, currentClockConfig->clock_name);  // Correct type

            // Check if clockHandle is valid (non-NULL)
            if (IS_ERR(clockHandle))
            {
                // Print error if getting the clock handle failed
                printk(KERN_ERR "Failed to get clock %s\n", currentClockConfig->clock_name);

                // Free previously allocated clocks and return error
                for (int32_t i = 0; i < clockIndex; i++)
                {
                    clk_put(clocksArray[i]);
                }
                kfree(clocksArray);
                return PTR_ERR(clockHandle);  // Return the error code
            }

            // Store the clock handle in the allocated memory
            clocksArray[clockIndex] = clockHandle;

            // If the clock rate is specified, set it
            int32_t clockRateResult = 0;
            if (currentClockConfig->clock_rate != 0xFFFF)  // Check if valid rate is specified
            {
                clockRateResult = clk_set_rate(clockHandle, currentClockConfig->clock_rate);
                if (clockRateResult < 0)
                {
                    // Print error if setting the clock rate failed
                    printk(KERN_ERR "Failed to set clock rate for %s\n", currentClockConfig->clock_name);
                    // Free previously allocated clocks and return error
                    for (int32_t i = 0; i <= clockIndex; i++)
                    {
                        clk_put(clocksArray[i]);
                    }
                    kfree(clocksArray);
                    return clockRateResult;
                }
            }

            // Move to the next clock config and increment clockIndex
            clockIndex++;
            currentClockConfig++;  // Move to the next ClockConfig entry
        }

        // Store the allocated clocks array in the IspSubdev struct
        ispSubdev->allocated_clocks = clocksArray;

        // Return success
        return 0;
    }
    else
    {
        // If there are no clocks, set allocated_clocks to null
        ispSubdev->allocated_clocks = NULL;
    }

    return 0;  // Return success if no clocks to process
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
//    // Check if the misc_deregister_flag is non-zero
//    if (arg1->misc_deregister_flag != 0)
//    {
//        // Deregister the misc device
//        misc_deregister((int32_t*)((char*)arg1 + 0xc));  // Accessing misc device pointer
//    }
//
//    // Release clocks associated with the subdevice
      isp_subdev_release_clks(sd);
//
//    // Free input pads if they are allocated
//    int32_t input_pads = arg1->input_pads;
//    if (input_pads != 0)
//    {
//        kfree(input_pads);
//    }
//
//    // Free allocated input pads if they are allocated
//    int32_t allocated_input_pads = arg1->allocated_input_pads;
//    if (allocated_input_pads != 0)
//    {
//        kfree(allocated_input_pads);
//    }
//
//    // Unmap register-mapped address if it is non-zero
//    int32_t register_mapped_address = arg1->register_mapped_address;
//    if (register_mapped_address != 0)
//    {
//        iounmap(register_mapped_address);
//    }
//
//    // Handle memory region release
      void* memory_region = sd->base;
      int32_t irq_number;
//
      if (memory_region == NULL)
      {
	  irq_number = sd->irqdev.irq;
      }
      else
      {
          // Release the memory region if it's allocated
          int32_t start_address = *(uint32_t*)memory_region;
          release_mem_region(start_address, *((uint32_t*)((char*)memory_region + 4)) + 1 - start_address);
          sd->base = NULL;  // Set memory region to NULL
	  irq_number = sd->irqdev.irq;
      }
//
//    // Free the IRQ if it's valid
    if (irq_number != 0)
    {
	tx_isp_free_irq(&sd->irqdev.irq);
    }

    // Call the module deinit function and store the result
    tx_isp_module_deinit(sd);
}
EXPORT_SYMBOL(tx_isp_subdev_deinit);

// Individual device proc handlers
static int isp_fs_show(struct seq_file *m, void *v)
{
    struct isp_device_status *status = m->private;
    int i;

    if (!status) {
        seq_puts(m, "Error: No status data available\n");
        return 0;
    }

    for (i = 0; i < MAX_FRAMESOURCE_CHANNELS; i++) {
        seq_printf(m, "############## framesource %d ###############\n", i);
        seq_printf(m, "chan status: %s\n",
                  status->fs_status.chan_status[i] ? "running" : "stop");
    }
    return 0;
}

static int isp_m0_show(struct seq_file *m, void *v)
{
    struct isp_device_status *status = m->private;

    if (!status) {
        seq_puts(m, "Error: No status data available\n");
        return 0;
    }

    seq_puts(m, "****************** ISP INFO **********************\n");
    if (!status->sensor_enabled) {
        seq_puts(m, "sensor doesn't work, please enable sensor\n");
    } else {
        seq_puts(m, "sensor is working\n");
    }
    return 0;
}

static int isp_w00_show(struct seq_file *m, void *v)
{
    struct isp_device_status *status = m->private;

    if (!status) {
        seq_puts(m, "Error: No status data available\n");
        return 0;
    }

    if (!status->sensor_enabled) {
        seq_puts(m, "sensor doesn't work, please enable sensor\n");
    }
    return 0;
}

static int isp_w02_show(struct seq_file *m, void *v)
{
    struct isp_device_status *status = m->private;
    int i;

    if (!status) {
        seq_puts(m, "Error: No status data available\n");
        return 0;
    }

    // First line with two values
    seq_printf(m, " %d, %d\n", status->w02_values[0], status->w02_values[1]);

    // Second line with remaining values
    for (i = 2; i < 13; i++) {
        seq_printf(m, "%d%s", status->w02_values[i],
                  (i < 12) ? ", " : "\n");
    }
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

// Add these to your global variables
static struct proc_data *proc_data_fs = NULL;
static struct proc_data *proc_data_m0 = NULL;
static struct proc_data *proc_data_w00 = NULL;
static struct proc_data *proc_data_w01 = NULL;
static struct proc_data *proc_data_w02 = NULL;


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

int read_soc_info(void);

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



#define BUFFER_SIZE  (1024 * 1024)  // 1MB buffer size (adjust as needed)

static int isp_device_memory_setup(struct isp_device *dev)
{
    int ret;

    if (!dev) {
        pr_err("ISP device is NULL\n");
        return -EINVAL;
    }

    pr_info("Memory buffers for ISP device set up successfully\n");

    return 0;
}

/* ISP Format definitions - typically seen in t31 ISP */
enum isp_format {
    ISP_FORMAT_RAW = 0,       /* RAW sensor data */
    ISP_FORMAT_RGB = 1,       /* RGB format */
    ISP_FORMAT_YUV = 2,       /* YUV format */
    ISP_FORMAT_BAYER = 3,     /* Bayer pattern */
};

/* Optional: More detailed format definitions if needed */
#define ISP_FORMAT_RGB888     (ISP_FORMAT_RGB | (0 << 8))  /* 24-bit RGB */
#define ISP_FORMAT_RGB565     (ISP_FORMAT_RGB | (1 << 8))  /* 16-bit RGB */
#define ISP_FORMAT_YUV422     (ISP_FORMAT_YUV | (0 << 8))  /* YUV 4:2:2 */
#define ISP_FORMAT_YUV420     (ISP_FORMAT_YUV | (1 << 8))  /* YUV 4:2:0 */
#define ISP_FORMAT_BAYER_RGGB (ISP_FORMAT_BAYER | (0 << 8)) /* RGGB pattern */
#define ISP_FORMAT_BAYER_GRBG (ISP_FORMAT_BAYER | (1 << 8)) /* GRBG pattern */
#define ISP_FORMAT_BAYER_GBRG (ISP_FORMAT_BAYER | (2 << 8)) /* GBRG pattern */
#define ISP_FORMAT_BAYER_BGGR (ISP_FORMAT_BAYER | (3 << 8)) /* BGGR pattern */

static int isp_device_pipeline_setup(struct isp_device *dev)
{
    if (!dev) {
        pr_err("ISP device is NULL\n");
        return -EINVAL;
    }

    // Set up the input and output pipelines
    // Example: Configure the input video format, scaling, and output format
    dev->pipeline.input_format = ISP_FORMAT_YUV420;
    dev->pipeline.output_format = ISP_FORMAT_RGB;
    dev->pipeline.scaling_enabled = true;

    // Configure pipeline parameters (e.g., resolution, aspect ratio, etc.)
    dev->pipeline.input_width = 1920;
    dev->pipeline.input_height = 1080;
    dev->pipeline.output_width = 1920;
    dev->pipeline.output_height = 1080;

    pr_info("ISP pipeline configured with input and output formats\n");

    return 0;
}

static int isp_device_interrupts_setup(struct IMPISPDev *dev)
{
    if (!dev) {
        pr_err("ISP device is NULL\n");
        return -EINVAL;
    }

    // Check if interrupts are supported by the hardware (check the irq field in IMPISPDev)
    if (!dev->irq) {
        pr_info("ISP device does not support interrupts\n");
        return 0;  // No interrupts supported
    }

    // Request the interrupt line
    // Assuming you will uncomment the request_irq code when it's ready for use
    if (request_irq(dev->irq, isp_irq_handler, IRQF_SHARED, "isp_device", dev)) {
        pr_err("Failed to request IRQ\n");
        return -EBUSY;
    }

    // TODO: Enable interrupts in the hardware
    // Enable interrupts in the hardware (you will need to modify this line
    // to use the correct register or function for your hardware)
    // For example, write to a specific register to enable interrupts:
    // write_register(dev->regs, ISP_INT_ENABLE_REG, ISP_INT_FRAME_DONE);

    pr_info("ISP interrupts set up successfully\n");

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

    // Step 1: Configure hardware registers
    ret = isp_device_registers_configure(dev);
    if (ret) {
        pr_err("Failed to configure ISP hardware registers\n");
        return ret;
    }

    // Step 2: Set up memory buffers (e.g., DMA buffers)
    ret = isp_device_memory_setup(dev);
    if (ret) {
        pr_err("Failed to set up memory buffers for ISP device\n");
        return ret;
    }

    // Step 3: Configure ISP pipeline (input/output configuration, scaling, etc.)
    ret = isp_device_pipeline_setup(dev);
    if (ret) {
        pr_err("Failed to set up ISP pipeline\n");
        return ret;
    }

    // Step 4: Set up interrupts, if required
    ret = isp_device_interrupts_setup(dev);  // Corrected to pass the device struct
    if (ret) {
        pr_err("Failed to set up interrupts for ISP device\n");
        return ret;
    }

    // Mark the device as initialized
    pr_info("ISP device successfully configured\n");

    return 0;  // Success
}


static int tisp_open(struct inode *inode, struct file *file)
{
    if (!gISPdev) {
        pr_err("ISP device not initialized\n");
        return -ENODEV;  // Return device not initialized error
    }

    if (gISPdev->is_open) {
        pr_info("Device is already opened\n");
        return -EBUSY;
    }

    // Perform any necessary hardware register configuration
    if (isp_device_configure(gISPdev)) {
        pr_err("Failed to configure ISP hardware\n");
        return -EIO;
    }

    // Check or initialize interrupts if required
    if (isp_device_interrupts_setup(gISPdev)) {
        pr_err("Failed to setup interrupts\n");
        return -EIO;
    }

    // Mark the device as opened
    gISPdev->is_open = 1;
    pr_info("Device opened successfully\n");

    return 0;  // Success
}


static int tisp_release(struct inode *inode, struct file *file)
{
    if (gISPdev) {
        // Check if the device is open
        if (gISPdev->is_open) {
            // Mark the device as closed
            gISPdev->is_open = 0;
            pr_info("Device closed successfully\n");
        } else {
            pr_info("Device was not opened or already closed\n");
        }
    } else {
        pr_err("ISP device not initialized\n");
    }

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

/* Pad capabilities/flags from decompiled code inspection */
#define PAD_FL_SINK       BIT(0)
#define PAD_FL_SOURCE     BIT(1)
#define PAD_FL_ACTIVE     BIT(2) // value 0x4 in decompiled code

/* Link types/states seen in decompiled */
#define LINK_STATE_INACTIVE  0
#define LINK_STATE_SOURCE    3  // Seen in decompiled at 0xe418, 0xe430
#define LINK_STATE_ENABLED   4  // From check at 0xe390

/* Maximum number of links from decompiled array size at 0x6ad80 */
#define MAX_LINK_PADS    16

/**
 * struct isp_link_config - Link configuration data from decompiled
 * Based on the array at offset 0x6ad7c and 0x6ad80 in decompiled
 */
struct isp_link_config {
    struct isp_pad_desc *pads;
    int num_pads;
};

/* From decompiled - appears to be a fixed config table */
static struct isp_link_config link_configs[] = {
    /* Would contain the OEM's pad configurations */
    /* We'd need to reverse engineer the full table */
};

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
 * setup_video_link - Configure video link routing
 * @dev: IMPISPDev structure
 * @link: Link identifier from userspace
 *
 * Based on decompiled code at case 0x800456d0
 */
int setup_video_link(struct IMPISPDev *dev, unsigned int link)
{
    struct isp_pad_desc *source_pad, *sink_pad;
    struct isp_link_config *config;
    struct isp_pad_desc *pad;
    int i;

    /* Validate link index - must be < 2 as seen at 0xe2a4 */
    if (link >= 2) {
        dev_err(dev->dev, "link(%d) is invalid!\n", link);
        return -EINVAL;
    }

    /* Store the current link in our global dev structure */
    if (link == dev->current_link)
        return 0;

    /* Get link configuration */
    config = &link_configs[link];
    pad = config->pads;

    /* Process all pads in this configuration */
    for (i = 0; i < config->num_pads; i++, pad += 5) { // +5 from decompiled struct size
        /* Find source and sink pads - matches logic at 0xe31c */
        source_pad = find_subdev_link_pad(dev, pad);
        sink_pad = find_subdev_link_pad(dev, pad + 2); // +2 matches decompiled offset

        if (!source_pad || !sink_pad)
            continue;

        /* Verify pad types are compatible - logic from 0xe35c */
        if (!(source_pad->flags & sink_pad->flags & pad->link_type)) {
            dev_err(dev->dev, "The link type is mismatch!\n");
            return -EINVAL;
        }

        /* Check if either pad is already active - check at 0xe390 */
        if (source_pad->link_state == LINK_STATE_ENABLED ||
            sink_pad->link_state == LINK_STATE_ENABLED) {
            dev_err(dev->dev, "Please stop active links firstly!\n");
            return -EINVAL;
        }

        /* Handle existing source links */
        if (source_pad->link_state == LINK_STATE_SOURCE &&
            sink_pad != source_pad->sink) {
            /* Destroy existing link */
            subdev_video_destroy_link((struct isp_pad_desc *)source_pad->source);
        }

        /* Handle existing sink links */
        if (sink_pad->link_state == LINK_STATE_SOURCE &&
            source_pad != sink_pad->source) {
            /* Destroy existing link */
            subdev_video_destroy_link((struct isp_pad_desc *)sink_pad->source);
        }

        /* Setup new link - matches logic from 0xe408 to 0xe430 */
        source_pad->source = source_pad;
        source_pad->sink = sink_pad;
        source_pad->entity = &source_pad->source;
        source_pad->link_type = pad->link_type | 1; // Set enabled bit
        source_pad->link_state = LINK_STATE_SOURCE;

        sink_pad->source = sink_pad;
        sink_pad->sink = source_pad;
        sink_pad->entity = &source_pad->source;
        sink_pad->link_type = pad->link_type | 1; // Set enabled bit
        sink_pad->link_state = LINK_STATE_SOURCE;
    }

    /* Update current link setting - matches 0xe468 */
    dev->current_link = link;

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

/* WDR Mode definitions seen in decompiled code */
#define WDR_MODE_NONE   0
#define WDR_MODE_LINE   1    /* Line-interleaved WDR */
#define WDR_MODE_FRAME  2    /* Frame-based WDR */

/* WDR-related IOCTLs seen in decompiled */
#define TX_ISP_WDR_SET_BUF    _IOW('V', 0x56d6, struct isp_wdr_buf_info)
#define TX_ISP_WDR_GET_BUF    _IOR('V', 0x56d7, struct isp_wdr_buf_size)
#define TX_ISP_WDR_OPEN       _IOW('V', 0x56d8, int)
#define TX_ISP_WDR_CLOSE      _IOW('V', 0x56d9, int)


// Custom IOCTL commands matching the decompiled code
#define TX_ISP_SET_BUF           _IOW('V', 0x56d5, struct isp_buf_info)
#define TX_ISP_VIDEO_LINK_SETUP  _IOW('V', 0x56d0, unsigned int)
#define TX_ISP_SET_AE_ALGO_OPEN  _IOW('V', 0x56dd, struct isp_ae_algo)
#define TX_ISP_SET_AWB_ALGO_OPEN _IOW('V', 0x56e2, struct isp_awb_algo)


// Register definitions from the decompiled code
#define ISP_REG_BASE      0x7800
#define ISP_REG_BUF0     (ISP_REG_BASE + 0x20)  // 0x7820
#define ISP_REG_BUF0_SIZE (ISP_REG_BASE + 0x24)  // 0x7824
#define ISP_REG_BUF1     (ISP_REG_BASE + 0x28)  // 0x7828
#define ISP_REG_BUF1_SIZE (ISP_REG_BASE + 0x2c)  // 0x782c
#define ISP_REG_BUF2     (ISP_REG_BASE + 0x30)  // 0x7830
#define ISP_REG_BUF2_SIZE (ISP_REG_BASE + 0x34)  // 0x7834
#define ISP_REG_BUF3     (ISP_REG_BASE + 0x40)  // 0x7840
#define ISP_REG_BUF3_SIZE (ISP_REG_BASE + 0x44)  // 0x7844
#define ISP_REG_BUF4     (ISP_REG_BASE + 0x48)  // 0x7848
#define ISP_REG_BUF4_SIZE (ISP_REG_BASE + 0x4c)  // 0x784c
#define ISP_REG_BUF5     (ISP_REG_BASE + 0x50)  // 0x7850
#define ISP_REG_BUF5_SIZE (ISP_REG_BASE + 0x54)  // 0x7854
#define ISP_REG_BUF6     (ISP_REG_BASE + 0x58)  // 0x7858
#define ISP_REG_BUF6_SIZE (ISP_REG_BASE + 0x5c)  // 0x785c
#define ISP_REG_BUF7     (ISP_REG_BASE + 0x60)  // 0x7860
#define ISP_REG_BUF7_SIZE (ISP_REG_BASE + 0x64)  // 0x7864
#define ISP_REG_BUF8     (ISP_REG_BASE + 0x68)  // 0x7868
#define ISP_REG_BUF8_SIZE (ISP_REG_BASE + 0x6c)  // 0x786c
#define ISP_REG_CTRL     (ISP_REG_BASE + 0x38)  // 0x7838
#define ISP_REG_START    (ISP_REG_BASE + 0x3c)  // 0x783c

// WDR Register definitions from decompiled code
#define WDR_REG_BASE     0x2000
#define WDR_REG_BUF      (WDR_REG_BASE + 0x04)  // 0x2004
#define WDR_REG_LINE     (WDR_REG_BASE + 0x08)  // 0x2008
#define WDR_REG_HEIGHT   (WDR_REG_BASE + 0x0c)  // 0x200c

// WDR modes from decompiled code
#define WDR_MODE_NONE    0
#define WDR_MODE_LINE    1
#define WDR_MODE_FRAME   2

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

static inline void system_reg_write(u32 reg, u32 val)
{
    void __iomem *addr = ioremap(reg, 4);

    if (!addr)
        return;

    writel(val, addr);
    iounmap(addr);
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


/**
 * setup_isp_buffers - Configure ISP buffer memory regions
 * @isp: ISP device structure
 * @buf_info: Buffer information from userspace
 *
 * Based on decompiled code at case 0x800856d4
 */
int setup_isp_buffers(struct isp_device *isp, struct isp_buf_info *buf_info)
{
    uint32_t width, height;
    uint32_t base_addr = buf_info->paddr;  // Changed from addr to paddr
    uint32_t total_size = buf_info->size;
    uint32_t offset = 0;
    struct isp_reg_t *regs = isp->regs;

    // Get width and height using helper functions
    width = isp_get_width(regs);
    height = isp_get_height(regs);

    // Calculate main buffer size (matches decompiled logic)
    uint32_t line_size = ((width + 7) >> 3) << 3;
    uint32_t main_size = line_size * height;

    // Verify buffer size is sufficient
    if (total_size < main_size) {
        pr_err("%s,%d: buf size too small\n", __func__, __LINE__);
        return -EINVAL;
    }

    // Setup main buffer (BUF0)
    system_reg_write(ISP_REG_BUF0, base_addr);
    system_reg_write(ISP_REG_BUF0_SIZE, line_size);
    offset += main_size;

    // Setup second buffer (BUF1)
    // Size is main_size >> 1 in decompiled code
    uint32_t second_size = main_size >> 1;
    if (total_size < (offset + second_size)) {
        pr_err("%s,%d: buf size too small\n", __func__, __LINE__);
        return -EINVAL;
    }
    system_reg_write(ISP_REG_BUF1, base_addr + offset);
    system_reg_write(ISP_REG_BUF1_SIZE, line_size);
    offset += second_size;

    // Setup third buffer (BUF2)
    // Complex size calculation from decompiled code
    uint32_t third_line_size = ((((width + 0x1f) >> 5) + 7) >> 3) << 3;
    uint32_t third_height = (((height + 0xf) >> 4) + 1);
    uint32_t third_size = third_line_size * third_height;

    if (total_size < (offset + third_size)) {
        pr_err("%s,%d: buf size too small\n", __func__, __LINE__);
        return -EINVAL;
    }
    system_reg_write(ISP_REG_BUF2, base_addr + offset);
    system_reg_write(ISP_REG_BUF2_SIZE, third_line_size);
    offset += third_size;

    // Memory optimization path splitting based on isp_memopt
    if (isp_memopt) {
        // Simple path - just set addresses with zero sizes
        system_reg_write(ISP_REG_BUF3, base_addr + offset);
        system_reg_write(ISP_REG_BUF3_SIZE, 0);
        system_reg_write(ISP_REG_BUF4, base_addr + offset);
        system_reg_write(ISP_REG_BUF4_SIZE, 0);
        system_reg_write(ISP_REG_BUF5, base_addr + offset);
        system_reg_write(ISP_REG_BUF5_SIZE, 0);
    } else {
        // Complex path with additional buffers
        uint32_t stat_size = third_size << 2;

        // Setup statistics buffers
        system_reg_write(ISP_REG_BUF3, base_addr + offset);
        system_reg_write(ISP_REG_BUF3_SIZE, third_line_size);
        offset += third_size;

        system_reg_write(ISP_REG_BUF4, base_addr + offset);
        system_reg_write(ISP_REG_BUF4_SIZE, third_line_size);
        offset += third_size;

        system_reg_write(ISP_REG_BUF5, base_addr + offset);
        system_reg_write(ISP_REG_BUF5_SIZE, third_line_size);
        offset += third_size;

        // Setup additional scaled buffers
        uint32_t scaled_line_size = ((((width >> 1) + 7) >> 3) << 3);
        uint32_t scaled_size = scaled_line_size * height;

        if (total_size < (offset + scaled_size)) {
            pr_err("%s,%d: buf size too small\n", __func__, __LINE__);
            return -EINVAL;
        }

        system_reg_write(ISP_REG_BUF6, base_addr + offset);
        system_reg_write(ISP_REG_BUF6_SIZE, scaled_line_size);
        offset += scaled_size;

        uint32_t half_scaled_size = scaled_size >> 1;
        if (total_size < (offset + half_scaled_size)) {
            pr_err("%s,%d: buf size too small\n", __func__, __LINE__);
            return -EINVAL;
        }

        system_reg_write(ISP_REG_BUF7, base_addr + offset);
        system_reg_write(ISP_REG_BUF7_SIZE, scaled_line_size);
        offset += half_scaled_size;

        // Final small buffer
        uint32_t tiny_line_size = ((((width >> 5) + 7) >> 3) << 3);
        uint32_t tiny_size = (tiny_line_size * height) >> 5;

        if (total_size < (offset + tiny_size)) {
            pr_err("%s,%d: buf size too small\n", __func__, __LINE__);
            return -EINVAL;
        }

        system_reg_write(ISP_REG_BUF8, base_addr + offset);
        system_reg_write(ISP_REG_BUF8_SIZE, tiny_line_size);
    }

    // Final control registers (matching decompiled code)
    system_reg_write(ISP_REG_CTRL, 0);
    system_reg_write(ISP_REG_START, 1);

    return 0;
}

//static long tx_isp_unlocked_ioctl(void *priv, unsigned int cmd, void __user *arg)
//{
//    struct isp_device *isp = priv;
//    int ret = 0;
//
//    switch (cmd) {
//        case TX_ISP_SET_BUF: {
//            struct isp_buf_info buf;
//            if (copy_from_user(&buf, arg, sizeof(buf))) {
//                pr_err("[%s][%d] copy from user error\n", __func__, __LINE__);
//                return -EFAULT;
//            }
//
//            // Validate buffer size and setup memory regions
//            // This matches the extensive buffer setup code seen in the decompilation
//            ret = setup_isp_buffers(isp, &buf);
//            break;
//        }
//
//        case TX_ISP_WDR_SET_BUF: {
//            struct isp_wdr_buf_info wdr_buf;
//            if (copy_from_user(&wdr_buf, arg, sizeof(wdr_buf))) {
//                pr_err("[%s][%d] copy from user error\n", __func__, __LINE__);
//                return -EFAULT;
//            }
//
//            // Setup WDR buffer handling matching decompiled logic
//            ret = setup_wdr_buffers(isp, &wdr_buf);
//            break;
//        }
//
//        case TX_ISP_VIDEO_LINK_SETUP: {
//            unsigned int link;
//            if (copy_from_user(&link, arg, sizeof(link))) {
//                pr_err("[%s][%d] copy from user error\n", __func__, __LINE__);
//                return -EFAULT;
//            }
//
//            if (link >= 2) {
//                pr_err("link(%d) is invalid!\n", link);
//                return -EINVAL;
//            }
//
//            // Handle video link setup matching decompiled logic
//            ret = setup_video_link(isp, link);
//            break;
//        }
//
//        case TX_ISP_SET_AE_ALGO_OPEN: {
//            struct isp_ae_algo ae;
//            if (copy_from_user(&ae, arg, sizeof(ae))) {
//                pr_err("[%s][%d] copy from user error\n", __func__, __LINE__);
//                return -EFAULT;
//            }
//
//            if (ae.magic != 0x336ac) {
//                pr_err("AE Algo Function registration failed - invalid magic\n");
//                return -EINVAL;
//            }
//
//            // Initialize AE algorithm matching decompiled logic
//            ret = init_ae_algo(isp, &ae);
//            break;
//        }
//
//        case TX_ISP_SET_AWB_ALGO_OPEN: {
//            struct isp_awb_algo awb;
//            if (copy_from_user(&awb, arg, sizeof(awb))) {
//                pr_err("[%s][%d] copy from user error\n", __func__, __LINE__);
//                return -EFAULT;
//            }
//
//            // Initialize AWB algorithm matching decompiled logic
//            ret = init_awb_algo(isp, &awb);
//            break;
//        }
//
//        // Add other IOCTL commands seen in decompilation
//        // Including 0x800856d7, 0x800456d8, etc.
//
//        default:
//            return -ENOTTY;
//    }
//
//    return ret;
//}

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
    int ret = 0;

    // Basic validation
    if (!gISPdev || !gISPdev->is_open) {
        pr_err("ISP device not initialized or not open\n");
        return -EINVAL;
    }

    switch (cmd) {
	case VIDIOC_REGISTER_SENSOR: {
	    char sensor_name[80];  // Match size with IMPISPDev sensor_name

	    if (!gISPdev || !gISPdev->is_open) {
	        dev_err(gISPdev->dev, "ISP device not initialized or not open\n");
	        return -EINVAL;
	    }

	    if (copy_from_user(sensor_name, (void __user *)arg, sizeof(sensor_name))) {
	        dev_err(gISPdev->dev, "[%s][%d] copy from user error\n",
	                __func__, __LINE__);
	        return -EFAULT;
	    }

	    // Let's add some debug prints
	    pr_info("Registering sensor: %s, gISPdev=%p\n", sensor_name, gISPdev);

	    // Store in global device structure
	    strncpy(gISPdev->sensor_name, sensor_name, sizeof(gISPdev->sensor_name) - 1);
	    gISPdev->sensor_name[sizeof(gISPdev->sensor_name) - 1] = '\0';

	    // Print confirmation
	    dev_info(gISPdev->dev, "Successfully registered sensor: %s\n",
	             gISPdev->sensor_name);

	    break;
	}
    case VIDIOC_GET_SENSOR_LIST: {
        struct sensor_list {
            char name[32];
            int count;
        } sensors;

        // Always perform null checks first
        if (!argp) {
            dev_err(gISPdev->dev, "Null userspace pointer\n");
            return -EINVAL;
        }

        // Clear our local structure
        memset(&sensors, 0, sizeof(sensors));

        // Copy from user and verify zeros
        if (copy_from_user(&sensors, argp, sizeof(sensors))) {
            dev_err(gISPdev->dev, "Failed to copy from user\n");
            return -EFAULT;
        }

        // At this point we could populate sensors.name and sensors.count
        // if we had a valid sensor, but OEM returns zeros so we will too

        // Copy our zero-filled structure back to user
        if (copy_to_user(argp, &sensors, sizeof(sensors))) {
            dev_err(gISPdev->dev, "Failed to copy to user\n");
            return -EFAULT;
        }

        return 0;
    }
    case TX_ISP_SET_BUF: {
        struct isp_buf_info buf;
        if (copy_from_user(&buf, argp, sizeof(buf))) {
            dev_err(gISPdev->dev, "[%s][%d] copy from user error\n",
                    __func__, __LINE__);
            return -EFAULT;
        }
        ret = setup_isp_buffers(gISPdev, &buf);
        break;
    }

    case TX_ISP_VIDEO_LINK_SETUP: {
        unsigned int link;
        if (copy_from_user(&link, argp, sizeof(link))) {
            dev_err(gISPdev->dev, "[%s][%d] copy from user error\n",
                    __func__, __LINE__);
            return -EFAULT;
        }
        ret = setup_video_link(gISPdev, link);
        break;
    }

    case TX_ISP_SET_AE_ALGO_OPEN: {
        struct isp_ae_algo ae;
        if (copy_from_user(&ae, argp, sizeof(ae))) {
            dev_err(gISPdev->dev, "[%s][%d] copy from user error\n",
                    __func__, __LINE__);
            return -EFAULT;
        }
        ret = init_ae_algo(gISPdev, &ae);
        if (!ret && copy_to_user(argp, &ae, sizeof(ae))) {
            dev_err(gISPdev->dev, "[%s][%d] copy to user error\n",
                    __func__, __LINE__);
            return -EFAULT;
        }
        break;
    }

    case TX_ISP_SET_AWB_ALGO_OPEN: {
        struct isp_awb_algo awb;
        if (copy_from_user(&awb, argp, sizeof(awb))) {
            dev_err(gISPdev->dev, "[%s][%d] copy from user error\n",
                    __func__, __LINE__);
            return -EFAULT;
        }
        ret = init_awb_algo(gISPdev, &awb);
        if (!ret && copy_to_user(argp, &awb, sizeof(awb))) {
            dev_err(gISPdev->dev, "[%s][%d] copy to user error\n",
                    __func__, __LINE__);
            return -EFAULT;
        }
        break;
    }

    case TX_ISP_WDR_OPEN: {
        // Handle WDR enable - matches case 0x800456d8
        unsigned int wdr_en = 1;
        ret = isp_set_wdr_mode(gISPdev, wdr_en);
        break;
    }

    case TX_ISP_WDR_CLOSE: {
        // Handle WDR disable - matches case 0x800456d9
        unsigned int wdr_en = 0;
        ret = isp_set_wdr_mode(gISPdev, wdr_en);
        break;
    }

    case TX_ISP_WDR_GET_BUF: {
        // Calculate and return WDR buffer size - matches case 0x800856d7
        struct isp_wdr_buf_size size;
        ret = calculate_wdr_buffer_size(gISPdev, &size);
        if (!ret && copy_to_user(argp, &size, sizeof(size))) {
            dev_err(gISPdev->dev, "[%s][%d] copy to user error\n",
                    __func__, __LINE__);
            return -EFAULT;
        }
        break;
    }

    case TX_ISP_SET_AE_ALGO_CLOSE: {
        // TODO
        // Cleanup AE algorithm - matches case 0x800456de
        // tisp_ae_algo_deinit();
        // tisp_ae_algo_init(0, NULL);
        kfree(ae_info_mine);
        kfree(ae_statis_mine);
        break;
    }

    case TX_ISP_SET_AWB_ALGO_CLOSE: {
        // TODO
        // Cleanup AWB algorithm - matches case 0x800456e4
        //tisp_awb_algo_deinit();
        //tisp_awb_algo_init(0);
        kfree(awb_info_mine);
        break;
    }

    default:
        dev_dbg(gISPdev->dev, "Unhandled ioctl cmd: 0x%x\n", cmd);
        return -ENOTTY;
    }

    return ret;
}

// Update file operations structure
static const struct file_operations isp_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = isp_driver_ioctl,
    .open = tisp_open,
    .release = tisp_release,
    .read = tisp_read,
    .write = tisp_write,
};

// Implementation of IMP_ISP_Open for kernel space
//int IMP_ISP_Open(void) {
//    int result = 0;
//    int err;
//
//    // Check if gISPdev is properly allocated and initialized
//    if (gISPdev == NULL) {
//        pr_err("gISPdev is NULL, cannot proceed with IMP_ISP_Open\n");
//        return -ENOMEM;  // Return an error indicating memory allocation failure
//    }
//    // Example of checking if the device is already open
//    if (gISPdev->cdev.owner != NULL) {
//        pr_warn("ISP device is already opened\n");
//        return -EBUSY;
//    }
//
//    pr_info("IMP_ISP_Open called\n");
//
//    // Check if device is already initialized
//    if (gISPdev == NULL) {
//        gISPdev = kzalloc(sizeof(IMPISPDev), GFP_KERNEL);
//        if (gISPdev == NULL) {
//            imp_log_fun(IMP_LOG_ERROR, IMP_Log_Get_Option(), 2,
//                        __FILE__, __func__, __LINE__,
//                        "Failed to alloc gISPdev!\n");
//            return -ENOMEM;
//        }
//
//        // Allocate a major/minor number for the device
//        err = alloc_chrdev_region(&tisp_dev_number, 0, 1, "tx-isp");
//        if (err < 0) {
//            imp_log_fun(IMP_LOG_ERROR, IMP_Log_Get_Option(), 2,
//                        __FILE__, __func__, __LINE__,
//                        "Failed to allocate device number\n");
//            kfree(gISPdev);
//            gISPdev = NULL;
//            return err;
//        }
//
//        // Create device class
//        tisp_class = class_create(THIS_MODULE, "tx-isp");
//        if (IS_ERR(tisp_class)) {
//            imp_log_fun(IMP_LOG_ERROR, IMP_Log_Get_Option(), 2,
//                        __FILE__, __func__, __LINE__,
//                        "Failed to create device class\n");
//            unregister_chrdev_region(tisp_dev_number, 1);
//            kfree(gISPdev);
//            gISPdev = NULL;
//            return PTR_ERR(tisp_class);
//        }
//
//        // Initialize the cdev structure
//        cdev_init(&gISPdev->cdev, &isp_fops);
//        gISPdev->cdev.owner = THIS_MODULE;
//
//        // Add the character device to the system
//        err = cdev_add(&gISPdev->cdev, tisp_dev_number, 1);
//        if (err) {
//            imp_log_fun(IMP_LOG_ERROR, IMP_Log_Get_Option(), 2,
//                        __FILE__, __func__, __LINE__,
//                        "Failed to add character device\n");
//            class_destroy(tisp_class);
//            unregister_chrdev_region(tisp_dev_number, 1);
//            kfree(gISPdev);
//            gISPdev = NULL;
//            return err;
//        }
//
//        // Create the device node in /dev
//        tisp_device = device_create(tisp_class, NULL, tisp_dev_number, NULL, "tx-isp");
//        if (IS_ERR(tisp_device)) {
//            imp_log_fun(IMP_LOG_ERROR, IMP_Log_Get_Option(), 2,
//                        __FILE__, __func__, __LINE__,
//                        "Failed to create device\n");
//            cdev_del(&gISPdev->cdev);
//            class_destroy(tisp_class);
//            unregister_chrdev_region(tisp_dev_number, 1);
//            kfree(gISPdev);
//            gISPdev = NULL;
//            return PTR_ERR(tisp_device);
//        }
//
//        strcpy(gISPdev->dev_name, "tx-isp");
//        gISPdev->is_open = 1;
//        imp_log_fun(IMP_LOG_INFO, IMP_Log_Get_Option(), 2,
//                    __FILE__, __func__, __LINE__,
//                    "~~~~~~ %s[%d] ~~~~~~~\n", "IMP_ISP_Open", __LINE__);
//
//        // Detect and log CPU ID
//        if (read_soc_info() < 0) {
//            imp_log_fun(IMP_LOG_ERROR, IMP_Log_Get_Option(), 2,
//                        __FILE__, __func__, __LINE__,
//                        "Failed to detect CPU ID\n");
//        }
//    }
//
//    return result;
//}

static int32_t IMP_ISP_AddSensor(char* sensor_name)
{
    int ret;
    char sensor_buf[80] = {0};

    if (gISPdev == NULL) {
        imp_log_fun(6, IMP_Log_Get_Option(), 2, __FILE__, __func__, __LINE__,
                    "ISP device not initialized");
        return -1;
    }

    // Register the sensor using the driver's ioctl handler
    ret = isp_driver_ioctl(NULL, VIDIOC_REGISTER_SENSOR, (unsigned long)sensor_name);
    if (ret) {
        imp_log_fun(6, IMP_Log_Get_Option(), 2, __FILE__, __func__, __LINE__,
                    "VIDIOC_REGISTER_SENSOR(%s) error", sensor_name);
        return -1;
    }

    // Allocate and set buffer info
    gISPdev->buf_info = kmalloc(0x94, GFP_KERNEL);
    if (!gISPdev->buf_info) {
        imp_log_fun(6, IMP_Log_Get_Option(), 2, __FILE__, __func__, __LINE__,
                    "Memory allocation failed");
        return -ENOMEM;
    }

    ret = isp_driver_ioctl(NULL, VIDIOC_SET_BUF_INFO, (unsigned long)gISPdev->buf_info);
    if (ret) {
        imp_log_fun(6, IMP_Log_Get_Option(), 2, __FILE__, __func__, __LINE__,
                    "VIDIOC_SET_BUF_INFO error");
        kfree(gISPdev->buf_info);
        return -1;
    }

    imp_log_fun(4, IMP_Log_Get_Option(), 2, __FILE__, __func__, __LINE__,
                "Sensor [%s] successfully added", sensor_name);
    return 0;  // Remove incorrect error path
}

/* Helper function to allocate and map DMA memory */
static void __iomem *allocate_and_map_buffer(struct device *dev, size_t size,
                                            dma_addr_t *dma_handle, const char *name)
{
    void *vaddr;

    vaddr = dma_alloc_coherent(dev, size, dma_handle, GFP_KERNEL);
    if (!vaddr) {
        pr_err("Failed to allocate DMA memory for %s\n", name);
        return NULL;
    }

    /* Clear the allocated memory */
    memset(vaddr, 0, size);
    pr_info("%s buffer allocated: virt=%p, phys=%pad\n",
            name, vaddr, dma_handle);

    return (__force void __iomem *)vaddr;  // Cast for consistent return type
}

/* Configure DEIR control registers */
static void configure_deir_control(void)
{
    /* Add your DEIR control register configurations here */
    /* Use readl/writel for register access */
}

int tiziano_load_parameters(const char *filename, void __iomem *dest, size_t offset)
{
    struct file *file;
    mm_segment_t old_fs;
    loff_t pos = 0;
    ssize_t bytes_read;
    size_t file_size;
    char *buffer = NULL;
    int ret = 0;

    pr_info("Loading parameters from file: %s\n", filename);

    // Step 1: Open the file
    old_fs = get_fs();
    set_fs(KERNEL_DS);
    file = filp_open(filename, O_RDONLY, 0);
    set_fs(old_fs);

    if (IS_ERR(file)) {
        pr_err("Failed to open parameter file: %s\n", filename);
        return -ENOENT;
    }

    // Step 2: Get the file size
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

// Update the tisp_init function to properly handle register access
static int tisp_init(struct device *dev)
{
    int ret;
    const char *param_file = "/etc/sensor/sc2336-t31.bin";
    uint32_t reg_val;

    pr_info("Starting tisp_init...\n");

    // Verify registers are mapped using global gISPdev
    if (!gISPdev || !gISPdev->regs) {
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

    // Step 4: Write the parameters to hardware registers using global gISPdev->regs
    writel((unsigned long)tparams_day, gISPdev->regs + 0x84b50);
    dev_info(dev, "tparams_day written to register successfully\n");

    return 0;

cleanup:
    if (tparams_night)
        vfree(tparams_night);
    if (tparams_day)
        vfree(tparams_day);
    return ret;
}

// Function to register a misc device and create optional /proc entries
int private_misc_register(struct miscdevice *misc_dev, const char *proc_name, const struct file_operations *fops)
{
    int ret;
    struct proc_dir_entry *proc_entry;

    ret = misc_register(misc_dev);
    if (ret < 0) {
        pr_err("Failed to register misc device: %s\n", misc_dev->name);
        return ret;
    }

    if (proc_name && fops) {
        proc_entry = proc_create_data(proc_name, 0644, NULL, fops, misc_dev);
        if (!proc_entry) {
            pr_err("Failed to create /proc entry for %s\n", proc_name);
            misc_deregister(misc_dev);
            return -ENOMEM;
        }
    }

    pr_info("Successfully registered misc device: %s\n", misc_dev->name);
    return 0;  // Remove incorrect error path
}


void private_misc_deregister(struct miscdevice *misc_dev)
{
    if (misc_dev->minor >= 0) {
	// TODO
	//remove_proc_entry((long)misc_dev->minor, NULL);  // Cast minor to long
    }
    misc_deregister(misc_dev);
}
EXPORT_SYMBOL(private_misc_deregister);


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

static int tisp_probe(struct platform_device *pdev)
{
    struct isp_graph_data *graph_data;
    void __iomem *base;
    unsigned int max_devices = 10;
    unsigned int registered_count = 0;
    int ret, i;

    pr_info("Probing TISP device...\n");
    pr_info("In tisp_probe, gISPdev is %p\n", gISPdev);


    // Map registers
    base = map_isp_registers(pdev);
    if (IS_ERR(base)) {
        dev_err(&pdev->dev, "Failed to map ISP registers\n");
        return PTR_ERR(base);
    }

    // Use the global gISPdev instead of creating a local one
    if (!gISPdev) {
        dev_err(&pdev->dev, "Global ISP device structure not allocated\n");
        return -ENOMEM;
    }

    // Set reg_base in global gISPdev
    gISPdev->regs = base;
    pr_info("Set gISPdev->regs to %p\n", gISPdev->regs);
    gISPdev->dev = &pdev->dev;
    platform_set_drvdata(pdev, gISPdev);

    // Allocate graph data with proper alignment
    graph_data = kzalloc(sizeof(*graph_data) + (max_devices * sizeof(void *)), GFP_KERNEL | GFP_DMA);
    if (!graph_data) {
        dev_err(&pdev->dev, "Failed to allocate graph data\n");
        return -ENOMEM;
    }

    // Initialize graph data
    graph_data->dev = gISPdev;  // Use global gISPdev
    graph_data->device_count = 0;
    graph_data->devices = kzalloc(max_devices * sizeof(struct platform_device *), GFP_KERNEL | GFP_DMA);
    if (!graph_data->devices) {
        ret = -ENOMEM;
        goto free_graph;
    }

    // Initialize ISP
    ret = tisp_init(&pdev->dev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to initialize ISP subsystem\n");
        goto free_devices;
    }

        // Register child devices
    for (i = 0; i < ARRAY_SIZE(device_names) && registered_count < max_devices; i++) {
        struct platform_device *child_dev;
        child_dev = platform_device_register_simple(device_names[i], -1, NULL, 0);
        if (IS_ERR(child_dev)) {
            dev_warn(&pdev->dev, "Failed to register %s device\n", device_names[i]);
            continue;
        }
        graph_data->devices[registered_count] = child_dev;
        registered_count++;
    }
    // Update final count
    graph_data->device_count = registered_count;
    dev_info(&pdev->dev, "Successfully registered %u devices\n", registered_count);
    // Create graph and nodes
    if (registered_count > 0) {
        ret = tx_isp_create_graph_and_nodes(graph_data);
        if (ret) {
            dev_err(&pdev->dev, "Failed to create ISP graph and nodes\n");
            goto err_unregister_devices;
        }
    } else {
        dev_warn(&pdev->dev, "No devices were registered\n");
        ret = -ENODEV;
        goto free_devices;
    }
    // After successful graph creation, create proc entries
    ret = create_isp_proc_entries(graph_data);
    if (ret) {
        dev_err(&pdev->dev, "Failed to create proc entries\n");
        goto err_unregister_devices;
    }
    dev_info(&pdev->dev, "TISP device probed successfully\n");


    // Return success
    return 0;

err_unregister_devices:
    for (i = 0; i < registered_count; i++) {
        if (graph_data->devices[i])
            platform_device_unregister(graph_data->devices[i]);
    }
    remove_isp_proc_entries();  // Clean up proc entries if created
free_graph:
    kfree(graph_data->devices);
free_devices:
    kfree(graph_data);
    return ret;
}


/* Platform driver remove function */
static int tisp_remove(struct platform_device *pdev)
{
    struct tisp_device *dev = platform_get_drvdata(pdev);

    remove_isp_proc_entries();
    tx_isp_module_deinit(dev);

    dev_info(&pdev->dev, "TISP device removed\n");
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

    // Create the tx-isp device first
    ret = alloc_chrdev_region(&tisp_dev_number, 0, 1, "tx-isp");
    if (ret < 0) {
        pr_err("Failed to allocate device number\n");
        return ret;
    }

    tisp_class = class_create(THIS_MODULE, "tx-isp");
    if (IS_ERR(tisp_class)) {
        unregister_chrdev_region(tisp_dev_number, 1);
        return PTR_ERR(tisp_class);
    }

    gISPdev = kzalloc(sizeof(struct IMPISPDev), GFP_KERNEL);
    if (!gISPdev) {
        class_destroy(tisp_class);
        unregister_chrdev_region(tisp_dev_number, 1);
        return -ENOMEM;
    }
    pr_info("gISPdev allocated at %p\n", gISPdev);  // Add this debug print

    cdev_init(&gISPdev->cdev, &isp_fops);
    gISPdev->cdev.owner = THIS_MODULE;

    ret = cdev_add(&gISPdev->cdev, tisp_dev_number, 1);
    if (ret) {
        kfree(gISPdev);
        class_destroy(tisp_class);
        unregister_chrdev_region(tisp_dev_number, 1);
        return ret;
    }

    tisp_device = device_create(tisp_class, NULL, tisp_dev_number, NULL, "tx-isp");
    if (IS_ERR(tisp_device)) {
        cdev_del(&gISPdev->cdev);
        kfree(gISPdev);
        class_destroy(tisp_class);
        unregister_chrdev_region(tisp_dev_number, 1);
        return PTR_ERR(tisp_device);
    }

    /* Register the platform driver */
    pr_info("Registering platform driver\n");
    ret = platform_driver_register(&tisp_platform_driver);
    if (ret) {
        pr_err("Failed to register platform driver: %d\n", ret);
        goto err_destroy_device;
    }
    pr_info("Platform driver registered successfully\n");

    /* Create and register the platform device */
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
    cdev_del(&gISPdev->cdev);
    kfree(gISPdev);
    class_destroy(tisp_class);
    unregister_chrdev_region(tisp_dev_number, 1);
    return ret;
}

/* Module cleanup function */
static void __exit isp_driver_exit(void)
{
    struct platform_device *pdev = NULL;
    struct device *dev = NULL;

    remove_isp_proc_entries();  // Clean up proc entries

    /* Find our platform device */
    dev = bus_find_device_by_name(&platform_bus_type, NULL, "tisp-driver");
    if (dev) {
        pdev = to_platform_device(dev);
        platform_device_unregister(pdev);
        put_device(dev);
    }

    platform_driver_unregister(&tisp_platform_driver);

    if (gISPdev) {
        if (tisp_class && tisp_dev_number) {
            device_destroy(tisp_class, tisp_dev_number);
            cdev_del(&gISPdev->cdev);
        }
        kfree(gISPdev);
        gISPdev = NULL;
    }

    if (tisp_class) {
        class_destroy(tisp_class);
        tisp_class = NULL;
    }

    if (tisp_dev_number) {
        unregister_chrdev_region(tisp_dev_number, 1);
    }

    pr_info("ISP driver unloaded\n");
}

module_init(isp_driver_init);
module_exit(isp_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Custom ISP Driver with SoC Detection");
