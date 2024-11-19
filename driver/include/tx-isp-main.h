#ifndef TX_ISP_MAIN_H
#define TX_ISP_MAIN_H

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/videodev2.h>


// Add at the top of the file with other definitions
#define RMEM_START  0x2A80000          // Starting address of the reserved memory region
#define DRIVER_RESERVED_SIZE (4 * 1024 * 1024) // Size reserved for our driver: 4MB


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


// Main device structure
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
#define ISP_CTRL_ENABLE    BIT(0)    // Enable bit in control register
#define ISP_CTRL_RESET     BIT(1)    // Reset bit
#define ISP_CTRL_CAPTURE   BIT(2)    // Capture enable
#define RMEM_SIZE 22544384  // Match exact size LIBIMP expects

#define ISP_ALLOC_KMALLOC 1
#define ISP_ALLOC_CONTINUOUS 2

// Match libimp's DMA buffer layout
#define ISP_FRAME_BUFFER_OFFSET 0x1094d4  // From libimp
#define ISP_FRAME_BUFFER_ALIGN  0x1000
#define ISP_DMA_BUFFER_BASE    0x2a80000

// Add these defines to match pad expectations
#define PAD_TYPE_SOURCE  1
#define PAD_TYPE_SINK    2
#define PAD_FLAGS_BOTH   0x3
#define LINK_STATE_INACTIVE  0
#define LINK_STATE_SOURCE    3


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
#define VIDIOC_DQBUF _IOWR('V', 0x11, struct v4l2_buffer)
#define VIDIOC_CANCEL_READY_BUF _IOW('V', 0x6bf, int)

#define ISP_INT_FRAME_DONE  BIT(0)    // Frame complete
#define ISP_INT_DMA_ERR    BIT(1)    // DMA error
#define ISP_INT_BUF_FULL   BIT(2)    // Buffer full
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
#define ISP_CTRL_REG       0x100
#define ISP_STATUS_REG     0x104
#define ISP_INT_MASK_REG   0x108
#define ISP_INT_CLEAR_REG  0x10C
#define ISP_BUF0_OFFSET    0x1000
#define ISP_STREAM_CTRL    0x7838
#define ISP_STREAM_START   0x783c


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


// Add these defines at the top of the file
#define SENSOR_CMD_BASIC         0x2000000  // Register sensor
#define SENSOR_CMD_READ_ID       0x2000011  // Read sensor ID
#define SENSOR_CMD_WRITE_REG     0x2000012  // Write sensor register
#define SENSOR_CMD_READ_REG      0x2000013  // Read sensor register
#define SENSOR_CMD_SET_GAIN      0x2000005  // Set sensor gain
#define SENSOR_CMD_SET_EXP       0x2000006  // Set exposure time
#define SENSOR_CMD_STREAM_ON     0x2000007  // Start streaming
#define SENSOR_CMD_STREAM_OFF    0x2000008  // Stop streaming


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

// Add these defines
#define T31_CPM_BASE       0x10000000
#define T31_CPM_CLKGATE    0x20
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


// Update base address - it should be physical
#define ISP_PHYS_BASE     0x13300000
#define ISP_REG_SIZE      0x10000

#define ISP_IPU_CTRL     0x180
#define ISP_IPU_STATUS   0x184
#define ISP_MIPI_TIMING  0x38

/* ISP Control Register Bits */
#define ISP_CTRL_ENABLE          (1 << 0)  // Enable ISP core
#define ISP_CTRL_START_STREAM    (1 << 1)  // Start streaming
#define ISP_CTRL_IRQ_EN         (1 << 2)  // Enable interrupts

/* ISP Status/Interrupt bits */
#define ISP_INT_FRAME_DONE      (1 << 0)  // Frame complete
#define ISP_INT_ERR            (1 << 1)  // Error condition
#define ISP_INT_OVERFLOW       (1 << 2)  // Buffer overflow


