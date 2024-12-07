#ifndef TX_LIBIMP_H
#define TX_LIBIMP_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <net/net_namespace.h>
#include <linux/types.h>
#include <linux/semaphore.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/ioctl.h>

// Control codes from libimp

#define ISP_CTRL_SATURATION     0x980902
#define ISP_CTRL_SHARPNESS      0x98091b
#define ISP_CTRL_HFLIP         0x980914
#define ISP_CTRL_VFLIP         0x980915
#define ISP_CTRL_BYPASS        0x8000164
#define ISP_CTRL_PROCESS       0x8000164
#define ISP_CTRL_SHADING       0x8000166

// IOCTL commands from libimp
#define ISP_CORE_S_CTRL      0xc008561c  // Set control command
#define ISP_CORE_G_CTRL      0xc008561b  // Get control command
#define ISP_TUNING_ENABLE    0xc00c56c6  // Enable tuning command

// ISP Control codes from libimp
#define ISP_CTRL_SATURATION   0x980902  // From IMP_ISP_Tuning_SetSaturation
#define ISP_CTRL_SHARPNESS    0x98091b  // From IMP_ISP_Tuning_SetSharpness
#define ISP_CTRL_HFLIP        0x980914  // From IMP_ISP_Tuning_SetISPHflip
#define ISP_CTRL_VFLIP        0x980915  // From IMP_ISP_Tuning_SetISPVflip
#define ISP_CTRL_BYPASS       0x8000164 // From IMP_ISP_Tuning_SetISPBypass
#define ISP_CTRL_SHADING      0x8000166 // From IMP_ISP_Tuning_SetShading
#define ISP_CTRL_PROCESS      0x8000164 // Same as bypass, used for process control

#define VIDIOC_GET_SENSOR_INFO   0x40045626
#define VIDIOC_SET_BUF_INFO      0x800856d4
#define VIDIOC_GET_BUF_INFO      0x800856d5
#define ISP_ENABLE_LINKS         0x800456d0
#define ISP_DESTROY_LINKS        0x800456d1
#define ISP_ENABLE_ROUTE         0x800456d2
#define ISP_DISABLE_LINKS        0x800456d3
#define SENSOR_STREAMON          0x80045612
#define SENSOR_SET_INFO          0xc0045627
#define ISP_READ_SENSOR_ID       0x805056c1
#define ISP_SET_SENSOR_INFO      0xc050561a


// ISP register definitions
#define ISP_AE_HIST_BASE   0x7000  // Base address for AE histogram data
#define ISP_AE_STATE_BASE  0x7100  // Base address for AE state registers

#define ISP_CTRL_BRIGHTNESS  0x980900
#define ISP_CTRL_CONTRAST    0x980901
#define ISP_CTRL_SATURATION  0x980902
#define ISP_CTRL_HFLIP      0x980914
#define ISP_CTRL_VFLIP      0x980915
#define ISP_CTRL_SHARPNESS  0x98091b
#define ISP_CTRL_DPC        0x98091f  // DPC control
#define ISP_CTRL_GAMMA      0x9a091a  // Gamma control

// Core tuning IOCTLs
#define ISP_CORE_G_CTRL         0xc008561b
#define ISP_CORE_S_CTRL         0xc008561c
#define ISP_TUNING_ENABLE       0xc00c56c6

#define ISP_TUNING_CID_AE_ZONE    0x8000030  // From GetAeZone
#define ISP_TUNING_CID_AE_STATE   0x8000036  // From GetAeState
#define ISP_TUNING_CID_AF_ZONE    0x8000046  // From GetAfZone
#define ISP_TUNING_CID_AE_HIST_ORIGIN 0x8000031

// Size definitions from decompiled AF zone function (0x384 = 900 bytes)
#define MAX_AF_ZONES     225    // Typical 15x15 zone grid = 225 * 4 bytes = 900 bytes

// Histogram/AE definitions from earlier decompiled code
#define MAX_AE_ZONES     225    // Same 15x15 grid as AF
#define MAX_HIST_BINS    256    // Standard histogram size
#define ISP_AF_ZONE_BASE    0x7000  // Base address for AF zone registers
#define ISP_AF_ZONE_SIZE    (MAX_AF_ZONES * 4)  // 900 bytes of zone data

#define AE_HIST_SIZE     0x400   // 1024 bytes of histogram data
#define AE_HIST_BUF_SIZE 0x42c   // Full buffer size including extra data

#define ISP_VALUE_MAX 255
#define ISP_CTRL_BRIGHTNESS  0x980900
#define ISP_CTRL_CONTRAST    0x980901
#define ISP_CTRL_SATURATION  0x980902  // Add missing saturation control
#define ISP_CTRL_HFLIP      0x980914
#define ISP_CTRL_VFLIP      0x980915
#define ISP_CTRL_SHARPNESS  0x98091b
#define ISP_CTRL_DPC        0x98091f
#define ISP_CTRL_GAMMA      0x9a091a

