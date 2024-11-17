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

#define ISP_BASE_IRQ    36   // TX-ISP base interrupt
#define ISP_IPC_IRQ     35   // ISP IPC interrupt
#define ISP_M0_IRQ      37   // Main ISP/VIC interrupt
#define ISP_W02_IRQ     38   // ISP module interrupt

static int __init_or_module early_init(void) __attribute__((constructor));
static int early_init(void)
{
    printk(KERN_INFO "ISP: Early initialization starting\n");
    return 0;
}


// Define a generic callback type for show functions
typedef int (*show_func_t)(struct seq_file *, void *);

// Structure to pass both function and data
struct proc_data {
    show_func_t show_func;
    void *private_data;
};

// Add these to your global variables
static struct proc_data *proc_data_fs = NULL;
static struct proc_data *proc_data_m0 = NULL;
static struct proc_data *proc_data_w00 = NULL;
static struct proc_data *proc_data_w01 = NULL;
static struct proc_data *proc_data_w02 = NULL;


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
#define ISP_REG_BASE 0x13300000
#define ISP_REG_SIZE 0x1000

// Buffer control registers - exact offsets from libimp
#define ISP_BUF_OFFSET     0x1000
#define ISP_BUF0_OFFSET    0x1000  // Base buffer register
#define ISP_BUF_SIZE_STEP  0x8     // Spacing between buffer registers
#define ISP_BUF_COUNT      4       // Match frame source buffer count

// Possibly supported video formats
#define ISP_FMT_YUV422    0x0a        // Current YUV422 format code
#define ISP_FMT_NV12      0x3231564e   // NV12 format code (from libimp)
#define V4L2_PIX_FMT_NV12 0x3231564e   // Match libimp's NV12 format code

// Use these for safety
#define MIN_LINE_SIZE      1920    // Minimum line size for 1080p
#define MAX_LINE_SIZE      4096    // Maximum supported line size
#define MIN_FRAME_SIZE     (1920 * 1080 * 2)  // Minimum frame size for 1080p YUV422
#define MAX_FRAME_SIZE     (4096 * 2160 * 2)  // Maximum supported frame size

// Control registers
#define ISP_CTRL_REG       0x100
#define ISP_STATUS_REG     0x104
#define ISP_INT_MASK_REG   0x108
#define ISP_INT_CLEAR_REG  0x10C

// Add these register definitions at the top with other registers
#define ISP_BYPASS_BASE   0x140    // Base offset for bypass control
#define ISP_BYPASS_CTRL   0x140    // Bypass control register
#define ISP_BYPASS_STATUS 0x144    // Bypass status register

// Add these bit definitions
#define ISP_BYPASS_EN     BIT(0)   // Enable bypass mode
#define ISP_DIRECT_PATH   BIT(1)   // Enable direct path

// WDR Register definitions from decompiled code
#define WDR_REG_BASE     0x2000
#define WDR_REG_BUF      (WDR_REG_BASE + 0x04)  // 0x2004
#define WDR_REG_LINE     (WDR_REG_BASE + 0x08)  // 0x2008
#define WDR_REG_HEIGHT   (WDR_REG_BASE + 0x0c)  // 0x200c

// WDR modes from decompiled code
#define WDR_MODE_NONE    0
#define WDR_MODE_LINE    1
#define WDR_MODE_FRAME   2


#define ISP_BUFFER_ALIGN    4096
#define ISP_MIN_BUFFER_SIZE (1 * 1024 * 1024)  // 1MB
#define ISP_MAX_BUFFER_SIZE (16 * 1024 * 1024) // 16MB
#define ISP_ALLOC_KMALLOC   1
#define ISP_RMEM_BASE       0x02a80000
#define ISP_RMEM_SIZE       22544384

/* ISP Status register bits */
#define ISP_STATUS_STREAMING  BIT(0)    // Streaming active
#define ISP_STATUS_BUSY      BIT(1)    // Hardware busy
#define ISP_STATUS_FRAME     BIT(2)    // Frame complete
#define ISP_STATUS_ERROR     BIT(3)    // Error condition

/* ISP Control register bits */
#define ISP_CTRL_ENABLE      BIT(0)    // Enable ISP core
#define ISP_CTRL_CAPTURE     BIT(1)    // Start capture
#define ISP_CTRL_UPDATE      BIT(2)    // Update settings

/* MIPI Status register bits */
#define MIPI_STATUS_READY    BIT(0)    // MIPI interface ready
#define MIPI_STATUS_ERROR    BIT(1)    // MIPI error occurred
#define MIPI_STATUS_ACTIVE   BIT(2)    // MIPI transfer active

/* MIPI Control register bits */
#define MIPI_CTRL_ENABLE     BIT(31)   // Global MIPI enable
#define MIPI_CTRL_CLK_EN     BIT(8)    // MIPI clock enable
#define MIPI_CTRL_LANES      0x3       // 2 data lanes

/* Stream Control register bits */
#define STREAM_CTRL_ENABLE   BIT(0)    // Enable streaming
#define STREAM_CTRL_ONESHOT  BIT(1)    // Single frame capture
#define STREAM_CTRL_ABORT    BIT(2)    // Abort current operation

/* Interrupt Control bits */
#define ISP_INT_FRAME        BIT(0)    // Frame complete
#define ISP_INT_ERROR        BIT(1)    // Error occurred
#define ISP_INT_OVERFLOW     BIT(2)    // Buffer overflow
#define ISP_INT_MIPI_ERROR   BIT(3)    // MIPI interface error

// Match libimp's DMA buffer layout
#define ISP_FRAME_BUFFER_OFFSET 0x1094d4  // From libimp
#define ISP_FRAME_BUFFER_ALIGN  0x1000
#define ISP_DMA_BUFFER_BASE    0x2a80000

/* Magic number from decompiled code at 0xef64 */
#define AE_ALGO_MAGIC   0x336ac
#define MAX_ISP_TASKS 32   // Maximum number of IRQ tasks to support


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
#define ISP_TUNING_ENABLE 0xc00c56c6
#define VIDIOC_SET_CHANNEL_ATTR 0xc07056c3 // Set channel attributes
#define VIDIOC_SET_FRAME_DEPTH 0xc0145608
#define VIDIOC_SET_FIFO_ATTR 0xc044560f
#define VIDIOC_STREAM_ON  0x80045612
#define VIDIOC_GET_BUFFER_INFO 0x400456bf
#define VIDIOC_SET_FRAME_MODE 0xc0445611
#define VIDIOC_VIDEO_LINK_SETUP 0x800456d0

// 0x44 byte structure based on command code
struct frame_mode {
    uint32_t channel;       // Channel number
    uint32_t mode;          // Frame mode
    uint32_t width;         // Frame width
    uint32_t height;        // Frame height
    uint32_t format;        // Pixel format
    uint32_t flags;         // Mode flags
    uint32_t buf_size;      // Buffer size
    uint32_t reserved[10];  // Padding to 0x44 bytes
} __attribute__((aligned(8)));  // Force 8-byte alignment

struct buffer_info {
    uint32_t channel;      // Channel number
    uint32_t buffer_size;  // Buffer size
    uint32_t count;        // Buffer count
    uint32_t flags;        // Buffer flags
    uint32_t status;       // Buffer status
} __attribute__((aligned(4)));

// Define the channel attribute structure (112 bytes / 0x70 bytes)
struct channel_attr {
    uint32_t enable;          // 0x00: Channel enable
    uint32_t width;           // 0x04: Frame width
    uint32_t height;          // 0x08: Frame height
    uint32_t format;          // 0x0c: Pixel format
    uint32_t crop_enable;     // 0x10: Crop enable
    struct {
        uint32_t x;           // 0x14: Crop x
        uint32_t y;           // 0x18: Crop y
        uint32_t width;       // 0x1c: Crop width
        uint32_t height;      // 0x20: Crop height
    } crop;
    uint32_t scaler_enable;   // 0x24: Scaler enable
    uint32_t scaler_outwidth; // 0x28: Output width
    uint32_t scaler_outheight;// 0x2c: Output height
    uint32_t picwidth;        // 0x30: Picture width
    uint32_t picheight;       // 0x34: Picture height
    char pad[0x38];          // Padding to 0x70 bytes
} __attribute__((packed));

struct frame_depth_config {
    uint32_t channel;        // Channel number
    uint32_t depth;          // Frame buffer depth
    uint32_t reserved[3];    // Reserved/padding
} __attribute__((packed));

// 0x44 byte structure for FIFO attributes
struct fifo_attr {
    uint32_t channel;      // Channel number
    uint32_t depth;        // FIFO depth
    uint32_t thresh;       // Threshold
    uint32_t flags;        // Flags/mode
    uint32_t watermark;    // Watermark level
    struct {
        uint32_t width;    // Frame width
        uint32_t height;   // Frame height
        uint32_t format;   // Pixel format
    } frame_info;
    uint32_t reserved[8];  // Padding to 0x44 bytes
} __attribute__((aligned(4))); // Important: Add alignment

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
#define ISP_STATUS_OFFSET   0x104    // Status register
#define ISP_INT_MASK       0x108    // Interrupt mask
#define ISP_INT_CLEAR      0x10C    // Interrupt clear
#define ISP_STREAM_CTRL    0x110    // Stream control
#define ISP_STREAM_START   0x114    // Stream start

#define W02_REG_BASE     0x2000
#define W02_STATUS_REG   0x04
#define W02_CTRL_REG     0x00
#define W02_INT_MASK     0x08
#define W02_INT_CLEAR    0x0C

// W02 interrupt status bits
#define W02_INT_FRAME_DONE  BIT(0)    // Frame processing complete
#define W02_INT_ERROR       BIT(1)    // Processing error
#define W02_INT_OVERFLOW    BIT(2)    // Buffer overflow
#define W02_INT_AE_DONE     BIT(3)    // Auto exposure complete
#define W02_INT_AWB_DONE    BIT(4)    // Auto white balance complete


