#ifndef __TX_ISP_VIC_H__
#define __TX_ISP_VIC_H__

#include <linux/errno.h>
#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/completion.h>

/* Forward declarations */
struct tx_isp_subdev;
struct tx_isp_config;
struct tx_isp_sensor_attribute;

/* VIC Constants */
#define VIC_MAX_CHAN        2

/* VIC Register Definitions */
#define VIC_CTRL            0x00
#define VIC_STATUS          0x04
#define VIC_BUFFER_ADDR     0x08
#define VIC_FRAME_SIZE      0x0C
#define VIC_INT_STATUS      0x10
#define VIC_INT_MASK        0x14

/* VIC Control Register Bits */
#define VIC_CTRL_EN         BIT(0)
#define VIC_CTRL_START      BIT(1)
#define VIC_CTRL_STOP       BIT(2)
#define VIC_CTRL_RST        BIT(3)

/* VIC Status Register Bits */
#define STATUS_BUSY         BIT(0)
#define STATUS_IDLE         BIT(1)

/* VIC Interrupt Bits */
#define INT_FRAME_DONE      BIT(0)
#define INT_ERROR           BIT(1)

extern uint32_t vic_start_ok;  /* Global VIC interrupt enable flag declaration */

/* VIC Functions */
int tx_isp_vic_probe(struct platform_device *pdev);
int tx_isp_vic_remove(struct platform_device *pdev);

/* VIC Operations - Use existing vic_device from tx_isp.h */
int tx_isp_vic_stop(struct tx_isp_subdev *sd);
int tx_isp_vic_set_buffer(struct tx_isp_subdev *sd, dma_addr_t addr, u32 size);
int tx_isp_vic_wait_frame_done(struct tx_isp_subdev *sd, int channel, int timeout_ms);
int tx_isp_vic_activate_subdev(struct tx_isp_subdev *sd);
int tx_isp_vic_slake_subdev(struct tx_isp_subdev *sd);

/* Missing function declaration that caused implicit declaration error */
int tx_isp_subdev_pipo(struct tx_isp_subdev *sd, void *arg);

/* VIC File Operations */
int vic_chardev_open(struct inode *inode, struct file *file);
int vic_chardev_release(struct inode *inode, struct file *file);
long vic_chardev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

/* VIC Proc Operations - from spec driver */
int isp_vic_frd_show(struct seq_file *seq, void *v);
int dump_isp_vic_frd_open(struct inode *inode, struct file *file);
int vic_event_handler(void *subdev, int event_type, void *data);

// Forward declarations for initialization functions
void isp_core_tuning_deinit(void *core_dev);
int sensor_early_init(void *core_dev);

/* VIC States */
#define VIC_STATE_OFF       0
#define VIC_STATE_IDLE     1
#define VIC_STATE_ACTIVE   2
#define VIC_STATE_ERROR    3

/* VIC Error Flags */
#define VIC_ERR_CONFIG     BIT(0)
#define VIC_ERR_TIMEOUT    BIT(1)
#define VIC_ERR_OVERFLOW   BIT(2)
#define VIC_ERR_HARDWARE   BIT(3)

/* VIC Operation Structures - exported from implementation */
extern struct tx_isp_subdev_ops vic_subdev_ops;


/* CRITICAL FIX: VIC device structure with proper MIPS alignment and Binary Ninja compatibility */
struct tx_isp_vic_device {
    /* CRITICAL: Base subdev structure MUST be first for container_of() to work */
    struct tx_isp_subdev sd;                    /* 0x00: Base subdev structure */

    
    /* CRITICAL: VIC register base at offset 0xb8 (Binary Ninja expects this) */
    void __iomem *vic_regs;                     /* 0xb8: VIC register base */

    /* CRITICAL: Frame dimensions at expected offsets */
    uint32_t width;                             /* 0xdc: Frame width (Binary Ninja expects this) */
    uint32_t height;                            /* 0xe0: Frame height (Binary Ninja expects this) */
    
    /* Device properties (properly aligned) */
    u32 stride;                                 /* Line stride */
    uint32_t pixel_format;                      /* Pixel format */
    