#define VIDIOC_CREATE_ENCODER_GROUP 0x400456e0
#define VIDIOC_CREATE_ENCODER_CHN   0x400456e1
#define VIDIOC_REGISTER_ENCODER_CHN 0x400456e2


#define ISP_AE_STATS_OFFSET   0x1200  // Verify this offset matches your hardware
#define ISP_STAT_BASE         0x1000  // Base offset for statistics
#define ISP_AE_GRID_SIZE     15      // 15x15 zone grid


#define VIDIOC_INIT_ENCODER        0x40045690
#define VIDIOC_CREATE_ENCODER_CHN  0x40045691
#define VIDIOC_ENABLE_ENCODER_CHN  0x40045692

/* Add these state definitions */
// Core states (offset 0xe8)
#define ISP_STATE_INIT      1  // Initial state
#define ISP_STATE_READY     2  // Ready but not streaming
#define ISP_STATE_ENABLED   3  // Enabled
#define ISP_STATE_STREAMING 4  // Actively streaming

// VIC states (offset 0x128)
#define VIC_STATE_INIT      2  // Initial VIC state
#define VIC_STATE_ENABLED   3  // VIC enabled
#define VIC_STATE_STREAMING 4  // VIC streaming

#define SENSOR_NAME_SIZE 80
#define PADDING_SIZE_1 0x24
#define PADDING_SIZE_2 0x24

#define ISP_STATE_INVALID    0  // Initial invalid state
#define ISP_STATE_INIT      1  // After basic init
#define ISP_STATE_READY     2  // After resource setup
#define ISP_STATE_ENABLED   3  // After full initialization
#define ISP_STATE_STREAMING 4  // Actively streaming

// VIC states - match decompiled offsets
#define VIC_STATE_INVALID   0
#define VIC_STATE_INIT      2  // After VIC init
#define VIC_STATE_ENABLED   3  // VIC ready
#define VIC_STATE_STREAMING 4  // VIC streaming

// First add these to the header:
#define ISP_FW_STATE_INIT     0
#define ISP_FW_STATE_RUNNING  1
#define ISP_FW_STATE_STOPPED  2

#define ISP_PHYS_BASE       0x13300000
#define ISP_REG_SIZE        0x10000

#define ISP_CTRL_OFFSET     0x100
#define ISP_STATUS_OFFSET   0x104
#define ISP_INT_MASK        0x108
#define ISP_INT_CLEAR      0x10C

#define ISP_CSI_OFFSET     0xb8
#define ISP_CSI_SIZE       0x100
#define V4L2_CID_ISP_CONTRAST    (V4L2_CID_PRIVATE_BASE + 0)
#define V4L2_CID_ISP_SHARPNESS   (V4L2_CID_PRIVATE_BASE + 1)
#define V4L2_CID_ISP_SATURATION  (V4L2_CID_PRIVATE_BASE + 2)
#define V4L2_CID_ISP_BRIGHTNESS  (V4L2_CID_PRIVATE_BASE + 3)
#define V4L2_CID_ISP_HFLIP      (V4L2_CID_PRIVATE_BASE + 4)
#define V4L2_CID_ISP_VFLIP      (V4L2_CID_PRIVATE_BASE + 5)
#define VIDIOC_SET_STREAM_FMT   0xc07056c3

/* ISP Image Control Register Offsets */
#define ISP_BRIGHT_OFFSET     0x1100  // Brightness control
#define ISP_CONTRAST_OFFSET   0x1104  // Contrast control
#define ISP_SAT_OFFSET       0x1108  // Saturation control
#define ISP_SHARP_OFFSET     0x110C  // Sharpness control
#define ISP_HFLIP_OFFSET     0x1110  // Horizontal flip
#define ISP_VFLIP_OFFSET     0x1114  // Vertical flip
#define ISP_TUNING_CONTRAST    0x980901
#define ISP_TUNING_SATURATION  0x980902
#define ISP_TUNING_SHARPNESS   0x98091b
// Register offsets based on T31 ISP
#define ISP_BASE_TUNING     0x1100   // Base offset for tuning registers