#define ISP_CTRL_REG         (ISP_REG_BASE + 0x00)
#define ISP_FLIP_CTRL_REG    (ISP_REG_BASE + 0x28)   // Adjust this offset based on hardware
#define ISP_WDR_REG_BASE     (ISP_REG_BASE + 0x1000)
#define ISP_AE_REG_BASE      (ISP_REG_BASE + 0x100)
#define ISP_AWB_REG_BASE     (ISP_REG_BASE + 0x200)
#define ISP_LINK_ENABLE_REG   0x04
#define ISP_ROUTE_REG         0x08
#define ISP_BYPASS_REG        0x0C

#define ISP_NOTIFY_AE      0x1000000   // AE callback notification
#define ISP_NOTIFY_STATS   0x2000000   // Stats callback notification
#define ISP_ERR_CALLBACK   0xfffffdfd  // Error status for callback failure

/* AE state tracking */
#define AE_STATE_DISABLED  0
#define AE_STATE_ENABLED   1
#define AE_STATE_RUNNING   2

// Parameter offsets in tuning state
#define TUNING_OFF_CONTRAST    0x01
#define TUNING_OFF_BRIGHTNESS  0x02
#define TUNING_OFF_SATURATION  0x0a
#define TUNING_OFF_SHARPNESS   0x0b
#define TUNING_OFF_GAMMA       0x09
#define TUNING_OFF_HFLIP       0x0e
#define TUNING_OFF_VFLIP       0x0f
#define TUNING_OFF_DPC         0x11

#define ISP_TUNING_SET_FPS     0x80000e0  // FPS control command code
#define ISP_CTRL_BYPASS    0x8000164
#define ISP_CTRL_ANTIFLICKER 0x980918
#define ISP_LINK_CTRL         0x0000  // Base + offset for link control
#define ISP_BYPASS_CTRL       0x0004  // Base + offset for bypass control
#define ISP_ANTIFLICKER_CTRL  0x0008  // Base + offset for antiflicker

#define ISP_CTRL_REG          0x00
#define ISP_LINK_ENABLE_REG   0x04
#define ISP_ROUTE_REG         0x08
#define ISP_BYPASS_REG        0x0C


/* ISP AF Zone register definitions */
#define ISP_AF_ZONE_BASE      0x13380000   // Base address for AF zone registers
#define ISP_AF_ZONE_METRICS   0x00         // Offset to zone metrics
#define ISP_AF_ZONE_STATUS    0x40         // Offset to zone status
#define ISP_AF_ZONE_CONFIG    0x44         // Offset to zone configuration

/* AF Zone structures */
#define MAX_AF_ZONES      16  // Common size for AF windows/regions

struct af_zone_info {
    uint32_t zone_metrics[MAX_AF_ZONES];  // Zone metrics like contrast values
    uint32_t zone_status;                 // Overall AF status
    uint32_t flags;                       // Zone configuration flags
    struct {
        uint16_t x;                       // Zone X position
        uint16_t y;                       // Zone Y position
        uint16_t width;                   // Zone width
        uint16_t height;                  // Zone height
    } windows[MAX_AF_ZONES];              // AF windows configuration
};

// Global AF zone data as seen in original code
static struct af_zone_data {
    uint32_t zone_metrics[MAX_AF_ZONES];  // Current zone metrics
    uint32_t status;                      // Current AF status
} af_zone_data;

/* ISP Pipeline Register Structure */
// Strucure for LIBIMP
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


// Add these structures to match libimp's buffer management
struct imp_frame_node {
    uint32_t magic;             // 0x0: Magic number identifier
    void *data;                 // 0x4: Buffer data pointer
    uint32_t frame_size;        // 0x8: Frame data size
    uint32_t seq;              // 0xc: Frame sequence number
    struct list_head list;      // 0x10: List management
    uint32_t flags;            // 0x20: Frame flags
    uint8_t state;             // 0x24: Frame state
};

struct frame_node {
    /* Core frame data - matches OEM layout */
    uint32_t magic;             // 0x00: Magic number (0x100100)
    uint32_t index;            // 0x04: Buffer index in array
    uint32_t frame_size;        // 0x08: Frame data size
    uint32_t bytesused;         // 0x0C: Actual bytes used in frame
    struct list_head list;      // 0x10: List management
    uint32_t length;           // 0x1C: Total buffer length
    uint32_t flags;            // 0x20: Frame flags (V4L2_BUF_FLAG_*)
    uint32_t state;            // 0x24: Frame state (matches OEM states)
    void *data;                 // 0x28: Buffer data pointer
    uint32_t magic_tail;       // 0x2C: Magic tail number (0x200200)

