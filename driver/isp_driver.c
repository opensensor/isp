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
#include <linux/semaphore.h>
#include <linux/err.h>         // For IS_ERR and PTR_ERR
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/mutex.h>
#include <linux/videodev2.h>


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

#define ISP_BUFFER_ALIGN    4096
#define ISP_MIN_BUFFER_SIZE (1 * 1024 * 1024)  // 1MB
#define ISP_MAX_BUFFER_SIZE (16 * 1024 * 1024) // 16MB
#define ISP_ALLOC_KMALLOC   1
#define ISP_RMEM_BASE       0x02a80000
#define ISP_RMEM_SIZE       22544384
#define ISP_CTRL_ENABLE    BIT(0)    // Enable bit in control register
#define ISP_CTRL_RESET     BIT(1)    // Reset bit
#define ISP_CTRL_CAPTURE   BIT(2)    // Capture enable

struct isp_memory_info {
    void *virt_addr;         // Kernel virtual address
    dma_addr_t phys_addr;    // Physical/DMA address
    size_t size;             // Size of allocation
    bool initialized;
};

static struct isp_memory_info isp_mem = {0};

#define ISP_BASE_ADDR        0x13300000
#define ISP_MAP_SIZE         0x1000
#define ISP_OFFSET_PARAMS    0x1000
#define REG_CONTROL          0x100
// Update defines
#define ISP_STREAM_CTRL      0x7838
#define ISP_STREAM_START     0x783c
#define ISP_BUF_BASE        0x1000

// CPU info register addresses
#define CPU_ID_ADDR          0x1300002c
#define CPPSR_ADDR           0x10000034
#define SUBTYPE_ADDR         0x13540238
#define SUBREMARK_ADDR       0x13540231

#define VIDIOC_GET_SENSOR_ENUMERATION 0xc050561a
#define VIDIOC_GET_SENSOR_INFO 0x40045626
#define VIDIOC_ENABLE_STREAM 0x800456d2


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

/* Standard V4L2 IOCTL definitions are in the 0x56xx range */
/* Custom ISP commands */
#define VIDIOC_BASE         0x56AA  // Choose a base that doesn't conflict
#define VIDIOC_SET_ROTATION _IOWR('V', VIDIOC_BASE + 0x20, struct isp_rotation_params)

/* Structure definitions for IOCTL parameters */
struct isp_rotation_params {
    int angle;          // Rotation angle (0, 90, 180, 270)
    int rot_height;     // Output height after rotation
    int rot_width;      // Output width after rotation
    unsigned int flags; // Reserved for future use
};

/* Additional ISP-specific commands */
#define VIDIOC_SET_FLIP     _IOW('V', VIDIOC_BASE + 0x21, int)
#define VIDIOC_SET_MIRROR   _IOW('V', VIDIOC_BASE + 0x22, int)
#define VIDIOC_SET_CROP     _IOWR('V', VIDIOC_BASE + 0x23, struct v4l2_crop)
#define VIDIOC_SET_FORMAT   _IOWR('V', VIDIOC_BASE + 0x24, struct v4l2_format)

/* Define error codes */
#define ISP_ERROR_BASE          1000
#define ISP_ERROR_INVALID_PARAM (ISP_ERROR_BASE + 1)
#define ISP_ERROR_BUSY         (ISP_ERROR_BASE + 2)
#define ISP_ERROR_TIMEOUT      (ISP_ERROR_BASE + 3)
#define ISP_ERROR_NO_MEMORY    (ISP_ERROR_BASE + 4)
#define ISP_ERROR_NOT_READY    (ISP_ERROR_BASE + 5)

// Hardware register definitions
#define ISP_BASE            0x13300000

// Buffer control registers
#define ISP_BUF0_REG       (ISP_BASE + 0x1000)
#define ISP_BUF0_SIZE_REG  (ISP_BASE + 0x1004)
#define ISP_BUF1_REG       (ISP_BASE + 0x1008)
#define ISP_BUF1_SIZE_REG  (ISP_BASE + 0x100C)
#define ISP_BUF2_REG       (ISP_BASE + 0x1010)
#define ISP_BUF2_SIZE_REG  (ISP_BASE + 0x1014)

// Control registers
#define ISP_CTRL_REG       (ISP_BASE + 0x1100)
#define ISP_START_REG      (ISP_BASE + 0x1104)
#define SENSOR_NAME_SIZE 80
#define PADDING_SIZE_1 0x24
#define PADDING_SIZE_2 0x24


// Add these defines
#define SENSOR_REG_STANDBY    0x0100
#define SENSOR_REG_MODE       0x0101
#define SENSOR_REG_H_SIZE     0x0102
#define SENSOR_REG_V_SIZE     0x0103


// Add these register definitions
#define ISP_REG_BASE       0x13300000
#define ISP_REG_SIZE       0x10000

#define ISP_CTRL_OFFSET    0x1104
#define ISP_STAT_OFFSET    0x0100
#define ISP_CONF_OFFSET    0x0130
#define ISP_INIT_OFFSET    0x0118


#define MAX_CHANNELS 3  // Define the maximum number of frame channels
static struct class *framechan_class;
static dev_t framechan_dev;
static struct cdev framechan_cdev;

// Updated structure based on your previous definition and observations
struct sensor_list_info {
    char name[80];          // Sensor name
    int id;                 // Sensor ID
    int status;             // Sensor status (e.g., 1 for active)
    int resolution_width;   // Sensor resolution width
    int resolution_height;  // Sensor resolution height
    int framerate;          // Max frame rate
    int capabilities;       // Bitmask for sensor capabilities
    void *sensor_ops;       // Pointer to sensor operations (expected by binary)
    void *reserved_ptr;     // Reserved space (pointer)
};


// Add at the top of the file with other definitions
#define RMEM_START  0x2A80000          // Starting address of the reserved memory region
#define DRIVER_RESERVED_SIZE (4 * 1024 * 1024) // Size reserved for our driver: 4MB


// Global variables
void *reserved_buffer_virt;
dma_addr_t reserved_buffer_phys = RMEM_START;
size_t reserved_buffer_size = DRIVER_RESERVED_SIZE;

struct sensor_buffer_info {
    char unused[0x80];         // Padding to align with offsets
    uint32_t method;           // Offset: 0x80
    uint32_t buffer_start;     // Offset: 0x84 (Physical Address)
    uint32_t virt_addr;        // Offset: 0x88 (Virtual Address)
    uint32_t buffer_size;      // Offset: 0x8C (Size)
    uint32_t flags;            // Offset: 0x90 (Flags)
    uint32_t frame_count;      // Offset: 0x94 (Frame Count)
    uint8_t is_buffer_full;    // Offset: 0x98 (Buffer Full Indicator)
    char padding[3];           // Align to 32-bit boundary
} __attribute__((packed, aligned(4)));

struct sensor_control_info {
    uint32_t buffer_start;     // 0x00: Buffer start address
    uint32_t buffer_size;      // 0x04: Buffer size
    char padding[0x60];        // Padding for alignment
    char name[32];             // Named buffer identifier
    uint32_t method;           // Memory allocation method
    uint32_t phys_addr;        // Physical address
    uint32_t virt_addr;        // Virtual address
    uint32_t flags;            // Control flags
} __attribute__((packed, aligned(4)));


#define MAX_FRAME_SOURCES 3

struct isp_framesource_state {
    uint32_t magic;          // Magic identifier
    uint32_t flags;          // State flags
    uint32_t chn_num;        // Channel number
    uint32_t state;          // State (1=ready, 2=streaming)
    uint32_t width;          // Frame width
    uint32_t height;         // Frame height
    uint32_t fmt;            // Pixel format

    // Memory management
    uint32_t buf_cnt;       // Buffer count
    uint32_t buf_flags;     // Buffer flags
    void    *buf_base;      // Buffer base address
    dma_addr_t dma_addr;    // DMA address
    uint32_t buf_size;      // Buffer size per frame

    // Frame management
    uint32_t frame_cnt;     // Frame counter
    uint32_t buf_index;     // Current buffer index
    struct file *fd;        // File pointer
    struct video_device *vdev;
    struct task_struct *thread;
    void    *ext_buffer;    // Extended buffer pointer
    struct isp_buffer_info *bufs;  // Buffer info array
    spinlock_t lock;               // Add spinlock for IRQ handler
    void    *private;             // Private data pointer

    // Synchronization
    struct semaphore sem;
    wait_queue_head_t wait;
    int is_open;           // 1 = initialized
};


