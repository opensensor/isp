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

#define MAX_TASKS 8  // Adjust based on OEM code
#define ISP_PAD_SINK    0
#define ISP_PAD_SOURCE  1



struct tx_isp_pad {
    struct list_head list;
    u32 type;           // 0=sink, 1=source
    u32 index;
    struct tx_isp_channel *channel;
};

struct tx_isp_channel {
    u32 width;
    u32 height;
    u32 stride;
    u32 size;
    u32 meta_size;
    void *meta_addr;
};


struct irq_task {
    void (*task_function)(void);  // Task callback function
    int status;                   // Task status/enable flag
};

struct irq_handler_data {
    spinlock_t lock;
    int irq_number;
    void (*handler_function)(void *);    // Enable function
    void (*disable_function)(void *);    // Disable function
    void *irq_priv;
    struct irq_task task_list[MAX_TASKS];  // Array of task handlers
    int task_count;                        // Number of registered tasks
};


// Link module types
enum imp_isp_mod_type {
    IMP_ISP_MOD_CSI = 0,
    IMP_ISP_MOD_VIC,
    IMP_ISP_MOD_VIN,
    IMP_ISP_MOD_CORE,
    IMP_ISP_MOD_FS,
    IMP_ISP_MOD_DDR
};

// Link pad definition
struct imp_isp_pad {
    uint32_t type;      // 0 = sink/input, 1 = source/output
    uint32_t mod;       // Module type (CSI/VIC/DDR)
    uint32_t pad_id;    // Pad index
};

// Full link structure
struct imp_isp_link {
    struct imp_isp_pad src;  // Source pad
    struct imp_isp_pad dst;  // Destination pad
};


struct frame_source_device {
    struct tx_isp_subdev *sd;
    void *priv;
    struct mutex lock;
    int state;
};

struct vic_device {
    void __iomem *regs;         // Base registers
    struct tx_isp_subdev *sd;
    spinlock_t lock;            // IRQ lock
    int irq_enabled;            // IRQ enable state
    void (*irq_handler)(void *);  // IRQ handler function
    void (*irq_disable)(void *);  // IRQ disable function
    void *irq_priv;             // Private data for IRQ

    // Buffer management
    u32 mdma_en;               // DMA enable flag
    u32 ch0_buf_idx;           // Channel 0 buffer index
    u32 ch0_sub_get_num;       // Channel 0 buffer count
    struct completion frame_complete;  // Frame completion

    // Additional state
    u32 width;                 // Frame width
    u32 height;                // Frame height
    u32 stride;                // Frame stride
};

struct vin_device {
    struct tx_isp_subdev *sd;
    struct mutex lock;            // Keep mutex
    struct list_head sensors;     // List of sensors
    struct tx_isp_sensor *active; // Currently active sensor
    int refcnt;                  // Reference count
    int state;                   // State flag
};

//int tx_isp_enable_irq(struct IMPISPDev *dev);
//void tx_isp_disable_irq(struct IMPISPDev *dev);

int init_hw_resources(struct IMPISPDev *dev);
void tx_isp_cleanup_memory(struct IMPISPDev *dev);
void dump_isp_regs(void);
int configure_isp_clocks(struct IMPISPDev *dev);
int configure_vic_for_streaming(struct IMPISPDev *dev);
int init_vic_control(struct IMPISPDev *dev);
int detect_sensor_type(struct IMPISPDev *dev);
int tx_vic_irq_init(struct irq_dev *dev,
                    void (*handler)(void *),
                    void (*disable)(void *),
                    void *priv);
int tx_isp_irq_init(struct irq_dev *dev,
                    void (*handler)(void *),
                    void (*disable)(void *),
                    void *priv);
int configure_streaming_hardware(struct IMPISPDev *dev);
int configure_isp_buffers(struct IMPISPDev *dev);
void verify_isp_state(struct IMPISPDev *dev);
int isp_power_on(struct IMPISPDev *dev);
int isp_reset_hw(struct IMPISPDev *dev);
int reset_vic(struct IMPISPDev *dev);
void dump_vic_state(struct IMPISPDev *dev);

struct tx_isp_subdev_pad *find_pad(struct IMPISPDev *dev,
                                         enum imp_isp_mod_type mod,
                                         u32 type,  // 0=sink, 1=source
                                         u32 pad_id);


// Forward declarations for VIC functions
int vic_s_stream(struct tx_isp_subdev *sd, int enable);
int vic_link_stream(struct tx_isp_subdev *sd, int enable);
int vic_link_setup(const struct tx_isp_subdev_pad *local,
                         const struct tx_isp_subdev_pad *remote, u32 flags);
int vic_init(struct tx_isp_subdev *sd, int on);
int vic_reset(struct tx_isp_subdev *sd, int on);
irqreturn_t vic_isr(struct tx_isp_subdev *sd, u32 status, bool *handled);
irqreturn_t vic_isr_thread(struct tx_isp_subdev *sd, void *data);

// Forward declarations for CSI functions
int csi_s_stream(struct tx_isp_subdev *sd, int enable);
int csi_link_stream(struct tx_isp_subdev *sd, int enable);
int csi_init(struct tx_isp_subdev *sd, int on);
int csi_reset(struct tx_isp_subdev *sd, int on);


u32 isp_reg_read(void __iomem *base, u32 offset);
void isp_reg_write(void __iomem *base, u32 offset, u32 value);

#endif /* _TX_ISP_HW_H_ */