    /* Memory management */
    void *virt_addr;           // 0x30: Our DMA buffer virtual address
    dma_addr_t phys_addr;      // 0x34: Physical/DMA address
    void *metadata;            // 0x38: Metadata area pointer
    uint32_t metadata_offset;   // 0x3C: Offset to metadata

    /* Buffer management */
    uint32_t sequence;         // 0x40: Frame sequence number
    uint32_t total_size;       // 0x44: Total size including metadata
    struct timespec timestamp; // 0x48: Frame timestamp

    /* USERPTR management */
    unsigned long userptr;   // 0x50: Userspace buffer pointer
    bool is_mapped;           // 0x54: Whether userptr is mapped

    /* Memory mapping support */
    struct page **pages;       // For USERPTR/MMAP support
    int nr_pages;             // Number of pages mapped

    /* Reserved space to match OEM structure size */
    uint32_t queued_count;  // Track number of times queued
    bool is_queued;         // Flag to prevent double queueing
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

// Common struct for both QBUF and DQBUF operations
struct __attribute__((packed, aligned(4))) frame_qbuf_request {
    __u32 index;
    __u32 type;
    __u32 bytesused;
    __u32 flags;
    __u32 field;
    struct timeval timestamp;
    __u32 sequence;
    __u32 memory;
    union {
        __u32 offset;
        unsigned long userptr;
        __u32 reserved[8];
    } m;
    __u32 length;
    __u32 reserved2[4];
};

struct __attribute__((packed, aligned(4))) frame_buffer {
    // Match V4L2 ordering
    __u32 index;
    __u32 type;
    __u32 bytesused;
    __u32 flags;
    __u32 field;
    struct timeval timestamp;
    __u32 sequence;
    __u32 memory;
    union {
        __u32 offset;
        unsigned long userptr;
        __u32 reserved[8];  // Extra padding for alignment
    } m;
    __u32 length;

    // Our additional fields
    __u32 state;          // Internal state tracking
    __u32 reserved2[4];   // Padding to ensure size alignment
};



struct dqbuf_resp {
    u32 index;           // 0x00
    u32 channel_num;     // 0x04
    u32 bytesused;       // 0x08
    u32 flags;           // 0x0c
    u32 field;           // 0x10
    u32 width;           // 0x14
    u32 height;          // 0x18
    u32 pad1;            // 0x1C
    u32 timestamp_sec;   // 0x20
    u32 timestamp_usec;  // 0x24
    u32 fps_num;         // 0x28
    u32 fps_den;         // 0x2c
    u32 pad2[172];       // Padding to 0x3e0
    u32 ref_count;       // 0x3e0
    u32 format;          // 0x3e4
    u32 flags2;          // 0x3e8
    u32 sequence;        // 0x3ec
    u32 pad3[4];         // Additional padding to ensure proper alignment
};

struct isp_core_ctrl {
    u32 cmd;     // Control command
    u32 value;   // Control value/result
    u32 flag;    // Additional flags/data
};

struct isp_tuning_data {
    void __iomem *regs;     // ISP register mapping
    uint32_t state;         // Tuning state
    uint32_t offset_1c;     // Matches 0x1c allocation (libimp's userspace lock)
    u8 contrast;           // 0x01
    u8 brightness;         // 0x02
    u8 reserved2;          // 0x03
    u8 exposure;           // 0x04
    u8 reserved3;          // 0x05
    u8 color_temp;         // 0x06
    u8 auto_wb;            // 0x07
    u8 shading;            // 0x08
    u8 gamma;              // 0x09
    u8 saturation;         // 0x0a
    u8 sharpness;          // 0x0b
    u8 hist_eq;            // 0x0c
    u8 ae_enable;          // 0x0d
    u8 hflip;              // 0x0e
    u8 vflip;              // 0x0f
    u8 test_pattern;       // 0x10
    u8 dpc;               // 0x11
    u8 antiflicker;       // 0x1c

    // Additional needed fields
    uint32_t wb;           // 0x8000004 - White balance setting
    uint32_t wb_stats;     // WB statistics from 0x8000005
    uint32_t wb_golden;    // Golden WB stats from 0x8000009
    uint32_t sensor_attr;  // Sensor attributes from 0x8000045

    // Expanded WB control
    uint16_t wb_rgain;    // Red gain
    uint16_t wb_bgain;    // Blue gain
    uint16_t wb_ggain;    // Green gain

