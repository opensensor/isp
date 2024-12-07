#ifndef TX_ISP_MAIN_H
#define TX_ISP_MAIN_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/videodev2.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <media/videobuf2-core.h>
#include <media/v4l2-dev.h>
#include <media/v4l2-device.h>
#include <media/v4l2-subdev.h>

#include <tx-isp-device.h>
#include <tx-libimp.h>



// Add at the top of the file with other definitions
#define RMEM_START  0x2A80000          // Starting address of the reserved memory region
#define DRIVER_RESERVED_SIZE (4 * 1024 * 1024) // Size reserved for our driver: 4MB

#define MAX_VIDEO_BUFFERS 32  // Typical max buffer count
#define FRAME_TIMING_SIZE 16  // Size of timing data per buffer

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
#define ISP_BYPASS_CTRL  (ISP_REG_BASE + 0x15c)

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
// #define ISP_BYPASS_CTRL   0x140    // Bypass control register
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
#define VIDIOC_SET_FRAME_DEPTH 0xc0145608
#define VIDIOC_SET_FIFO_ATTR 0xc044560f
#define VIDIOC_STREAM_ON  0x80045612
#define VIDIOC_GET_BUFFER_INFO 0x400456bf
#define VIDIOC_SET_FRAME_MODE 0xc0445611
#define VIDIOC_CANCEL_READY_BUF _IOW('V', 0x6bf, int)
#define VIDIOC_SET_FRAME_DEPTH    0xc0145608
#define VIDIOC_SET_FIFO_ATTR      0xc044560f
#define VIDIOC_QBUF              0xc0445610  // Queue buffer
#define VIDIOC_DQBUF             0x400456bf   // Dequeue buffer
#define VIDIOC_S_FMT             0xc0445611  // Dequeue buffer
#define VIDIOC_STREAM_ON          0x80045612
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

#define ISP_CTRL_OFFSET    0x1104
#define ISP_STAT_OFFSET    0x0100
#define ISP_CONF_OFFSET    0x0130
#define ISP_INIT_OFFSET    0x0118

#define MAX_CHANNELS 6  // Define the maximum number of frame channels
#define MAX_COMPONENTS 8 // Define the maximum number of components


// Add these defines at the top of the file
#define SENSOR_CMD_BASIC         0x2000000  // Register sensor
#define SENSOR_CMD_READ_ID       0x2000011  // Read sensor ID
#define SENSOR_CMD_WRITE_REG     0x2000012  // Write sensor register
#define SENSOR_CMD_READ_REG      0x2000013  // Read sensor register
#define SENSOR_CMD_SET_GAIN      0x2000005  // Set sensor gain
#define SENSOR_CMD_SET_EXP       0x2000006  // Set exposure time
#define SENSOR_CMD_STREAM_ON     0x2000007  // Start streaming
#define SENSOR_CMD_STREAM_OFF    0x2000008  // Stop streaming


// Add these defines at the top
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

#define ISP_CTRL_OFFSET     0x100
#define ISP_STATUS_OFFSET   0x104
#define ISP_INT_MASK        0x108
#define ISP_INT_CLEAR      0x10C

#define ISP_CSI_OFFSET     0x60000
#define ISP_CSI_SIZE       0x1000
#define CSI_REG_SIZE     0x1000

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


#define MAX_ERRORS 10  // Maximum allowed errors before reset
/* ISP Interrupt flags - match hardware register bits */
#define ISP_INT_FRAME_DONE    (1 << 0)    // Frame completed
#define ISP_INT_DMA_START     (1 << 1)    // DMA transfer started
#define ISP_INT_ERR          (1 << 8)    // General error
#define ISP_INT_OVERFLOW     (1 << 9)    // Buffer overflow
#define ISP_INT_CSI_ERR      (1 << 10)   // CSI interface error

/* Register offsets */
#define ISP_STATUS_REG        0x40c4    // Status register
#define ISP_INT_MASK_REG      0x40c8    // Interrupt mask
#define ISP_INT_CLEAR_REG     0x40cc    // Interrupt clear

/* T31 ISP Interrupt Flags */
#define ISP_INT_FRAME_DONE    (1 << 0)  // Frame complete
#define ISP_INT_SIZE_ERR      (1 << 8)  // Size error
#define ISP_INT_FORMAT_ERR    (1 << 9)  // Format error
#define ISP_INT_OVERFLOW      (1 << 10) // Overflow error

/* Additional ISP Control/Status registers for T31 */
#define ISP_CTRL_REG          0x40c0
#define ISP_STATUS_REG        0x40c4
#define ISP_INT_MASK_REG      0x40c8
#define ISP_INT_CLEAR_REG     0x40cc
#define ISP_CSI_CTRL          0x40d0
#define ISP_CSI_STATE         0x40d4

#define ISP_CSI_DPHY_CTRL     0x40d8  // DPHY control
#define ISP_CSI_LANE_CTRL     0x40dc  // Lane control
#define ISP_CSI_TIMING        0x40e0  // CSI timing

/* CSI PHY registers */
#define CSI_PHY_TST_CTRL0     0x30    // PHY test control 0
#define CSI_PHY_TST_CTRL1     0x34    // PHY test control 1
#define CSI_PHY_LANE_ENABLE   0x38    // Lane enable

#define VBM_MAX_POOLS 4  // Match maximum number of buffers
#define ISP_CTRL_REG        0x100  // Base control register
#define ISP_STATUS_REG      0x104  // Status register
#define ISP_INT_MASK_REG    0x108  // Interrupt mask
#define ISP_INT_CLEAR_REG   0x10C  // Interrupt clear
#define ISP_IRQ_EN_REG      0x110  // IRQ enable register

