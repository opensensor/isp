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


#define VIC_IRQ_INDEX 0xd


#define VIC_STATE_IDLE       1
#define VIC_STATE_CONFIGURED 2
#define VIC_STATE_INITIALIZED 3
#define VIC_STATE_STREAMING  4

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

// VIC register offsets
#define VIC_IRQ_ENABLE     0x93c   // IRQ Enable register - set to 0x000033fb in OEM code
#define VIC_IRQ_MASK       0x9e8   // IRQ Mask register - set to 0 to allow interrupts
#define VIC_IRQ_STATUS     0xb4    // IRQ Status register - read-only
#define VIC_IRQ_STATUS2    0xb8    // IRQ Status register 2 - read-only

// VIC IRQ enable bits from OEM - 0x000033fb value breakdown
#define VIC_IRQ_FRAME_START   BIT(0)  // Frame start
#define VIC_IRQ_FRAME_DONE    BIT(1)  // Frame complete
#define VIC_IRQ_DMA_Y_DONE    BIT(3)  // Y plane DMA complete
#define VIC_IRQ_DMA_UV_DONE   BIT(4)  // UV plane DMA complete
#define VIC_IRQ_BUF_FULL      BIT(5)  // Buffer full
#define VIC_IRQ_FIFO_FULL     BIT(8)  // FIFO full
#define VIC_IRQ_FRAME_DROP    BIT(9)  // Frame dropped
#define VIC_IRQ_DATA_TIMEOUT  BIT(10) // Data timeout


#define T31_INTC_BASE     0x10001000
#define INTC_ISR          0x00   // Interrupt Status
#define INTC_IMR          0x04   // Interrupt Mask
#define INTC_IMSR         0x08   // Interrupt Mask Set
#define INTC_IMCR         0x0c   // Interrupt Mask Clear
#define INTC_IPR          0x10   // Interrupt Pending

// Interrupt numbers from your logs
#define T31_IRQ_I2C0      68    // I2C interrupt number
#define T31_IRQ_ISP       37    // ISP interrupt
#define T31_IRQ_VIC       38    // VIC interrupt

// Combined IRQ mask used by OEM
#define VIC_IRQ_ENABLE_MASK   0x000033fb

/* ISP Register Offsets */
#define ISP_BASE            0x13300000
#define ISP_INT_STATUS      0xb4    // Interrupt Status Register
#define ISP_INT_MASK       0xb0    // Interrupt Mask Register
#define ISP_INT_CLEAR      0xb8    // Interrupt Clear Register

/* Interrupt Mask Bits */
#define ISP_INT_I2C        BIT(8)  // I2C interrupt bit
#define ISP_INT_VIC        BIT(9)  // VIC interrupt bit
#define ISP_INT_FRAME_DONE BIT(0)  // Frame completion interrupt

/* T31 INTC Register definitions */
#define T31_INTC_BASE     0x10001000
#define INTC_ISR_OFF      0x00
#define INTC_IMR_OFF      0x04
#define INTC_IMCR_OFF     0x0c

/* ISP Register offsets */
#define ISP_CTRL_OFF      0x00
#define ISP_INT_STAT_OFF  0xb4
#define ISP_INT_MASK_OFF  0xb0
#define ISP_INT_CLR_OFF   0xb8

// Additional register definitions for second interrupt bank
#define INTC_ISR2         0x20   // Second bank Status Register
#define INTC_IMR2         0x24   // Second bank Mask Register
#define INTC_IMSR2        0x28   // Second bank Mask Set Register
#define INTC_IMCR2        0x2c   // Second bank Mask Clear Register
#define INTC_IPR2         0x30   // Second bank Pending Register

// Helper macros to determine register bank and bit position
#define IRQ_BANK(irq)     ((irq) / 32)
#define IRQ_BIT(irq)      ((irq) % 32)
#define IRQ_MASK(irq)     BIT(IRQ_BIT(irq))

// VIC Register definitions
#define VIC_IRQ_ENABLE     0x93c    // VIC interrupt enable register
#define VIC_IRQ_STATUS     0xb4     // VIC interrupt status register
#define VIC_IRQ_STATUS2    0xb8     // VIC interrupt status register 2

// ISP Register definitions
#define ISP_INT_MASK       0xb0     // ISP interrupt mask register
#define ISP_INT_STATUS     0xb4     // ISP interrupt status register
#define ISP_INT_CLEAR      0xb8     // ISP interrupt clear register

// Interrupt bits
#define ISP_INT_FRAME_DONE BIT(0)   // Frame completion bit
#define VIC_INT_FRAME_DONE BIT(1)   // VIC frame completion bit

#define VIC_IRQ_CLEAR     0x940    // VIC interrupt clear register
#define VIC_FRAME_DONE    BIT(1)   // VIC frame completion bit

#define PZ_BASE       0x10017000
#define PZINTS        (PZ_BASE + 0x14)
#define PZINTC        (PZ_BASE + 0x18)
#define PZMSKS        (PZ_BASE + 0x24)
#define PZMSKC        (PZ_BASE + 0x28)
#define PZPAT1S       (PZ_BASE + 0x34)
#define PZPAT1C       (PZ_BASE + 0x38)
#define PZPAT0S       (PZ_BASE + 0x44)
#define PZPAT0C       (PZ_BASE + 0x48)
#define PZGID2LD      (PZ_BASE + 0xF0)