    // New fields from recent tuning commands
    int32_t ae_comp;       // 0x8000023 - AE compensation
    int32_t max_again;     // 0x8000028 - Maximum analog gain
    int32_t max_dgain;     // 0x8000029 - Maximum digital gain
    int32_t total_gain;    // 0x8000027 - Total gain
    uint8_t bcsh_hue;      // 0x8000101 - BCSH hue control
    int32_t defog_strength; // 0x8000039 - Defog strength
    int32_t dpc_strength;   // 0x8000062 - Dead pixel correction
    int32_t drc_strength;   // 0x80000a2 - Dynamic range compression
    u32 sinter_strength;  // Spatial noise reduction strength
    u32 temper_strength;  // Temporal noise reduction strength
    uint32_t running_mode;
    /* BCSH Parameters */
    uint32_t bcsh_ev;    // Exposure value

    /* BCSH state variables */
    uint32_t bcsh_saturation_value;    // Current saturation value
    uint32_t bcsh_saturation_max;      // Max saturation limit
    uint32_t bcsh_saturation_min;      // Min saturation limit
    uint32_t bcsh_saturation_mult;     // Saturation multiplier

    uint8_t bcsh_brightness;
    uint8_t bcsh_contrast;
    uint8_t bcsh_saturation;

    uint32_t wb_temp;
    struct {
        uint32_t r;
        uint32_t g;
        uint32_t b;
    } wb_gains;

    /* Lookup tables for saturation interpolation */
    uint32_t bcsh_au32SminListS_now[9];  // Saturation min values
    uint32_t bcsh_au32SmaxListS_now[9];  // Saturation max values
    uint32_t bcsh_au32EvList_now[9];     // EV breakpoints
    uint32_t bcsh_au32SminListM_now[9];  // Min multiplier values
    uint32_t bcsh_au32SmaxListM_now[9];  // Max multiplier values
    uint32_t bcsh_au32OffsetRGB_now;     // RGB offset value

    /* Color transform matrices */
    uint32_t bcsh_h_matrix;          // Hue transform matrix
    uint32_t bcsh_ccm_matrix;        // Color correction matrix
    uint32_t bcsh_rgb2yuv_offset;    // RGB to YUV conversion offset

    // FPS state
    uint32_t fps_num;                            // Frame rate numerator at offset 0xc
    uint32_t fps_den;                            // Frame rate denominator at offset 0x10
    uint32_t fps_hi;
    uint32_t fps_lo;

    uint32_t move_state;
    atomic_t initialized;
    struct mutex lock;
};

struct isp_ae_hist_cfg {
    // Configuration bytes (0x10 bytes total)
    uint8_t config[4];        // Main config bytes
    uint16_t window_x;        // Histogram window X
    uint16_t window_y;        // Histogram window Y
    uint16_t window_width;    // Window width
    uint16_t window_height;   // Window height
    uint16_t num_bins;        // Number of histogram bins
    uint8_t extra_cfg[2];     // Additional config bytes