// Interrupt bits
#define ISP_INT_FRAME_DONE  BIT(0)  // Frame complete
#define ISP_INT_ERR         BIT(1)  // Error condition
#define ISP_INT_OVERFLOW    BIT(2)  // Buffer overflow

// ISP Register offsets from T31 datasheet/reference code
#define ISP_CTRL_OFFSET     0x00   // Base control register
#define ISP_STATUS_OFFSET   0x04   // Status register
#define ISP_INT_CLR_OFFSET  0x0C   // Interrupt clear
#define ISP_IRQ_EN_OFFSET   0x10   // IRQ enable

// Interrupt definitions
#define ISP_INT_FRAME_DONE  BIT(0)
#define ISP_INT_ERR         BIT(1)
#define ISP_INT_OVERFLOW    BIT(2)
#define ISP_INT_ALL         (ISP_INT_FRAME_DONE | ISP_INT_ERR | ISP_INT_OVERFLOW)

#define ISP_CTRL_OFFSET     0x00
#define ISP_STATUS_OFFSET   0x04
#define ISP_INT_CLR_OFFSET  0x0C

#define ISP_IRQ_ENABLE_BIT  BIT(0)
#define ISP_INT_FRAME_DONE  BIT(0)
#define ISP_INT_ERR         BIT(1)
#define ISP_INT_OVERFLOW    BIT(2)
#define ISP_CTRL_DEFAULT   0x54560031  // Keep this value
#define ISP_FRAME_DONE     BIT(0)      // Only enable frame done interrupt

#define ISP_IRQ_FLAGS (IRQF_SHARED | IRQF_TRIGGER_HIGH)
#define ISP_INT_FRAME_DONE  (1 << 0)
#define ISP_INT_ERR        (1 << 1)
#define ISP_INT_OVERFLOW   (1 << 2)
#define ISP_INT_MASK_ALL   (ISP_INT_FRAME_DONE | ISP_INT_ERR | ISP_INT_OVERFLOW)

#define ISP_CTRL_OFFSET     0x00
#define ISP_IRQ_EN_OFFSET   0x04
#define ISP_INT_CLR_OFFSET  0x0C
#define ISP_INT_MASK_REG    0x08
#define ISP_INT_CLEAR_REG   0x0C
// Add to register definitions
#define ISP_STATUS_REG         0x04  // Status register offset
#define ISP_STATUS_FRAME_DONE  ISP_INT_FRAME_DONE  // Use same bit position

// Base register offsets from regs
#define ISP_BASE            0x0000   // ISP core registers base

// ISP Core register offsets (from ISP_BASE)
#define ISP_CTRL_OFFSET     0x0000   // Control register
#define ISP_STATUS_OFFSET   0x0004   // Status register
#define ISP_INT_CLR_OFFSET  0x000C   // Interrupt clear
#define ISP_STREAM_CTRL     0x0010   // Stream control
#define ISP_STREAM_START    0x0014   // Stream start

// Base register offsets from regs
#define VIC_BASE            0x7000   // VIC registers base

#define CSI_IOCTL_INIT    0x200000c  // Initializes CSI
#define CSI_IOCTL_STATE3  0x200000e  // Sets state to 3
#define CSI_IOCTL_STATE4  0x200000f  // Sets state to 4

#define CSI_PHY_BASE    0x10022000
#define CSI_PHY_SIZE    0x1000
// Map CSI registers at correct offset from ISP base
#define CSI_REG_SIZE    0x1000      // Cover full register range

#define CSI_OFFSET      0x60000     // CSI offset from ISP base
#define CSI_REG_SIZE    0x1000      // CSI register block size

// CSI register offsets
#define CSI_VERSION     0x00
#define CSI_N_LANES     0x04
#define CSI_PHY_SHUTDOWNZ 0x08
#define CSI_DPHY_RSTZ   0x0c
#define CSI_CSI2_RESETN 0x10
#define CSI_PHY_STATE   0x14
#define CSI_DATA_IDS_1  0x18
#define CSI_ERR1        0x20
#define CSI_ERR2        0x24
#define CSI_MASK1       0x28
#define CSI_MASK2       0x2c

#define ISP_BASE_ADDR    0x13300000
#define CSI_REG_SIZE     0x1000

// Add more precise CSI control registers
#define CPM_BASE         0x10000000
#define CPM_CLKGR        0x20
#define CPM_CLKGR1       0x28
#define CPM_CSI_CTRL     0xb8    // CSI clock control
#define CPM_CSI_CFG      0xbc    // CSI configuration
#define CPM_CSI_SRST     0x2c    // CSI reset control
#define CPM_CSI_PWR      0x34    // CSI power domain

#define CPM_BASE         0x10000000
#define CPM_CLKGR        0x20    // Clock gate register
#define CPM_CLKGR1       0x28    // Clock gate register 1
#define CPM_SRSR         0xc4    // Software reset status register
#define CPM_SRBC         0xc8    // Soft reset bus control
#define CPM_SPCR0        0xb8    // Special purpose clock register 0
#define CPM_CSICDR       0x88    // CSI clock divider register
#define CPM_PCSR         0x2c    // Power control status register

// CSI specific bits
#define CPM_CSI_CLK_MASK    (1 << 23)  // CSI clock gate in CLKGR
#define CPM_CSI_PWR_MASK    (1 << 2)   // CSI power domain in CLKGR1
#define CPM_CSI_RST_MASK    (1 << 20)  // CSI reset bit in SRSR
#define CSI_CLK_MASK     (1 << 23)  // CSI clock gate
#define CSI_PWR_MASK     (1 << 2)   // CSI power domain
#define CSI_SRST_MASK    (1 << 20)  // CSI soft reset

