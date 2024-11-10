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

#include "isp_driver_common.h"

#define ISP_BASE_ADDR        0x13300000
#define ISP_MAP_SIZE         0x1000
#define ISP_OFFSET_PARAMS    0x1000
#define REG_CONTROL          0x100

// CPU info register addresses
#define CPU_ID_ADDR          0x1300002c
#define CPPSR_ADDR           0x10000034
#define SUBTYPE_ADDR         0x13540238
#define SUBREMARK_ADDR       0x13540231

#define VIDIOC_REGISTER_SENSOR 0x805056c1
#define VIDIOC_SET_BUF_INFO    0x800856d4
#define VIDIOC_SET_WDR_BUF_INFO 0x800856d6
#define VIDIOC_GET_SENSOR_LIST 0xc050561a

// Log level definitions
#define IMP_LOG_ERROR   6
#define IMP_LOG_INFO    3
#define IMP_LOG_DEBUG   4

// Device structure for ISP
typedef struct {
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
    char padding2[0x28];   // Padding to match 0xE0 size
    struct device *dev;    // Device pointer
} IMPISPDev;

static void __iomem *reg_base;
static uint32_t soc_id = 0xFFFFFFFF;
static uint32_t cppsr = 0xFFFFFFFF;
static uint32_t subsoctype = 0xFFFFFFFF;
static uint32_t subremark = 0xFFFFFFFF;
static IMPISPDev* gISPdev = NULL;
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

// Struct representing the configuration of the ISP device
struct IspDeviceConfig
{
    __padding char _0[4];
    void* field_04;
    int32_t field_08;
    __padding char _c[4];
    __padding char _10[4];
    __padding char _14[0x1c];
    int32_t misc_deregister_flag;
    int32_t field_34;
    __padding char _38[8];
    __padding char _40[0x38];
    int32_t field_78;
    void* field_7c;
    int32_t irq_number;              // Ensure this field exists
    __padding char _84[8];
    __padding char _8c[0x28];
    void* memory_region;             // Ensure this field exists
    int32_t register_mapped_address;
    __padding char _bc[4];
    int32_t clock_settings;
    int32_t parameter_value;         // Ensure this field exists
    int16_t field_c8;
    int16_t field_ca;
    int32_t input_pads;
    int32_t allocated_input_pads;
};

void isp_printf(int level, struct seq_file *seq, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    seq_vprintf(seq, fmt, args);
    va_end(args);
}
EXPORT_SYMBOL(isp_printf);

// Private wrapper functions (example)
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

void *private_i2c_get_clientdata(struct i2c_client *client)
{
    return i2c_get_clientdata(client);
}

int private_capable(int capability)
{
    return capable(capability);
}