/* GPIO Z Shadow Register Offsets */
#define PZINTS    0x14  // Interrupt Set
#define PZINTC    0x18  // Interrupt Clear
#define PZMSKS    0x24  // Mask Set
#define PZMSKC    0x28  // Mask Clear
#define PZPAT1S   0x34  // Pattern 1 Set
#define PZPAT1C   0x38  // Pattern 1 Clear
#define PZPAT0S   0x44  // Pattern 0 Set
#define PZPAT0C   0x48  // Pattern 0 Clear
#define PZFLGC    0x58  // Flag Clear
#define PZGID2LD  0xF0  // Group ID to Load

/* T31 Interrupt Controller Register Offsets */
#define ICMRn     0x04  // Interrupt Controller Mask Register
#define ICMSRn    0x08  // Interrupt Controller Mask Set Register
#define ICMCRn    0x0C  // Interrupt Controller Mode Configuration Register
#define IPMRn     0x10  // Interrupt Pending Mask Register
#define ISRn      0x14  // Interrupt Source Register

// ISP interrupt configuration
#define VIC_INT   (1 << 30)  // VIC interrupt bit positioned correctly for BE
#define ISP_INT   (1 << 31)  // ISP interrupt bit (assuming it's adjacent to VIC)

/* Add these constants at top */
#define VIC_INT_ENABLE   BIT(31)  // DMA frame completion IRQ enable
#define VIC_ROUTE_IRQ    BIT(1)   // Route interrupts to INTC

#define GPIO_PORT_A   0x10010000
#define GPIO_PORT_B   0x10011000
#define GPIO_PORT_C   0x10012000
#define GPIO_PORT_Z   0x10017000  // Shadow group

#define ISP_INT_CSI      BIT(3)    // CSI interrupt bit in ISP interrupt status register

// States observed in the OEM code:
#define TX_ISP_MODULE_INIT      1  // After initialization
#define TX_ISP_MODULE_READY     2  // Device ready but not streaming
#define TX_ISP_MODULE_STOPPING  3  // Stream stop requested
#define TX_ISP_MODULE_STREAMING 4  // Actively streaming

#define TX_ISP_MODULE_INIT      1  // After initialization
#define TX_ISP_MODULE_READY     2  // Device ready but not streaming
#define TX_ISP_MODULE_STOPPING  3  // Stream stop requested
#define TX_ISP_MODULE_STREAMING 4  // Actively streaming

#define SENSOR_TYPE_MIPI_OTHER  0x1
#define SENSOR_TYPE_MIPI_SONY   0x2
#define SENSOR_TYPE_BT656       0x3
#define SENSOR_TYPE_BT601       0x4

#define VIC_INT_BIT     (1 << 30)   // VIC interrupt is bit 30
#define ISP_M0_INT_BIT  (1 << 37)   // ISP M0 interr

#define VIC_INT_BIT (1 << 30)  // VIC is bit 30 in INTC
#define ISP_M0_BIT  (1 << 37)  // ISP M0 is bit 37

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
    struct mutex state_lock;    // Now should be 32-bit aligned

    // State tracking
    int state;                  // Track states: 1=INIT, 2=READY, etc
    u32 mdma_en;               // Group 32-bit values together
    u32 ch0_buf_idx;
    u32 ch0_sub_get_num;
    struct completion frame_complete;

    // Status flags (keep these together)
    bool started;
    bool processing;
    u16 pad2;              // Pad to ensure 32-bit alignment

    // Rest unchanged...
    u32 width;
    u32 height;
    u32 stride;

    void (*irq_handler)(void *);
    void (*irq_disable)(void *);
    void *irq_priv;
    int irq_enabled;
} __attribute__((aligned(4)));  // Force overall structure alignment

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
int tx_isp_csi_init(struct platform_device *pdev);

int init_hw_resources(struct IMPISPDev *dev);
void tx_isp_cleanup_memory(struct IMPISPDev *dev);
void dump_isp_regs(void);
int configure_isp_clocks(struct IMPISPDev *dev);
int configure_vic_for_streaming(struct IMPISPDev *dev);
int init_vic_control(struct IMPISPDev *dev);
int detect_sensor_type(struct IMPISPDev *dev);
//int tx_vic_irq_init(struct irq_dev *dev,
//                    void (*handler)(void *),
//                    void (*disable)(void *),
//                    void *priv);
int tx_isp_irq_init(struct IMPISPDev *dev,
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
int vic_core_s_stream(struct IMPISPDev *isp, int enable);

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
void tx_vic_disable_irq(struct IMPISPDev *dev);
void tx_vic_enable_irq(struct IMPISPDev *dev);
void dump_irq_info(void);

// Forward declarations for CSI functions
int csi_s_stream(struct tx_isp_subdev *sd, int enable);
int csi_link_stream(struct tx_isp_subdev *sd, int enable);
int csi_init(struct tx_isp_subdev *sd);
int csi_reset(struct tx_isp_subdev *sd);
// int complete_csi_init(struct IMPISPDev *dev);
// int setup_csi_power_domain(struct IMPISPDev *dev);
int csi_hw_init(struct csi_device *csi);
int csi_power_on(struct csi_device *csi);
int init_csi_phy(struct csi_device *csi_dev);
int tx_isp_csi_s_stream(struct csi_device *csi_dev, int enable);

u32 isp_reg_read(void __iomem *base, u32 offset);
void isp_reg_write(void __iomem *base, u32 offset, u32 value);

int configure_i2c_gpio(struct IMPISPDev *dev);

#endif /* _TX_ISP_HW_H_ */