#define CPM_BASE         0x10000000
#define CPM_CLKGR        0x20    // Clock gate register
#define CPM_CLKGR1       0x28    // Clock gate register 1
#define CPM_SRSR         0xc4    // Software reset status register
#define CPM_SRBC         0xc8    // Soft reset bus control
#define CPM_SPCR0        0xb8    // Special purpose clock register 0
#define CPM_CSICDR       0x88    // CSI clock divider register
#define CPM_PCSR         0x2c    // Power control status register

// CSI specific bits
#define CPM_CSI_CLK_MASK    (1 << 23)  // CSI clock gate in CLKGR
#define CPM_CSI_PWR_MASK    (1 << 2)   // CSI power domain in CLKGR1
#define CPM_CSI_RST_MASK    (1 << 20)  // CSI reset bit in SRSR

#define ISP_BASE_ADDR    0x13300000  // Main ISP
#define MIPI_PHY_ADDR    0x10022000  // MIPI PHY
#define ISP_DEVICE_ADDR  0x10023000  // ISP device

#define ISP_REG_SIZE     0x10000     // 64KB for main ISP
#define CSI_REG_SIZE     0x1000      // 4KB for CSI/MIPI
#define PHY_REG_SIZE     0x1000      // 4KB for PHY

// Add these register definitions
#define ISP_INT_FRAME_DONE    BIT(0)
#define ISP_INT_ERR           BIT(1)
#define ISP_INT_OVERFLOW      BIT(2)
#define ISP_INT_MASK_ALL     (ISP_INT_FRAME_DONE | ISP_INT_ERR | ISP_INT_OVERFLOW)


/* WDR register offsets within main register block */
#define ISP_WDR_WIDTH_OFFSET    0x124  /* From decompiled code */
#define ISP_WDR_HEIGHT_OFFSET   0x128
#define ISP_WDR_MODE_OFFSET     0x90
#define ISP_WDR_FRAME_OFFSET    0xe8
#define AE_ALGO_MAGIC   0x336ac

#define MIPI_PHY_ADDR   0x10022000
#define ISP_W01_ADDR    0x10023000  // Should be our CSI controller

// Add these defines at the top
#define RMEM_BASE     0x2A80000   // Reserved memory base - matches binary

#define ISP_DEFAULT_SIZE   (44 * 1024 * 1024)  // 44MB - matches libimp expectation
#define ISP_INIT_MAGIC    0x00000001
#define ISP_ALLOC_METHOD  0x203a726f
#define ISP_ALLOC_MAGIC   0x33326373


// Structure for managing physical/virtual memory addresses
// Add at the top of the file
#define ISP_ALLOC_MAGIC1   0x203a726f
#define ISP_ALLOC_MAGIC2   0x33326373

#define RMEM_BASE     0x2A80000   // Reserved memory base address
#define RMEM_SIZE     0x1580000   // Size from logs
#define ISP_BUFFER_ALIGN 32       // Required alignment

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

#define T31_ISP_FREQ     250000000   // 250MHz ISP core clock (matches AHB)
#define T31_CGU_ISP_FREQ 125000000   // 125MHz CGU_ISP clock (matches APB)

// Add CGU ISP bit positions
#define ISP_CLKGR_BIT    (1 << 23)   // ISP clock gate bit in CLKGR
#define CGU_ISP_BIT      (1 << 2)    // CGU_ISP clock gate bit in CLKGR1
#define ISP_MAX_FRAME_SOURCES 3


#define ISP_BASE_OFFSET     0x7820   // Base ISP register from OEM
#define ISP_STRIDE_REG     0x7824    // Stride config register
#define ISP_HEIGHT_REG     0x7828    // Height config register
#define ISP_FORMAT_REG     0x782C    // Format config register


#define VBM_POOL_SIZE 0x99888
#define VBM_ALLOC_METHOD 0x3  // Seen in decompiled code
#define FRAME_SIZE_1080P 3133440
#define FRAME_SIZE_360P  353280
// Channel attribute command

// Test pattern format and registers
#define TEST_PATTERN_FORMAT       0x4008      // Test pattern pixel format
#define TEST_PATTERN_CTRL        0x0001      // Enable test pattern generation
#define TEST_PATTERN_CTRL_REG    0x7840      // Control register offset
#define TEST_PATTERN_SIZE_REG    0x7844      // Size configuration register

// Add these defines at the top of the file
#define TEST_PATTERN_CTRL_REG     0x100   // Example register offset for test pattern control
#define TEST_PATTERN_SIZE_REG     0x104   // Example register for pattern size
#define TEST_PATTERN_COLOR_REG    0x108   // Register for pattern color values
#define TEST_PATTERN_ENABLE       0x1     // Enable bit for test pattern
#define TEST_PATTERN_FORMAT       0x3231564E  // NV12 format for test pattern



// Clock/power related definitions
#define CPM_BASE            0x10000000
#define CPM_CLKGR           0x0020
#define CPM_CLKGR1          0x0028
#define CPM_SRSR            0x002C
#define CPM_CSICDR          0x0084

#define CPM_CSI_CLK_MASK    BIT(20)
#define CPM_CSI_PWR_MASK    BIT(20)
#define CPM_CSI_RST_MASK    BIT(20)

#define ISP_CLK_CTRL        0x0008
#define ISP_RST_CTRL        0x000C
#define ISP_POWER_CTRL      0x0010
// Add these register definitions
#define ISP_FRAME_DONE_REG    0x40c4
#define ISP_TUNING_STATE_REG  0x40a4
#define ISP_FRAME_STATUS_REG  0x40c8

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


#define ISP_SOFT_RESET     (1 << 31)
#define ISP_POWER_ON       (1 << 23)

#define ISP_CLK_CTRL 0x08