// Add at top with other defines
#define ISP_CTRL_OFFSET     0x100   // Control register
#define ISP_STATUS_OFFSET   0x104   // Status register
#define ISP_INT_MASK       0x108   // Interrupt mask
#define ISP_INT_CLEAR      0x10C   // Interrupt clear
#define ISP_CONFIG_OFFSET   0x110   // Configuration register
#define ISP_INIT_CTRL      0x118   // Initialization control

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
    uint32_t write_idx;		// Write index
    struct file *fd;        // File pointer
    struct video_device *vdev;
    struct task_struct *thread;
    void    *ext_buffer;    // Extended buffer pointer
    struct isp_buffer_info *bufs;  // Buffer info array
    spinlock_t lock;               // Add spinlock for IRQ handler
    void    *private;             // Private data pointer

    // Add work queue support
    struct work_struct work;      // Work queue for frame processing
    struct workqueue_struct *wq;  // Work queue for this frame source

    // FIFO management
    struct list_head ready_queue;
    struct list_head done_queue;
    int fifo_depth;
    int frame_depth;
    bool fifo_initialized;

    // Synchronization
    struct semaphore sem;
    wait_queue_head_t wait;
    int is_open;           // 1 = initialized
   	// Add scaling/cropping info to state flags rather than separate fields
    /* Flag bits:
     * 0x1 = Enabled
     * 0x2 = Crop enabled
     * 0x4 = Scale enabled
     */

    // Store scaling/cropping in buffer info
    struct {
        struct {
            uint32_t x;
            uint32_t y;
            uint32_t width;
            uint32_t height;
        } crop;
        struct {
            uint32_t in_width;
            uint32_t in_height;
            uint32_t out_width;
            uint32_t out_height;
        } scale;
    } config;
};


/* Structures to match OEM driver */
struct isp_task {
    void *task_ptr;          // Task pointer
    void *task_data;         // Task specific data
    int (*task_func)(void);  // Function pointer
};

struct isp_irq_data {
    struct isp_task *tasks;     // Task array starts at -0x48
    void *buffer_start;         // End of task buffer at -0x08
    void *interrupt_handler;    // Main interrupt handler at +0x44
    u32 task_status;           // Task status at +0x24
};

/* Note for Claude, GPT, or anyone  that will listen
This is our internal driver structure and should never be passed back to libimp.
Instead, we need to return specific structures that libimp expects outside of this.
 */
struct IMPISPDev {
    // Base device info - verified offsets
    char dev_name[32];                    // 0x00: Device name
    struct cdev cdev;                     // Char device structure
    int major;                            // Major number
    int minor;                            // Minor number
    int fd;                 // Current fd for this instance
    int stream_count;           // Stream counter
    struct list_head list;  // For tracking multiple opens
    spinlock_t fd_lock;     // Protect fd operations
    int is_open;
    char sensor_name[SENSOR_NAME_SIZE];
    struct isp_subdev **subdevs; // Subdevice list pointer
    spinlock_t lock;                                  // at offset 0x80
    void (*irq_handler)(struct IMPISPDev*, unsigned long flags); // at offset 0x84
    int irq_enabled;                                 // at offset 0x88
    // Add IRQ-related members
    struct isp_irq_data *irq_data;   // IRQ task management
    struct isp_irq_context *irq_context;  // Add here with other IRQ members
    int irq;
    int irq_m0;              // Main ISP interrupt
    int irq_w02;            // Module interrupt
    void *interrupt_handler; // Interrupt handler function

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

    /* Tuning support */
    void *tuning_data;         // Buffer for tuning data (0x1c bytes)
    void __iomem *tuning_regs; // Tuning registers mapping
    int tuning_state;          // Tuning state (matches offset 0xa8)
    bool tuning_enabled;       // Tuning enabled flag
    // Algorithm completion flags
    int ae_algo_comp;
    int awb_algo_comp;

    // Algorithm status
    void *ae_stats;       // AE statistics buffer
    void *awb_stats;      // AWB statistics buffer
    // Add these new members for AE/AWB
    bool ae_enabled;      // Auto exposure enabled flag
    bool awb_enabled;     // Auto white balance enabled flag
    struct isp_ae_info *ae_info;  // Auto exposure info
    struct isp_awb_info *awb_info;  // Auto white balance info

    // Parameter regions
    void __iomem *isp_params;
    void __iomem *wdr_params;
    struct clk *isp_clk;       // ISP core clock
    struct clk *cgu_isp_clk;   // CGU ISP clock
    struct clk *csi_clk;       // CSI clock
    struct clk *ipu_clk;       // IPU clock
    struct isp_framesource_state frame_sources[MAX_FRAME_SOURCES];
    struct tx_isp_sensor_win_setting *sensor_window_size;

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
    spinlock_t lock;
    void* handler_function;
    int32_t irq_number;
    void* disable_function;
    struct irq_task *task_list;
    int task_count;
};

// Struct for passing IRQ-related information to functions
struct irq_info {
    int32_t irq_type;   // IRQ type/flags
    void *dev_id;       // Device ID for IRQ - critical!
    const char *name;   // IRQ name
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
    char sensor_name[80];  // Can hold the "sc2336" sensor name
    int num_sensors;       // Number of currently registered sensors
};

#define MAX_SENSORS 1

struct sensor_info {
    char name[32];
    bool registered;
};

struct frame_buffer_info {
    uint32_t index;      // Buffer index
    uint32_t type;       // Buffer type
    uint32_t memory;     // Memory type
    uint32_t bytesused;  // Bytes used in buffer
    uint32_t length;     // Total buffer length
    uint32_t flags;      // Buffer flags
    uint32_t field;      // Field number
    uint32_t timestamp;  // Frame timestamp
    uint32_t sequence;   // Frame sequence number
};

static struct sensor_info registered_sensors[MAX_SENSORS];
static int num_registered_sensors = 0;

// Add these structures to match libimp's buffer management
struct frame_node {
    uint32_t magic;         // 0x336ac
    uint32_t index;         // Buffer index
    uint32_t frame_size;    // Frame data size
    uint32_t timestamp;     // Frame timestamp
    void *data;            // Frame data pointer
    struct frame_buffer_info buf_info;
    dma_addr_t dma_addr;       // DMA address of this buffer
    void *virt_addr;           // Virtual address
    struct list_head list;     // List management
};

struct frame_queue {
    spinlock_t lock;                // Protect queue access
    struct list_head ready_list;    // Ready frames
    struct list_head done_list;     // Completed frames
    wait_queue_head_t wait;         // Wait queue for frames
    atomic_t frame_count;           // Number of frames in queue
    uint32_t max_frames;            // Maximum frames in queue
    struct frame_node *buffer_nodes;    // Array of buffer nodes
};

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


// Add these defines to match pad expectations
#define PAD_TYPE_SOURCE  1
#define PAD_TYPE_SINK    2
#define PAD_FLAGS_BOTH   0x3
#define LINK_STATE_INACTIVE  0
#define LINK_STATE_SOURCE    3

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
} __attribute__((packed, aligned(4)));

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


// Add to your existing CPM defines
#define T31_CPM_SRBC       0x14    // Soft reset and bus control
#define T31_CPM_MIPI_CTRL  0x0c    // MIPI control register

// MIPI reset and control bits
#define T31_MIPI_RST      BIT(26)  // MIPI reset bit
#define T31_MIPI_EN       BIT(27)  // MIPI clock enable
#define T31_MIPI_STOP     BIT(28)  // MIPI clock stop
#define T31_MIPI_DIV_MASK 0x3F     // MIPI clock divider mask

// Extended MIPI status bits
/* MIPI register map */
// Add to your register definitions
#define ISP_MIPI_BASE        0x7800  // MIPI register block offset
#define ISP_MIPI_CTRL        0x00    // Control register
#define ISP_MIPI_STATUS      0x04    // Status register
#define ISP_MIPI_TIMING      0x08    // Timing register
#define ISP_MIPI_FORMAT      0x0C    // Data format register
#define ISP_MIPI_VCID        0x10    // Virtual channel ID

// MIPI control bits
#define MIPI_CTRL_ENABLE     BIT(31) // Global enable
#define MIPI_CTRL_CLK_EN     BIT(12) // Clock lane enable
#define MIPI_CTRL_DPHY_EN    BIT(16) // DPHY enable
#define MIPI_CTRL_LANES      0x3     // Two data lanes

// MIPI status bits
#define MIPI_STATUS_READY    BIT(0)  // Interface ready
#define MIPI_STATUS_STOP     BIT(1)  // Lanes in stop state
#define MIPI_STATUS_ERR      BIT(2)  // Error detected
#define MIPI_STATUS_SYNC     BIT(3)  // Lanes synchronized

// T31 CPM bits for MIPI
#define T31_CPM_BASE         0x10000000
#define T31_CPM_CLKGR        0x20    // Clock gate register
#define T31_CPM_MIPI_CLK     BIT(4)  // MIPI clock bit
#define T31_CPM_MIPI_CTRL    0x0c    // MIPI control register

// Add to register definitions
#define VIC_BASE            0x1e0    // VIC register base offset
#define VIC_CTRL            0x100    // VIC control register
#define VIC_STATUS          0x004    // VIC status register
#define VIC_INT_MASK        0x1e8    // VIC interrupt mask
#define VIC_INT_ENABLE      0x1e0    // VIC interrupt enable
#define VIC_INT_CLEAR       0x008    // VIC interrupt clear
#define VIC_DMA_ADDR        0x200    // VIC DMA buffer address
#define VIC_DMA_SIZE        0x204    // VIC DMA buffer size
#define VIC_LINE_STRIDE     0x208    // VIC line stride
#define VIC_CH_ENABLE       0x3a8    // VIC channel enable

// VIC status bits
#define VIC_STATUS_READY    BIT(0)   // VIC ready
#define VIC_STATUS_BUSY     BIT(1)   // VIC busy
#define VIC_STATUS_ERROR    BIT(2)   // VIC error

#define MIPI_STATUS_READY    BIT(0)  // Ready bit
#define ISP_MIPI_CTRL        0x00    // Control register offset
#define ISP_MIPI_STATUS      0x04    // Status register offset
#define ISP_MIPI_BASE        0x7800  // Base offset from ISP


// Add these registers
#define ISP_MIPI_CTRL     0x30
#define ISP_MIPI_STATUS   0x34
#define ISP_MIPI_TIMING   0x38

// Add these defines at the top
#define ISP_MIPI_BASE     0x7800  // Base offset for MIPI registers
#define ISP_MIPI_CTRL     0x30    // Control register offset
#define ISP_MIPI_STATUS   0x34    // Status register offset
#define ISP_MIPI_TIMING   0x38    // Timing register offset
#define ISP_MIPI_RESET    0x3C    // Reset register offset

// MIPI control bits
#define MIPI_CTRL_ENABLE  BIT(31)
#define MIPI_CTRL_CLK_EN  BIT(8)
#define MIPI_CTRL_LANES   0x3     // 2 data lanes