    // Large data buffer follows in memory
    uint32_t data[0x400];     // Histogram data buffer
} __attribute__((packed));


// Tuning state structure based on decompiled code
struct isp_tuning_state {
    bool enabled;          // Tuning enabled flag
    bool initialized;      // Tuning initialized
    void __iomem *regs;   // Tuning registers mapping
    struct mutex lock;     // Protection
    uint32_t contrast;     // 0x09 offset in userspace struct
    uint32_t brightness;   // For tuning params
    uint32_t saturation;
    uint32_t sharpness;
};

struct isp_zone_ctrl {
    __u32 enable;     // var_18 = 1
    __u32 cmd;        // var_14 = control code
    __u32 value;      // var_10 = arg1 (output buffer)
};

// AE Zone data structure (from apical_isp_ae_zone_g_ctrl.isra.84)
struct ae_zone_info {
    u8 zones[MAX_AE_ZONES];    // Zone data
    u16 metrics[4];           // Various metrics
    u16 hist_data[MAX_HIST_BINS];
    u8 additional[4];         // Additional data seen in tisp_g_ae_hist
};


// AE histogram and state structures
struct ae_hist_data {
    // From decompiled code where we see 0x42c bytes allocated
    u32 histogram[256];    // 0x400 bytes of histogram data
    u8 stats[0x14];       // Additional statistics data
    u8 status[0x18];      // Status bytes including those at 0x414, 0x418, etc
};

struct ae_state_info {
    u32 exposure;         // Current exposure value
    u32 gain;            // Current gain value
    u32 status;          // AE status/flags
};

// Hardware register structure
struct isp_ae_regs {
    u32 gain;
    u32 exposure;
    u32 gain_factor;
    u32 exposure_factor;
    u32 stats[64];  // AE statistics
    // ... other registers
};

struct isp_flip_ioctl {
    int32_t enable;  // Enable value
    int32_t cmd;  // Command code (0x980914 or 0x980915)
    int32_t value;  // Value to set
};

// Structure for ISP controls with control value
struct isp_ctrl_msg {
    u32 cmd;      // Command code
    u32 value;    // Value to set
};

// Structure for FPS control
struct isp_fps_param {
    uint32_t enable;     // 1 = set, 0 = get
    uint32_t cmd;        // Command (0x80000e0)
    uint32_t value;      // Packed FPS value (num << 16 | den)
};

struct ae_callback_data {
    void (*frame_cb)(void *);    // Frame callback at offset 0x1c
    void (*stats_cb)(void *);    // Stats callback at offset 0x8
    void *priv;                  // Private data
};

struct isp_callback_info {
    void *callback_data;      // At offset 0xc4
    int32_t (*ae_cb)(void);  // AE callback at offset 0x1c
    int32_t (*stats_cb)(void); // Stats callback at offset 0x8
};

struct buffer_state {
    uint32_t flags;          // 0x4c
    uint32_t state;          // 0x48
    uint32_t buffer_addr;    // 0x34
    uint32_t buffer_size;    // 0x38
    uint32_t metadata_flags; // 0x0c
} __attribute__((packed));


#define ISP_TUNING_GET_TOTAL_GAIN 0x8000027
#define ISP_TOTAL_GAIN_REG    0x1084

struct isp_gain_info {
    uint32_t total_gain;   // Current total gain value
    uint32_t flags;        // Optional flags/status
};

struct isp_tuning_ctrl {
    uint32_t value;
    uint32_t cmd;
    void __user *data;  // Optional data pointer
};

struct frame_fmt {
    __u32 type;           // 0x00: type=1
    __u32 width;          // 0x04: e.g. 0x780 (1920)
    __u32 height;         // 0x08: e.g. 0x438 (1080)
    union {               // 0x0C
        __u32 pixelformat;
        char pix_str[4];      // "NV12"
    };
    __u32 field;          // 0x10
    __u32 bytesperline;   // 0x14
    __u32 sizeimage;      // 0x18
    __u32 colorspace;     // 0x1C: Must be 8 (SRGB)
    __u32 priv;           // 0x20
    __u32 flags;          // 0x24
    __u32 reserved[26];   // Pad to 0x70 bytes
} __attribute__((packed));

struct frame_chan_attr {
    u32 width;          // 0x00
    u32 height;         // 0x04
    u32 format;         // 0x08
    u32 picWidth;       // 0x0C
    u32 picHeight;      // 0x10
    u32 buf_type;       // 0x14 - Matches offset 0x24
    u32 mem_type;       // 0x18 - Matches offset 0x3c
    u32 buf_count;      // 0x1C - Matches offset 0x20c
    u32 state;          // 0x20 - Matches offset 0x2d0
    u32 streaming;      // 0x24 - Matches offset 0x230
    u32 reserved[10];   // Pad to match 80 byte copy
} __packed;

struct group_metadata {
    uint32_t magic;         // 0x00: Magic header
    void *group;            // 0x04: Pointer to frame group
    uint32_t frame_num;     // 0x08: Frame number in sequence
    uint32_t ref_count;     // 0x0c: Reference count (1)
    uint32_t state;         // 0x10: Frame state
    uint32_t flags;         // 0x14: Frame flags (0)
    uint32_t size;          // 0x18: Frame size
    uint32_t type;          // 0x1c: Frame type
    void *handler;          // 0x20: Must match group handler (0x9a654)
    uint32_t channel;       // 0x24: Channel ID
    uint32_t sequence;      // 0x28: Frame sequence number
    uint32_t timestamp;     // 0x2c: Timestamp
    uint32_t padding[3];    // 0x30-0x38: Reserved
    uint32_t done_flag;     // 0x3c: Frame done flag
} __attribute__((packed));

struct frame_metadata {
    uint32_t magic;         // 0x00: Magic (0x100100)
    void *group_ptr;        // 0x04: Critical for group_update
    uint32_t frame_num;     // 0x08: Buffer index
    uint32_t ref_count;     // 0x0c: Reference count (start at 1)
    uint32_t state;         // 0x10: Frame state
    uint32_t flags;         // 0x14: Frame flags
    uint32_t size;          // 0x18: Frame size
    uint32_t type;          // 0x1c: Frame type (format)
    void *handler;          // 0x20: Must be 0x9a654
    uint32_t channel;       // 0x24: Channel ID
    uint32_t sequence;      // 0x28: Frame sequence number
    uint32_t timestamp;     // 0x2c: Timestamp
    uint32_t padding[3];    // 0x30-0x38: Reserved padding
    uint32_t done_flag;     // 0x3c: Frame done flag
} __attribute__((packed, aligned(4)));


/* Control structure matching userspace */
struct isp_flip_ctrl {
    u32 cmd;    /* 0x980914 or 0x980915 */
    u32 value;  /* Flip value */
};


/* Must match libimp video element layout - 0x14 bytes */
struct video_elem {
    struct video_elem *next;  // 0x00: Next pointer in queue
    struct video_elem *prev;  // 0x04: Previous pointer in queue
    void *data;              // 0x08: Buffer data pointer
    uint32_t size;           // 0x0c: Buffer size
    uint32_t flags;          // 0x10: Status flags
} __attribute__((packed));

/* Must match libimp container layout - 0x68 bytes */
struct video_container {
    struct mutex lock;       // 0x00: Main container lock
    struct video_elem *free_head;  // 0x18: Head of free queue
    struct video_elem *free_tail;  // 0x1c: Tail of free queue
    struct mutex buf_lock;   // 0x20: Buffer queue lock
    struct video_elem *ready_head; // 0x38: Head of ready queue
    struct video_elem *ready_tail; // 0x3c: Tail of ready queue
    struct mutex done_lock;  // 0x40: Done queue lock
    struct video_elem *done_head;  // 0x58: Head of done queue
    struct video_elem *done_tail;  // 0x5c: Tail of done queue
    struct video_elem *buffers;    // 0x60: Array of video elements
    uint32_t count;         // 0x64: Number of buffers
} __attribute__((packed));

//// Buffer structure - total size 0xd0 bytes
//struct video_buffer {
//    uint32_t magic;             // 0x0: Magic number identifier
//    void *data;                 // 0x4: Buffer data pointer
//    uint32_t frame_size;        // 0x8: Frame data size
//    uint32_t seq;              // 0xc: Frame sequence number
//    struct list_head list;      // 0x10: List management
//    uint32_t flags;            // 0x20: Frame flags
//    uint8_t state;             // 0x24: Frame state
//};

struct video_buffer {
    uint32_t index;        // 0x00: Buffer index
    uint32_t type;         // 0x04: Buffer type
    uint8_t pad1[0x30];    // 0x08-0x37: Padding
    uint32_t memory;       // 0x38: Memory type
    uint8_t pad2[0xc];     // 0x3c-0x47: Padding
    uint32_t flags;        // 0x48: Buffer state (1=queued, 2=done)
    uint32_t status;       // 0x4c: Buffer status
    uint8_t pad3[0x8];     // 0x50-0x57: Padding
    struct list_head list; // 0x58: List head for queues
    void *queue;           // 0x60: Points back to frame_queue
    uint8_t pad4[0xc];     // 0x64-0x6f: Padding
    void *data;            // 0x70: Buffer data pointer
    struct frame_metadata *meta;  // 0x74: Metadata pointer
    uint8_t pad5[0x54];    // 0x78-0xcf: Remaining padding
} __attribute__((packed));

// Frame queue structure - maintains exact offsets
struct frame_queue {
    struct mutex lock;           // 0x00: Queue lock
    struct video_buffer **bufs;  // 0x24: Array of buffer pointers
    uint32_t memory_type;        // 0x3c: Memory type (V4L2_MEMORY_*)
    uint8_t pad1[0x1d0];        // 0x40-0x20f: Padding to maintain offsets
    uint32_t buf_count;         // 0x210: Number of buffers
    struct list_head ready_list; // 0x214: Ready list head
    struct list_head done_list;  // 0x21c: Done list head
    spinlock_t queue_lock;       // 0x224: Queue spinlock
    wait_queue_head_t wait;      // 0x228: Wait queue
    uint32_t stream_state;       // 0x230: Stream state
} __attribute__((packed));

struct vbm_frame_info {
    uint32_t pool_id;       // Pool ID
    uint32_t state;         // Frame state (2 = done)
    uint32_t ref_count;     // Reference count
    struct mutex lock;      // Frame lock
    void *frame_data;       // Frame data pointer
};

// This needs to match exactly what libimp expects
struct group_module {
    char pad1[0x50];
    void *update_fn;         // Offset 0x50: points to our update wrapper
    char pad2[0xdc];        // Padding to 0x12c
    void *self;             // Offset 0x12c: points to self
    uint32_t channel;       // Offset 0x130: channel number
    char pad3[0x10];        // Padding to 0x144
    uint32_t handler;       // Offset 0x144: handler value (0x9a654)
    char update_block[512]; // Start update block right after handler
} __attribute__((packed, aligned(4)));


struct vbm_frame {
    __u32 index;          // 0x000
    __u32 pool_id;        // 0x004
    __u32 width;          // 0x008
    __u32 height;         // 0x00c
    __u32 format;         // 0x010
    __u32 size;           // 0x014
    __u32 virt_addr;      // 0x018
    __u32 phys_addr;      // 0x01c
    __u32 frame_data1;    // 0x020
    __u32 frame_data2;    // 0x024
    __u32 reserved1[0xD]; // 0x028-0x057 (52 bytes)
    __u32 zone_info;      // 0x058
    __u32 reserved2[0xE7];// 0x05c-0x3db (0x380 bytes)
    __u32 q_count;        // 0x3dc
    __u32 dq_count;       // 0x3e0
    __u16 reserved3;      // 0x3e4-0x3e5
    __u16 state_flags;    // 0x3e6
    __u32 state1;         // 0x3e8
    __u32 state2;         // 0x3ec
    __u32 state3;         // 0x3f0
    __u32 reserved4[0xE]; // 0x3f4-0x428 (56 bytes)
} __attribute__((packed));

struct vbm_pool {
    __u32 pool_id;        // 0x000
    __u32 user_id;        // 0x004
    __u32 reserved1[1];   // 0x008-0x00b
    __u32 width;          // 0x00c
    __u32 height;         // 0x010
    __u32 format;         // 0x014
    __u32 reserved2[2];   // 0x018-0x01f
    __u32 config_fmt;     // 0x020
    __u32 reserved3[0x4D];// 0x024-0x157 (0x134 bytes)
    __u32 base_phys;      // 0x158
    __u32 base_virt;      // 0x15c
    __u32 reserved4[3];   // 0x160-0x16b
    __u32 phys_addr;      // 0x16c
    __u32 virt_addr;      // 0x170
    __u32 get_frame_fn;   // 0x174
    __u32 rel_frame_fn;   // 0x178
    __u32 bound_chn;      // 0x17c
    struct vbm_frame frames[]; // 0x180
} __attribute__((packed));


// VBM Frame entry (size 0x428)
struct vbm_frame_entry {
    uint32_t index;         // 0x00
    uint32_t pool_id;       // 0x04
    uint32_t width;         // 0x08
    uint32_t height;        // 0x0c
    uint32_t format;        // 0x10
    uint32_t size;          // 0x14
    uint32_t virt_addr;     // 0x18
    uint32_t phys_addr;     // 0x1c
    uint8_t padding[0x408]; // Rest of frame struct
} __attribute__((packed));



struct reqbuf_request {
    // Standard V4L2 fields (0x00-0x13)
    __u32 count;           // 0x00: Number of buffers requested
    __u32 type;           // 0x04: Buffer type (capture/output)
    __u32 memory;         // 0x08: Memory type (mmap/userptr)
    __u32 capabilities;   // 0x0C: Capabilities flags
    __u32 reserved[1];    // 0x10: Reserved