// Control register offsets from base
#define ISP_REG_CONTRAST    (ISP_BASE_TUNING + 0x00)
#define ISP_REG_SATURATION  (ISP_BASE_TUNING + 0x04)
#define ISP_REG_SHARPNESS   (ISP_BASE_TUNING + 0x08)

#define ISP_TUNING_BYPASS       0x980010
#define ISP_TUNING_RUNNING_MODE 0x980011
#define ISP_TUNING_AE_COMP     0x980020
#define ISP_TUNING_MAX_AGAIN   0x980021
#define ISP_TUNING_MAX_DGAIN   0x980022
#define ISP_TUNING_DPC         0x980030
#define ISP_TUNING_DRC         0x980031
#define ISP_TUNING_DEFOG       0x980032

#define ISP_STATS_AE  (1 << 0)
#define ISP_STATS_AWB (1 << 1)

#define CH_STATE_INIT      0
#define CH_STATE_READY     1
#define CH_STATE_STREAMING 2

#define ISP_BUF1_OFFSET      0x40  // Output buffer registers
#define ISP_CTRL_ENABLE      0x1
#define ISP_CTRL_START       0x2
#define ISP_CTRL_IRQ_ENABLE  0x4
#define ISP_INT_FRAME_DONE   0x1
#define ISP_INT_ERR         0x2
#define ISP_INT_OVERFLOW    0x4
#define ISP_FMT_RAW10       0x2B
#define ISP_FMT_NV12        0x3231564e

#define ISP_BUF0_OFFSET     0x200  // First buffer registers
#define ISP_BUF1_OFFSET     0x220  // Second buffer registers
#define ISP_BUF_SIZE_STEP   0x20   // Size of each buffer register block

#define ISP_FORMAT_REG      0x10C  // Format control register
#define ISP_FMT_RAW10      0x2B    // RAW10 format value
#define ISP_FMT_NV12       0x3231564e // NV12 format value

#define ISP_CSI_PHY_READY   0x4
#define ISP_CSI_DATA_READY  0x100

// Register offsets
#define CSI_VERSION_REG     0x00
#define CSI_N_LANES_REG     0x04
#define CSI_CTRL0_REG       0x08
#define CSI_CTRL1_REG       0x0C
#define CSI_CTRL2_REG       0x10
#define CSI_DPHY_CTRL       0x14
#define CSI_DATA_IDS0       0x18
#define CSI_DATA_IDS1       0x1C
#define CSI_ERR1            0x20
#define CSI_ERR2            0x24
#define CSI_MASK1           0x28
#define CSI_MASK2           0x2C

// Add these defines at the top
#define ISP_BUF0_OFFSET     0x200   // RAW10 input buffer
#define ISP_BUF1_OFFSET     0x220   // Y buffer
#define ISP_BUF2_OFFSET     0x240   // UV buffer
#define ISP_BUF_SIZE_STEP   0x20    // Buffer register block size

#define ISP_FORMAT_REG      0x10C   // Format control register
#define ISP_INPUT_FMT_REG   0x110   // Input format register
#define ISP_OUTPUT_FMT_REG  0x114   // Output format register

#define ISP_CTRL_PATH_REG   0x140   // Path control register
#define ISP_BYPASS_REG      0x144   // Bypass control register

#define ISP_FRAME_WIDTH     0x100   // Frame width register
#define ISP_FRAME_HEIGHT    0x104   // Frame height register

#define V4L2_BUF_TYPE_VIDEO_CAPTURE      1
#define V4L2_MEMORY_MMAP                 1
#define V4L2_FIELD_NONE                  0
#define V4L2_BUF_FLAG_DONE               0x00000001