struct IMPISPDev {
    // Base device info - verified offsets
    char dev_name[32];                    // 0x00: Device name
    struct cdev cdev;                     // Char device structure
    int major;                            // Major number
    int minor;                            // Minor number
    int fd;                              // 0x20: File descriptor
    int is_open;                         // 0x24: Open status
    char sensor_name[SENSOR_NAME_SIZE];   // 0x28: 80 byte sensor name buffer
    char padding1[PADDING_SIZE_1];        // 0x78: Padding to align to 0xAC

    // Critical offsets - must match prudynt
    struct sensor_buffer_info *buf_info;  // 0xAC: Buffer info
    int wdr_mode;                        // 0xB0: WDR mode
    struct sensor_control_info *wdr_buf_info; // 0xB4: WDR buffer info
    unsigned int current_link;            // Track link config
    char padding2[PADDING_SIZE_2];        // Additional required padding

    // Device and hardware access - after aligned section
    struct device *dev;
    void __iomem *regs;                  // Register base
    void __iomem *ctrl_regs;             // Control registers
    int irq;
    struct i2c_client *sensor_i2c_client;

    // Frame dimensions
    unsigned int width;
    unsigned int height;

    // DMA info
    dma_addr_t dma_addr;
    void *dma_buf;
    size_t dma_size;

    // Runtime state
    struct clk **clocks;
    int num_clocks;
    bool memory_initialized;
    struct isp_framesource_state *fs_info;  // Add this field
    uint32_t format;                        // Add this field



    // Parameter regions
    void __iomem *isp_params;
    void __iomem *wdr_params;
    struct clk *isp_clk;       // ISP core clock
    struct clk *cgu_isp_clk;   // CGU ISP clock
    struct clk *csi_clk;       // CSI clock
    struct clk *ipu_clk;       // IPU clock
    struct isp_framesource_state frame_sources[MAX_FRAME_SOURCES];

} __attribute__((packed, aligned(4)));

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



// Add validation macros
#define VALIDATE_FS_STATE(fs) do { \
    if (!fs || !fs->buf_base) { \
        pr_err("Invalid frame source state at %s:%d\n", __func__, __LINE__); \
        return -EINVAL; \
    } \
} while(0)

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

static struct ClockConfig isp_clocks[] = {
    { .clock_name = "isp", .clock_rate = 125000000 },   // Set ISP clock to 125 MHz
};
static const int num_clocks = ARRAY_SIZE(isp_clocks);


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
    char sensor_name[80];  // Can hold the sensor name
    int num_sensors;       // Number of currently registered sensors
};

#define MAX_SENSORS 1

#define SENSOR_PROC_PATH "/proc/jz/sensor/"

