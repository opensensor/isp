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
    // 0x00: Basic buffer info
    u32 index;          // Index in array
    u32 type;           // Buffer type
    u32 memory;         // Memory type (mmap)
    u32 flags;          // Status flags - track available/owned here

    // 0x10: Image format
    u32 width;
    u32 height;
    u32 format;
    u32 field;

    // 0x20: Timing
    struct timeval timestamp;
    u32 sequence;       // Track sequence here
    u32 memory_offset;  // Offset into DMA buffer

    // 0x30: Buffer info
    u32 length;         // Total buffer length
    u32 bytesused;      // Actual bytes used
    u32 input;          // Input number
    u32 reserved;

    // 0x40: Status
    u32 state;          // Internal state
    u32 flags2;         // Additional flags

    // 0x48: Frame rate
    u32 fps_num;        // Must be at 0x48
    u32 fps_den;        // Must be at 0x4C
} __attribute__((packed));


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



#endif //TX_LIBIMP_H