#define ISP_CTRL_REG         0x00
#define ISP_STATUS_REG       0x04
#define ISP_INT_MASK_REG     0x08
#define ISP_INT_CLEAR_REG    0x0C
#define ISP_STREAM_CTRL      0x10
#define ISP_STREAM_START     0x14
#define ISP_DMA_STATUS       0x18

#define ISP_INT_FRAME_DONE   (1 << 0)
#define ISP_INT_ERR         (1 << 1)
#define ISP_INT_DMA_DONE    (1 << 2)

#define ISP_DMA_BUSY        (1 << 0)
#define ISP_DMA_CTRL         0x128

#define ISP_BUF0_OFFSET     0x100  // Input buffer registers
#define ISP_BUF1_OFFSET     0x110  // Y buffer registers
#define ISP_BUF2_OFFSET     0x120  // UV buffer registers
#define ISP_DMA_CTRL        0x128  // DMA control register
#define ISP_CTRL_PATH_REG   0x140  // Path control
#define ISP_BYPASS_REG      0x144  // Bypass control
#define ISP_FRAME_WIDTH     0x148  // Frame width
#define ISP_FRAME_HEIGHT    0x14C  // Frame height
#define ISP_INPUT_FMT_REG   0x150  // Input format
#define ISP_OUTPUT_FMT_REG  0x154  // Output format

#define ISP_FMT_RAW10       0x2B   // RAW10 format code
#define ISP_FMT_NV12        0x3231564E  // NV12 format code

#define ISP_INT_FRAME_DONE   (1 << 0)
#define ISP_INT_ERR         (1 << 1)
#define ISP_INT_DROP        (1 << 2)
#define ISP_INT_OVERFLOW    (1 << 3)
#define ISP_INT_AE_DONE     (1 << 4)

/* ISP Pipeline Register Structure */
struct isp_pipeline_regs {
    u32 bypass_ctrl;     // 0x1140
    u32 running_mode;    // 0x1144
    u32 ae_comp;         // 0x1148
    u32 max_again;       // 0x114C
    u32 max_dgain;       // 0x1150
    u32 dpc_strength;    // 0x1154
    u32 drc_strength;    // 0x1158
    u32 defog_strength;  // 0x115C
} __attribute__((packed, aligned(4)));

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

// Define a generic callback type for show functions
typedef int (*show_func_t)(struct seq_file *, void *);

// Structure to pass both function and data
struct proc_data {
    show_func_t show_func;
    void *private_data;
};


// Add frame grabbing thread
struct isp_frame_state {
    uint32_t state;          // 0x318 offset - frame state
    uint32_t format;         // 0x68 offset - frame format
    void *buffer_addr;       // 0x31c offset - target buffer
    uint32_t width;
    uint32_t height;
    uint8_t is_buffer_full;
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


struct isp_memory_info {
    void *virt_addr;         // Kernel virtual address
    dma_addr_t phys_addr;    // Physical/DMA address
    size_t size;             // Size of allocation
    bool initialized;
};

static struct isp_memory_info isp_mem = {0};

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


/* Structure definitions for IOCTL parameters */
struct isp_rotation_params {
    int angle;          // Rotation angle (0, 90, 180, 270)
    int rot_height;     // Output height after rotation
    int rot_width;      // Output width after rotation
    unsigned int flags; // Reserved for future use
};

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


struct isp_framesource_state {
    uint32_t magic;          // Magic identifier
    uint32_t flags;          // State flags
    uint32_t chn_num;        // Channel number
    uint32_t state;          // State (1=ready, 2=streaming)
    uint32_t width;          // Frame width
    uint32_t height;         // Frame height
    uint32_t fmt;            // Pixel format

    // Memory management
    uint32_t buf_cnt __aligned(8);   // Keep this alignment
    uint32_t buf_flags;      // Buffer flags
    dma_addr_t dma_addr __aligned(4);
    void *buf_base __aligned(4);
    uint32_t buf_size __aligned(4);