    // VBM info section (0x14-0x23)
    void *pool_ptr;       // 0x14: VBM pool pointer
    void *ctrl_ptr;       // 0x18: VBM control pointer
    __u32 pool_flags;     // 0x1C: VBM pool flags
    __u32 padding[1];     // 0x20: Padding

    // Frame config (0x24-0x4C)
    __u32 width;          // 0x24: Frame width
    __u32 height;         // 0x28: Frame height
    __u32 frame_count;    // 0x2C: Number of frames
    __u32 phys_size;      // 0x30: Physical memory size
    __u32 virt_size;      // 0x34: Virtual memory size
    __u32 padding2;       // 0x38: More padding

    // Reserved section (0x3C-0x6B)
    __u32 reserved2[12];  // 0x3C-0x6B: Reserved

    // Metadata section (0x6C-0x70)
    __u32 meta_size;      // 0x6C: Size of metadata region
} __attribute__((packed, aligned(4))); // Total size: 0x70 (112) bytes // Total size: 0x70 (112) bytes


struct isp_event_handler {
    void *ctx;           // Offset 0x0: Context pointer
    void *priv;          // Offset 0xc: Private data containing function ptr at 0x1c
};

// Buffer info passed with BUFFER_ENQUEUE
struct vb_enqueue_info {
    __u32 index;          // Buffer index
    __u32 state;          // State (3 = queued)
    __u32 flags;          // Buffer flags
    __u32 field;          // Field type
    __u32 timestamp;      // Timestamp
    __u32 timecode;       // Timecode
    __u32 sequence;       // Sequence number
    __u32 memory;         // Memory type
    __u32 offset;         // Buffer offset
    __u32 length;         // Buffer length
    void  *priv;          // Private data
};

// Buffer done info from hardware
struct vb_done_info {
    __u32 index;          // Buffer index
    __u32 state;          // New state (4 = done)
    __u32 sequence;       // Sequence number
    __u32 field;          // Field type
    __u32 timestamp;      // Completion timestamp
    __u8  flags;          // Status flags
    __u8  reserved[3];    // Padding
    __u32 error;          // Error code if any
};

struct isp_buf_request {
    __u32 count;
    __u32 type;
    __u32 memory;
    __u32 channel;
    __u32 size;
    __u32 flags;
    void *pool_ptr;
    void *ctrl_ptr;
};

struct isp_buf_enqueue {
    __u32 index;
    __u32 type;
    __u32 memory;
    __u32 flags;
    void *data;
    __u32 length;
};

struct isp_buf_done {
    __u32 index;
    __u32 sequence;
    __u32 timestamp;
    __u32 flags;
    __u32 length;
    __u32 reserved[3];
};

struct isp_event_msg {
    __u32 type;
    __u32 channel;
    __u32 length;
    union {
        struct isp_buf_request req;
        struct isp_buf_done done;
        __u8 data[128];
    };
};

struct irq_dev {
    spinlock_t lock;               // 0x130 offset
    volatile u32 irq_enabled;      // 0x13c offset
    void (*irq_handler)(void *);   // 0x84 offset
    void (*irq_disable)(void *);   // 0x88 offset
    void *irq_priv;               // 0x80 offset
};

// Event callback type
typedef int (*isp_event_cb)(void *priv, u32 event, void *data);

// Channel event state
// Event callback structure
struct isp_event_callbacks {
    void (*frame_cb)(void *priv);     // Frame event callback
    void (*stats_cb)(void *priv);     // Statistics callback
    void *priv;                       // Private data for callbacks
};

// Channel event state
struct isp_channel_event {
    struct isp_channel *channel;      // Back pointer to parent channel
    isp_event_cb event_handler;       // Main event handler function
    void *priv;                       // Private data for event handler
    spinlock_t lock;                  // Protects event state