// Update IRQ handlers to match OEM pattern:
#define ISP_INT_MASK_ALL (ISP_INT_FRAME_DONE | ISP_INT_ERR | ISP_INT_OVERFLOW)

// Add these defines for consistent memory values
#define ISP_MAGIC_METHOD    0x203a726f
#define ISP_MAGIC_PHYS      0x33326373
#define ISP_INIT_PHYS       0x1
#define ISP_FINAL_PHYS      0x02a80000
#define ISP_ALLOC_SIZE      0x2a80000
#define ISP_ALLOC_KMALLOC   1



#define VBM_POOL_SIZE 0x99888
#define VBM_ALLOC_METHOD 0x3  // Seen in decompiled code

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define DBG_PATTERN 1  // Set to 1 to enable pattern debug prints


#define V4L2_MEMORY_MMAP             1
#define V4L2_MEMORY_USERPTR         2
#define V4L2_MEMORY_OVERLAY         3
#define V4L2_MEMORY_DMABUF          4

/* T31 with 32MB RAM */
#define TASK_SIZE      0x02000000UL    /* 32MB total memory */
#define TASK_SIZE_MAX  0x01000000UL    /* 16MB max task size */

// Add register definitions
#define ISP_VIC_FRAME_DONE_EN   0x308  // Enable frame done interrupt
#define ISP_VIC_FRAME_SIZE      0x304  // Frame dimensions
#define ISP_VIC_FRAME_STRIDE    0x310  // Frame stride

#define VIC_DMA_CTRL    0x000
#define VIC_IRQ_EN      0x93c   // From OEM: *(*(arg1 + 0xb8) + 0x93c) for IRQ enable
//#define VIC_CTRL        0x140   // From OEM: *(*(arg1 + 0xb8) + 0x140) for core enable
//#define VIC_STATUS      0x1E0   // VIC status register
#define VIC_CLEAR       0x1E4   // Interrupt clear register
#define VIC_MASK        0x9e8   // Interrupt mask register

/* VIC Register offsets from VIC base (ISP base + 0x300) */
#define VIC_CTRL         0x140    // Control register
#define VIC_FRAME_SIZE   0x04    // Frame size (width<<16|height)
#define VIC_IRQ_STATUS   0x08    // IRQ status
#define VIC_MODE         0x0C    // Mode register
#define VIC_ROUTE        0x10    // Route register
#define VIC_FRAME_STRIDE 0x18    // Frame stride register
#define VIC_STATUS       0xB4    // Status register
#define VIC_IRQ_EN       0x93c   // IRQ enable register
#define VIC_DMA_ADDR     0x1A0   // DMA address register
#define VIC_MAGIC_CFG    0x1A4   // Magic config register
#define VIC_MAGIC_CFG1   0x1AC   // Second magic register
#define VIC_MAGIC_CFG2   0x1B0   // Third magic register
#define VIC_MAGIC_CFG3   0x1B4   // Fourth magic register
#define VIC_IRQ_MASK     0x9e8   // IRQ mask register

/* Interrupt bits */
#define VIC_INT_FRAME_DONE    BIT(0)     // Frame complete
#define VIC_INT_ERROR         BIT(1)     // General error
#define VIC_INT_FRAME_BREAK   BIT(8)     // Frame break error
#define VIC_INT_OVERFLOW      BIT(9)     // Buffer overflow
#define VIC_INT_STATS_DONE    BIT(12)    // Statistics ready
#define VIC_INT_IP_DONE      BIT(13)    // IP operation done

#define VIC_INT_ERROR_MASK    0x3F8      // Error status bits

// Full interrupt mask combining all interrupt sources
#define VIC_INT_ALL_MASK     (VIC_INT_FRAME_DONE | VIC_INT_ERROR | \
VIC_INT_FRAME_BREAK | VIC_INT_OVERFLOW | \
VIC_INT_STATS_DONE | VIC_INT_IP_DONE | \
VIC_INT_ERROR_MASK)

/* VIC Register offsets - from decompiled code */
#define VIC_STATUS_REG       0xb4     // Status register at 0xb4
#define VIC_ENABLE_REG       0xb8     // Enable register at 0xb8
#define VIC_IRQSTATUS_REG    0x1e0    // IRQ status at 0x1e0
#define VIC_IRQENABLE_REG    0x93c    // IRQ enable at 0x93c
#define VIC_MASK_REG         0x9e8    // Mask register
#define VIC_FRAME_DONE_BIT   0x1      // Frame done bit

/* VIC Interrupt bits */
#define VIC_INT_FRAME_DONE   (1 << 0)    // 0x1
#define VIC_INT_ERROR        (1 << 1)    // 0x2
#define VIC_INT_FRAME_BREAK  (1 << 8)    // 0x100
#define VIC_INT_OVERFLOW     (1 << 9)    // 0x200
#define VIC_INT_STATS_DONE   (1 << 12)   // 0x1000
#define VIC_INT_IP_DONE      (1 << 13)   // 0x2000

/* VIC Interrupt masks from OEM */
#define VIC_INT_OEM_MASK     0x33fb      // OEM mask value
#define VIC_INT_ERROR_MASK   0x3F8       // Error interrupt bits

#define VIC_ENABLE      0xb8   // Enable register (OEM offset)
#define VIC_IRQ_STATUS  0x1e0  // IRQ status register
#define VIC_FRAME_CTRL  0x008   // Frame control register


#define SENSOR_DVP_TIMING_BT601   0
#define SENSOR_DVP_TIMING_BT656   1
#define SENSOR_DVP_TIMING_BT1120  2


