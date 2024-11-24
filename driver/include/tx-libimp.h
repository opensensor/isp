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
    uint32_t magic;             // 0x00: Magic number (0x100100)
    void *data;                 // 0x04: Buffer data pointer
    uint32_t frame_size;        // 0x08: Frame data size
    uint32_t seq;              // 0x0C: Frame sequence number
    struct list_head list;      // 0x10: List management
    uint32_t flags;            // 0x20: Frame flags
    uint8_t state;             // 0x24: Frame state
    uint32_t index;            // 0x28: Buffer index in array
    // Additional fields seen in decompiled:
    uint32_t magic_tail;       // 0x2C: Magic tail number (0x200200)
    void *virt_addr;           // 0x30: Virtual address
    dma_addr_t phys_addr;      // 0x34: Physical address
    // For USERPTR memory management
    struct page **pages;
    int nr_pages;
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
    u8 reserved1;           // 0x00
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
    u32 instance;               // First word is instance/arg1
    u8 params[0x4000];         // Parameter space
    spinlock_t lock;           // Spin lock at 0x102e
    struct mutex mlock;        // Mutex also at 0x102e
    u32 state;                 // State at 0x1031 (1 = init, 2 = enabled)
    u32 event_handler;         // Event handler at 0x1033
    u32 param_size;            // Size field at end (0x736b0 seen in init)
};

// From the decompiled code, we see a struct of 4-word entries being copied in a loop
// up to offset 0x70 (112 bytes), suggesting this structure size
struct imp_channel_attr {
    __u32 attr[28];  // 28 words = 112 bytes (0x70)
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

// AF Zone data structure (from apical_isp_af_zone_g_ctrl.isra.85)
struct af_zone_data {
    u32 zone_metrics[MAX_AF_ZONES];  // 900 bytes (0x384) copied to user
};

struct isp_tuning_ctrl {
    __u32 cmd;     // Command ID
    __u32 value;   // Value/pointer
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

#endif //TX_LIBIMP_H
