#ifndef TX_LIBIMP_H
#define TX_LIBIMP_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>


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
    uint32_t magic;             // 0x0: Magic number identifier
    void *data;                 // 0x4: Buffer data pointer
    uint32_t frame_size;        // 0x8: Frame data size
    uint32_t seq;              // 0xc: Frame sequence number
    struct list_head list;      // 0x10: List management
    uint32_t flags;            // 0x20: Frame flags
    uint8_t state;             // 0x24: Frame state
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

struct frame_buffer {
    // Offset 0x00: Buffer management
    u32 index;          // Index in array
    u32 type;           // Buffer type
    u32 memory;         // Memory type (mmap)
    u32 flags;          // Status flags

    // Offset 0x10: Image format
    u32 width;
    u32 height;
    u32 format;
    u32 field;

    // Offset 0x20: Timing
    struct timeval timestamp;
    u32 sequence;
    u32 memory_offset;

    // Offset 0x30: Buffer info
    u32 length;         // Total length
    u32 bytesused;      // Bytes used
    u32 input;          // Input number
    u32 reserved;

    // Offset 0x40: Critical status
    u32 state;          // Internal state
    u32 flags2;         // More flags

    // Offset 0x48: Additional
    u32 fps_num;        // Must be at 0x48
    u32 fps_den;        // Must be at 0x4C
} __attribute__((packed));

struct frame_response {
    u32 buf_index;      // Offset 0x00: Buffer index
    u32 padding[7];     // Offset 0x04-0x1C: Padding
    u32 timestamp_l;    // Offset 0x20: Timestamp low
    u32 timestamp_h;    // Offset 0x24: Timestamp high
    u32 fps_num;        // Offset 0x28: FPS numerator
    u32 fps_den;        // Offset 0x2C: FPS denominator
    u32 padding2[172];  // Padding to offset 0x3E0
    u32 ref_count;      // Offset 0x3E0: Reference count
    u32 format;         // Offset 0x3E4: Format
    u8  padding3[2];    // Small padding
    u16 flags;          // Flags
    u32 size;           // Frame size
    u32 priv[4];       // Private data
} __attribute__((packed));

#endif //TX_LIBIMP_H