// VIC interrupt control registers
#define VIC_IRQ_EN      0x93c   // From OEM: *(*(arg1 + 0xb8) + 0x93c) for IRQ enable
#define VIC_CTRL        0x140   // From OEM: *(*(arg1 + 0xb8) + 0x140) for core enable
#define VIC_STATUS      0x1E0   // VIC status register
#define VIC_CLEAR       0x1E4   // Interrupt clear register
#define VIC_MASK        0x9e8   // Interrupt mask register

struct tisp_param_info {
    uint32_t data[8];  // Array size can be adjusted based on needs
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


struct isp_pipeline {
    unsigned int input_format;
    unsigned int output_format;
    bool scaling_enabled;
    unsigned int input_width;
    unsigned int input_height;
    unsigned int output_width;
    unsigned int output_height;
};

/* Structure definitions for IOCTL parameters */
struct isp_rotation_params {
    int angle;          // Rotation angle (0, 90, 180, 270)
    int rot_height;     // Output height after rotation
    int rot_width;      // Output width after rotation
    unsigned int flags; // Reserved for future use
};


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


// The firmware expects attributes at offset 0x20 from channel base
// and copies 0x50 bytes (0x70 - 0x20) of attribute data

// Corrected structure to match firmware layout
struct imp_channel_attr {
    uint32_t enable;          // 0x00
    uint32_t width;           // 0x04
    uint32_t height;          // 0x08
    uint32_t format;          // 0x0c
    uint32_t crop_enable;     // 0x10
    struct {
        uint32_t x;           // 0x14
        uint32_t y;           // 0x18
        uint32_t width;       // 0x1c
        uint32_t height;      // 0x20
    } crop;
    uint32_t scaler_enable;   // 0x24
    uint32_t scaler_outwidth; // 0x28
    uint32_t scaler_outheight;// 0x2c
    uint32_t picwidth;        // 0x30
    uint32_t picheight;       // 0x34
    uint32_t fps_num;         // 0x38
    uint32_t fps_den;         // 0x3c
    // Total size must be 0x50 bytes to match firmware copy size
    uint32_t reserved[4];     // Pad to 0x50 bytes
} __attribute__((aligned(4)));



struct ae_zone {
    uint32_t mean;       // Mean luminance
    uint32_t var;        // Variance
    uint32_t weight;     // Zone weight
};
struct ae_zone_stats {
    struct ae_zone zones[ISP_AE_GRID_SIZE][ISP_AE_GRID_SIZE];
    uint32_t frame_count;
};

struct ae_statistics {
    struct ae_zone zones[ISP_AE_GRID_SIZE][ISP_AE_GRID_SIZE];
    uint32_t frame_count;
    bool valid;
};

// AE (Auto Exposure) info structure
struct ae_info {
    u32 gain;                // Basic gain value
    u32 exposure;            // Basic exposure value
    u32 gain_factor;         // Gain multiplication factor
    u32 exposure_factor;     // Exposure multiplication factor
    u32 wdr_gain;           // WDR-specific gain
    u32 wdr_exposure;        // WDR-specific exposure
    u32 default_gain;        // Default gain value
    u32 default_exposure;    // Default exposure value
    u32 min_gain;           // Minimum allowed gain
    u32 max_gain;           // Maximum allowed gain
    u32 min_exposure;       // Minimum allowed exposure
    u32 max_exposure;       // Maximum allowed exposure
    u32 target_luminance;   // Target luminance level
    uint32_t enabled;            // AE enabled flag
    uint32_t state;             // Current AE state
    struct ae_callback_data cb;  // Callback structure
    spinlock_t lock;            // Protect callback access
    uint32_t frame_cnt;         // Frame counter
    uint32_t target_fps_num;    // Target FPS numerator
    uint32_t target_fps_den;    // Target FPS denominator
};

// AWB info structure - complete definition based on usage
struct awb_info {
    u32 r_gain;             // Red gain
    u32 g_gain;             // Green gain
    u32 b_gain;             // Blue gain
    u32 temperature;        // Color temperature
    u32 default_gains[3];   // Default RGB gains
    u32 current_gains[3];   // Current RGB gains
    u32 target_gains[3];    // Target RGB gains
    // Add any other fields needed by tiziano_awb_init
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

struct device_list_entry {
    void *device;           // Device pointer
    struct callback_data {
        void *frame_cb;     // Offset 0x1c
        void *stats_cb;     // Offset 0x8
    } *callback_data;       // At offset 0xc4
};

struct IspModule {
    uint32_t id;            // Module ID
    uint32_t type;          // Module type
    uint8_t offset_2;       // From decompiled: module_info->__offset(0x2).b
    uint8_t pad[3];         // Padding to maintain alignment
    uint32_t flags;
    char name[32];
};

// Define CSI device config structure
struct isp_device_config {
    char device_name[32];     // 0x00: Device name
    uint32_t offset_10;       // 0x10: Referenced in init
    struct IspModule *module_info;  // Module info pointer
    struct mutex mlock;       // 0x12c: Device mutex
    void __iomem *reg_base;   // 0x13c: Register base
    uint32_t magic;          // 0x140: Magic value
    uint32_t ctrl;           // 0x144: Control register
};

struct csi_regs {
    u32 version;    // 0x00
    u32 n_lanes;    // 0x04
    u32 phy_shutdownz;  // 0x08
    u32 dphy_rstz;  // 0x0c
    // etc...
};

// Generic sensor device info
struct tx_isp_device_info {
    const char *name;           // Device name
    unsigned long mem_start;    // Memory region start
    unsigned long mem_size;     // Memory region size
    int irq;                    // IRQ number if needed
    void *plat_data;           // Platform data if needed
};

struct csi_device {
    char device_name[32];     // 0x00: Device name
    struct device *dev;       // Device pointer
    uint32_t offset_10;       // 0x10: Referenced in init
    struct IspModule *module_info;  // Module info pointer

    struct tx_isp_subdev *sd;

