#ifndef _TX_ISP_HW_H_
#define _TX_ISP_HW_H_
#include <linux/ioport.h>

#include "tx-isp.h"

/* ISP Register definitions */
#define ISP_REG_SIZE        0x10000

#define ISP_CTRL_REG        0x0000
#define ISP_STATUS_REG      0x0004
#define ISP_INT_MASK_REG    0x0008
#define ISP_INT_CLEAR_REG   0x000C

#define ISP_STREAM_CTRL     0x0010
#define ISP_STREAM_START    0x0014
#define ISP_FRAME_SIZE      0x0018

#define ISP_BUF0_OFFSET     0x0020
#define ISP_BUF1_OFFSET     0x0028
#define ISP_BUF2_OFFSET     0x0030

#define ISP_DMA_CTRL        0x0040
#define ISP_FRAME_CTRL      0x0044

/* VIC Register offsets */
#define VIC_BASE            0x0300
#define VIC_CTRL            0x0000
#define VIC_STATUS          0x0004
#define VIC_IRQ_EN          0x0008
#define VIC_MASK            0x000C
#define VIC_DMA_CTRL        0x0010
#define VIC_FRAME_CTRL      0x0014


// Our internal pad descriptor structure
struct isp_pad_config {
    uint32_t subdev_id;  // ID to identify which subdev this pad belongs to
    uint32_t pad_id;     // Which pad within the subdev
    uint32_t flags;      // Pad flags/capabilities
    uint32_t reserved;   // Padding/alignment
    uint32_t link_flags; // The flags that determine link compatibility
};

// Our internal state enums
#define ISP_PAD_STATE_FREE     0
#define ISP_PAD_STATE_LINKED   3
#define ISP_PAD_STATE_ACTIVE   4

#define ISP_LINK_STATE_INACTIVE 0
#define ISP_LINK_STATE_ACTIVE   1

#define MAX_LINKS 8

// Likely flags/types used in configurations
#define ISP_PAD_LINK_TYPE_VIDEO   0x01
#define ISP_PAD_LINK_TYPE_META    0x02
#define ISP_PAD_LINK_TYPE_MEMORY  0x04

// Number of links in each configuration
static const uint32_t pad_link_counts[2] = {
    [0] = 3,  // Basic configuration with 3 links
    [1] = 4,  // Extended configuration with 4 links
};

// Detailed pad configurations
static const struct isp_pad_config pad_link_configs[2][MAX_LINKS] = {
    [0] = {  // Basic configuration
        // Link 1: Sensor -> ISP Input
        {
            .subdev_id = 0,    // Sensor subdev ID
            .pad_id = 0,       // Output pad ID
            .flags = ISP_PAD_LINK_TYPE_VIDEO,
            .reserved = 0,
            .link_flags = ISP_PAD_LINK_TYPE_VIDEO
        },
        // Link 2: ISP Core Processing
        {
            .subdev_id = 1,    // ISP core subdev ID
            .pad_id = 0,       // Input pad ID
            .flags = ISP_PAD_LINK_TYPE_VIDEO,
            .reserved = 0,
            .link_flags = ISP_PAD_LINK_TYPE_VIDEO
        },
        // Link 3: Memory Output
        {
            .subdev_id = 2,    // Memory interface subdev ID
            .pad_id = 0,       // Output pad ID
            .flags = ISP_PAD_LINK_TYPE_MEMORY,
            .reserved = 0,
            .link_flags = ISP_PAD_LINK_TYPE_MEMORY
        },
    },
    [1] = {  // Extended configuration (e.g., with metadata)
        // Link 1: Sensor -> ISP Input
        {
            .subdev_id = 0,
            .pad_id = 0,
            .flags = ISP_PAD_LINK_TYPE_VIDEO,
            .reserved = 0,
            .link_flags = ISP_PAD_LINK_TYPE_VIDEO
        },
        // Link 2: ISP Core Processing
        {
            .subdev_id = 1,
            .pad_id = 0,
            .flags = ISP_PAD_LINK_TYPE_VIDEO,
            .reserved = 0,
            .link_flags = ISP_PAD_LINK_TYPE_VIDEO
        },
        // Link 3: Memory Output
        {
            .subdev_id = 2,
            .pad_id = 0,
            .flags = ISP_PAD_LINK_TYPE_MEMORY,
            .reserved = 0,
            .link_flags = ISP_PAD_LINK_TYPE_MEMORY
        },
        // Link 4: Metadata Path
        {
            .subdev_id = 3,
            .pad_id = 0,
            .flags = ISP_PAD_LINK_TYPE_META,
            .reserved = 0,
            .link_flags = ISP_PAD_LINK_TYPE_META
        },
    }
};

int tx_isp_enable_irq(struct IMPISPDev *dev);
void tx_isp_disable_irq(struct IMPISPDev *dev);

int init_hw_resources(struct IMPISPDev *dev);
int tx_isp_init_memory(struct IMPISPDev *dev);
void tx_isp_cleanup_memory(struct IMPISPDev *dev);

int configure_isp_clocks(struct IMPISPDev *dev);
int init_vic_control(struct IMPISPDev *dev);

int configure_streaming_hardware(struct IMPISPDev *dev);
int configure_isp_buffers(struct IMPISPDev *dev);
int init_vic_control(struct IMPISPDev *dev);
void verify_isp_state(struct IMPISPDev *dev);
int isp_power_on(struct IMPISPDev *dev);
int isp_reset_hw(struct IMPISPDev *dev);
int reset_vic(struct IMPISPDev *dev);

u32 isp_reg_read(void __iomem *base, u32 offset);
void isp_reg_write(void __iomem *base, u32 offset, u32 value);

#endif /* _TX_ISP_HW_H_ */