struct sensor_info {
    char name[32];
    u32 chip_id;
    u32 width;
    u32 height;
    u32 i2c_addr;
    u32 max_fps;
    u32 min_fps;
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


/* Update pad descriptor structure to match libimp exactly */
struct isp_pad_desc {
    uint32_t flags;            // 0x00: Pad flags - binary checks this at +6
    uint8_t type;             // 0x04: Pad type (1=source, 2=sink)
    uint8_t index;            // 0x05: Pad index
    uint8_t reserved[2];      // 0x06-0x07: Alignment padding
    uint32_t link_state;      // 0x08: Link state
    void *source;             // 0x0C: Source pad pointer
    void *sink;               // 0x10: Sink pad pointer
    void *entity;             // 0x14: Entity pointer
} __attribute__((packed, aligned(4)));

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
    void __iomem *regs;
    struct tx_isp_subdev_ops *ops;
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



#define VBM_POOL_SIZE 0x99888  // From OEM driver
#define VBM_MAX_POOLS 8
#define ISP_INT_FRAME_DONE  BIT(0)    // Frame completion bit
#define ISP_INT_STATUS_REG  0x24      // Status register offset
#define ISP_INT_CLEAR_REG   0x28      // Clear register offset
#define ISP_INT_MASK_REG    0x2C      // Interrupt mask register

// Buffer control registers
#define ISP_BUF0_REG       (ISP_BASE + 0x1000)
#define ISP_BUF0_SIZE_REG  (ISP_BASE + 0x1004)
#define ISP_BUF1_REG       (ISP_BASE + 0x1008)
#define ISP_BUF1_SIZE_REG  (ISP_BASE + 0x100C)
#define ISP_BUF2_REG       (ISP_BASE + 0x1010)
#define ISP_BUF2_SIZE_REG  (ISP_BASE + 0x1014)


/* Pad flags matching decompiled values */
#define PAD_FL_SINK      (1 << 0)
#define PAD_FL_SOURCE    (1 << 1)
#define PAD_FL_ACTIVE    (1 << 2)

/* Pad link states */
#define LINK_STATE_INACTIVE  0
#define LINK_STATE_SOURCE    3
#define LINK_STATE_ENABLED   4

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

// Custom function to check if a character is whitespace
static int is_whitespace(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v');
}

// Function to trim leading and trailing whitespace
static void trim_whitespace(char *str) {
    char *end;

    // Trim leading whitespace
    while (is_whitespace(*str)) str++;

    // If the string is empty after trimming
    if (*str == '\0')
        return;

    // Trim trailing whitespace
    end = str + strlen(str) - 1;
    while (end > str && is_whitespace(*end)) end--;

    // Null-terminate the string
    *(end + 1) = '\0';
}
// Helper function to read a string from /proc
static int read_proc_string(const char *proc_entry, char *buffer, size_t buffer_size) {
    struct file *file;
    mm_segment_t old_fs;
    loff_t offset = 0;
    int ret = -EINVAL;

    old_fs = get_fs();
    set_fs(KERNEL_DS);
    file = filp_open(proc_entry, O_RDONLY, 0);
    if (IS_ERR(file)) {
        pr_err("Failed to open %s\n", proc_entry);
        set_fs(old_fs);
        return PTR_ERR(file);
    }

    // Adjusted for Linux 3.10 kernel API
    ret = kernel_read(file, offset, buffer, buffer_size - 1);
    if (ret > 0) {
        buffer[ret] = '\0'; // Null-terminate the string
        ret = 0;
    }

    filp_close(file, NULL);
    set_fs(old_fs);
    return ret;
}

// Helper function to read an unsigned integer from /proc
static int read_proc_u32(const char *proc_entry, u32 *value) {
    char buffer[16];
    int ret;

    ret = read_proc_string(proc_entry, buffer, sizeof(buffer));
    if (ret < 0)
        return ret;

    ret = kstrtou32(buffer, 0, value);
    if (ret < 0)
        pr_err("Failed to parse u32 from %s\n", buffer);

    return ret;
}

static int get_sensor_info(struct sensor_info *info) {
    if (read_proc_string(SENSOR_PROC_PATH "name", info->name, sizeof(info->name)) < 0) {
        pr_err("Failed to read sensor name\n");
        return -EINVAL;
    }
    if (read_proc_u32(SENSOR_PROC_PATH "chip_id", &info->chip_id) < 0) {
        pr_err("Failed to read sensor chip_id\n");
        return -EINVAL;
    }
    if (read_proc_u32(SENSOR_PROC_PATH "width", &info->width) < 0) {
        pr_err("Failed to read sensor width\n");
        return -EINVAL;
    }
    if (read_proc_u32(SENSOR_PROC_PATH "height", &info->height) < 0) {
        pr_err("Failed to read sensor height\n");
        return -EINVAL;
    }
    if (read_proc_u32(SENSOR_PROC_PATH "i2c_addr", &info->i2c_addr) < 0) {
        pr_err("Failed to read sensor i2c_addr\n");
        return -EINVAL;
    }
    if (read_proc_u32(SENSOR_PROC_PATH "max_fps", &info->max_fps) < 0) {
        pr_err("Failed to read sensor max_fps\n");
        return -EINVAL;
    }
    if (read_proc_u32(SENSOR_PROC_PATH "min_fps", &info->min_fps) < 0) {
        pr_err("Failed to read sensor min_fps\n");
        return -EINVAL;
    }
    return 0;
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

#define ISP_INT_FRAME_DONE  BIT(0)    // Frame complete
#define ISP_INT_DMA_ERR    BIT(1)    // DMA error
#define ISP_INT_BUF_FULL   BIT(2)    // Buffer full

struct frame_source_channel {
    uint32_t state;              // 0x1c: State (1=ready, 2=streaming)
    void *buf_base;              // Base buffer address
    dma_addr_t dma_addr;         // DMA address
    uint32_t buf_size;           // Buffer size per frame
    uint32_t buf_cnt;            // Number of buffers (4)
    uint32_t write_idx;          // Current write index
    uint32_t frame_cnt;          // Frame counter
    struct semaphore sem;        // Buffer semaphore
    struct mutex lock;           // Access lock
    uint32_t channel_offset;     // Channel offset in memory
    // Add fields to match 0x2e8 total size seen in code
};

static irqreturn_t isp_irq_handler(int irq, void *dev_id) {
    struct IMPISPDev *dev = dev_id;
    struct isp_framesource_state *fs;
    struct frame_source_channel *fc;
    u32 status;

    if (!dev || !dev->regs) {
        pr_err("Invalid device in IRQ handler\n");
        return IRQ_NONE;
    }

    // Read and log status for debugging
    status = readl(dev->regs + ISP_INT_STATUS_REG);
    pr_debug("ISP IRQ status: 0x%08x\n", status);

    if (!(status & ISP_INT_FRAME_DONE)) {
        if (status & ISP_INT_DMA_ERR)
            pr_err("ISP DMA error\n");
        if (status & ISP_INT_BUF_FULL)
            pr_err("ISP buffer full\n");
        return IRQ_NONE;
    }

    // Clear only the handled interrupts
    writel(status & ISP_INT_FRAME_DONE, dev->regs + ISP_INT_CLEAR_REG);
    wmb();

    fs = &dev->frame_sources[0];
    if (!fs || !fs->is_open) {
        pr_err("No active frame source in IRQ\n");
        return IRQ_HANDLED;
    }

    fc = fs->private;
    if (!fc) {
        pr_err("No frame channel data in IRQ\n");
        return IRQ_HANDLED;
    }

    // Lock access to buffer state
    if (mutex_trylock(&fc->lock)) {
        // Update buffer indices
        fs->buf_index = (fs->buf_index + 1) % fs->buf_cnt;
        fc->write_idx = (fc->write_idx + 1) % fc->buf_cnt;
        fs->frame_cnt++;

        // Configure next buffer
        writel(fc->dma_addr + (fs->buf_index * fc->buf_size),
               dev->regs + ISP_BUF0_REG);
        writel(fc->buf_size, dev->regs + ISP_BUF0_SIZE_REG);
        wmb();

        mutex_unlock(&fc->lock);
    }

    // Signal new frame
    up(&fc->sem);
    wake_up(&fs->wait);

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
    char type[I2C_NAME_SIZE];          // Sensor type/name
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


static int tx_isp_mmap(struct file *filp, struct vm_area_struct *vma) {
    struct IMPISPDev *dev = filp->private_data;
    unsigned long size = vma->vm_end - vma->vm_start;

    pr_info("tx_isp: mmap request size=%lu\n", size);

    if (size > ISP_RMEM_SIZE) {
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

// Update defines to match correct frequencies
#define T31_ISP_FREQ     250000000   // 250MHz ISP core clock (matches AHB)
#define T31_CGU_ISP_FREQ 125000000   // 125MHz CGU_ISP clock (matches APB)

static struct isp_platform_data isp_pdata = {
    .clock_rate = T31_ISP_FREQ,
};


// Add CGU ISP bit positions
#define ISP_CLKGR_BIT    (1 << 23)   // ISP clock gate bit in CLKGR
#define CGU_ISP_BIT      (1 << 2)    // CGU_ISP clock gate bit in CLKGR1


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
    int i;

    if (!gISPdev) {
        seq_puts(m, "Error: ISP device not initialized\n");
        return 0;
    }

    for (i = 0; i < MAX_FRAMESOURCE_CHANNELS; i++) {
        seq_printf(m, "############## framesource %d ###############\n", i);
        seq_printf(m, "chan status: %s\n",
                  (gISPdev->is_open && gISPdev->sensor_name[0]) ? "running" : "stop");
    }
    return 0;
}

static int isp_m0_show(struct seq_file *m, void *v)
{
    seq_puts(m, "****************** ISP INFO **********************\n");

    if (!gISPdev || !gISPdev->is_open || !gISPdev->sensor_name[0]) {
        seq_puts(m, "sensor doesn't work, please enable sensor\n");
    } else {
        seq_printf(m, "sensor %s is working\n", gISPdev->sensor_name);
    }
    return 0;
}

static int isp_w00_show(struct seq_file *m, void *v)
{
    if (!gISPdev || !gISPdev->is_open || !gISPdev->sensor_name[0]) {
        seq_puts(m, "sensor doesn't work, please enable sensor\n");
    } else {
        seq_printf(m, "sensor %s is active\n", gISPdev->sensor_name);
    }
    return 0;
}

static int isp_w02_show(struct seq_file *m, void *v)
{
    if (!gISPdev || !gISPdev->is_open || !gISPdev->sensor_name[0]) {
        seq_puts(m, "sensor doesn't work, please enable sensor\n");
        return 0;
    }

    // If we have WDR mode info in gISPdev, we could use it here
    seq_printf(m, " %d, %d\n", gISPdev->wdr_mode ? 1 : 0, 0);  // Example values

    // Add any other relevant info from gISPdev structure
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


#define ISP_INIT_TIMEOUT_MS 200
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


#define RMEM_SIZE 22544384  // Match exact size LIBIMP expects
#define ISP_ALLOC_KMALLOC 1
#define ISP_ALLOC_CONTINUOUS 2
static struct IMPISPDev *gISPdev_bak = NULL;  // Backup for reopen

// Add these offsets from OEM driver
#define SHARED_BUF_OFFSET 0x130
#define CHAN_BUF_OFFSET  0x118

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
    pr_info("dev offset 0x20=0x%x\n",
            *((unsigned int*)((char*)dev + 0x20)));

    // Set up entry counts matching LIBIMP
    fc->buf_cnt = 4;
    fc->state = 1;  // Ready state
    fc->channel_offset = channel * 0x2e8;  // From decompiled offset calc
    fc->write_idx = 0;

    // Calculate buffer layout
    fc->buf_size = fs->width * fs->height * 2; // YUV422

    // Calculate buffer addresses with proper base offset
    uint32_t base_offset = 0x1094d4;  // Offset from decompiled code
    fc->buf_base = dev->dma_buf + base_offset + (channel * fc->buf_size * fc->buf_cnt);
    fc->dma_addr = dev->dma_addr + base_offset + (channel * fc->buf_size * fc->buf_cnt);

    sema_init(&fc->sem, 0);  // Start with no frames
    mutex_init(&fc->lock);

    // Initialize frame source state
    fs->frame_cnt = 0;
    fs->buf_index = 0;
    fs->state = 1;  // Ready state
    fs->private = fc;

    pr_info("Frame source channel %d initialized:\n", channel);
    pr_info("  buffer_base: %p\n", fc->buf_base);
    pr_info("  dma_addr: 0x%x\n", (uint32_t)fc->dma_addr);
    pr_info("  buf_size: %u x %d buffers\n", fc->buf_size, fc->buf_cnt);

    return 0;
}

// Add frame grabbing thread
struct isp_frame_state {
    uint32_t state;          // 0x318 offset - frame state
    uint32_t format;         // 0x68 offset - frame format
    void *buffer_addr;       // 0x31c offset - target buffer
    uint32_t width;
    uint32_t height;
    uint8_t is_buffer_full;
};

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
        if (fs->fmt == 0xa) {  // YUV422
            memcpy(dst, src, fc->buf_size);
        } else if (fs->fmt == 0xf) {
            // Special format handling if needed
            memcpy(dst, src, fc->buf_size * 2);
        }

        mutex_unlock(&fc->lock);
        up(&fc->sem);

        // Signal frame completion
        wake_up(&fs->wait);
    }

    return 0;
}


static int start_frame_source(struct IMPISPDev *dev, int channel) {
    struct isp_framesource_state *fs = &dev->frame_sources[channel];
    struct sensor_buffer_info *buf_info;
    u32 val;

    if (!fs || !fs->is_open || !dev->buf_info) {
        pr_err("Invalid frame source state\n");
        return -EINVAL;
    }

    // Get buffer info
    buf_info = fs->bufs;  // Array of buffer infos
    if (!buf_info) {
        pr_err("No buffer info\n");
        return -EINVAL;
    }

    // Important: Update the frame source state flags matching libimp
    fs->state = 2;  // Running state
    fs->flags |= 0x2;  // Set bit from decompiled code
    fs->frame_cnt = 0;
    fs->buf_index = 0;

    // Update base registers
    writel(fs->dma_addr, dev->regs + (ISP_BUF0_REG - ISP_BASE));
    writel(fs->buf_size, dev->regs + (ISP_BUF0_SIZE_REG - ISP_BASE));
    wmb();

    // Important: Initialize buffer metadata array
    int i;
    for (i = 0; i < fs->buf_cnt; i++) {
        buf_info[i].method = ISP_ALLOC_KMALLOC;
        buf_info[i].buffer_start = fs->dma_addr + (i * fs->buf_size);
        buf_info[i].buffer_size = fs->buf_size;
        buf_info[i].virt_addr = (unsigned long)(fs->buf_base + (i * fs->buf_size));
        buf_info[i].flags = 0;
        buf_info[i].frame_count = 0;
        buf_info[i].is_buffer_full = 0;
    }

    // Set streaming state
    val = readl(dev->regs + ISP_CTRL_REG);
    val |= ISP_CTRL_ENABLE;
    writel(val, dev->regs + ISP_CTRL_REG);
    wmb();

    pr_info("Started streaming on channel %d with:\n", channel);
    pr_info("  DMA addr: 0x%08x\n", (unsigned int)fs->dma_addr);
    pr_info("  Buffer size: %zu\n", fs->buf_size);
    pr_info("  State flags: 0x%x\n", fs->flags);

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

static int init_frame_source(struct IMPISPDev *dev, int channel)
{
    struct isp_framesource_state *fs = &dev->frame_sources[channel];

    // Clear structure
    memset(fs, 0, sizeof(*fs));

    // Initialize basic parameters
    fs->magic = 0x336ac;  // Magic from decompiled code
    fs->buf_cnt = 4;
    fs->fmt = 0xa;  // YUV422 default format
    fs->state = 1;  // Ready state
    fs->chn_num = channel;

    // Set default resolution based on channel
    if (channel == 0) {
        fs->width = 1920;
        fs->height = 1080;
    } else {
        fs->width = 640;
        fs->height = 360;
    }

    // Calculate buffer size right away
    fs->buf_size = fs->width * fs->height * 2; // YUV422

    // Initialize synchronization
    sema_init(&fs->sem, 1);
    init_waitqueue_head(&fs->wait);

    // Pre-allocate buffer info structures
    fs->bufs = kzalloc(sizeof(struct isp_buffer_info) * fs->buf_cnt, GFP_KERNEL);
    if (!fs->bufs)
        return -ENOMEM;

    // Mark as initialized
    fs->is_open = 1;

    pr_info("Initialized frame source %d with default %dx%d buf_size=%d\n",
            channel, fs->width, fs->height, fs->buf_size);

    return 0;
}


static int tisp_open(struct file *file)
{
    int ret;

    pr_info("ISP device open called from pid %d\n", current->pid);

    if (!gISPdev) {
        pr_err("ISP device not initialized\n");
        return -ENODEV;
    }

    pr_info("ISP device open: file=%p f_flags=0x%x\n",
        file, file->f_flags);

    // Store file->f_flags as the fd - this should match what libimp expects
    gISPdev->fd = file->f_flags;
    pr_info("Stored fd %d at offset 0x20\n", gISPdev->fd);
    gISPdev->is_open = 0;  // Reset counter

    // Initialize frame sources first
    ret = init_frame_source(gISPdev, 0);
    if (ret) {
        pr_err("Failed to initialize frame source: %d\n", ret);
        return ret;
    }

    // Critical: Set private_data before returning
    file->private_data = gISPdev->dev;
    pr_info("Set file->private_data to gISPdev=%p\n", gISPdev);

    gISPdev->is_open = 1;
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

// Add these structures to match sensor expectations
struct sensor_win_size {
    uint32_t width;          // 0x00: Frame width
    uint32_t height;         // 0x04: Frame height
    uint32_t fps;           // 0x08: Frame rate
    uint32_t max_fps;       // 0x0C: Maximum frame rate
    uint32_t format;        // 0x10: Image format
};

// Update memory setup to match sensor requirements
static int setup_isp_buffers(struct IMPISPDev *dev)
{
    struct sensor_win_size *wsize = dev->sensor_i2c_client->dev.platform_data;
    if (!wsize) {
        pr_err("No sensor window size information\n");
        return -EINVAL;
    }

    // Use sensor window size for buffer calculations
    uint32_t width = wsize->width;
    uint32_t height = wsize->height;
    uint32_t line_size = ((width + 7) >> 3) << 3;
    uint32_t frame_size = line_size * height;

    pr_info("Setting up buffers for %dx%d frame\n", width, height);

    // Configure main buffer
    writel(dev->dma_addr, dev->regs + ISP_BUF0_REG);
    writel(line_size, dev->regs + ISP_BUF0_SIZE_REG);

    // Configure second buffer at offset
    writel(dev->dma_addr + frame_size, dev->regs + ISP_BUF1_REG);
    writel(line_size, dev->regs + ISP_BUF1_SIZE_REG);

    // Additional buffer setup
    uint32_t third_line_size = ((((width + 0x1f) >> 5) + 7) >> 3) << 3;
    uint32_t third_addr = dev->dma_addr + (frame_size * 2);
    writel(third_addr, dev->regs + ISP_BUF2_REG);
    writel(third_line_size, dev->regs + ISP_BUF2_SIZE_REG);

    pr_info("Buffer setup complete:\n");
    pr_info("  Main buffer: 0x%08x size=%u\n", dev->dma_addr, frame_size);
    pr_info("  Second buffer: 0x%08x size=%u\n",
            dev->dma_addr + frame_size, frame_size);
    pr_info("  Third buffer: 0x%08x line_size=%u\n",
            third_addr, third_line_size);

    return 0;
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


/**
 * setup_video_link - Configure video link routing
 * @dev: IMPISPDev structure
 * @link: Link identifier from userspace
 *
 * Based on decompiled code at case 0x800456d0
 */
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

static int setup_video_link(struct IMPISPDev *dev, int link_num)
{
    void **subdev_list;
    struct isp_subdev *isp_sd;
    struct isp_pad_desc *src_pad, *sink_pad;
    uint32_t link_type;
    int ret;

    pr_info("Setting up video link %d\n", link_num);

    if (link_num >= 2) {
        pr_err("Invalid link number: %d\n", link_num);
        return -EINVAL;
    }

    // Get link type from configuration - match binary
    link_type = link_types[link_num].type;

    // Get subdev list
    subdev_list = *((void***)(((char*)dev) + 0x38));
    if (!subdev_list || !subdev_list[0]) {
        pr_err("No subdevs initialized\n");
        return -EINVAL;
    }

    isp_sd = subdev_list[0];
    if (!isp_sd) {
        pr_err("Invalid subdev\n");
        return -EINVAL;
    }

    // Get pads
    sink_pad = (struct isp_pad_desc *)(uintptr_t)isp_sd->sink_pads;
    src_pad = (struct isp_pad_desc *)(uintptr_t)isp_sd->src_pads;

    if (!sink_pad || !src_pad) {
        pr_err("Invalid pad pointers\n");
        return -EINVAL;
    }

    // Debug print pad flags
    pr_info("Pad flags: src=0x%x sink=0x%x link=0x%x\n",
            src_pad->flags, sink_pad->flags, link_type);

    // Match binary's exact validation
    if ((sink_pad->flags & src_pad->flags & link_type) == 0) {
        pr_err("The link type is mismatch!\n");
        return -EINVAL;
    }

    // Check current states
    if (sink_pad->link_state == LINK_STATE_ENABLED ||
        src_pad->link_state == LINK_STATE_ENABLED) {
        pr_err("Links already enabled\n");
        return -EINVAL;
    }

    // First destroy existing links
    sink_pad->link_state = LINK_STATE_INACTIVE;
    src_pad->link_state = LINK_STATE_INACTIVE;
    sink_pad->source = NULL;
    src_pad->sink = NULL;

    // Setup new links
    sink_pad->source = src_pad;
    src_pad->sink = sink_pad;
    sink_pad->link_state = LINK_STATE_SOURCE;  // Must be 3
    src_pad->link_state = LINK_STATE_SOURCE;   // Must be 3

    // Configure hardware path
    writel(link_num == 0 ? 1 : 0, dev->regs + 0x140);
    writel(link_num == 0 ? 0 : 1, dev->regs + 0x144);
    wmb();

    dev->current_link = link_num;
    return 0;
}

struct isp_subdev_link {
    struct isp_subdev *subdev;
    uint32_t flags;
    uint32_t state;
    void *source;
    void *sink;
};

static int setup_isp_subdevs(struct IMPISPDev *dev)
{
    struct isp_subdev *isp_sd;
    struct isp_pad_desc *sink_pad, *src_pad;
    void **subdev_list;
    int i;

    // Allocate subdev list at offset 0x38
    subdev_list = kzalloc(0x40, GFP_KERNEL);
    if (!subdev_list)
        return -ENOMEM;

    // Store subdev list pointer at offset 0x38
    *((void**)(((char*)dev) + 0x38)) = subdev_list;

    // Create main ISP subdev
    isp_sd = kzalloc(sizeof(*isp_sd), GFP_KERNEL);
    if (!isp_sd) {
        kfree(subdev_list);
        return -ENOMEM;
    }

    // Initialize subdev fields
    strlcpy(isp_sd->name, "isp-subdev", sizeof(isp_sd->name));
    isp_sd->name_ptr = isp_sd->name;
    isp_sd->regs = dev->regs;
    isp_sd->num_sink_pads = 1;
    isp_sd->num_src_pads = 1;

    // Allocate pads
    sink_pad = kzalloc(sizeof(*sink_pad), GFP_KERNEL);
    src_pad = kzalloc(sizeof(*src_pad), GFP_KERNEL);
    if (!sink_pad || !src_pad) {
        kfree(sink_pad);
        kfree(src_pad);
        kfree(isp_sd);
        kfree(subdev_list);
        return -ENOMEM;
    }

    // Critical: Set flags to match binary expectations
    sink_pad->type = 2;  // Sink
    sink_pad->index = 0; // First pad
    sink_pad->flags = 0x3;  // Both source/sink flags
    sink_pad->link_state = LINK_STATE_INACTIVE;
    sink_pad->entity = dev;

    // Initialize source pad
    src_pad->type = 1;   // Source
    src_pad->index = 0;  // First pad
    src_pad->flags = 0x3;  // Both source/sink flags
    src_pad->link_state = LINK_STATE_INACTIVE;
    src_pad->entity = dev;

    // Store pad addresses
    isp_sd->sink_pads = (uint32_t)(uintptr_t)sink_pad;
    isp_sd->src_pads = (uint32_t)(uintptr_t)src_pad;

    // Store subdev in list
    subdev_list[0] = isp_sd;

    return 0;
}

static int enable_isp_streaming(struct IMPISPDev *dev, struct file *file, int channel, bool enable)
{
    struct isp_framesource_state *fs;
    struct frame_source_channel *fc;
    void __iomem *reg_base;

    if (!dev || !dev->regs) {
        pr_err("Invalid device state\n");
        return -EINVAL;
    }

    reg_base = dev->regs;
    fs = &dev->frame_sources[channel];
    fc = fs->private;

    if (!fs || !fc) {
        pr_err("Invalid frame source state\n");
        return -EINVAL;
    }

    if (enable) {
        // Enable streaming
        writel(0x1, reg_base + 0x7838);
        wmb();
        writel(0x1, reg_base + 0x783c);
        wmb();

        fc->state = 2;  // Set streaming state
        dev->is_open += 2;  // Increment open count like OEM
    } else {
        // Disable streaming
        writel(0x0, reg_base + 0x7838);
        writel(0x0, reg_base + 0x783c);
        wmb();

        fc->state = 1;  // Back to ready state
    }

    return 0;
}

static int tisp_release(struct inode *inode, struct file *file)
{
    struct IMPISPDev *dev = file->private_data;
    int channel = 0;

    pr_info("\n=== ISP Release Debug ===\n");
    pr_info("file=%p flags=0x%x private_data=%p\n",
            file, file->f_flags, file->private_data);

    if (!dev) {
        pr_err("No device in release\n");
        return -EINVAL;
    }

    // Get channel if this is a frame channel device
    if (file->private_data) {
        struct isp_framesource_state *fs = file->private_data;
        channel = fs->chn_num;
        pr_info("Releasing channel %d\n", channel);
    }

    // Stop streaming with proper file handle
    if (dev->is_open) {
        enable_isp_streaming(dev, file, channel, false);
    }

    // Clean up frame source state
    cleanup_frame_source(dev, channel);

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



static int sensor_setup_streaming(struct IMPISPDev *dev)
{
    struct i2c_client *client = dev->sensor_i2c_client;
    int ret;

    if (!client) {
        pr_err("No sensor I2C client\n");
        return -ENODEV;
    }

    // Basic sensor initialization sequence
    ret = isp_sensor_write_reg(client, SENSOR_REG_STANDBY, 0x00);  // Exit standby
    if (ret < 0)
        return ret;

    // Set resolution (example for 1920x1080)
    ret = isp_sensor_write_reg(client, SENSOR_REG_H_SIZE, 0x07);  // 1920 >> 8
    if (ret < 0)
        return ret;

    ret = isp_sensor_write_reg(client, SENSOR_REG_H_SIZE + 1, 0x80);  // 1920 & 0xFF
    if (ret < 0)
        return ret;

    ret = isp_sensor_write_reg(client, SENSOR_REG_V_SIZE, 0x04);  // 1080 >> 8
    if (ret < 0)
        return ret;

    ret = isp_sensor_write_reg(client, SENSOR_REG_V_SIZE + 1, 0x38);  // 1080 & 0xFF
    if (ret < 0)
        return ret;

    // Enable streaming
    ret = isp_sensor_write_reg(client, SENSOR_REG_MODE, 0x01);
    if (ret < 0)
        return ret;

    pr_info("Sensor streaming setup complete\n");
    return 0;
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

// Add validation before streaming
static int init_isp_memory(struct IMPISPDev *dev) {
    void *virt_addr;
    dma_addr_t phys_addr = ISP_RMEM_BASE;

    pr_info("Initializing ISP memory at 0x%08x size=%u\n",
            (unsigned int)phys_addr, ISP_RMEM_SIZE);

    // Map the physical memory
    virt_addr = ioremap(phys_addr, ISP_RMEM_SIZE);
    if (!virt_addr) {
        pr_err("Failed to map ISP memory\n");
        return -ENOMEM;
    }

    // Store the mappings
    dev->dma_buf = virt_addr;
    dev->dma_addr = phys_addr;
    dev->dma_size = ISP_RMEM_SIZE;

    pr_info("ISP memory mapped: virt=%p phys=0x%08x\n",
            dev->dma_buf, (unsigned int)dev->dma_addr);

    return 0;
}


// Add validation before streaming
static int validate_streaming_setup(struct IMPISPDev *dev) {
    if (!dev || !dev->dma_buf || !dev->buf_info) {
        pr_err("Invalid device state for streaming\n");
        return -EINVAL;
    }

    // Verify memory mappings
    if (!dev->buf_info->buffer_start || !dev->buf_info->virt_addr) {
        pr_err("Invalid buffer mappings\n");
        return -EINVAL;
    }

    // Debug dump
    dump_memory_info(dev);
    return 0;
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


// Add these defines at the top of the file
#define SENSOR_CMD_BASIC         0x2000000  // Register sensor
#define SENSOR_CMD_READ_ID       0x2000011  // Read sensor ID
#define SENSOR_CMD_WRITE_REG     0x2000012  // Write sensor register
#define SENSOR_CMD_READ_REG      0x2000013  // Read sensor register
#define SENSOR_CMD_SET_GAIN      0x2000005  // Set sensor gain
#define SENSOR_CMD_SET_EXP       0x2000006  // Set exposure time
#define SENSOR_CMD_STREAM_ON     0x2000007  // Start streaming
#define SENSOR_CMD_STREAM_OFF    0x2000008  // Stop streaming

// Define a structure that might match the expectations of libimp.so
struct sensor_frame_channel {
    void *channel_ptr;      // Pointer to frame channel
    void *control_data_ptr; // Pointer to control data
    int status;             // Status code
    char reserved[8];       // Reserved/padding for alignment
};

struct sensor_reg_data {
    uint16_t reg;
    uint8_t val;
};


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
		    struct i2c_client *client = gISPdev->sensor_i2c_client;

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

        case SENSOR_CMD_STREAM_ON: {
            // SC2336 specific streaming configuration
            ret = isp_sensor_write_reg(client, 0x0100, 0x01); // Stream on
            if (ret < 0) {
                pr_err("Failed to start sensor streaming\n");
                return ret;
            }
            pr_info("Sensor streaming started\n");
            break;
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
		    char sensor_name[SENSOR_NAME_SIZE];

		    // Get the -1 value passed in
		    if (copy_from_user(&result_val, (void __user *)arg, sizeof(result_val))) {
		        pr_err("Failed to get result value\n");
		        return -EFAULT;
		    }

		    // Read the sensor name from /proc/jz/sensor/name
		    if (read_proc_string("/proc/jz/sensor/name", sensor_name, sizeof(sensor_name)) < 0) {
		        pr_err("Failed to read sensor name from /proc\n");
		        return -ENODEV;
		    }

		    // Store the dynamically read sensor name in our structure
		    strlcpy(gISPdev->sensor_name, sensor_name, SENSOR_NAME_SIZE);

		    pr_info("Stored sensor name: %s\n", gISPdev->sensor_name);
		    return 0;
		}
        case 0x800856d7: {
		    struct sensor_buffer_info wdr_info = {0};

		    pr_info("tx_isp: Handling WDR buffer info request\n");

		    if (!gISPdev || !gISPdev->wdr_mode) {
		        pr_err("tx_isp: WDR not enabled\n");
		        return -EINVAL;
		    }

		    // Ensure WDR control buffer is allocated
		    if (!gISPdev->wdr_buf_info) {
		        gISPdev->wdr_buf_info = kzalloc(sizeof(struct sensor_buffer_info), GFP_KERNEL);
		        if (!gISPdev->wdr_buf_info) {
		            pr_err("tx_isp: Failed to allocate WDR buffer info\n");
		            return -ENOMEM;
		        }
		    }

		    // Calculate WDR buffer size (assuming YUV420 format)
		    uint32_t y_size = gISPdev->width * gISPdev->height;
		    uint32_t uv_size = y_size / 2;
		    uint32_t wdr_size = y_size + uv_size;
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

    if (!gISPdev) {
        dev_err(&pdev->dev, "ISP device not initialized\n");
        return -EINVAL;
    }

    // Store the base address
    gISPdev->dma_addr = RMEM_BASE;
    gISPdev->dma_size = RMEM_SIZE;

    // Map the reserved memory region
    gISPdev->dma_buf = ioremap(RMEM_BASE, RMEM_SIZE);
    if (!gISPdev->dma_buf) {
        dev_err(&pdev->dev, "Failed to map reserved memory\n");
        return -ENOMEM;
    }

    pr_info("tx_isp: Reserved memory initialized:\n");
    pr_info("  Physical address: 0x%08x\n", (uint32_t)gISPdev->dma_addr);
    pr_info("  Virtual address: %p\n", gISPdev->dma_buf);
    pr_info("  Size: %zu bytes\n", gISPdev->dma_size);

    // Add validation print
    pr_info("tx_isp: Validating memory setup:\n");
    pr_info("  gISPdev = %p\n", gISPdev);
    pr_info("  dma_addr = 0x%08x\n", (uint32_t)gISPdev->dma_addr);
    pr_info("  dma_buf = %p\n", gISPdev->dma_buf);

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
#define RMEM_SIZE 22544384  // Match exact size LIBIMP expects
#define ISP_ALLOC_KMALLOC 1
#define ISP_ALLOC_CONTINUOUS 2

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

        // Validate frame source setup
        VALIDATE_FS_STATE(fs);

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

static int reset_gpio = GPIO_PA(18);  // Default reset GPIO
static int pwdn_gpio = -1;  // Default power down GPIO disabled


static int setup_i2c_sensor(struct IMPISPDev *dev, char *sensor_name)
{
    struct i2c_client *client;
    struct i2c_adapter *adapter;
    struct i2c_board_info board_info = {
        .addr = 0x30  // Match sensor I2C address
    };

    // Copy the sensor name into the fixed-size array
    strlcpy(board_info.type, sensor_name, I2C_NAME_SIZE);

    adapter = i2c_get_adapter(0);  // Primary I2C bus
    if (!adapter)
        return -ENODEV;

    client = i2c_new_device(adapter, &board_info);
    if (!client) {
        i2c_put_adapter(adapter);
        return -ENODEV;
    }

    dev->sensor_i2c_client = client;
    return 0;
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
    struct sensor_list_info sensor_list[MAX_SENSORS];
    int ret = 0;
    int channel = 0;

    pr_info("ISP IOCTL called: cmd=0x%x\n", cmd);  // Add this debug line
    pr_info("\n=== IOCTL Debug ===\n");
    pr_info("cmd=0x%x arg=0x%lx\n", cmd, arg);
    pr_info("file=%p flags=0x%x private_data=%p\n",
            file, file->f_flags, file->private_data);

    // Get channel number if this is a frame channel device
    if (file->private_data) {
        struct isp_framesource_state *fs = file->private_data;
        channel = fs->chn_num;
        pr_info("Using channel %d from frame source\n", channel);
    }

    // Basic validation
    if (!gISPdev || !gISPdev->is_open) {
        pr_err("ISP device not initialized or not open\n");
        return -EINVAL;
    }

    switch (cmd) {
	case VIDIOC_REGISTER_SENSOR: { // cmd=0x805056c1
	    struct i2c_client *client;
	    struct sensor_info sensor;
	    char param_file[128];
	    unsigned char val_h = 0, val_l = 0;
	    int i, ret;

	    // Step 1: Read sensor information from /proc/jz/sensor/
	    if (get_sensor_info(&sensor) < 0) {
	        pr_err("Failed to read sensor information from /proc\n");
	        return -ENODEV;
	    }

	    // Trim any whitespace from sensor name
	    trim_whitespace(sensor.name);

	    pr_info("Detected sensor: '%s'\n", sensor.name);

	    // FIXED: Properly construct parameter file path without the newline issue
	    snprintf(param_file, sizeof(param_file), "/etc/sensor/%s-t31.bin",
	             sensor.name); // Combines name with suffix in one operation
	    pr_info("Using parameter file: %s\n", param_file);

	    // Step 1: Allocate memory using vmalloc
	    tparams_day = vmalloc(ISP_OFFSET_PARAMS);
	    if (!tparams_day) {
	        dev_err(gISPdev->dev, "Failed to allocate tparams_day\n");
	        return -ENOMEM;
	    }
	    memset(tparams_day, 0, ISP_OFFSET_PARAMS);

	    tparams_night = vmalloc(ISP_OFFSET_PARAMS);
	    if (!tparams_night) {
	        dev_err(gISPdev->dev, "Failed to allocate tparams_night\n");
	        vfree(tparams_day);
	        return -ENOMEM;
	    }
	    memset(tparams_night, 0, ISP_OFFSET_PARAMS);

	    dev_info(gISPdev->dev, "tparams_day and tparams_night buffers allocated successfully\n");

	    // Step 2: Load parameters from the properly constructed file path
	    ret = tiziano_load_parameters(param_file, tparams_day, ISP_OFFSET_PARAMS);
	    if (ret) {
	        dev_err(gISPdev->dev, "Failed to load parameters from file: %s\n", param_file);
	        vfree(tparams_night);
	        vfree(tparams_day);
	        return ret;
	    }
	    dev_info(gISPdev->dev, "Parameters loaded successfully from %s\n", param_file);

	    // Step 3: Handle isp_memopt settings
	    if (isp_memopt == 1) {
	        dev_info(gISPdev->dev, "Applying isp_memopt settings\n");
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
	    dev_info(gISPdev->dev, "tparams_day written to register successfully\n");

        // Setup I2C sensor
    	ret = setup_i2c_sensor(gISPdev, sensor.name);
    	if (ret) {
        	dev_err(&gISPdev->dev, "Failed to setup I2C sensor\n");
            return -ENODEV;
   		}
        client = gISPdev->sensor_i2c_client;

	    pr_info("Registering sensor: %s (ID: 0x%x)\n", sensor.name, sensor.chip_id);
	    pr_info("Resolution: %dx%d, I2C Addr: 0x%x, FPS: %d-%d\n",
	            sensor.width, sensor.height, sensor.i2c_addr, sensor.min_fps, sensor.max_fps);

	    // Step 2: Reset the sensor with a delay
	    isp_sensor_write_reg(client, 0x0103, 0x01); // Reset sensor
	    msleep(20);

	    // Step 3: Initialize basic sensor settings using the dynamically fetched info
	    isp_sensor_write_reg(client, 0x3018, 0x72); // Example init register from sc2336.c
	    isp_sensor_write_reg(client, 0x3031, 0x0A);
	    msleep(10);

	    // Step 4: Read sensor ID with multiple retries
	    for (i = 0; i < 3; i++) {
	        // Read ID from correct registers now
	        ret = isp_sensor_read_reg(client, 0x3107, &val_h);
	        ret |= isp_sensor_read_reg(client, 0x3108, &val_l);
	        pr_info("Sensor ID = 0x%02x%02x (retry %d)\n", val_h, val_l, i);

	        // Check if the read ID matches the expected chip_id
	        if (ret == 0 && ((val_h << 8) | val_l) == sensor.chip_id) {
	            pr_info("Sensor ID matched: 0x%02x%02x\n", val_h, val_l);
	            break;
	        }
	        msleep(10);
	    }

	    if (i == 3) {
	        pr_err("Failed to verify sensor ID after multiple retries\n");
	        return -ENODEV;
	    }

	    return ret;
	}
	case VIDIOC_GET_SENSOR_ENUMERATION: {
		struct sensor_list_req {
	        int idx;    // Input index
	        char name[32];  // Output name
	    } __attribute__((packed));

	    struct sensor_list_req req;
	    struct sensor_info sensor;

	    // Get the request struct from userspace
	    if (copy_from_user(&req, (void __user *)arg, sizeof(req))) {
	        pr_err("Failed to copy request from user\n");
	        return -EFAULT;
	    }

	    // Check if index is valid
	    if (req.idx >= MAX_SENSORS) {
	        return -EINVAL;
	    }

       	// Step 1: Read sensor information from /proc/jz/sensor/
	    if (get_sensor_info(&sensor) < 0) {
	        pr_err("Failed to read sensor information from /proc\n");
	        return -ENODEV;
	    }
	    // Fill in the name for this index
	    snprintf(req.name, sizeof(req.name), "%s", sensor.name);

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

        // Check if the sensor is initialized
        if (!gISPdev || !gISPdev->sensor_i2c_client) {
            pr_err("Sensor not initialized\n");
            return -ENODEV;
        }

        struct isp_sensor_info info;
        info.is_initialized = 1;
        info.chip_id = 0xcb3a;  // TODO: Get the actual chip ID
        info.width = gISPdev->width;
        info.height = gISPdev->height;

        // Copy the info structure back to user space
        if (copy_to_user((void __user *)arg, &info, sizeof(info))) {
            pr_err("Failed to copy sensor info to user space\n");
            return -EFAULT;
        }

        break;
    }
	case VIDIOC_ENABLE_STREAM: {
        int channel = 0;  // Default channel
        ret = enable_isp_streaming(gISPdev, file, channel, 1);
        pr_info("Stream enable returned %d\n", ret);
        break;
	}
    case TX_ISP_SET_BUF: {
        return handle_get_buf_ioctl(gISPdev, arg);
    }
    case VIDIOC_SET_BUF_INFO: {
        struct sensor_buffer_info buf_info = {0};
        void __user *argp = (void __user *)arg;

        pr_info("tx_isp: Handling ioctl VIDIOC_SET_BUF_INFO\n");

        if (!gISPdev || !gISPdev->buf_info || !gISPdev->dma_buf) {
            pr_err("tx_isp: Invalid device state\n");
            return -EINVAL;
        }

        if (copy_from_user(&buf_info, argp, sizeof(buf_info))) {
            pr_err("tx_isp: Failed to copy from user\n");
            return -EFAULT;
        }

        // Use actual mapped addresses
        buf_info.method = ISP_ALLOC_KMALLOC;
        buf_info.buffer_start = gISPdev->dma_addr;
        buf_info.buffer_size = gISPdev->dma_size;
        buf_info.virt_addr = (unsigned long)gISPdev->dma_buf;
        buf_info.flags = 1;

        // Store consistent info
        memcpy(gISPdev->buf_info, &buf_info, sizeof(buf_info));

        pr_info("tx_isp: Buffer info configured: phys=0x%x virt=%p size=%u\n",
                (unsigned int)buf_info.buffer_start,
                (void *)buf_info.virt_addr,
                buf_info.buffer_size);

        if (copy_to_user(argp, &buf_info, sizeof(buf_info)))
            return -EFAULT;

        pr_info("tx_isp: Buffer setup completed successfully\n");
        return 0;
    }
    case TX_ISP_VIDEO_LINK_SETUP: {
      	pr_info("TX_ISP_VIDEO_LINK_SETUP\n");
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
      	pr_info("TX_ISP_SET_AE_ALGO_OPEN\n");
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
      	pr_info("TX_ISP_SET_AWB_ALGO_OPEN\n");
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
      	pr_info("TX_ISP_WDR_OPEN\n");
        // Handle WDR enable - matches case 0x800456d8
        unsigned int wdr_en = 1;
        ret = isp_set_wdr_mode(gISPdev, wdr_en);
        break;
    }

    case TX_ISP_WDR_CLOSE: {
      	pr_info("TX_ISP_WDR_CLOSE\n");
        // Handle WDR disable - matches case 0x800456d9
        unsigned int wdr_en = 0;
        ret = isp_set_wdr_mode(gISPdev, wdr_en);
        break;
    }

    case TX_ISP_WDR_GET_BUF: {
        pr_info("TX_ISP_WDR_GET_BUF\n");
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
        // ret = set_rotation(gISPdev, argp); // TODO
        break;
	}
    // 0x80045612
    case VIDIOC_STREAMON:  // 0x80045612
    {
        pr_info("Stream ON requested\n");
        channel = 0;  // Default to first channel for ISP device

        struct isp_framesource_state *fs = &gISPdev->frame_sources[channel];
        if (!fs) {
            pr_err("No frame source available\n");
            return -EINVAL;
        }

        // Make sure state is initialized
        if (!fs->is_open) {
            fs->width = 1920;  // Default resolution
            fs->height = 1080;
            fs->buf_cnt = 4;
            fs->buf_size = 0;  // Will be calculated in setup
            fs->state = 0;
            fs->is_open = 1;
            sema_init(&fs->sem, 1);
            init_waitqueue_head(&fs->wait);
        }

        ret = setup_frame_source(gISPdev, channel);
        if (ret) {
            pr_err("Failed to setup frame source: %d\n", ret);
            return ret;
        }

        ret = start_frame_source(gISPdev, channel);
        if (ret) {
            pr_err("Failed to start frame source: %d\n", ret);
            return ret;
        }

        return 0;
    }
	case VIDIOC_STREAMOFF:
	    pr_info("Stream OFF requested\n");
	    return enable_isp_streaming(gISPdev, file, channel, false);
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
        pr_info("Sensor command: 0x%x\n", cmd);
        ret = handle_sensor_ioctl(gISPdev, cmd, argp);
        break;

    default:
        dev_dbg(gISPdev->dev, "Unhandled ioctl cmd: 0x%x\n", cmd);
        return -ENOTTY;
    }

    return ret;
}

static int configure_isp_clocks(struct IMPISPDev *dev)
{
    int ret;

    pr_info("Configuring ISP clocks using standard API\n");

   	// CSI clock must be enabled at 125MHz for ISP functionality
    dev->csi_clk = clk_get(dev->dev, "csi");
    if (!IS_ERR(dev->csi_clk)) {
        ret = clk_prepare_enable(dev->csi_clk);
        if (ret) {
            pr_err("Failed to enable CSI clock\n");
            goto err_put_csi;
        }
    }


    // Get IPU clock first
    dev->ipu_clk = clk_get(dev->dev, "ipu");
    if (IS_ERR(dev->ipu_clk)) {
        pr_warn("IPU clock not available\n");
        dev->ipu_clk = NULL;
    }

    // Get ISP core clock
    dev->isp_clk = clk_get(dev->dev, "isp");
    if (IS_ERR(dev->isp_clk)) {
        ret = PTR_ERR(dev->isp_clk);
        pr_err("Failed to get ISP clock: %d\n", ret);
        dev->isp_clk = NULL;
        return ret;
    }

    // Get CGU ISP clock
    dev->cgu_isp_clk = clk_get(dev->dev, "cgu_isp");
    if (IS_ERR(dev->cgu_isp_clk)) {
        ret = PTR_ERR(dev->cgu_isp_clk);
        pr_err("Failed to get CGU ISP clock: %d\n", ret);
        dev->cgu_isp_clk = NULL;
        goto err_put_isp;
    }

    // Enable IPU clock
    if (dev->ipu_clk) {
        ret = clk_prepare_enable(dev->ipu_clk);
        if (ret)
            pr_warn("Failed to enable IPU clock\n");
    }

    // Set CGU ISP clock rate to 125MHz
    ret = clk_set_rate(dev->cgu_isp_clk, 125000000);
    if (ret) {
        pr_err("Failed to set CGU ISP clock rate: %d\n", ret);
        goto err_put_cgu;
    }

    // Enable ISP core clock
    ret = clk_prepare_enable(dev->isp_clk);
    if (ret) {
        pr_err("Failed to enable ISP clock: %d\n", ret);
        goto err_put_cgu;
    }

    // Enable CGU ISP clock
    ret = clk_prepare_enable(dev->cgu_isp_clk);
    if (ret) {
        pr_err("Failed to enable CGU ISP clock: %d\n", ret);
        goto err_disable_isp;
    }

    // Enable all required peripheral clocks
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

    // Verify rates
    // Verify rates including IPU
    pr_info("Clock rates after configuration:\n");
    pr_info("  ISP Core: %lu Hz\n", clk_get_rate(dev->isp_clk));
    pr_info("  CGU ISP: %lu Hz\n", clk_get_rate(dev->cgu_isp_clk));
    pr_info("  IPU: %lu Hz\n", clk_get_rate(dev->ipu_clk));

    return 0;

err_disable_isp:
    clk_disable_unprepare(dev->isp_clk);
err_disable_csi:
    if (dev->csi_clk)
        clk_disable_unprepare(dev->csi_clk);
err_put_csi:
    if (dev->csi_clk)
        clk_put(dev->csi_clk);
err_put_cgu:
    clk_put(dev->cgu_isp_clk);
    dev->cgu_isp_clk = NULL;
err_put_isp:
    clk_put(dev->isp_clk);
    dev->isp_clk = NULL;
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
    uint32_t reg_val;

    pr_info("Starting tisp_init...\n");

    // Verify registers are mapped using global gISPdev
    if (!gISPdev || !gISPdev->regs) {
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
    writel(0x1, gISPdev->regs + ISP_CLK_CTRL);
    msleep(10);

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


// And add buffer setup in open
static int framechan_open(struct inode *inode, struct file *file) {
    struct isp_framesource_state *fs;
    int minor = iminor(inode);
    uint32_t base_offset = 0x1094d4;  // From decompiled code

    fs = &gISPdev->frame_sources[minor];
    if (!fs->is_open) {
        fs->width = 1920;
        fs->height = 1080;
        fs->buf_cnt = 4;
        fs->buf_size = fs->width * fs->height * 2;

        // Map memory with proper offsets
        fs->buf_base = gISPdev->dma_buf + base_offset +
                      (minor * fs->buf_size * fs->buf_cnt);
        fs->dma_addr = gISPdev->dma_addr + base_offset +
                      (minor * fs->buf_size * fs->buf_cnt);

        mutex_init(&fs->sem);
        init_waitqueue_head(&fs->wait);
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
    struct IMPISPDev *dev = gISPdev;
    struct isp_framesource_state *fs = file->private_data;
    u32 buf_index;

    pr_info("Frame channel IOCTL: cmd=0x%x arg=0x%lx\n", cmd, arg);

    if (!dev || !fs || !fs->is_open) {
        pr_err("Invalid device state\n");
        return -EINVAL;
    }

    // Log memory layout before handling command
    pr_info("Memory state:\n");
    pr_info("  fs=%p private=%p\n", fs, fs->private);
    pr_info("  buf_base=%p dma_addr=0x%x\n",
            fs->buf_base, (unsigned int)fs->dma_addr);
    pr_info("  dev buf_info=%p\n", dev->buf_info);

    switch (cmd) {
        case VIDIOC_S_FMT: {
            struct v4l2_format fmt;
            if (copy_from_user(&fmt, (void __user *)arg, sizeof(fmt)))
                return -EFAULT;
            pr_info("Set format: %dx%d\n", fmt.fmt.pix.width, fmt.fmt.pix.height);
            fs->width = fmt.fmt.pix.width;
            fs->height = fmt.fmt.pix.height;
            return 0;
        }

        case VIDIOC_REQBUFS: {
            struct v4l2_requestbuffers req;
            if (copy_from_user(&req, (void __user *)arg, sizeof(req)))
                return -EFAULT;
            pr_info("Request buffers: count=%d\n", req.count);
            fs->buf_cnt = req.count;
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

        default:
            return -ENOTTY;
    }
}

static int framechan_release(struct inode *inode, struct file *file)
{
    struct isp_framesource_state *fs = file->private_data;

    if (fs && fs->is_open) {
        mutex_lock(&fs->sem);
        if (fs->buf_base) {
            iounmap(fs->buf_base);
            fs->buf_base = NULL;
        }
        fs->is_open = 0;
        mutex_unlock(&fs->sem);
    }
    return 0;
}

static struct file_operations framechan_fops = {
    .owner = THIS_MODULE,
    .open = framechan_open,
    .release = framechan_release,
    .unlocked_ioctl = framechan_ioctl,
};


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

static int tisp_probe(struct platform_device *pdev)
{
    struct isp_graph_data *graph_data;
    struct resource *res;
    void __iomem *base;
    unsigned int max_devices = 10;
    unsigned int registered_count = 0;
    int ret, i;

    pr_info("Probing TISP device...\n");

    // Validate gISPdev
    if (!gISPdev) {
        dev_err(&pdev->dev, "Global ISP device structure not allocated\n");
        return -ENOMEM;
    }

    // Initialize device structure properly
    gISPdev->dev = &pdev->dev;
    if (!gISPdev->dev) {
        dev_err(&pdev->dev, "Failed to set device pointer\n");
        return -EINVAL;
    }

    // Map registers with proper range
    base = ioremap(ISP_BASE_ADDR, ISP_MAP_SIZE);
    if (!base) {
        dev_err(&pdev->dev, "Failed to map ISP registers at 0x%08x\n",
                ISP_BASE_ADDR);
        return -ENOMEM;
    }

    // Store in device structure
    gISPdev->regs = base;

    // Verify mapping with test read
    uint32_t test_val = readl(base);
    pr_info("ISP register mapping: base=%p test_read=0x%x\n",
            base, test_val);

    // Set the IRQ value to 36
    gISPdev->irq = 36;
    pdev->dev.platform_data = &isp_pdata;

    // Now configure clocks with proper error handling
    ret = configure_isp_clocks(gISPdev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to configure ISP clocks: %d\n", ret);
        goto err_unmap_regs;
    }

    // Use the global gISPdev instead of creating a local one
    if (!gISPdev) {
        dev_err(&pdev->dev, "Global ISP device structure not allocated\n");
        return -ENOMEM;
    }

    gISPdev->dev = &pdev->dev;
    platform_set_drvdata(pdev, gISPdev);

 	// Initialize reserved memory
    ret = init_isp_reserved_memory(pdev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to initialize reserved memory\n");
        return ret;
    }

    // Add this call here after memory initialization
    ret = setup_isp_memory_regions(gISPdev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to setup memory regions\n");
        goto err_free_memory;
    }

    // setup subdevs
    ret = setup_isp_subdevs(gISPdev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to setup subdevs: %d\n", ret);
        goto err_free_memory;
    }

    // Allocate graph data with proper alignment
    graph_data = kzalloc(sizeof(*graph_data) + (max_devices * sizeof(void *)), GFP_KERNEL | GFP_DMA);
    if (!graph_data) {
        dev_err(&pdev->dev, "Failed to allocate graph data\n");
        return -ENOMEM;
    }

    // Initialize graph data
    graph_data->dev = gISPdev;
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
    pr_info("Registering child devices\n");
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

    ret = create_framechan_devices(&pdev->dev);
	if (ret) {
    	dev_err(&pdev->dev, "Failed to create frame channels\n");
    	goto err_free_memory;
	}

    dev_info(&pdev->dev, "TISP device probed successfully\n");

    // Return success
    return 0;

err_free_memory:
    // Add proper cleanup
    if (gISPdev->dma_buf) {
        iounmap(gISPdev->dma_buf);
        gISPdev->dma_buf = NULL;
    }
    return ret;
err_unregister_devices:
    remove_framechan_devices();
err_unmap_regs:
    if (gISPdev->regs)
        iounmap(gISPdev->regs);
    remove_isp_proc_entries();
free_graph:
    kfree(graph_data->devices);
free_devices:
    kfree(graph_data);
    return ret;
}




/* Platform driver remove function */
static int tisp_remove(struct platform_device *pdev) {
    struct IMPISPDev *dev = platform_get_drvdata(pdev);

    pr_info("ISP device remove called\n");

    if (dev) {

        // Clean up memory first
        cleanup_isp_memory(dev);

        // Clean up device clocks
		cleanup_isp_clocks(gISPdev);

        // Clean up I2C
        if (dev->sensor_i2c_client) {
            i2c_unregister_device(dev->sensor_i2c_client);
            dev->sensor_i2c_client = NULL;
        }

        // Remove proc entries and misc device
        remove_isp_proc_entries();

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
    if (!gISPdev)
        return sprintf(buf, "ISP device not initialized\n");

    return sprintf(buf, "ISP Status:\n"
                       "  Open: %s\n"
                       "  Sensor: %s\n"
                       "  WDR Mode: %d\n",
                       gISPdev->is_open ? "yes" : "no",
                       gISPdev->sensor_name[0] ? gISPdev->sensor_name : "none",
                       gISPdev->wdr_mode);
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

    // Set class permissions to be readable/writable by all
    tisp_class->dev_uevent = isp_dev_uevent;

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
        pr_err("Failed to create tx-isp device\n");
        cdev_del(&gISPdev->cdev);
        kfree(gISPdev);
        class_destroy(tisp_class);
        unregister_chrdev_region(tisp_dev_number, 1);
        return PTR_ERR(tisp_device);
    }

    if (tisp_device) {
        // Create status attribute
        if (device_create_file(tisp_device, &dev_attr_status) < 0) {
            pr_err("Failed to create device attributes\n");
        }
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

    // Add this block
    if (gISPdev && gISPdev->fs_info) {
        cleanup_frame_buffers(gISPdev->fs_info);
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
    if (gISPdev) {
        cleanup_isp_memory(gISPdev);
        if (gISPdev->buf_info) {
            kfree(gISPdev->buf_info);
            gISPdev->buf_info = NULL;
        }
        if (gISPdev->wdr_buf_info) {
            kfree(gISPdev->wdr_buf_info);
            gISPdev->wdr_buf_info = NULL;
        }
        kfree(gISPdev);
        gISPdev = NULL;
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