    // CSI register access - changed to single pointer like VIC
    void __iomem *csi_regs;   // Single pointer like VIC uses
    void __iomem *phy_regs;   // MIPI PHY registers
    struct resource *phy_res;  // PHY memory resource

    // State management
    struct mutex mutex;       // Synchronization
    int state;               // CSI state (1=init, 2=enabled)
    spinlock_t lock;         // Protect register access
};

struct ddr_device {
    struct tx_isp_subdev *sd;   // Pointer to subdev
    void __iomem *ddr_regs;     // DDR registers
    int state;                  // Device state
};

struct tx_isp_resources {
    struct resource *mmio_res;            // Memory mapped I/O resource
    void __iomem *regs;                   // Virtual address of registers
    struct resource *csi_res;             // CSI memory resource
    void __iomem *csi_regs;              // CSI register mapping
    phys_addr_t mmio_phys;               // Physical address of MMIO
    phys_addr_t csi_phys;                // Physical address of CSI
};


/* Status tracking for ISP components */
struct isp_component_status {
    uint32_t state;      // State field at key offsets (0xf4 for VIN)
    uint32_t count;      // Counter seen in activation (0xf8 for VIN)
    struct mutex lock;   // Lock at 0xe8
    void *priv;         // Private pointer at 0xe4
};

/* Frame source status - integrated into component status */
struct isp_framesource_status {
    struct isp_component_status base;
    uint32_t current_frame;
    uint32_t dropped_frames;
    uint32_t buffer_status;
    struct {
        uint32_t width;
        uint32_t height;
        uint32_t format;
    } current_config;
};

/* Device status tracking - integrated with IMPISPDev */
struct isp_device_status {
    struct isp_component_status core;
    struct isp_component_status vic;
    struct isp_framesource_status fs[MAX_CHANNELS];
    struct {
        uint32_t values[MAX_COMPONENTS];
        uint32_t count;
    } component_values;
    bool memory_initialized;
};

struct isp_device_link_state {
    int current_link;          // Current active link
    uint32_t link_flags;       // Link state flags
};


// Frame thread data structure
struct frame_thread_data {
    struct isp_channel *chn;    // Channel being processed
    atomic_t should_stop;       // Stop flag
    atomic_t thread_running;    // Running state
    struct task_struct *task;   // Thread task structure
};

struct isp_pool_config {
    uint32_t format;         // Format type (e.g. NV12)
    uint32_t width;          // Frame width
    uint32_t height;         // Frame height
    uint32_t buffer_stride;  // Buffer stride
    uint32_t buffer_size;    // Total buffer size
    uint32_t num_buffers;    // Number of buffers in pool
};

struct isp_pool {
    struct isp_pool_config config;   // Pool configuration
    bool bound;               // Whether pool is bound to channel
    int pool_id;             // Pool ID (-1 if not allocated)
};

struct isp_channel {
    int fd;
    /* Core identification */
    uint32_t magic;                    // Magic identifier
    int channel_id;                    // Channel number
    uint32_t state;                    // Channel state flags
    uint32_t type;                     // Channel type
    struct device *dev;                // Parent device
    struct tx_isp_subdev subdev;  // Add this
    struct tx_isp_subdev_pad pad; // And this
    uint32_t flags;                    // Channel flags
    uint32_t is_open;                  // Open count
    bool streaming;                    // Streaming state
    struct vm_area_struct *vma;

    /* Channel attributes */
    struct imp_channel_attr attr;      // Channel attributes (maintains firmware layout)
    struct frame_thread_data *thread_data;    // Thread data

    /* Format and frame info */
    uint32_t width;                    // Frame width
    uint32_t height;                   // Frame height
    uint32_t fmt;                      // Pixel format
    uint32_t sequence;                 // Frame sequence number

    /* VBM Pool Management */
    struct {
        uint32_t pool_id;
        uint32_t pool_flags;  // Add pool flags from reqbuf
        bool bound;
        void *pool_ptr;      // VBM pool pointer
        void *ctrl_ptr;      // VBM control pointer
    } pool;                  // Consolidate pool-related fields

    /* Event Management */
    void *remote_dev;             // 0x2bc: Remote device handlers
    struct isp_event_handler *event_hdlr;  // Event handler structure
    spinlock_t event_lock;        // 0x2c4: Event lock
    struct completion done;       // 0x2d4: Completion
    struct isp_channel_event *event;
    isp_event_cb event_cb;
    void *event_priv;

    /* Memory management */
    void *buf_base;                    // Virtual base address
    dma_addr_t dma_addr;               // DMA base address
    dma_addr_t *buffer_dma_addrs;      // DMA addresses of each video_buffer
    uint32_t group_offset;             // Group offset
    uint32_t buf_size;                 // Size per buffer
    uint32_t buf_count;                // Number of buffers
    uint32_t channel_offset;           // Channel memory offset
    uint32_t memory_type;              // Memory allocation type
    uint32_t required_size;            // Required buffer size
    void **vbm_table;                  // VBM entries array
    u32 vbm_count;                     // Number of VBM entries
    uint32_t data_offset;              // Frame data offset
    uint32_t metadata_offset;          // Metadata offset
    dma_addr_t *meta_dma;              // Array of metadata DMA addresses
    uint32_t phys_size;                // Physical memory size per buffer
    uint32_t virt_size;                // Virtual memory size per buffer
    struct vbm_pool *vbm_ptr;          // Pointer to VBM pool structure
    struct vbm_ctrl *ctrl_ptr;
    void __iomem *mapped_vbm;  // Mapped VBM pool memory
    uint32_t stride;                   // Line stride in bytes for frame data
    uint32_t buffer_stride;            // Total stride between buffers including metadata
    bool pre_dequeue_enabled;          // Whether pre-dequeue is enabled for ch0
    void __iomem *ctrl_block;     // Mapped control block
    bool ctrl_block_mapped;       // Control block mapping status
    // For 3.10, just store the physical address and size
    unsigned long ctrl_phys;
    size_t ctrl_size;