    /* CRITICAL: Sensor attributes with proper alignment */
    struct tx_isp_sensor_attribute sensor_attr __attribute__((aligned(4))); /* Sensor attributes */
    
    /* CRITICAL: Synchronization primitives with proper alignment */
    spinlock_t lock __attribute__((aligned(4)));                    /* General spinlock */
    struct mutex mlock __attribute__((aligned(4)));                 /* Main mutex */
    struct mutex state_lock __attribute__((aligned(4)));            /* State mutex */
    struct completion frame_complete __attribute__((aligned(4)));   /* Frame completion */
    
    /* CRITICAL: Device state (4-byte aligned) */
    int state __attribute__((aligned(4)));                          /* State: 1=init, 2=ready, 3=active, 4=streaming */
    int streaming __attribute__((aligned(4)));                      /* Streaming state */
    
    /* CRITICAL: Buffer management with proper alignment and expected offsets */
    /* These need to be at specific offsets for Binary Ninja compatibility */
    spinlock_t buffer_mgmt_lock;                /* 0x1f4: Buffer management spinlock */
    
    int stream_state;                           /* 0x210: Stream state: 0=off, 1=on */
    
    bool processing;                            /* 0x214: Processing flag */
    
    uint32_t active_buffer_count;               /* 0x218: Active buffer count */
    
    /* Additional buffer management */
    uint32_t buffer_count;                      /* General buffer count */
    
    /* Error tracking (properly aligned) */
    uint32_t vic_errors[13] __attribute__((aligned(4)));            /* Error array (13 elements) */
    uint32_t total_errors __attribute__((aligned(4)));              /* Total error count */
    uint32_t frame_count __attribute__((aligned(4)));               /* Frame counter */
    
    /* Buffer management structures (properly aligned) */
    spinlock_t buffer_lock __attribute__((aligned(4)));             /* Buffer lock */
    struct list_head queue_head __attribute__((aligned(4)));        /* Buffer queues */
    struct list_head free_head __attribute__((aligned(4)));
    struct list_head done_head __attribute__((aligned(4)));
    
    /* Buffer index array for VIC register mapping */
    int buffer_index[5] __attribute__((aligned(4)));                /* Buffer index array (5 buffers max) */

    /* IRQ handling members */
    void (*irq_handler)(void *) __attribute__((aligned(4)));
    void (*irq_disable)(void *) __attribute__((aligned(4)));
    void *irq_priv __attribute__((aligned(4)));
    int irq_enabled __attribute__((aligned(4)));
    int irq_number __attribute__((aligned(4)));                     /* IRQ number from platform device */
    irq_handler_t irq_handler_func __attribute__((aligned(4)));     /* IRQ handler function pointer */
} __attribute__((aligned(4), packed));


//
///* VIC device struct from tx_isp.h - corrected version */
//struct vic_device {
//    void __iomem *regs;         // Base registers
//    void __iomem *vic_regs;     // VIC register base (for Binary Ninja compatibility)
//    struct tx_isp_subdev *sd;
//    spinlock_t lock;            // IRQ lock
//    struct mutex state_lock;    // Now should be 32-bit aligned
//
//    // State tracking
//    int state;                  // Track states: 1=INIT, 2=READY, etc
//    int streaming;              // Streaming state flag (for Binary Ninja compatibility)
//    u32 buffer_count;           // Number of buffers (for Binary Ninja compatibility)
//    u32 mdma_en;               // Group 32-bit values together
//    u32 ch0_buf_idx;
//    u32 ch0_sub_get_num;
//    struct completion frame_complete;
//
//    // Status flags (keep these together)
//    bool started;
//    bool processing;
//    u16 pad2;              // Pad to ensure 32-bit alignment
//
//    // Rest unchanged...
//
//
//    /* Missing members that caused compilation errors */
//    struct tx_isp_sensor_attribute sensor_attr;
//    uint32_t total_errors;
//    uint32_t vic_errors[13];  /* 13 error counters */
//    uint32_t frame_count;     /* Frame counter */
//} __attribute__((aligned(4)));  // Force overall structure alignment

#endif /* __TX_ISP_VIC_H__ */