    // Frame management
    uint32_t frame_cnt;      // Frame counter
    uint32_t buf_index;      // Current buffer index
    uint32_t write_idx;      // Write index
    struct file *fd;         // File pointer
    struct video_device *vdev;
    struct task_struct *thread;
    void    *ext_buffer;     // Extended buffer pointer
    struct isp_buffer_info *bufs;  // Buffer info array
    struct mutex lock __aligned(4);  // Keep this alignment
    void    *private;        // Private data pointer

    // FIFO management
    struct list_head ready_queue;
    struct list_head done_queue;
    int fifo_depth;
    int frame_depth;
    bool fifo_initialized;

    // Synchronization
    struct semaphore sem;
    wait_queue_head_t wait;
    int is_open;            // 1 = initialized

    // Scale/crop stored in flags:
    // 0x1 = Enabled
    // 0x2 = Crop enabled
    // 0x4 = Scale enabled
    uint32_t x_offset;       // Replace config struct with direct members
    uint32_t y_offset;
    uint32_t crop_width;
    uint32_t crop_height;
    uint32_t scale_in_width;
    uint32_t scale_in_height;
    uint32_t scale_out_width;
    uint32_t scale_out_height;
} __attribute__((aligned(8)));


struct ae_zone {
    uint32_t mean;       // Mean luminance
    uint32_t var;        // Variance
    uint32_t weight;     // Zone weight
};
struct ae_zone_stats {
    struct ae_zone zones[ISP_AE_GRID_SIZE][ISP_AE_GRID_SIZE];
    uint32_t frame_count;
};

#define MAX_EVENTS 16

// AE (Auto Exposure) info structure
struct ae_info {
    uint32_t gain;
    uint32_t exposure;
    uint32_t flags;
    // Add other AE parameters as needed
};

// AWB (Auto White Balance) info structure
struct awb_info {
    uint32_t gain;
    uint32_t exposure;
    uint32_t color_temp;
    // Add other AWB parameters as needed
};

// Event handling structure
struct isp_events {
    void (*handlers[MAX_EVENTS])(void);
    spinlock_t lock;
};

struct isp_event_callback {
    void (*callback)(void *);  // Offset 0x1c for frame callbacks
    void (*stats_cb)(void *);  // Offset 0x8 for stats callbacks
    void *priv;
    struct list_head list;
};

struct isp_event_handle {
    struct list_head callbacks;  // At offset 0xc4 in device structure
    spinlock_t lock;
    atomic_t enabled;
};



struct our_tx_isp_subdev_padding {
    unsigned int flags;        // Pad flags
    struct list_head list;     // List for managing pads
    void *priv;               // Private data
    struct isp_event_handle events;  // Event handling for this pad
};


struct isp_stats_work {
    struct work_struct work;
    u32 stats_mask;  // Indicates which stats to collect
};


struct device_list_entry {
    void *device;           // Device pointer
    struct callback_data {
        void *frame_cb;     // Offset 0x1c
        void *stats_cb;     // Offset 0x8
    } *callback_data;       // At offset 0xc4
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
    uint32_t cpu_id;
    uint32_t soc_id;

    // Critical offsets - must match prudynt
    struct sensor_buffer_info *buf_info;  // 0xAC: Buffer info
    int wdr_mode;                        // 0xB0: WDR mode
    struct sensor_control_info *wdr_buf_info; // 0xB4: WDR buffer info
    unsigned int current_link;            // Track link config
    char padding2[PADDING_SIZE_2];        // Additional required padding

    // Device and hardware access - after aligned section
    struct device *dev;
    void __iomem *regs __aligned(4);
    void __iomem *ctrl_regs __aligned(4);             // Control registers
    int irq;
    void (*irq_handlers[16])(void);  // Array of IRQ handlers
    struct i2c_client *sensor_i2c_client;
    struct irq_handler_data *irq_data;
    struct IspDevice *isp_dev;
    // Frame dimensions
    unsigned int width;
    unsigned int height;