    /* Queue and buffer management */
    struct frame_queue *queue;
    atomic_t queued_bufs;              // Available buffer count
    struct frame_group *group;          // Frame grouping info
    struct group_data *group_data;      // Group data
    unsigned long group_phys_mem;       // Physical memory pages
    void __iomem *group_mapped_addr;    // Mapped memory address
    struct frame_node *last_frame;      // Last processed frame

    /* Thread management */
    struct task_struct *frame_thread;   // Frame processing thread
    atomic_t thread_running;            // Thread state
    atomic_t thread_should_stop;        // Thread stop flag

    /* Synchronization */
    spinlock_t vbm_lock;               // VBM access protection
    spinlock_t state_lock;             // State protection
    struct completion frame_complete;   // Frame completion tracking

    /* Statistics */
    struct ae_statistics ae_stats;      // AE statistics
    spinlock_t ae_lock;                // AE lock
    bool ae_valid;                     // AE validity flag
    uint32_t last_irq;                 // Last IRQ status
    uint32_t error_count;              // Error counter
} __attribute__((aligned(8)));

struct periph_clock {
    struct clk *clk;
    struct list_head list;
};

struct isp_state_mem {
    uint32_t registers[0x1100];  // Make room for full 0x1029 offset plus extra
};

/* Main device structure */
/* Note for Claude, GPT, or anyone  that will listen
This is our internal driver structure and should never be passed back to libimp.
Instead, we need to return specific structures that libimp expects outside of this.
 */
struct IMPISPDev {
    /* Core device info */
    struct device *dev;
    struct cdev cdev;
    void __iomem *reg_base;
    spinlock_t lock;
    struct tx_isp_resources resources;
    struct proc_context *proc_context;
    struct list_head periph_clocks;  /* List of peripheral clocks */
    spinlock_t clock_lock;           /* Protect clock list operations */

    /* Device identifiers */
    int major;
    int minor;
    char sensor_name[32];
    u32 sensor_type;
    u32 sensor_mode;
    uint32_t sensor_height;
    uint32_t sensor_width;
    u32 sensor_interface_type;
    u32 vic_status;
    bool is_open;

    /* VIC specific */
    uint32_t vic_started;
    int vic_processing;
    u32 vic_frame_size;
    struct list_head vic_buf_queue;


    /* Memory management */
    dma_addr_t rmem_addr; // Reserved memory address
    size_t rmem_size;
    dma_addr_t dma_addr;
    size_t dma_size;
    void *dma_buf;
    dma_addr_t param_addr;
    void *param_virt;
    uint32_t frame_buf_offset;

    /* Frame sources */
    struct isp_channel  channels[MAX_CHANNELS];
    struct tx_isp_sensor_win_setting *sensor_window_size;

    /* Hardware subsystems */
    struct csi_device *csi_dev;
    atomic_t csi_configured;

    /* Status tracking - integrated status structure */
    struct isp_device_status status;
    struct isp_device_link_state link_state;

    /* Status tracking - match decompiled offsets */
    struct isp_component_status core;
    struct isp_component_status vic;
    struct isp_component_status vin;

    /* Platform devices */
    struct platform_device *pdev;
    struct platform_device *vic_pdev;
    struct platform_device *csi_pdev;
    struct platform_device *vin_pdev;
    struct platform_device *core_pdev;
    struct platform_device *fs_pdev;    // Frame source platform device

    /* Clocks */
    struct clk *cgu_isp;
    struct clk *isp_clk;
    struct clk *ipu_clk;
    struct clk *csi_clk;

    /* GPIO control */
    int reset_gpio;
    int pwdn_gpio;

    /* I2C */
    struct i2c_client *sensor_i2c_client;
    struct i2c_adapter *i2c_adapter;

    /* IRQs */
    int isp_irq;
    struct irq_handler_data *irq_data;
    // IRQ handling
    void (*irq_handler)(void *);      // IRQ handler callback
    void (*irq_disable)(void *);      // IRQ disable callback
    void *irq_priv;                   // Private data for IRQ callbacks
    volatile u32 irq_enabled;         // IRQ enabled state

    // VIC specific
    int vic_irq;
    void (*vic_irq_handler)(void *);
    void (*vic_irq_disable)(void *);
    void *vic_irq_priv;
    spinlock_t vic_lock;
    volatile u32 vic_irq_enabled;
    struct irq_handler_data *vic_irq_data;
    void __iomem *vic_regs;
    struct vic_device *vic_dev;
    struct ddr_device *ddr_dev;
    struct vin_device *vin_dev;
    struct frame_source_device *fs_dev;
    void __iomem *phy_base;

    /* Statistics */
    struct ae_statistics ae_stats;
    spinlock_t ae_lock;
    bool ae_valid;

    /* Module support */
    struct list_head modules;
    spinlock_t modules_lock;
    int module_count;

    /* Format info */
    uint32_t width;
    uint32_t height;
    uint32_t format;
    uint32_t frame_wait_cnt;

