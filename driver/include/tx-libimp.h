#ifndef TX_LIBIMP_H
#define TX_LIBIMP_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>



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
    u8 dpc;               // 0x11 - Using first byte of reserved4
    u8 antiflicker;      // 0x1c
};


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

// Buffer structure - total size 0xd0 bytes
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
    char update_block[512];  // Update data block
} __attribute__((packed, aligned(4)));

/* Add memory type if not defined */
#ifndef V4L2_MEMORY_KMALLOC
#define V4L2_MEMORY_KMALLOC  0x8000
#endif

#endif //TX_LIBIMP_H