// MIPI status bits
#define MIPI_STATUS_READY BIT(0)
#define MIPI_STATUS_ERROR BIT(1)
// Add these defines
#define T31_CPM_BASE       0x10000000
#define T31_CPM_MIPI_CTRL  0x0c   // MIPI control in CPM
#define T31_CPM_CLKGATE    0x20   // Clock gate register
#define T31_CPM_MIPI_CLK   BIT(4) // MIPI clock bit
#define T31_CPM_CLKGATE    0x20
#define T31_CPM_CLKGATE1   0x28        // Clock gate 1 register offset
#define T31_CPM_CLKGR_ISP  BIT(23)     // ISP clock gate bit
#define T31_CPM_CLKGR1_CGU BIT(2)      // CGU ISP clock gate bit
// Clock control registers
#define T31_CPM_ISPCDR     0x88        // ISP clock divider register
#define T31_CPM_LCR        0x04        // Low power control register
#define T31_CPM_OPCR       0x24        // Operating parameter control

#define T31_CPM_MIPI_CTRL  0x0c
#define T31_CPM_SRBC       0x14   // Soft reset and bus control
#define T31_CPM_OPCR       0x24   // Operating parameter control


#define T31_MIPI_DIV_MASK  0x3F
#define T31_MIPI_STOP      BIT(28)
#define T31_MIPI_EN        BIT(27)
#define T31_MIPI_RST       BIT(26)

// Add these defines
#define CPM_MIPI_CTRL_EN   BIT(31)
#define CPM_MIPI_RST_MASK  BIT(30)
#define CPM_MIPI_CLK_EN    BIT(29)
#define CPM_MIPI_CLK_SEL   BIT(28)


// Base ISP interrupt status bits
#define ISP_BASE_FRAME_DONE  BIT(0)    // Frame processing complete
#define ISP_BASE_ERROR       BIT(1)    // Error occurred

// IPC interrupt status bits
#define ISP_IPC_MSG_RX      BIT(0)    // Message received
#define ISP_IPC_ERROR       BIT(1)    // IPC error

// Register offsets
#define ISP_BASE_STATUS_REG  0x104    // Base status register
#define ISP_BASE_CLEAR_REG   0x108    // Base clear register
#define ISP_IPC_STATUS_REG   0x204    // IPC status register
#define ISP_IPC_CLEAR_REG    0x208    // IPC clear register

// Base ISP interrupt bits
#define ISP_BASE_FRAME_DONE  BIT(0)    // Frame processing complete
#define ISP_BASE_ERROR       BIT(1)    // Error occurred

// VIC interrupt bits
#define VIC_STATUS_READY    BIT(0)    // VIC ready
#define VIC_STATUS_BUSY     BIT(1)    // VIC busy
#define VIC_STATUS_ERROR    BIT(2)    // VIC error
#define VIC_STATUS_FRAME    BIT(3)    // Frame complete

#define VIC_INT_FRAME       BIT(0)    // Frame interrupt enable
#define VIC_INT_ERROR       BIT(1)    // Error interrupt enable
#define VIC_INT_OVERFLOW    BIT(2)    // Buffer overflow

// M0 interrupt bits
#define ISP_INT_FRAME_DONE  BIT(0)    // Frame complete
#define ISP_INT_ERROR       BIT(1)    // Error occurred
#define ISP_INT_OVERFLOW    BIT(2)    // Buffer overflow
#define ISP_INT_MIPI_ERROR  BIT(3)    // MIPI interface error

// W02 interrupt bits
#define W02_INT_FRAME_DONE  BIT(0)    // Frame processing complete
#define W02_INT_ERROR       BIT(1)    // Processing error
#define W02_INT_OVERFLOW    BIT(2)    // Buffer overflow
#define W02_INT_AE_DONE     BIT(3)    // Auto exposure complete
#define W02_INT_AWB_DONE    BIT(4)    // Auto white balance complete

static DECLARE_WAIT_QUEUE_HEAD(ae_wait);
static DECLARE_WAIT_QUEUE_HEAD(awb_wait);

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