    // Callback registration
    struct isp_event_callbacks cb;    // Registered callbacks

    // Event state tracking
    unsigned long flags;              // Event state flags
    atomic_t pending_events;          // Count of pending events

    // Statistics/debug
    unsigned int event_count;         // Total events processed
    unsigned int error_count;         // Error counter
};

// Event codes from decompiled libimp
#define ISP_EVENT_STREAM_START    0x3000003  // Start streaming
#define ISP_EVENT_STREAM_CANCEL   0x3000004  // Cancel streaming
#define ISP_EVENT_BUFFER_ENQUEUE  0x3000005  // Buffer enqueued to driver
#define ISP_EVENT_BUFFER_DONE     0x3000006  // Buffer completed by hardware
#define ISP_EVENT_QUEUE_FREE      0x3000007  // Free/cleanup queue
#define ISP_EVENT_BUFFER_REQUEST  0x3000008  // Request buffer allocation

#define ISP_NETLINK_PROTO    23       // 0x17 from decompilation
#define ISP_NETLINK_GROUP    0xca624  // From OEM driver

/* Add memory type if not defined */
#ifndef V4L2_MEMORY_KMALLOC
#define V4L2_MEMORY_KMALLOC  0x8000
#endif

// From OEM driver, netlink protocol setup
#define ISP_NETLINK_PROTO 0x17
#define ISP_NETLINK_GROUP 0xca624

/* Frame States - must match libimp */
#define FRAME_STATE_FREE     0  // Initial state
#define FRAME_STATE_QUEUED   1  // In ready queue
#define FRAME_STATE_ACTIVE   2  // Being processed
#define FRAME_STATE_DONE     3  // In done queue
#define FRAME_STATE_ERROR    4  // Error state

/* ISP Event types */
#define ISP_EVENT_BUFFER_REQUEST  0x1000
#define ISP_EVENT_BUFFER_ENQUEUE  0x1001
#define ISP_EVENT_BUFFER_DONE     0x1002
#define ISP_EVENT_FRAME_DONE      0x1003
#define ISP_EVENT_STATS_READY     0x1004
#define ISP_EVENT_STREAM_START    0x1005
#define ISP_EVENT_STREAM_CANCEL   0x1006
#define ISP_EVENT_QUEUE_FREE      0x1007

/* Event handler function type */
typedef int (*isp_event_cb)(void *priv, u32 event, void *data);

/* VBM frame states (offset 0x3e6) */
#define VBM_STATE_FREE    0x0000  // Free for use
#define VBM_STATE_QUEUED  0x0001  // Queued to hardware
#define VBM_STATE_ACTIVE  0x0002  // Being processed
#define VBM_STATE_DONE    0x0003  // Processing complete

// Channel States
#define CH_STATE_INIT      0   // Initial state
#define CH_STATE_READY     1   // Resources allocated
#define CH_STATE_STREAMING 2   // Actively streaming

// Frame queue state flags
#define QUEUE_STATE_STREAMING BIT(0)  // Streaming active
#define QUEUE_STATE_ERROR     BIT(1)  // Error occurred



// Structure expected by libimp
struct isp_sensor_id_info {
    __u32 id;          /* Sensor ID value */
    __u32 status;      /* Status of read operation */
};
#endif //TX_LIBIMP_H