    // Algorithm info
    struct ae_info ae_info;
    struct awb_info awb_info;

    // Event system
    struct isp_events events;

    // DMA info
    dma_addr_t dma_addr __aligned(4);
    size_t dma_size __aligned(4);
    void *dma_buf;

    // Runtime state
    struct clk **clocks;
    int num_clocks;
    bool memory_initialized;
    struct isp_framesource_state *fs_info;  // Add this field
    uint32_t format;                        // Add this field

    // Critical state tracking - match decompiled offsets
    spinlock_t state_lock;      // At 0xdc
    atomic_t core_state;        // At 0xe8 - core state
    atomic_t vic_state;         // At 0x128 - VIC state

    // Counters at exact offsets
    u32 frame_count;            // At 0x160
    u32 error_count;            // At 0x164
    u32 overflow_count;         // At 0x168
    u32 dropped_frames;         // At 0x170
    bool irq_enabled;           // Track IRQ state

    // Firmware thread control
    struct task_struct *fw_thread;    // Thread pointer
    atomic_t fw_state;                // Thread state
    wait_queue_head_t fw_wq;          // Wait queue
    spinlock_t fw_lock;               // Thread lock

    void __iomem *csi_base;  // Separate CSI register base
    struct clk **subdev_clks;  // field_bc in IspSubdev
    int num_clks;              // field_c0 in IspSubdev

    /* VIC support */
    void __iomem *vic_regs;     // VIC register base
    spinlock_t vic_lock;        // VIC register lock

    // VIC/Core state
    bool vic_started;
    bool core_started;

    /* device list */
    struct device_list_entry devices[16];
    wait_queue_head_t stats_wait;  // Wait queue for stats
    struct our_tx_isp_subdev_padding pads[2];  // Add as fixed array - matches 0x38 offset access

    /* Tuning support */
    void *tuning_data;         // Buffer for tuning data (0x1c bytes)
    void __iomem *tuning_regs; // Tuning registers mapping
    int tuning_state;          // Tuning state (matches offset 0xa8)
    bool tuning_enabled;       // Tuning enabled flag

    /* GPIO control */
    int reset_gpio;                   // Reset GPIO number
    int pwdn_gpio;                    // Power down GPIO number (optional)
    bool reset_active_low;            // Reset GPIO polarity
    struct gpio_desc *reset_gpio_desc; // GPIO descriptor for reset
    struct gpio_desc *pwdn_gpio_desc;  // GPIO descriptor for power down

    // Parameter regions
    void __iomem *isp_params;
    void __iomem *wdr_params;
    struct clk *isp_clk;       // ISP core clock
    struct clk *cgu_isp_clk;   // CGU ISP clock
    struct clk *csi_clk;       // CSI clock
    struct clk *ipu_clk;       // IPU clock
    struct isp_framesource_state frame_sources[MAX_CHANNELS];
    struct tx_isp_sensor_win_setting *sensor_window_size;
    // Encoder state
    uint8_t encoder_registered;   // Match 0x109398 offset from decompiled
    struct semaphore frame_sem;   // Match 0x109428 offset from decompiled
    struct ae_zone_stats ae_stats;
    spinlock_t ae_lock;
    bool ae_valid;

    struct isp_event_handle event_handle;
    struct isp_stats_work stats_work;
    struct workqueue_struct *stats_wq;
    spinlock_t stats_lock;
    bool stats_pending;

    atomic_t frame_cnt;       // Replace u32 frame_count with atomic_t
    atomic_t csi_error_cnt;   // Use atomic for error counting too

    /* CSI monitoring support */
    struct workqueue_struct *csi_check_wq;
    struct work_struct csi_check_work;
    atomic_t csi_monitor_enabled;
    spinlock_t csi_lock;
    u32 csi_error_count;
} __attribute__((packed, aligned(4)));



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
} __attribute__((packed, aligned(4)));