// Update the free IRQ function
static void tx_isp_free_irq(struct isp_irq_data *irq_data)
{
    if (!irq_data)
        return;

    if (irq_data->tasks) {
        // Free task array if allocated
        kfree(irq_data->tasks);
        irq_data->tasks = NULL;
    }

    irq_data->interrupt_handler = NULL;
    irq_data->task_status = 0;
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

#define ISP_INT_FRAME_DONE  BIT(0)    // Frame complete
#define ISP_INT_DMA_ERR    BIT(1)    // DMA error
#define ISP_INT_BUF_FULL   BIT(2)    // Buffer full

/* Update frame_source_channel struct */
struct frame_source_channel {
    uint8_t  reserved1[0x1c];    // 0x00-0x1c: Reserved space before state
    uint32_t state;              // 0x1c: State (1=ready, 2=streaming)
    void     *buf_base;          // 0x20: Base buffer address
    dma_addr_t dma_addr;         // 0x24: DMA address
    uint32_t buf_size;           // 0x28: Buffer size per frame
    uint32_t buf_cnt;            // 0x2c: Number of buffers (4)
    uint32_t write_idx;          // 0x30: Current write index
    uint32_t frame_cnt;          // 0x34: Frame counter

    // Synchronization primitives - careful with alignment
    struct semaphore sem;        // 0x38
    struct mutex lock;           // Keep aligned
    uint32_t channel_offset;     // Channel offset for indexing

    // Queue management - preserve alignment
    spinlock_t queue_lock;
    struct list_head ready_list;
    struct list_head done_list;
    wait_queue_head_t wait;
    atomic_t frame_count;
    uint32_t max_frames;

    // FIFO configuration
    uint32_t fifo_depth;
    uint32_t fifo_thresh;
    uint32_t fifo_flags;

    // Frame completion tracking
    atomic_t frames_completed;
    atomic_t buffer_index;
    spinlock_t state_lock;
    unsigned long buffer_states; 		// Bitmap of buffer states
    struct frame_node *buffer_nodes;  	// Array of buffer nodes

    // Pad the rest exactly to 0x2e8
    uint8_t padding[0x2e8 - 0x1c - sizeof(uint32_t) -
                   sizeof(void*) - sizeof(dma_addr_t) -
                   sizeof(uint32_t) * 4 -
                   sizeof(struct semaphore) - sizeof(struct mutex) -
                   sizeof(uint32_t) - sizeof(spinlock_t) -
                   sizeof(struct list_head) * 2 -
                   sizeof(wait_queue_head_t) - sizeof(atomic_t) -
                   sizeof(uint32_t) * 3 -
                   sizeof(atomic_t) * 2 - sizeof(spinlock_t) -
                   sizeof(unsigned long)];
} __attribute__((packed, aligned(4)));



// Helper to safely get a buffer node
static struct frame_node *get_buffer_node(struct frame_source_channel *fc, int index) {
    if (!fc || !fc->buffer_nodes || index >= fc->buf_cnt)
        return NULL;
    return &fc->buffer_nodes[index];
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


static void dump_frame_data(void *buf, size_t size)
{
    u8 *data = buf;
    int i;

    pr_info("Frame Data Sample (first 32 bytes):\n");
    for (i = 0; i < 32 && i < size; i++) {
        if ((i % 16) == 0)
            printk(KERN_CONT "\n%04x: ", i);
        printk(KERN_CONT "%02x ", data[i]);
    }
    printk(KERN_CONT "\n");
}

static void handle_frame_complete(struct isp_framesource_state *fs)
{
    struct frame_source_channel *fc = fs->private;
    unsigned long flags;
    u32 buf_index;

    if (!fc)
        return;

    spin_lock_irqsave(&fc->queue_lock, flags);

    buf_index = fs->buf_index;

    // Mark current buffer as filled
    mark_buffer_filled(fc, buf_index);

    // Advance to next buffer
    fs->buf_index = (buf_index + 1) % fs->buf_cnt;
    fs->frame_cnt++;

    // Debug print every 30 frames
    if ((fs->frame_cnt % 30) == 0) {
        pr_info("Frame complete: count=%d index=%d\n",
                fs->frame_cnt, buf_index);
    }

    spin_unlock_irqrestore(&fc->queue_lock, flags);

    // Wake up any waiting processes
    wake_up(&fs->wait);
}


static irqreturn_t isp_irq_thread_handler(int irq, void *dev_id)
{
    struct IMPISPDev *dev = dev_id;
    uint32_t var_30 = 0;
    void __iomem *isp_regs;
    uint32_t reg_val;

    if (!dev) {
        pr_err("Invalid device in IRQ handler\n");
        return IRQ_NONE;
    }

    if (!dev->regs) {
        pr_err("Invalid register mapping in IRQ handler\n");
        return IRQ_NONE;
    }

    // Calculate register offset with bounds checking
    isp_regs = dev->regs + 0xd4;
    if (!isp_regs) {
        pr_err("Invalid register offset calculation\n");
        return IRQ_NONE;
    }

    // Process frame interrupts - matching decompiled
    for (int i = 0; i < 7; i++) {
        uint32_t *irq_state = isp_regs + 0x180 + (i * 8);

        // Validate pointer arithmetic
        if (!access_ok(VERIFY_READ, irq_state, sizeof(uint32_t))) {
            pr_err("Invalid IRQ state access\n");
            continue;
        }

        reg_val = readl(irq_state);
        if (reg_val == 0)
            continue;

        // Track completion state
        var_30 = readl(irq_state + 1);

        if (i == 5)
            continue;

        // Safe register read with proper offset calculation
        void __iomem *status_reg = dev->regs + 0x120 + 0xf0;
        if (!access_ok(VERIFY_READ, status_reg, sizeof(uint32_t))) {
            pr_err("Invalid status register access\n");
            continue;
        }

        if (readl(status_reg) != 0) {
            // Process sensor frame
            if (dev->frame_sources) {
                handle_frame_complete(&dev->frame_sources[0]);
                writel(0, irq_state);  // Clear interrupt state
            } else {
                pr_err("No frame sources available\n");
            }
        } else {
            pr_info("Unhandled IRQ: %d\n", i);
        }
    }

    return IRQ_HANDLED;
}


static irqreturn_t isp_irq_handler(int irq, void *dev_id) {
    struct IMPISPDev *dev = dev_id;
    struct isp_framesource_state *fs;
    struct frame_source_channel *fc;
    unsigned long flags;
    u32 status;

    status = readl(dev->regs + ISP_STATUS_REG);
    if (!status)
        return IRQ_NONE;

    // Clear interrupts
    writel(status, dev->regs + ISP_INT_CLEAR);
    wmb();

    if (status & ISP_INT_FRAME_DONE) {
        fs = &dev->frame_sources[0];
        fc = fs->private;
        if (!fc)
            return IRQ_HANDLED;

        spin_lock_irqsave(&fc->queue_lock, flags);

        struct frame_node *node;
        list_for_each_entry(node, &fc->ready_list, list) {
            if (!test_bit(node->index, &fc->buffer_states)) {
                // Mark buffer as filled
                set_bit(node->index, &fc->buffer_states);
                struct timespec ts;
				ktime_get_ts(&ts);
				node->timestamp = ts.tv_sec * 1000000000LL + ts.tv_nsec;

                // Move to done queue
                list_move_tail(&node->list, &fc->done_list);

                // Wake up any waiting processes
                wake_up_interruptible(&fc->wait);
                break;
            }
        }

        spin_unlock_irqrestore(&fc->queue_lock, flags);
    }

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

static void ispcore_irq_fs_work(struct IMPISPDev *dev, unsigned long flags)
{
    uint32_t *isp_regs;
    uint32_t *irq_state;
    int i;

    if (!dev || !dev->regs)
        return;

    isp_regs = (uint32_t *)(dev->regs + 0xd4);  // Important offset from decompiled

    // Process frame interrupts - from decompiled loop
    for (i = 0; i < 7; i++) {
        irq_state = (uint32_t *)(isp_regs + 0x180 + (i * 8));
        if (*irq_state == 0)
            continue;

        // Skip special case
        if (i == 5)
            continue;

        // Check frame completion
        if (*(uint32_t *)(dev->regs + 0x120 + 0xf0) != 1) {
            handle_frame_complete(&dev->frame_sources[0]);
            *irq_state = 0;  // Clear interrupt state
        }
    }
}

static void tx_isp_enable_irq(struct irq_handler_data *handler)
{
    unsigned long flags;

    if (!handler || !ourISPdev || !ourISPdev->regs)
        return;

    spin_lock_irqsave(&handler->lock, flags);

    // Enable interrupts according to IRQ number
    switch (handler->irq_number) {
        case ISP_BASE_IRQ:  // 36 - Base ISP
            writel(0x3f9, ourISPdev->regs + ISP_INT_MASK);
            writel(0xFFFFFFFF, ourISPdev->regs + ISP_INT_CLEAR);
            break;

        case ISP_M0_IRQ:   // 37 - Main ISP/VIC
            writel(0x1, ourISPdev->regs + 0x13c); // VIC IRQ enable seen in logs
            break;

        case ISP_W02_IRQ:  // 38 - W02 module
            writel(0x3f9, ourISPdev->regs + W02_REG_BASE + W02_INT_MASK);
            writel(0xFFFFFFFF, ourISPdev->regs + W02_REG_BASE + W02_INT_CLEAR);
            break;
    }
    wmb();

    spin_unlock_irqrestore(&handler->lock, flags);
}

static void tx_isp_disable_irq(struct irq_handler_data *handler)
{
    unsigned long flags;

    if (!handler || !ourISPdev || !ourISPdev->regs)
        return;

    spin_lock_irqsave(&handler->lock, flags);

    // Disable interrupts according to IRQ number
    switch (handler->irq_number) {
        case ISP_BASE_IRQ:  // 36
            writel(0x0, ourISPdev->regs + ISP_INT_MASK);
            break;

        case ISP_M0_IRQ:   // 37
            writel(0x0, ourISPdev->regs + 0x13c);
            break;

        case ISP_W02_IRQ:  // 38
            writel(0x0, ourISPdev->regs + W02_REG_BASE + W02_INT_MASK);
            break;
    }
    wmb();

    spin_unlock_irqrestore(&handler->lock, flags);
}

// Updated request function to match OEM pattern
static int tx_isp_request_irq(struct platform_device *pdev, struct isp_irq_data *irq_data)
{
    int ret;

    // Get base ISP IRQ
    int base_irq = platform_get_irq_byname(pdev, "isp-base-irq");
    if (base_irq < 0) {
        dev_err(&pdev->dev, "Failed to get base ISP IRQ\n");
        return base_irq;
    }

    // Get M0 IRQ
    int m0_irq = platform_get_irq_byname(pdev, "isp-m0-irq");
    if (m0_irq < 0) {
        dev_err(&pdev->dev, "Failed to get M0 IRQ\n");
        return m0_irq;
    }

    // Get W02 IRQ
    int w02_irq = platform_get_irq_byname(pdev, "isp-w02-irq");
    if (w02_irq < 0) {
        dev_err(&pdev->dev, "Failed to get W02 IRQ\n");
        return w02_irq;
    }

    // Request base ISP IRQ
    ret = request_threaded_irq(base_irq, isp_irq_handler, isp_irq_thread_handler,
                              IRQF_SHARED | IRQF_ONESHOT,
                              "isp-base", irq_data);
    if (ret) {
        dev_err(&pdev->dev, "Failed to request base ISP IRQ\n");
        return ret;
    }

    // Store IRQ numbers in device structure
    ourISPdev->irq = base_irq;
    ourISPdev->irq_m0 = m0_irq;
    ourISPdev->irq_w02 = w02_irq;

    return 0;
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
            pr_info("Skipping NULL device at index %d\n", i);
            continue;
        }

        drvdata = platform_get_drvdata(pdev);
        if (!drvdata) {
            pr_info("No driver data for device %d\n", i);
            continue;
        }

        // Setup device node
        pr_info("Processing device %d: %s\n", i, pdev->name);

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
            pr_info("%s: %s", func, buf);
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
static struct IMPISPDev *ourISPdev_bak = NULL;  // Backup for reopen

// Add these offsets from OEM driver
#define SHARED_BUF_OFFSET 0x130
#define CHAN_BUF_OFFSET  0x118

#define VBM_POOL_SIZE 0x99888
#define VBM_ALLOC_METHOD 0x3


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
    struct frame_source_channel *fc = fs->private;

    while (!kthread_should_stop()) {
        // Wait for interrupt completion
        wait_event_interruptible(fs->wait,
            atomic_read(&fc->frames_completed) > fc->write_idx ||
            kthread_should_stop());

        if (kthread_should_stop())
            break;

        // Process frame
        spin_lock_irq(&fc->state_lock);
        if (is_buffer_filled(fc, fs->buf_index)) {
            // Handle filled buffer...
            mark_buffer_empty(fc, fs->buf_index);
            fs->buf_index = (fs->buf_index + 1) % fs->buf_cnt;
        }
        spin_unlock_irq(&fc->state_lock);
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

// Update memory setup to match sensor requirements
static int setup_isp_buffers(struct IMPISPDev *dev)
{
    struct sensor_win_size *wsize = dev->sensor_i2c_client->dev.platform_data;
    void __iomem *regs;

    if (!wsize) {
        pr_err("No sensor window size information\n");
        return -EINVAL;
    }

    if (!dev->regs) {
        pr_err("No register mapping\n");
        return -EINVAL;
    }

    regs = dev->regs;

    // Use sensor window size for buffer calculations
    uint32_t width = wsize->width;
    uint32_t height = wsize->height;
    uint32_t line_size = ((width + 7) >> 3) << 3;
    uint32_t frame_size = line_size * height;

    pr_info("Setting up buffers for %dx%d frame at base %p\n",
            width, height, regs);

    // Important: Use ISP_BUF_OFFSET instead of ISP_BUF0_REG
    writel(dev->dma_addr, regs + ISP_BUF_OFFSET);
    writel(line_size, regs + ISP_BUF_OFFSET + 0x4);
    wmb();

    // Configure second buffer at proper offset
    writel(dev->dma_addr + frame_size, regs + ISP_BUF_OFFSET + ISP_BUF_SIZE_STEP);
    writel(line_size, regs + ISP_BUF_OFFSET + ISP_BUF_SIZE_STEP + 0x4);
    wmb();

    // Configure third buffer
    uint32_t third_line_size = ((((width + 0x1f) >> 5) + 7) >> 3) << 3;
    uint32_t third_addr = dev->dma_addr + (frame_size * 2);
    writel(third_addr, regs + ISP_BUF_OFFSET + (ISP_BUF_SIZE_STEP * 2));
    writel(third_line_size, regs + ISP_BUF_OFFSET + (ISP_BUF_SIZE_STEP * 2) + 0x4);
    wmb();

    pr_info("Buffer setup complete:\n");
    pr_info("  Main buffer: 0x%08x size=%u\n", dev->dma_addr, frame_size);
    pr_info("  Second buffer: 0x%08x size=%u\n",
            dev->dma_addr + frame_size, frame_size);
    pr_info("  Third buffer: 0x%08x line_size=%u\n",
            third_addr, third_line_size);

    // Add register value verification
    pr_info("Register verification:\n");
    pr_info("  BUF0: addr=0x%08x size=0x%08x\n",
            readl(regs + ISP_BUF_OFFSET),
            readl(regs + ISP_BUF_OFFSET + 0x4));
    pr_info("  BUF1: addr=0x%08x size=0x%08x\n",
            readl(regs + ISP_BUF_OFFSET + ISP_BUF_SIZE_STEP),
            readl(regs + ISP_BUF_OFFSET + ISP_BUF_SIZE_STEP + 0x4));

    return 0;
}


static int start_frame_source(struct IMPISPDev *dev, int channel) {
    struct isp_framesource_state *fs = &dev->frame_sources[channel];
    struct frame_source_channel *fc = fs->private;
    void __iomem *regs = dev->regs;
    void __iomem *vic_regs = dev->regs + VIC_BASE;
    int ret;

    pr_info("Starting frame source on channel %d\n", channel);

    if (!fs || !fc || !fs->is_open || !regs) {
        pr_err("Invalid frame source state\n");
        return -EINVAL;
    }

    // Reset VIC first
    writel(0x0, vic_regs + VIC_CTRL);
    wmb();
    msleep(10);

    // Configure VIC format for NV12
    writel(0x2c0, vic_regs + 0x24);    // NV12 format
    writel(ALIGN(fs->width, 32), vic_regs + 0x28); // Line stride
    wmb();

    // Enable VIC
    writel(0x1, vic_regs + VIC_CTRL);
    writel(0x1, vic_regs + VIC_CH_ENABLE);
    wmb();

    // Configure channel-specific buffer
    uint32_t buf_offset = ISP_BUF0_OFFSET + (channel * ISP_BUF_SIZE_STEP);

    // Set up buffers for NV12 - Y plane followed by UV plane
    for (int i = 0; i < fc->buf_cnt; i++) {
        uint32_t buf_addr = fc->dma_addr + (i * fc->buf_size);
        writel(buf_addr, regs + buf_offset + (i * ISP_BUF_SIZE_STEP));
        writel(fc->buf_size, regs + buf_offset + (i * ISP_BUF_SIZE_STEP) + 0x4);
        wmb();

        pr_info("Buffer %d: addr=0x%08x size=%u\n",
                i, buf_addr, fc->buf_size);
    }

    // Channel-specific stream control
    if (channel == 0) {
        // Main channel gets special control values
        writel(0x80007000, regs + ISP_STREAM_CTRL);
        wmb();
        udelay(100);
        writel(0x777111, regs + ISP_STREAM_START);
        wmb();

        // W02 control is critical for NV12
        writel(0x00310001, regs + W02_REG_BASE + W02_CTRL_REG);
        wmb();
    }

    // Enable ISP interrupts
    writel(0x3f9, regs + ISP_INT_MASK);
    writel(0xFFFFFFFF, regs + ISP_INT_CLEAR);
    writel(0x3f9, regs + W02_REG_BASE + W02_INT_MASK);
    writel(0xFFFFFFFF, regs + W02_REG_BASE + W02_INT_CLEAR);
    wmb();

    // Update state
    fs->state = 2;  // Running state
    fs->flags |= 0x2;
    fs->frame_cnt = 0;
    fs->buf_index = 0;

    pr_info("Frame source %d streaming started:\n", channel);
    pr_info("  VIC: ctrl=0x%08x status=0x%08x\n",
            readl(vic_regs + VIC_CTRL),
            readl(vic_regs + 0x4));
    pr_info("  ISP: stream_ctrl=0x%08x start=0x%08x\n",
            readl(regs + ISP_STREAM_CTRL),
            readl(regs + ISP_STREAM_START));
    pr_info("  W02: ctrl=0x%08x mask=0x%08x\n",
            readl(regs + W02_REG_BASE + W02_CTRL_REG),
            readl(regs + W02_REG_BASE + W02_INT_MASK));

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


static void isp_frame_work(struct work_struct *work)
{
    struct isp_framesource_state *fs =
        container_of(work, struct isp_framesource_state, work);
    struct frame_source_channel *fc = fs->private;
    unsigned long flags;

    spin_lock_irqsave(&fc->queue_lock, flags);

    // Get current buffer
    int buf_idx = fs->buf_index;

    // Mark buffer as filled
    if (fs->bufs && buf_idx < fs->buf_cnt) {
        fs->bufs[buf_idx].is_buffer_full = 1;
        fs->bufs[buf_idx].frame_count++;
    }

    // Advance to next buffer
    fs->buf_index = (buf_idx + 1) % fs->buf_cnt;
    fs->frame_cnt++;

    spin_unlock_irqrestore(&fc->queue_lock, flags);

    // Wake up any waiting processes
    wake_up_interruptible(&fs->wait);
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

    // Initialize work queue
    INIT_WORK(&fs->work, isp_frame_work);

    // Enable IRQs for this channel
    writel(BIT(channel), dev->regs + ISP_INT_MASK);
    wmb();

    // Debug print to see our type flags
    pr_info("Channel %d initialized: %dx%d, buffers=%d size=%d type_flags=0x%x\n",
            channel, fs->width, fs->height, fs->buf_cnt, fs->buf_size, type_flags);

    // Also dump the actual value at the offset
    pr_info("Channel %d: value at 0x58 offset: 0x%x\n", channel,
            *(uint32_t *)((char *)dev + 0x58 + (channel * sizeof(uint32_t))));

    return 0;
}


static int init_vic_and_mipi(struct IMPISPDev *dev) {
    void __iomem *vic_regs = dev->regs + 0x1e0;
    void __iomem *mipi_regs = dev->regs + ISP_MIPI_BASE;

    // Reset VIC first
    writel(0x0, vic_regs + VIC_CTRL);
    wmb();
    msleep(10);

    // Configure format
    writel(0x2c0, vic_regs + 0x24); // Set NV12 format

    // Enable interrupts
    writel(VIC_INT_FRAME, vic_regs + 0x1e8); // Enable frame interrupt
    writel(0xFFFFFFFF, vic_regs + 0x8); // Clear pending
    wmb();

    // Enable VIC
    writel(0x1, vic_regs + VIC_CTRL);
    writel(0x1, vic_regs + VIC_CH_ENABLE);
    wmb();

    // Configure MIPI
    writel(MIPI_CTRL_ENABLE | MIPI_CTRL_CLK_EN | MIPI_CTRL_LANES,
           mipi_regs + ISP_MIPI_CTRL);
    wmb();

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

    // Add VIC/MIPI initialization here
    //ret = init_vic_and_mipi(ourISPdev);
    //if (ret) {
    //    pr_err("Failed to initialize VIC/MIPI: %d\n", ret);
    //    return ret;
    //}

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


static int setup_isp_interrupts(struct IMPISPDev *dev)
{
    if (!dev || !dev->regs) {
        pr_err("Invalid device state for interrupt setup\n");
        return -EINVAL;
    }

    pr_info("Setting up ISP interrupts\n");

    // Enable frame interrupts with full mask
    writel(0x3f9, dev->regs + ISP_INT_MASK);
    writel(0xFFFFFFFF, dev->regs + ISP_INT_CLEAR); // Clear any pending
    wmb();

    // Enable base ISP interrupt
    writel(ISP_CTRL_ENABLE, dev->regs + ISP_CTRL_REG);
    wmb();

    // Enable W02 interrupts with full mask
    writel(0x3f9, dev->regs + W02_REG_BASE + W02_INT_MASK);
    writel(0xFFFFFFFF, dev->regs + W02_REG_BASE + W02_INT_CLEAR);
    wmb();

    // Critical: Enable VIC IRQ
    writel(0x1, dev->regs + 0x13c);
    wmb();

    pr_info("ISP interrupts enabled:\n");
    pr_info("  CTRL: 0x%08x\n", readl(dev->regs + ISP_CTRL_REG));
    pr_info("  INT_MASK: 0x%08x\n", readl(dev->regs + ISP_INT_MASK));
    pr_info("  W02_INT_MASK: 0x%08x\n",
            readl(dev->regs + W02_REG_BASE + W02_INT_MASK));

    return 0;
}

static int setup_vic_interrupts(struct IMPISPDev *dev)
{
    unsigned long flags;
    u32 val;

    // Lock to match decompiled
    spin_lock_irqsave(&dev->lock, flags);

    // From decompiled - only enable if not already enabled
    if (!(readl(dev->regs + 0x13c))) {
        writel(1, dev->regs + 0x13c);

        // Enable VIC interrupts
        val = readl(dev->regs + 0x1e0);
        val |= 0x1;  // Enable bit
        writel(val, dev->regs + 0x1e0);
    }

    spin_unlock_irqrestore(&dev->lock, flags);

    return 0;
}

// Add these defines at the top with other defines
#define ISP_STREAM_MAGIC    0x80007000  // Magic value from logs
#define ISP_START_MAGIC     0x777111    // Magic value from logs
#define ISP_W02_CTRL_MAGIC  0x00310001  // Magic value from logs

// Update the existing enable_stream() function
static int enable_stream(struct IMPISPDev *dev, int channel, bool enable) {
    void __iomem *regs = dev->regs;
    void __iomem *vic_regs = dev->regs + VIC_BASE;
    struct isp_framesource_state *fs;
    struct frame_source_channel *fc;
    unsigned long flags;
    int ret;

    if (!dev || !dev->regs) {
        pr_err("Invalid device state\n");
        return -EINVAL;
    }

    fs = &dev->frame_sources[channel];
    if (!fs || !fs->is_open) {
        pr_err("Invalid frame source state\n");
        return -EINVAL;
    }

    fc = fs->private;
    if (!fc) {
        pr_err("No frame source channel data\n");
        return -EINVAL;
    }

    spin_lock_irqsave(&fc->queue_lock, flags);

    if (enable) {
        // Only channel 0 gets magic values - matches logs
        if (channel == 0) {
            // Setup stream control with proper sequence
            writel(ISP_STREAM_MAGIC, regs + ISP_STREAM_CTRL);
            wmb();
            udelay(100);  // Required delay
            writel(ISP_START_MAGIC, regs + ISP_STREAM_START);
            wmb();

            // W02 control - critical for getting frames
            writel(ISP_W02_CTRL_MAGIC, regs + W02_REG_BASE + W02_CTRL_REG);
            wmb();

            // Enable VIC proper sequence
            writel(0x2c0, vic_regs + 0x24);  // NV12 format
            writel(0x1, vic_regs);           // Enable VIC
            writel(0x1, vic_regs + VIC_CH_ENABLE);
            wmb();

            // Enable all required interrupts
            writel(0x3f9, regs + ISP_INT_MASK_REG);
            writel(0xFFFFFFFF, regs + ISP_INT_CLEAR_REG);
            writel(0x3f9, regs + W02_REG_BASE + W02_INT_MASK);
            writel(0xFFFFFFFF, regs + W02_REG_BASE + W02_INT_CLEAR);
            writel(0x1, regs + 0x13c);  // Critical VIC IRQ enable
            wmb();
        } else {
            // Other channels get zeroed control - matches logs
            writel(0x0, regs + ISP_STREAM_CTRL + (channel * 0x100));
            writel(0x0, regs + ISP_STREAM_START + (channel * 0x100));
            wmb();
        }

        // Update state
        fs->state = 2;  // Streaming state
        fs->flags |= 0x2;  // Streaming flag
    } else {
        // Stop streaming in reverse order
        uint32_t ctrl_offset = ISP_STREAM_CTRL + (channel * 0x100);
        uint32_t start_offset = ISP_STREAM_START + (channel * 0x100);

        writel(0x0, regs + start_offset);
        if (channel == 0) {
            writel(0x0, vic_regs + VIC_CH_ENABLE);
        }
        wmb();
        udelay(100);
        writel(0x0, regs + ctrl_offset);
        if (channel == 0) {
            writel(0x0, vic_regs);
        }
        wmb();

        fs->state = 1;
        fs->flags &= ~0x2;
        fs->frame_cnt = 0;
        fs->buf_index = 0;
        atomic_set(&fc->frames_completed, 0);
    }

    spin_unlock_irqrestore(&fc->queue_lock, flags);

    pr_info("Stream %s for channel %d complete\n",
            enable ? "enabled" : "disabled", channel);
    return 0;
}


static int enable_isp_streaming(struct IMPISPDev *dev, struct file *file, int channel, bool enable) {
    struct isp_framesource_state *fs;
    struct frame_source_channel *fc;
    void __iomem *regs;
    void __iomem *vic_regs;
    unsigned long flags;
    int ret = 0;

    if (!dev || !dev->regs) {
        pr_err("Invalid device state\n");
        return -EINVAL;
    }

    // Get frame source from file->private_data
    fs = file->private_data;
    if (!fs) {
        pr_err("No frame source state in file\n");
        return -EINVAL;
    }
    fc = fs->private;

    if (!fs || !fc) {
        pr_err("Invalid frame source state\n");
        return -EINVAL;
    }

    pr_info("Stream control: channel=%d enable=%d state=%d\n",
            channel, enable, fs->state);

    if (enable) {
        // First set ISP core control register
        writel(ISP_CTRL_ENABLE | ISP_CTRL_CAPTURE, regs + ISP_CTRL_REG);
        wmb();
        udelay(100);

        // Set up global interrupts
        writel(ISP_INT_FRAME_DONE | ISP_INT_ERROR, regs + ISP_INT_MASK);
        writel(0xFFFFFFFF, regs + ISP_INT_CLEAR);
        wmb();

        // Setup VIC
        writel(0x2, vic_regs);  // Reset VIC
        wmb();
        udelay(100);
        writel(0x4, vic_regs);  // Set operation mode
        wmb();
        udelay(100);

        // VIC format config
        writel(0x2c0, vic_regs + 0x24);  // Set NV12 format
        wmb();

        // Channel-specific buffer setup
        uint32_t ctrl_offset = ISP_STREAM_CTRL + (channel * 0x100);
        uint32_t start_offset = ISP_STREAM_START + (channel * 0x100);
        uint32_t buf_offset = ISP_BUF0_OFFSET + (channel * ISP_BUF_SIZE_STEP);

        // Configure all buffers for this channel
        for (int i = 0; i < fc->buf_cnt; i++) {
            uint32_t buf_addr = fc->dma_addr + (i * fc->buf_size);
            writel(buf_addr, regs + buf_offset + (i * ISP_BUF_SIZE_STEP));
            writel(fc->buf_size, regs + buf_offset + (i * ISP_BUF_SIZE_STEP) + 0x4);
            wmb();
        }

        // Channel control sequence
        writel(0x1, vic_regs);          // Enable VIC
        wmb();
        udelay(100);

        writel(0x1, regs + ctrl_offset); // Stream control
        wmb();
        udelay(100);

        writel(0x1, regs + start_offset); // Stream start
        writel(0x1, vic_regs + VIC_CH_ENABLE); // VIC channel
        wmb();

        // W02 interrupt setup
        writel(W02_INT_FRAME_DONE | W02_INT_ERROR, regs + W02_REG_BASE + W02_INT_MASK);
        writel(0xFFFFFFFF, regs + W02_REG_BASE + W02_INT_CLEAR);
        wmb();

        // Update state
        fs->state = 2;
        fs->flags |= 0x2;

        // Verify final register state
        pr_info("Channel %d streaming setup:\n", channel);
        pr_info("  ISP CTRL=0x%08x INT_MASK=0x%08x\n",
                readl(regs + ISP_CTRL_REG),
                readl(regs + ISP_INT_MASK));
        pr_info("  VIC CTRL=0x%08x STATUS=0x%08x\n",
                readl(vic_regs),
                readl(vic_regs + 0x4));
        pr_info("  STREAM CTRL=0x%08x START=0x%08x\n",
                readl(regs + ctrl_offset),
                readl(regs + start_offset));
        pr_info("  W02 INT_MASK=0x%08x\n",
                readl(regs + W02_REG_BASE + W02_INT_MASK));

        // Final check and force IRQ generation
        writel(ISP_CTRL_ENABLE | ISP_CTRL_CAPTURE | ISP_CTRL_UPDATE,
               regs + ISP_CTRL_REG);
        wmb();
    } else {
        // Check if in proper state
        if (fs->state != 2) {
            pr_warn("Channel %d not in streaming state\n", channel);
            return -EINVAL;
        }

        // Disable stream first
        ret = enable_stream(dev, channel, false);
        if (ret) {
            pr_err("Failed to disable stream: %d\n", ret);
            return ret;
        }

        // Reset state
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
    pr_info("Read operation\n");
    return 0;  // Remove incorrect error path
}

static ssize_t tisp_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
    pr_info("Write operation\n");
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

/* Global state variables seen in decompiled */
static void *ae_info_mine;
static void *ae_statis_mine;
static void *awb_info_mine;
static int ae_algo_comp;
static int awb_algo_comp;

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
    wmb();

    // Read back status to verify
    u32 status = readl(dev->regs + ISP_STAT_OFFSET);
    pr_info("ISP status after init: 0x%08x\n", status);

    // Configure DMA addresses
    if (dev->dma_addr) {
        writel(dev->dma_addr, dev->regs + ISP_BUF0_REG);
        writel(dev->dma_addr + dev->dma_size/2, dev->regs + ISP_BUF1_REG);
        wmb();

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


    // Ensure DMA coherency
    dma_sync_single_for_device(dev->dev, dev->dma_addr,
                              dev->dma_size, DMA_FROM_DEVICE);

    // Enable streaming with interrupts
    writel(ISP_CTRL_ENABLE | ISP_CTRL_CAPTURE | ISP_INT_FRAME_DONE,
           dev->regs + ISP_CTRL_REG);
    wmb();


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
		case VIDIOC_STREAMON: { // 0x80045612
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

static int configure_streaming_hardware(struct IMPISPDev *dev)
{
    void __iomem *regs = dev->regs;
    void __iomem *vic_regs = dev->regs + VIC_BASE; // 0x1e0 offset
    struct isp_framesource_state *fs;
    struct frame_source_channel *fc;
    int ret;

    if (!dev || !regs) {
        pr_err("Invalid device state\n");
        return -EINVAL;
    }

    fs = &dev->frame_sources[0];
    fc = fs->private;
    if (!fc) {
        pr_err("No frame source channel\n");
        return -EINVAL;
    }


    // Step 1: Reset everything first
    writel(0x0, regs + ISP_CTRL_REG);  // Reset ISP core
    writel(0x0, vic_regs + VIC_CTRL);  // Reset VIC
    wmb();
    msleep(10);

    // Step 2: Configure VIC format (matching channel 0)
    writel(0x2c0, vic_regs + 0x24);     // NV12 format
    wmb();

    // Step 3: Configure ISP core for channel 0
    writel(0x80007000, regs + ISP_STREAM_CTRL);
    wmb();
    udelay(100);
    writel(0x777111, regs + ISP_STREAM_START);
    wmb();

    // Step 4: Set up W02 control (value from logs)
    writel(0x00310001, regs + W02_REG_BASE + W02_CTRL_REG);
    wmb();

    // Step 5: Enable interrupts
    writel(0x3f9, regs + ISP_INT_MASK_REG);
    writel(0xFFFFFFFF, regs + ISP_INT_CLEAR_REG);
    writel(0x3f9, regs + W02_REG_BASE + W02_INT_MASK);
    writel(0xFFFFFFFF, regs + W02_REG_BASE + W02_INT_CLEAR);
    wmb();

    // Step 6: Enable core IRQs
    writel(0x1, regs + ISP_INIT_OFFSET);  // Core init
    writel(0x1, regs + 0x13c);            // VIC IRQ enable
    wmb();

    // Step 7: Configure channel-specific state
    for (int i = 0; i < MAX_FRAME_SOURCES; i++) {
        fs = &dev->frame_sources[i];
        if (fs && fs->is_open) {
            if (i == 0) {
                // Channel 0 gets full control values
                writel(0x80007000, regs + ISP_STREAM_CTRL + (i * 0x100));
                writel(0x777111, regs + ISP_STREAM_START + (i * 0x100));
            } else {
                // Other channels get zeroed
                writel(0x0, regs + ISP_STREAM_CTRL + (i * 0x100));
                writel(0x0, regs + ISP_STREAM_START + (i * 0x100));
            }
            wmb();
        }
    }

    pr_info("Hardware configuration complete:\n");
    pr_info("  Stream control: 0x%08x\n", readl(regs + ISP_STREAM_CTRL));
    pr_info("  Stream start:   0x%08x\n", readl(regs + ISP_STREAM_START));
    pr_info("  W02 control:    0x%08x\n", readl(regs + W02_REG_BASE + W02_CTRL_REG));
    pr_info("  ISP int mask:   0x%08x\n", readl(regs + ISP_INT_MASK_REG));
    pr_info("  W02 int mask:   0x%08x\n", readl(regs + W02_REG_BASE + W02_INT_MASK));
    pr_info("  VIC control:    0x%08x\n", readl(vic_regs + VIC_CTRL));

    return 0;
}


static int handle_stream_enable(struct IMPISPDev *dev, bool enable)
{
    void __iomem *regs = dev->regs;

    if (!regs)
        return -EINVAL;

    pr_info("Configuring stream enable=%d\n", enable);

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

static int prepare_streaming(struct IMPISPDev *dev)
{
    int ret;

    // Configure buffer locations
    ret = setup_isp_buffers(dev);
    if (ret)
        return ret;

    // Configure streaming hardware
    ret = configure_streaming_hardware(dev);
    if (ret)
        goto cleanup_buffers;

    return 0;

cleanup_buffers:
    cleanup_frame_buffers(&dev->frame_sources[0]);
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
	case VIDIOC_REGISTER_SENSOR: { // cmd=0x805056c1
	    struct i2c_client *client = ourISPdev->sensor_i2c_client;
	    unsigned char val_h = 0, val_l = 0;
	    int i;

        if (!client) {
            pr_err("No I2C client available\n");
            return -ENODEV;
        }

        pr_info("Registering sensor: Initializing SC2336\n");

        // Reset sensor with proper delay
        ret = isp_sensor_write_reg(client, 0x0103, 0x01);
        if (ret) {
            pr_err("Failed to reset sensor\n");
            return ret;
        }
        msleep(20);

        // Initialize essential registers (from SC2336 datasheet)
        ret = isp_sensor_write_reg(client, 0x3018, 0x72);
        if (ret) return ret;

        ret = isp_sensor_write_reg(client, 0x3031, 0x0a);
        if (ret) return ret;

        // Read sensor ID with retries
        int retries = 3;
        while (retries--) {
            ret = isp_sensor_read_reg(client, 0x3107, &val_h);
            if (ret) continue;

            ret = isp_sensor_read_reg(client, 0x3108, &val_l);
            if (ret) continue;

            pr_info("SC2336: ID = 0x%02x%02x\n", val_h, val_l);
            if ((val_h == 0x23) && (val_l == 0x36)) { // Expected SC2336 ID
                return 0;
            }
            msleep(10);
        }

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
   	case VIDIOC_VIDEO_LINK_SETUP: {  // 0x800456d0
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

		        // Force enable=1 to keep link enabled
		        if (sd->src_pads && sd->sink_pads) {
		            sd->src_pads->sink = sd->sink_pads;
		            sd->sink_pads->source = sd->src_pads;
		            // Always keep links in SOURCE state
		            sd->src_pads->link_state = LINK_STATE_SOURCE;
		            sd->sink_pads->link_state = LINK_STATE_SOURCE;
		        }

		        mutex_unlock(&sd->lock);
		    }

		    // Write back success
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


// Add CGU ISP bit positions
#define ISP_CLKGR_BIT    (1 << 23)   // ISP clock gate bit in CLKGR
#define CGU_ISP_BIT      (1 << 2)    // CGU_ISP clock gate bit in CLKGR1

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
    const char *param_file = "/etc/sensor/sc2336-t31.bin";
    uint32_t reg_val;
    struct irq_handler_data handler_data = {
        .task_list = NULL,
        .task_count = 0,
        .irq_number = 0,
        .handler_function = NULL,
        .disable_function = NULL
    };
    spin_lock_init(&handler_data.lock);

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

static int framechan_open(struct inode *inode, struct file *file)
{
    struct isp_framesource_state *fs;
    int minor = iminor(inode);
    uint32_t base_offset = 0x1094d4;

    fs = &ourISPdev->frame_sources[minor];
    if (!fs->is_open) {
        struct frame_source_channel *fc;

        fc = kzalloc(sizeof(*fc), GFP_KERNEL);
        if (!fc)
            return -ENOMEM;

        fs->width = 1920;
        fs->height = 1080;
        fs->buf_cnt = 4;

        // Calculate proper NV12 buffer size
        uint32_t y_size = fs->width * fs->height;
        uint32_t uv_size = y_size / 2;
        fs->buf_size = y_size + uv_size;

        // Map memory with alignment
        fs->buf_base = ourISPdev->dma_buf + ALIGN(base_offset, 32) +
                      (minor * fs->buf_size * fs->buf_cnt);
        fs->dma_addr = ourISPdev->dma_addr + ALIGN(base_offset, 32) +
                      (minor * fs->buf_size * fs->buf_cnt);

        pr_info("Frame channel buffer setup:\n");
        pr_info("  base=%p dma=0x%08x\n", fs->buf_base, (u32)fs->dma_addr);
        pr_info("  size=%u count=%d align=%d\n",
                fs->buf_size, fs->buf_cnt, 32);

        // Initialize channel data
        fc->buf_base = fs->buf_base;
        fc->dma_addr = fs->dma_addr;
        fc->buf_size = fs->buf_size;
        fc->buf_cnt = fs->buf_cnt;
        fc->state = 1;

        // Initialize buffer nodes
        fc->buffer_nodes = kzalloc(sizeof(struct frame_node) * fc->buf_cnt,
                                 GFP_KERNEL);
        if (!fc->buffer_nodes) {
            kfree(fc);
            return -ENOMEM;
        }

        // Setup each buffer node
        for (int i = 0; i < fc->buf_cnt; i++) {
            struct frame_node *node = &fc->buffer_nodes[i];

            // Initialize libimp expected fields
            node->magic = 0x336ac;
            node->index = i;
            node->frame_size = fc->buf_size;
            node->timestamp = 0;

            // Set up buffer pointers
            node->data = fc->buf_base + (i * fc->buf_size);
            node->dma_addr = fc->dma_addr + (i * fc->buf_size);
            node->virt_addr = node->data;

            // Initialize list entry
            INIT_LIST_HEAD(&node->list);
        }

        // Initialize synchronization primitives
        mutex_init(&fc->lock);
        spin_lock_init(&fc->queue_lock);
        init_waitqueue_head(&fc->wait);
        atomic_set(&fc->frame_count, 0);

        // Initialize buffer queues
        INIT_LIST_HEAD(&fc->ready_list);
        INIT_LIST_HEAD(&fc->done_list);
        fc->buffer_states = 0;

        fs->private = fc;
        fs->is_open = 1;
    }

    file->private_data = fs;
    pr_info("Frame channel %d ready for IRQs:\n"
            "  ISP irq: %d ctrl: 0x%08x\n",
            minor,
            ourISPdev->irq_m0, readl(ourISPdev->regs + ISP_CTRL_REG));
    pr_info("Other irqs: w02=%d ctrl: 0x%08x base=%d ctrl: 0x%08x\n",
            ourISPdev->irq_w02, readl(ourISPdev->regs + W02_REG_BASE + W02_CTRL_REG),
            ourISPdev->irq, readl(ourISPdev->regs + ISP_CTRL_REG));

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

		    pr_info("Channel attr request:\n"
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
		    fs->fmt = attr.format;  // Should be 0x3231564e for NV12

		    // Update buffer configuration if needed
		    if (attr.enable) {
		        // Calculate aligned buffer size for NV12
		        uint32_t width_aligned = ALIGN(attr.width, 32);  // 32-byte align for DMA

		        // Y plane: full resolution
		        uint32_t y_size = width_aligned * attr.height;

		        // UV plane: half height (NV12 format)
		        uint32_t uv_size = width_aligned * (attr.height / 2);

		        // Total size must be 4K aligned for DMA
		        fs->buf_size = ALIGN(y_size + uv_size, 4096);
		        fc->buf_size = fs->buf_size;

		        pr_info("NV12 buffer calculation:\n"
		                "  Width aligned: %d\n"
		                "  Y plane: %u bytes (%d x %d)\n"
		                "  UV plane: %u bytes (%d x %d)\n"
		                "  Total aligned: %u bytes\n",
		                width_aligned,
		                y_size, width_aligned, attr.height,
		                uv_size, width_aligned, attr.height/2,
		                fs->buf_size);

		        // Set channel state
		        fs->state = 1;  // Ready state
		        fs->flags |= 0x1;  // Enable flag
		        fc->state = 1;  // Channel ready

		        // Handle scaling if enabled
		        if (attr.scaler_enable) {
		            fs->width = attr.scaler_outwidth;
		            fs->height = attr.scaler_outheight;

		            // Recalculate buffer size for scaled dimensions
		            width_aligned = ALIGN(fs->width, 32);
		            y_size = width_aligned * fs->height;
		            uv_size = width_aligned * (fs->height / 2);
		            fs->buf_size = ALIGN(y_size + uv_size, 4096);
		            fc->buf_size = fs->buf_size;

		            pr_info("Scaled NV12 buffer:\n"
		                    "  New size: %dx%d\n"
		                    "  New buffer size: %u\n",
		                    fs->width, fs->height,
		                    fs->buf_size);
		        }

		        // Override with picture dimensions if specified
		        if (attr.picwidth && attr.picheight) {
		            fs->width = attr.picwidth;
		            fs->height = attr.picheight;

		            // Recalculate buffer size for picture dimensions
		            width_aligned = ALIGN(fs->width, 32);
		            y_size = width_aligned * fs->height;
		            uv_size = width_aligned * (fs->height / 2);
		            fs->buf_size = ALIGN(y_size + uv_size, 4096);
		            fc->buf_size = fs->buf_size;

		            pr_info("Picture dimensions buffer:\n"
		                    "  Pic size: %dx%d\n"
		                    "  New buffer size: %u\n",
		                    fs->width, fs->height,
		                    fs->buf_size);
		        }

		    } else {
		        // Channel disable
		        fs->state = 0;
		        fs->flags &= ~0x1;
		        fc->state = 0;

		        pr_info("Channel %d disabled\n", fs->chn_num);
		    }

		    // Important: Update channel parameters
		    if (!fs->buf_cnt) {
		        fs->buf_cnt = 4;  // Default buffer count
		    }

		    // Validate final configuration
		    if (fs->buf_size == 0 || fs->width == 0 || fs->height == 0) {
		        pr_err("Invalid channel configuration:\n"
		               "  size=%u width=%u height=%u\n",
		               fs->buf_size, fs->width, fs->height);
		        return -EINVAL;
		    }

		    pr_info("Channel %d configured: %dx%d fmt=0x%x size=%d state=%d\n"
		            "  Buffer count=%d flags=0x%x\n",
		            fs->chn_num, fs->width, fs->height, fs->fmt,
		            fs->buf_size, fs->state, fs->buf_cnt, fs->flags);

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
		    struct frame_buffer_info buf;
		    struct frame_source_channel *fc = fs->private;
		    struct frame_node *node;
		    unsigned long flags;

		    if (copy_from_user(&buf, (void __user *)arg, sizeof(buf)))
		        return -EFAULT;

		    if (buf.index >= fc->buf_cnt) {
		        pr_err("Invalid buffer index %d\n", buf.index);
		        return -EINVAL;
		    }

		    node = get_buffer_node(fc, buf.index);
		    if (!node)
		        return -EINVAL;

		    spin_lock_irqsave(&fc->queue_lock, flags);

		    // Mark buffer as ready for filling
		    clear_bit(buf.index, &fc->buffer_states);

		    // Add to ready queue if not already there
		    if (list_empty(&node->list))
		        list_add_tail(&node->list, &fc->ready_list);

		    atomic_inc(&fc->frame_count);

		    spin_unlock_irqrestore(&fc->queue_lock, flags);

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

            // TODO super useful logging that shows when both channel buffers are being requested
//            pr_info("Buffer info for channel %d: size=%u count=%u state=%d flags=0x%x\n",
//                     info.channel, info.buffer_size, info.count, info.status, info.flags);

            if (copy_to_user((void __user *)arg, &info, sizeof(info))) {
                pr_err("Failed to copy buffer info to user\n");
                return -EFAULT;
            }

            return 0;
        }
        case VIDIOC_SET_FRAME_MODE: { // 0xc0445611
		    struct frame_buffer_info buf;
		    struct frame_source_channel *fc = fs->private;
		    struct frame_node *node;
		    unsigned long flags;
		    int ret;

		    if (copy_from_user(&buf, (void __user *)arg, sizeof(buf)))
		        return -EFAULT;

		    spin_lock_irqsave(&fc->queue_lock, flags);

		    if (list_empty(&fc->done_list)) {
		        spin_unlock_irqrestore(&fc->queue_lock, flags);

		        ret = wait_event_interruptible_timeout(
		            fc->wait,
		            !list_empty(&fc->done_list),
		            msecs_to_jiffies(1000));

		        if (ret <= 0)
		            return ret == 0 ? -ETIMEDOUT : ret;

		        spin_lock_irqsave(&fc->queue_lock, flags);
		    }

		    node = list_first_entry(&fc->done_list, struct frame_node, list);
		    list_del_init(&node->list);

		    // Update buffer info
		    buf.index = node->index;
		    buf.bytesused = node->frame_size;
		    buf.timestamp = node->timestamp;
		    buf.flags = 0;

		    spin_unlock_irqrestore(&fc->queue_lock, flags);

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

static int setup_i2c_sensor(struct IMPISPDev *dev)
{
    struct i2c_client *client;
    struct i2c_adapter *adapter;
    struct sensor_win_size *wsize;

    // Allocate window size structure
    wsize = kzalloc(sizeof(*wsize), GFP_KERNEL);
    if (!wsize)
        return -ENOMEM;

    // Fill in window settings from driver
    wsize->width = 1920;
    wsize->height = 1080;
    wsize->fps = 30;
    wsize->max_fps = 30;

    struct i2c_board_info board_info = {
        .type = "sc2336",
        .addr = 0x30,  // Match sensor I2C address
        .platform_data = wsize,  // Set platform data
    };

    adapter = i2c_get_adapter(0);  // Primary I2C bus
    if (!adapter) {
        kfree(wsize);
        return -ENODEV;
    }

    client = i2c_new_device(adapter, &board_info);
    if (!client) {
        kfree(wsize);
        i2c_put_adapter(adapter);
        return -ENODEV;
    }

    // Store window size in sensor client platform data
    client->dev.platform_data = wsize;
    dev->sensor_i2c_client = client;

    pr_info("Sensor initialized with resolution %dx%d @ %d fps\n",
            wsize->width, wsize->height, wsize->fps);

    return 0;
}
struct isp_irq_context {
    struct irq_handler_data *base_handler;
    struct irq_handler_data *m0_handler;
    struct irq_handler_data *w02_handler;
};

static int tisp_probe(struct platform_device *pdev)
{
    struct resource *res;
    void __iomem *base;
    struct isp_graph_data *graph_data;
    int ret;

    pr_info("Probing TISP device...\n");

    if (!ourISPdev) {
        dev_err(&pdev->dev, "Global ISP device structure not allocated\n");
        return -ENOMEM;
    }

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res) {
        dev_err(&pdev->dev, "Failed to get memory resource\n");
        return -ENODEV;
    }

    if (!request_mem_region(res->start, resource_size(res), pdev->name)) {
        dev_err(&pdev->dev, "Failed to request memory region\n");
        return -EBUSY;
    }

    base = ioremap(ISP_BASE_ADDR, ISP_MAP_SIZE);
    if (!base) {
        dev_err(&pdev->dev, "Failed to map ISP registers\n");
        ret = -ENOMEM;
        goto err_release_mem;
    }

    // Store mapped registers and device
    ourISPdev->regs = base;
    ourISPdev->dev = &pdev->dev;

    // Verify mapping with test read
    uint32_t test_val = readl(base);
    dev_info(&pdev->dev, "ISP register mapping: base=%p test_read=0x%x\n",
             base, test_val);

    // Initialize device structures
    spin_lock_init(&ourISPdev->lock);

    // Initialize frame sources array
    for (int i = 0; i < MAX_FRAME_SOURCES; i++) {
        ourISPdev->frame_sources[i].chn_num = i;
        ourISPdev->frame_sources[i].state = 0;
        ourISPdev->frame_sources[i].is_open = 0;
        ourISPdev->frame_sources[i].private = NULL;
        init_waitqueue_head(&ourISPdev->frame_sources[i].wait);
    }

    // Allocate and initialize graph data
    graph_data = kzalloc(sizeof(*graph_data), GFP_KERNEL);
    if (!graph_data) {
        dev_err(&pdev->dev, "Failed to allocate graph data\n");
        ret = -ENOMEM;
        goto err_unmap;
    }

    // Initialize graph data
    graph_data->dev = ourISPdev;
    graph_data->dev_status.sensor_enabled = 0;
    memset(&graph_data->dev_status.fs_status, 0,
           sizeof(struct isp_framesource_status));

    global_graph_data = graph_data;

    // Initialize IRQ data first
    ourISPdev->irq_data = kzalloc(sizeof(struct isp_irq_data), GFP_KERNEL);
    if (!ourISPdev->irq_data) {
        ret = -ENOMEM;
        goto err_free_graph;
    }

    // Store IRQ numbers
    ourISPdev->irq = ISP_BASE_IRQ;      // 36
    ourISPdev->irq_m0 = ISP_M0_IRQ;     // 37
    ourISPdev->irq_w02 = ISP_W02_IRQ;   // 38

    // Request IRQ using irq_data structure
    ret = tx_isp_request_irq(pdev, ourISPdev->irq_data);
    if (ret) {
        dev_err(&pdev->dev, "Failed to request ISP IRQs\n");
        goto err_free_irq_data;
    }


    // Request each IRQ using irqdev directly
     ret = tx_isp_request_irq(pdev, ourISPdev->irq_data);
    if (ret) {
        dev_err(&pdev->dev, "Failed to request ISP IRQs\n");
        goto err_free_irq_data;
    }

    ourISPdev->irq_context = kzalloc(sizeof(struct isp_irq_context), GFP_KERNEL);
    if (!ourISPdev->irq_context) {
        ret = -ENOMEM;
        goto err_free_irq_data;
    }

    // Allocate and initialize IRQ handlers
    struct irq_handler_data *base_handler = kzalloc(sizeof(*base_handler), GFP_KERNEL);
    struct irq_handler_data *m0_handler = kzalloc(sizeof(*m0_handler), GFP_KERNEL);
    struct irq_handler_data *w02_handler = kzalloc(sizeof(*w02_handler), GFP_KERNEL);

    if (!base_handler || !m0_handler || !w02_handler) {
        ret = -ENOMEM;
        goto err_free_handlers;
    }

 // Initialize handler structures
    base_handler->task_list = NULL;
    base_handler->task_count = 0;
    base_handler->irq_number = 0;
    base_handler->handler_function = NULL;
    base_handler->disable_function = NULL;
    spin_lock_init(&base_handler->lock);

    m0_handler->task_list = NULL;
    m0_handler->task_count = 0;
    m0_handler->irq_number = 0;
    m0_handler->handler_function = NULL;
    m0_handler->disable_function = NULL;
    spin_lock_init(&m0_handler->lock);

    w02_handler->task_list = NULL;
    w02_handler->task_count = 0;
    w02_handler->irq_number = 0;
    w02_handler->handler_function = NULL;
    w02_handler->disable_function = NULL;
    spin_lock_init(&w02_handler->lock);
//
//    // Request IRQs
//    ret = tx_isp_request_irq(&pdev->dev, ISP_BASE_IRQ, base_handler, "isp-base");
//    if (ret) {
//        dev_err(&pdev->dev, "Failed to request base IRQ\n");
//        goto err_free_handlers;
//    }
//
//    ret = tx_isp_request_irq(&pdev->dev, ISP_M0_IRQ, m0_handler, "isp-m0");
//    if (ret) {
//        dev_err(&pdev->dev, "Failed to request M0 IRQ\n");
//        goto err_free_base_irq;
//    }
//
//    ret = tx_isp_request_irq(&pdev->dev, ISP_W02_IRQ, w02_handler, "isp-w02");
//    if (ret) {
//        dev_err(&pdev->dev, "Failed to request W02 IRQ\n");
//        goto err_free_m0_irq;
//    }

    // Store handlers in context
    ourISPdev->irq_context->base_handler = base_handler;
    ourISPdev->irq_context->m0_handler = m0_handler;
    ourISPdev->irq_context->w02_handler = w02_handler;

    pdev->dev.platform_data = &isp_pdata;

    // Configure clocks
    ret = configure_isp_clocks(ourISPdev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to configure ISP clocks: %d\n", ret);
        goto err_free_handlers;
    }

    platform_set_drvdata(pdev, ourISPdev);

    // Initialize reserved memory
    ret = init_isp_reserved_memory(pdev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to initialize reserved memory\n");
        goto err_cleanup_clocks;
    }

    ret = setup_isp_memory_regions(ourISPdev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to setup memory regions\n");
        goto err_cleanup_memory;
    }

    ret = setup_i2c_sensor(ourISPdev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to setup I2C sensor\n");
        goto err_cleanup_memory;
    }

    ret = create_isp_proc_entries(graph_data);
    if (ret) {
        dev_err(&pdev->dev, "Failed to create proc entries: %d\n", ret);
        goto err_cleanup_memory;
    }

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

    ret = create_framechan_devices(&pdev->dev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to create frame channels\n");
        goto err_cleanup_proc;
    }

    ret = misc_register(&isp_m0_miscdev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to register isp-m0 device\n");
        goto err_cleanup_proc;
    }

    dev_info(&pdev->dev, "TISP device probed successfully\n");
    return 0;

err_cleanup_proc:
    remove_isp_proc_entries();
err_cleanup_memory:
    cleanup_isp_memory(ourISPdev);
err_cleanup_clocks:
    cleanup_isp_clocks(ourISPdev);
err_free_handlers:
    kfree(w02_handler);
    kfree(m0_handler);
    kfree(base_handler);
    if (ourISPdev->irq_context)
        kfree(ourISPdev->irq_context);
err_free_irq_data:
    if (ourISPdev->irq_data)
        kfree(ourISPdev->irq_data);
err_free_graph:
    if (graph_data) {
        kfree(graph_data);
        global_graph_data = NULL;
    }
err_unmap:
    if (base)
        iounmap(base);
err_release_mem:
    release_mem_region(res->start, resource_size(res));
    return ret;
}

static int tisp_remove(struct platform_device *pdev)
{
    struct IMPISPDev *dev = platform_get_drvdata(pdev);

    if (dev) {
        // Free IRQ data first
        if (dev->irq_data) {
            tx_isp_free_irq(dev->irq_data);
            kfree(dev->irq_data);
            dev->irq_data = NULL;
        }

        // Unregister /dev/isp-m0 device
        misc_deregister(&isp_m0_miscdev);

        // Remove frame channel devices
        remove_framechan_devices();

        // Remove proc entries before cleaning up their data source
        remove_isp_proc_entries();

        // Clean up graph data
        if (global_graph_data) {
            kfree(global_graph_data);
            global_graph_data = NULL;
        }

        // Clean up device subdevices
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
        },
        {
            .start  = ISP_BASE_IRQ,    // 36
            .end    = ISP_BASE_IRQ,
            .flags  = IORESOURCE_IRQ,
            .name   = "isp-base-irq",
        },
        {
            .start  = ISP_M0_IRQ,      // 37
            .end    = ISP_M0_IRQ,
            .flags  = IORESOURCE_IRQ,
            .name   = "isp-m0-irq",
        },
        {
            .start  = ISP_W02_IRQ,     // 38
            .end    = ISP_W02_IRQ,
            .flags  = IORESOURCE_IRQ,
            .name   = "isp-w02-irq",
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