int private_i2c_set_clientdata(struct i2c_client *client, void *data)
{
    i2c_set_clientdata(client, data);
    return 0;
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

void private_clk_enable(struct clk *clk)
{
    clk_enable(clk);
}

void private_clk_put(struct clk *clk)
{
    clk_put(clk);
}

int private_clk_set_rate(struct clk *clk, unsigned long rate)
{
    return clk_set_rate(clk, rate);
}

struct DriverInterface {
    void* field_00;
};

int32_t private_driver_get_interface(struct DriverInterface* arg1) {
    // Return error if input is NULL
    if (arg1 == NULL)
        return 0xffffffff;

    // Get the common driver interface (as described in the disassembly)
    void* v0 = get_driver_common_interfaces();

    // Set the field_00 of arg1 to the value returned by get_driver_common_interfaces
    arg1->field_00 = v0;

    // Initialize result to 0
    int32_t result = 0;

    // Check if v0 is not NULL
    if (v0 != NULL) {
        // Dereference v0 to read the flags and compare them
        uint32_t a2_1 = *(uint32_t*)v0;
        uint32_t v1_1 = *(uint32_t*)((char*)v0 + 0x1e0);  // v0 + 0x1e0 (offset)

        // If the first flag is equal to 0xca654, set result to 0
        if (a2_1 == 0xca654) {
            result = 0;
        }

        // If the flags do not match, print the error and return 0xffffffff
        if (a2_1 != 0xca654 || v1_1 != a2_1) {
            isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x\n", "private_driver_get_interface");
            return 0xffffffff;
        }
    }

    // Return result, which will be 0 if everything is fine
    return result;
}

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
EXPORT_SYMBOL(private_driver_get_interface);
EXPORT_SYMBOL(private_clk_put);
EXPORT_SYMBOL(private_clk_set_rate);

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

int32_t isp_irq_handle(int32_t irq, void* device_context)
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
		if (request_threaded_irq(irqNumber, isp_irq_handle, isp_irq_thread_handle, irq_flags, "isp_driver", irqHandlerData) != 0)
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
void tx_isp_module_deinit(struct tisp_device *tisp_dev)
{
    if (!tisp_dev) return;

    if (tisp_dev->reg_base) {
        iounmap(tisp_dev->reg_base);
        tisp_dev->reg_base = NULL;
    }
    pr_info("ISP module deinitialized successfully\n");
}

int32_t tx_isp_subdev_init(int32_t* arg1, void* arg2, int32_t arg3) {
    struct platform_device *pdev;
    struct resource *mem_region;
    void __iomem *mapped_regs;
    int32_t ret;

    if ((arg1 == NULL) || (arg2 == NULL)) {
        isp_printf(2, "%s the parameters are invalid!\n", "tx_isp_subdev_init");
        return -EINVAL;
    }

    // Set the specific value at offset 0xc4
    *(uint32_t*)((char*)arg2 + 0xc4) = arg3;

    // Initialize the ISP module
    ret = tx_isp_module_init(arg1, arg2);
    if (ret != 0) {
        isp_printf(2, "Failed to init isp module!\n", "tx_isp_subdev_init");
        return -EFAULT;
    }

    // Get platform device
    pdev = to_platform_device(((struct device *)arg2));
    if (!pdev) {
        isp_printf(2, "Invalid platform device\n", "tx_isp_subdev_init");
        return -ENODEV;
    }

    // Handle memory regions if needed
    if (arg1[0x16]) {
        // Request IRQ
        ret = tx_isp_request_irq(arg1, ((char*)arg2 + 0x80));
        if (ret != 0) {
            isp_printf(2, "Failed to request irq!\n", "tx_isp_subdev_init");
            tx_isp_module_deinit(arg2);
            return ret;
        }

        // Get memory resource
        mem_region = platform_get_resource(pdev, IORESOURCE_MEM, 0);
        if (!mem_region) {
            isp_printf(2, "Unable to get memory resource\n", "tx_isp_subdev_init");
            ret = -ENODEV;
            goto err_free_irq;
        }

        // Map registers
        mapped_regs = ioremap(mem_region->start,
                            resource_size(mem_region));
        if (!mapped_regs) {
            isp_printf(2, "Unable to map registers\n", "tx_isp_subdev_init");
            ret = -ENOMEM;
            goto err_free_irq;
        }

        // Store mapped register base
        *(void __iomem **)((char*)arg2 + 0xb8) = mapped_regs;

        // Setup clocks
        char *s1_1 = (char*)arg1[0x16];
        *(uint32_t*)((char*)arg2 + 0xc0) = *(uint32_t*)(s1_1 + 4);

        struct ClockConfig *clock_config = (struct ClockConfig *)(s1_1 + 8);
        ret = isp_subdev_init_clks(arg2, clock_config);
        if (ret != 0) {
            isp_printf(2, "Failed to init clocks!\n", "tx_isp_subdev_init");
            goto err_unmap;
        }
    }

    return 0;

err_unmap:
    if (*(void __iomem **)((char*)arg2 + 0xb8)) {
        iounmap(*(void __iomem **)((char*)arg2 + 0xb8));
    }
    if (mem_region) {
        release_mem_region(mem_region->start, resource_size(mem_region));
    }
err_free_irq:
    tx_isp_free_irq(((char*)arg2 + 0x80));
    tx_isp_module_deinit(arg2);
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
            // Call private function to release each clock
            clk_put(*(uint32_t*)allocated_clocks_1);

            // Move to the next clock in the list
            i += 1;
            allocated_clocks_1 += 4;  // Assuming each clock entry is 4 bytes
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

int32_t tx_isp_subdev_deinit(struct IspDeviceConfig* arg1)
{
//    // Check if the misc_deregister_flag is non-zero
//    if (arg1->misc_deregister_flag != 0)
//    {
//        // Deregister the misc device
//        misc_deregister((int32_t*)((char*)arg1 + 0xc));  // Accessing misc device pointer
//    }
//
//    // Release clocks associated with the subdevice
      isp_subdev_release_clks(arg1);
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
//    void* memory_region = arg1->memory_region;
//    int32_t irq_number;
//
//    if (memory_region == NULL)
//    {
//        irq_number = arg1->irq_number;
//    }
//    else
//    {
//        // Release the memory region if it's allocated
//        int32_t start_address = *(uint32_t*)memory_region;
//        release_mem_region(start_address, *((uint32_t*)((char*)memory_region + 4)) + 1 - start_address);
//        arg1->memory_region = NULL;  // Set memory region to NULL
//        irq_number = arg1->irq_number;
//    }
//
//    // Free the IRQ if it's valid
    if (irq_number != 0)
    {
        tx_isp_free_irq(&arg1->irq_number);
    }

    // Call the module deinit function and store the result
    tx_isp_module_deinit(arg1);

    // Reset the parameter value
    arg1->parameter_value = 0;

    // Return the result from module deinit
    return 0;
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

/* Platform driver ID table */
static const struct of_device_id tisp_of_match[] = {
    { .compatible = "vendor,tisp-driver" },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, tisp_of_match);

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

// Implement basic file operations
static int tisp_open(struct inode *inode, struct file *file)
{
    pr_info("Device opened\n");
    return 0;  // Remove incorrect error path
}

// Update tisp_release function
static int tisp_release(struct inode *inode, struct file *file)
{
    pr_info("Device closed\n");
    return 0;  // Remove incorrect error path
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


static long isp_driver_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case VIDIOC_REGISTER_SENSOR:
            pr_info("Registering sensor: %s\n", (char *)arg);
            break;
        case VIDIOC_SET_BUF_INFO:
            pr_info("Setting buffer info\n");
            break;
        case VIDIOC_SET_WDR_BUF_INFO:
            pr_info("Setting WDR buffer info\n");
            break;
        case VIDIOC_GET_SENSOR_LIST:
            pr_info("Getting sensor list\n");
            break;
        default:
            return -ENOTTY;
    }

    return ret;  // Remove incorrect error path
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
int IMP_ISP_Open(void) {
    int result = 0;
    int err;

    pr_info("IMP_ISP_Open called\n");

    // Check if device is already initialized
    if (gISPdev == NULL) {
        gISPdev = kzalloc(sizeof(IMPISPDev), GFP_KERNEL);
        if (gISPdev == NULL) {
            imp_log_fun(IMP_LOG_ERROR, IMP_Log_Get_Option(), 2,
                        __FILE__, __func__, __LINE__,
                        "Failed to alloc gISPdev!\n");
            return -ENOMEM;
        }

        // Allocate a major/minor number for the device
        err = alloc_chrdev_region(&tisp_dev_number, 0, 1, "tx-isp");
        if (err < 0) {
            imp_log_fun(IMP_LOG_ERROR, IMP_Log_Get_Option(), 2,
                        __FILE__, __func__, __LINE__,
                        "Failed to allocate device number\n");
            kfree(gISPdev);
            gISPdev = NULL;
            return err;
        }

        // Create device class
        tisp_class = class_create(THIS_MODULE, "tx-isp");
        if (IS_ERR(tisp_class)) {
            imp_log_fun(IMP_LOG_ERROR, IMP_Log_Get_Option(), 2,
                        __FILE__, __func__, __LINE__,
                        "Failed to create device class\n");
            unregister_chrdev_region(tisp_dev_number, 1);
            kfree(gISPdev);
            gISPdev = NULL;
            return PTR_ERR(tisp_class);
        }

        // Initialize the cdev structure
        cdev_init(&gISPdev->cdev, &isp_fops);
        gISPdev->cdev.owner = THIS_MODULE;

        // Add the character device to the system
        err = cdev_add(&gISPdev->cdev, tisp_dev_number, 1);
        if (err) {
            imp_log_fun(IMP_LOG_ERROR, IMP_Log_Get_Option(), 2,
                        __FILE__, __func__, __LINE__,
                        "Failed to add character device\n");
            class_destroy(tisp_class);
            unregister_chrdev_region(tisp_dev_number, 1);
            kfree(gISPdev);
            gISPdev = NULL;
            return err;
        }

        // Create the device node in /dev
        tisp_device = device_create(tisp_class, NULL, tisp_dev_number, NULL, "tx-isp");
        if (IS_ERR(tisp_device)) {
            imp_log_fun(IMP_LOG_ERROR, IMP_Log_Get_Option(), 2,
                        __FILE__, __func__, __LINE__,
                        "Failed to create device\n");
            cdev_del(&gISPdev->cdev);
            class_destroy(tisp_class);
            unregister_chrdev_region(tisp_dev_number, 1);
            kfree(gISPdev);
            gISPdev = NULL;
            return PTR_ERR(tisp_device);
        }

        strcpy(gISPdev->dev_name, "tx-isp");
        gISPdev->is_open = 1;
        imp_log_fun(IMP_LOG_INFO, IMP_Log_Get_Option(), 2,
                    __FILE__, __func__, __LINE__,
                    "~~~~~~ %s[%d] ~~~~~~~\n", "IMP_ISP_Open", __LINE__);

        // Detect and log CPU ID
        if (read_soc_info() < 0) {
            imp_log_fun(IMP_LOG_ERROR, IMP_Log_Get_Option(), 2,
                        __FILE__, __func__, __LINE__,
                        "Failed to detect CPU ID\n");
        }
    }

    return result;
}

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

    // Verify reg_base is valid
    if (!reg_base) {
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

    // Step 4: Write the parameters to hardware registers
    writel((unsigned long)tparams_day, reg_base + 0x84b50);
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


// Function to deregister a misc device and remove associated /proc entry
void private_misc_deregister(struct miscdevice *misc_dev, const char *proc_name)
{
    if (proc_name) {
        remove_proc_entry(proc_name, NULL);
    }
    misc_deregister(misc_dev);
}


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

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res) {
        dev_err(&pdev->dev, "No memory resource found\n");
        return ERR_PTR(-ENODEV);
    }

    base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(base)) {
        dev_err(&pdev->dev, "Failed to map ISP registers\n");
        return base;
    }

    return base;
}

static int tisp_probe(struct platform_device *pdev)
{
    struct tisp_device *dev;
    struct isp_graph_data *graph_data;
    void __iomem *base;
    unsigned int max_devices = 10;
    unsigned int registered_count = 0;
    int ret, i;

    pr_info("Probing TISP device...\n");

    // Map registers
    base = map_isp_registers(pdev);
    if (IS_ERR(base)) {
        dev_err(&pdev->dev, "Failed to map ISP registers\n");
        return PTR_ERR(base);
    }

    // Allocate device structure
    dev = devm_kzalloc(&pdev->dev, sizeof(*dev), GFP_KERNEL);
    if (!dev) {
        dev_err(&pdev->dev, "Failed to allocate device structure\n");
        return -ENOMEM;
    }

    dev->reg_base = base;
    reg_base = base;

    // Allocate graph data with proper alignment
    graph_data = kzalloc(sizeof(*graph_data) + (max_devices * sizeof(void *)), GFP_KERNEL | GFP_DMA);
    if (!graph_data) {
        dev_err(&pdev->dev, "Failed to allocate graph data\n");
        return -ENOMEM;
    }

    // Initialize graph data
    graph_data->dev = dev;
    graph_data->device_count = 0;
    graph_data->devices = kzalloc(max_devices * sizeof(struct platform_device *), GFP_KERNEL | GFP_DMA);
    if (!graph_data->devices) {
        ret = -ENOMEM;
        goto free_graph;
    }

    // Store device info
    dev->dev = &pdev->dev;
    platform_set_drvdata(pdev, dev);
    tisp_dev = dev;

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

    IMP_ISP_Open();  // Call IMP_ISP_Open after successful probe
    IMP_ISP_AddSensor("sc2336");  // Add sensor after successful probe

    return 0;

err_unregister_devices:
    for (i = 0; i < registered_count; i++) {
        if (graph_data->devices[i])
            platform_device_unregister(graph_data->devices[i]);
    }
    tx_isp_module_deinit(dev);
    remove_isp_proc_entries();  // Clean up proc entries if created
free_devices:
    kfree(graph_data->devices);
free_graph:
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

/* Platform driver structure */
static struct platform_driver tisp_platform_driver = {
    .probe = tisp_probe,
    .remove = tisp_remove,
    .driver = {
        .name = "tisp-driver",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(tisp_of_match),
    }
}; // Added missing semicolon here

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

    gISPdev = kzalloc(sizeof(IMPISPDev), GFP_KERNEL);
    if (!gISPdev) {
        class_destroy(tisp_class);
        unregister_chrdev_region(tisp_dev_number, 1);
        return -ENOMEM;
    }

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
    ret = platform_driver_register(&tisp_platform_driver);
    if (ret) {
        pr_err("Failed to register platform driver\n");
        goto err_destroy_device;
    }

    /* Create and register the platform device */
    pdev = platform_device_register_simple("tisp-driver", -1, res, ARRAY_SIZE(res));
    if (IS_ERR(pdev)) {
        ret = PTR_ERR(pdev);
        pr_err("Failed to register platform device: %d\n", ret);
        platform_driver_unregister(&tisp_platform_driver);
        goto err_destroy_device;
    }

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