struct isp_reg_block_aligned {
    u32 ctrl;          // 0x100
    u32 status;        // 0x104
    u32 int_mask;      // 0x108
    u32 int_clear;     // 0x10C
    u32 reserved[4];   // Padding to 0x120
    u32 config[4];     // 0x120-0x130
} __attribute__((packed, aligned(4)));

struct isp_buffer_block {
    u32 base_addr;     // Base address
    u32 size;          // Size
    u32 stride;        // Stride
    u32 reserved;      // Keep alignment
} __attribute__((packed, aligned(4)));


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


#define MAX_TASKS 7  // Match the loop count from decompiled code

struct irq_handler_data {
    spinlock_t lock;  // Change from rlock to lock
    uint32_t irq_number;
    struct irq_task task_list[MAX_TASKS];
    void (*handler_function)(struct irq_handler_data *);
    void (*disable_function)(struct irq_handler_data *);
    int task_count;
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
    char sensor_name[80];  // Can hold the "sc2336" sensor name
    int num_sensors;       // Number of currently registered sensors
};



#define MAX_SENSORS 1

struct sensor_info {
    char name[32];
    bool registered;
};

/**
 * struct isp_link_config - Link configuration data from decompiled
 * Based on the array at offset 0x6ad7c and 0x6ad80 in decompiled
 */
struct isp_link_config {
    struct isp_pad_desc *pads;
    int num_pads;
};

// Add these structures to match libimp's buffer management
struct frame_node {
    uint32_t magic;             // 0x0: Magic number identifier
    void *data;                 // 0x4: Buffer data pointer
    uint32_t frame_size;        // 0x8: Frame data size
    uint32_t seq;              // 0xc: Frame sequence number
    struct list_head list;      // 0x10: List management
    uint32_t flags;            // 0x20: Frame flags
    uint8_t state;             // 0x24: Frame state
};

// Must match OEM layout exactly
struct frame_queue {
    struct frame_node *frames;      // Array of frame nodes
    spinlock_t lock;                // Queue lock
    struct list_head ready_list;    // List of ready frames
    struct list_head done_list;     // List of completed frames
    wait_queue_head_t wait;         // Wait queue
    atomic_t frame_count;           // Count of frames
    uint32_t max_frames;            // Maximum frames
    uint32_t write_idx;             // Current write index
    uint32_t read_idx;              // Current read index
};

struct encoder_chn_attr {
    uint32_t picWidth;          // Picture width
    uint32_t picHeight;         // Picture height
    uint32_t encType;           // Encoding type (H.264/H.265)
    uint32_t profile;           // Encoding profile
    uint32_t bufSize;           // Intermediate buffer size
    uint32_t frameRate;         // Frame rate numerator
    uint32_t rcMode;            // Rate control mode
    uint32_t maxGop;            // Maximum GOP size
    uint32_t maxQp;             // Maximum QP value
    uint32_t minQp;             // Minimum QP value
};