    /* Tuning attributes */
    struct isp_tuning_data *tuning_data;
    struct isp_tuning_state *tuning_state;
    int tuning_enabled;  // 0 = disabled, 2 = enabled
    bool tuning_initialized;
    bool bypass_enabled;
    bool links_enabled;
    u32 instance;  // For passing to tuning state
    struct ae_info *ae_info;
    struct awb_info *awb_info;
    uint32_t wdr_mode;
} __attribute__((aligned(4)));

/* Proc file system data structure */
struct isp_proc_data {
    int (*show_func)(struct seq_file *seq, void *v);
    void *private_data;
    struct IMPISPDev *dev;  // Direct reference to the device
};


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

// Struct for passing IRQ-related information to functions
struct irq_info
{
    uint32_t irq_type;  // IRQ type or identifier
    uint32_t flags;        // At offset 0x04 (previously irq_type)
    const char *name;      // At offset 0x08
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

#define MAX_SENSORS 1

struct encoder_state {
    uint32_t channel_id;   // Channel ID (0-8)
    uint32_t state;        // Must match offset 0x109290
    uint32_t registered;   // Must be at 0x109398
    struct semaphore sem;  // Must be at 0x109428
};

struct frame_fifo_entry {
    struct frame_fifo_entry *next;  // Next buffer in chain
    struct frame_fifo_entry *prev;  // Previous buffer in chain
    uint32_t magic_head;           // 0x100100
    uint32_t magic_tail;           // 0x200200
    void *data;                    // Buffer data pointer
};


struct frame_timing {
    u64 timestamp;
    u32 frame_count;
    u32 reserved[2];
};


struct group_data {
    void *base;              // arg1[0]: Base pointer that needs +0x20 access
    char base_block[0x30];   // Space for base pointer access
    void *update_ptr;        // arg1[1]: Update ptr that needs +0x3c access
    char update_block[0x40]; // Space for update ptr access
    uint32_t channel;        // Channel number
    uint32_t state;          // State flags
    uint32_t flags;          // Additional flags
    uint32_t handler;        // 0x9a654
};


struct frame_group {
    char name[0x14];               // 0x00-0x13: Name string
    uint32_t reserved1[0x3];       // 0x14-0x1F: Reserved
    void *handler_fn;              // 0x20: Handler function
    uint32_t reserved2[0x6];       // 0x24-0x3B: Reserved
    void *update_fn;               // 0x3C: Update function
    uint32_t reserved3[0x4];       // 0x40-0x4F: Reserved
    void *group_update;            // 0x50: Group update function
    uint32_t reserved4[0x36];      // 0x54-0x12B: Reserved
    void *self;                    // 0x12C: Self pointer
    uint32_t channel;              // 0x130: Channel ID
    uint32_t state;                // 0x134: State flags
    uint32_t reserved5[0x3];       // 0x138-0x143: Reserved
    uint32_t handler;              // 0x144: Handler value (0x9a654)
    char update_block[512];        // Rest of structure
} __attribute__((packed, aligned(4)));


/* State definitions */
enum frame_channel_state {
    FC_STATE_IDLE = 0,
    FC_STATE_READY = 1,
    FC_STATE_STREAMING = 2,
    FC_STATE_ERROR = 3
};


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

// Add proper CSI registers struct
struct csi_reg_t {
    u32 version;      // 0x00
    u32 n_lanes;      // 0x04
    u32 phy_shutdownz; // 0x08
    u32 dphy_rstz;    // 0x0c
    u32 csi2_resetn;  // 0x10
    u32 phy_state;    // 0x14
    u32 data_ids_1;   // 0x18
    u32 data_ids_2;   // 0x1c
    u32 err1;         // 0x20
    u32 err2;         // 0x24
    u32 mask1;        // 0x28
    u32 mask2;        // 0x2c
    u32 phy_tst_ctrl0; // 0x30
    u32 phy_tst_ctrl1; // 0x34
};

#define fc_queue_lock(fc) spin_lock(&(fc)->queue.lock)
#define fc_queue_unlock(fc) spin_unlock(&(fc)->queue.lock)
#define fc_queue_wait(fc) (&(fc)->queue.wait)
#define fc_frames_completed(fc) atomic_read(&(fc)->queue.frames_completed)
#define fc_buffer_states(fc) ((fc)->queue.buffer_states)
struct vbm_buffer {
    unsigned long virt_addr;
    dma_addr_t phys_addr;
    size_t size;
    int index;
    int flags;
};


// Structure to match libimp's expectation
struct imp_buffer_info {
    uint32_t method;
    uint32_t phys_addr;
    uint32_t virt_addr;
    uint32_t size;
    uint32_t flags;
};

struct frame_info {
    void *virt_addr;           // Virtual address
    dma_addr_t phys_addr;      // Physical address
    uint32_t state;            // Frame state
    uint32_t ready;            // Ready flag
    uint32_t size;             // Frame size
    uint32_t sequence;         // Frame sequence number
    uint32_t flags;            // Frame flags
    uint32_t status;          // Frame status
};

struct isp_i2c_board_info {
    char type[32];          // Sensor type/name
    u16 addr;               // I2C device address
    unsigned short flags;    // I2C flags
    void *platform_data;    // Platform specific data
    int irq;                // IRQ if needed
};

struct isp_sensor_info {
    int is_initialized;
    int chip_id;
    int width;
    int height;
};



// Add these structures to match sensor expectations
struct sensor_win_size {
    uint32_t width;          // 0x00: Frame width
    uint32_t height;         // 0x04: Frame height
    uint32_t fps;           // 0x08: Frame rate
    uint32_t max_fps;       // 0x0C: Maximum frame rate
    uint32_t format;        // 0x10: Image format
};

// Add this structure if not already present
struct isp_platform_data {
    unsigned long clock_rate;
};


/* Pad descriptor matching offsets in decompiled */
struct isp_pad_info {
    char name[32];        // 0x00: Name string
    uint8_t type;         // 0x20: 1=source, 2=sink
    uint8_t link_flags;   // 0x21: Link flags
    uint16_t pad_idx;     // Pad index within subdev
    uint32_t flags;       // Additional flags
};

#define MAX_BUFFERS 64

#endif