struct encoder_state {
    uint32_t channel_id;   // Channel ID (0-8)
    uint32_t state;        // Must match offset 0x109290
    uint32_t registered;   // Must be at 0x109398
    struct semaphore sem;  // Must be at 0x109428
};

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
    struct frame_queue frame_queue;

    // FIFO configuration
    uint32_t fifo_depth;
    uint32_t fifo_thresh;
    uint32_t fifo_flags;

    // Frame completion tracking
    atomic_t frames_completed;
    atomic_t buffer_index;
    spinlock_t state_lock;
    unsigned long buffer_states;

    // Add padding to reach VBM table offset
    uint8_t vbm_padding[0x109080 - 0x200];  // Adjust size based on previous members

    // VBM table at exact offset
    void *vbm_table[16];

    // Encoder state with fixed offsets as individual fields
    uint8_t enc_padding[0x109290 - (0x109080 + (16 * sizeof(void*)))];
    uint32_t enc_state_val;          // 0x109290 - was enc_state.state
    uint8_t enc_pad[0x108];          // Padding to 0x109398
    uint32_t enc_registered_val;      // 0x109398 - was enc_state.registered
    uint8_t sem_pad[0x30];           // Padding to 0x109428
    struct semaphore enc_sem;         // 0x109428 - was enc_state.sem

    // AE stats at the end
    struct ae_zone_stats ae_stats;
    spinlock_t ae_lock;
    bool ae_valid;
    // Debug statistics
    atomic_t frames_received;
    atomic_t frames_dropped;
    atomic_t buffer_overruns;
    u64 last_frame_time;
    u32 min_frame_interval;
    u32 max_frame_interval;

    // Frame queue
    struct {
        struct frame_node *frames;
        spinlock_t lock;
        struct list_head ready_list;
        struct list_head done_list;
        wait_queue_head_t wait;
        atomic_t frame_count;
        uint32_t max_frames;
        uint32_t write_idx;
        uint32_t read_idx;
    } queue;
} __attribute__((aligned(32)));  // 32-byte alignment for MIPS


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

struct encoder_group_param {
    uint32_t max_group_num;       // Maximum number of groups
    uint32_t max_chn_num;         // Maximum channels per group
    uint32_t pic_width;           // Picture width
    uint32_t pic_height;          // Picture height
    uint32_t stream_type;         // Stream type (0 = mainstream, 1 = substream)
};

// In your streaming setup:
struct encoder_group_param group_param = {
    .max_group_num = 1,
    .max_chn_num = 2,            // For both mainstream and substream
    .pic_width = 1920,           // Main stream width
    .pic_height = 1080,          // Main stream height
    .stream_type = 0,            // Main stream
};

struct encoder_chn_param {
    uint32_t chn_id;             // Channel ID
    uint32_t chn_type;           // Channel type (H.264/H.265)
    uint32_t pic_width;          // Picture width
    uint32_t pic_height;         // Picture height
    uint32_t buf_size;           // Buffer size
    uint32_t frame_rate;         // Frame rate
    uint32_t bit_rate;           // Bit rate
    uint32_t gop;                // GOP size
};

struct encoder_chn_param chn_param = {
    .chn_id = 0,                // First channel
    .chn_type = 2,              // H.264
    .pic_width = 1920,
    .pic_height = 1080,
    .buf_size = 1920 * 1080 * 3 / 2,  // NV12 size
    .frame_rate = 25,
    .bit_rate = 2000000,        // 2 Mbps
    .gop = 25,                  // 1 second GOP at 25fps
};

struct encoder_reg_param {
    uint32_t chn_id;            // Channel ID to register
    uint32_t group_id;          // Group ID this channel belongs to
    uint32_t enable;            // 1 to enable
};

struct encoder_reg_param reg_param = {
    .chn_id = 0,
    .group_id = 0,
    .enable = 1,
};


struct isp_stream_param {
    uint32_t enable;     // Stream enable/disable
    uint32_t channel;    // Channel number
    uint32_t flags;      // Stream flags
};


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


struct frame_entry { // Must match OEM offsets exactly
    uint32_t flags;          // +0x00
    uint32_t timestamp;      // +0x04
    uint32_t frame_size;     // +0x08
    uint32_t frame_type;     // +0x0C
    uint32_t frame_num;      // +0x10
    uint32_t frame_rate;     // +0x14
    uint32_t num_slices;     // +0x18
    void *slice_data;        // +0x1C
    uint8_t reserved[0x308 - 0x20];  // Pad to full size
} __attribute__((aligned(8)));

struct clock_config {
    const char *name;
};

static const struct clock_config csi_clocks[] = {
    { "csi" },  // Only get the main CSI clock
    { "cgu_isp" },
    { "vpu" },
    { "csi_phy" },
    { NULL }    // Terminator
};

#endif
