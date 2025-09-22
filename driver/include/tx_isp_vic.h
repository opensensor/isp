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

/* VIC Constants - REMOVED: Use VIC_MAX_CHAN=6 from tx-isp-device.h to match Binary Ninja reference */

/* REMOVED: VIC Register Definitions - Use definitions from tx-isp-device.h instead */
/* The Binary Ninja reference shows VIC registers use larger offsets like 0x300, 0x380, etc. */
/* Small offsets like 0x00, 0x04 don't match the actual hardware usage */

/* REMOVED: VIC Control Register Bits - Use definitions from tx-isp-device.h instead */
/* The bit definitions in tx-isp-device.h match the Binary Ninja reference driver */

/* REMOVED: VIC Status and Interrupt Bits - Use definitions from tx-isp-device.h instead */
/* The bit definitions in tx-isp-device.h match the Binary Ninja reference driver */

extern uint32_t vic_start_ok;  /* Global VIC interrupt enable flag declaration */

/* VIC Functions */
int tx_isp_vic_probe(struct platform_device *pdev);
int tx_isp_vic_remove(struct platform_device *pdev);
int tx_isp_vic_register_interrupt(struct tx_isp_vic_device *vic_dev, struct platform_device *pdev);

/* VIC interrupt enable/disable functions (matching reference driver names) */
void tx_vic_disable_irq(struct tx_isp_vic_device *vic_dev);

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

    /* CRITICAL: VIC register bases - dual VIC architecture */
    void __iomem *vic_regs;                     /* 0xb8: Primary VIC register base */
    void __iomem *vic_regs_secondary;                     /* 0xb8: Primary VIC register base */

    /* CRITICAL: Frame dimensions at expected offsets */
    uint32_t width;                             /* 0xdc: Frame width (Binary Ninja expects this) */
    uint32_t height;                            /* 0xe0: Frame height (Binary Ninja expects this) */

    /* Binary Ninja compatibility members */
    void *self_ptr;                             /* Self-pointer for validation */
    uint32_t format_magic;                      /* NV12 format magic number (0x3231564e) */

    /* Event callback structure - separate from subdev fields to prevent conflicts */
    struct vic_event_callback *event_callback;  /* VIC event callback structure */
    
    /* Device properties (properly aligned) */
    u32 stride;                                 /* Line stride */
    uint32_t pixel_format;                      /* Pixel format */
    
    /* REMOVED: sensor_attr member - modern hardware supports multiple sensors
     * VIC should get sensor attributes from subdev array starting at index 4
     * via tx_isp_get_sensor() which now properly searches the subdev array */
    
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

    /* Buffer addresses for vic_mdma_enable and isp_vic_cmd_set */
    dma_addr_t *buffer_addresses;               /* Array of buffer addresses */
    int buffer_address_count;                   /* Number of buffer addresses */

    /* BINARY NINJA COMPATIBILITY: Additional members referenced in interrupt handler */
    void *frame_channels;                       /* 0x150: Frame channel array pointer */
    uint32_t frame_info;                        /* Frame information for channel processing */
    void *callback_handler;                     /* 0x1bc: Callback handler pointer */
    uint32_t day_night_state;                   /* 0x178: Day/night switching state */
    uint32_t first_frame_flag;                  /* First frame processing flag */
    uint32_t mbus_config;                       /* MBUS configuration */
    uint32_t bayer_config;                      /* 0xf4: Bayer configuration */

    /* Error tracking (properly aligned) */
    uint32_t vic_errors[13] __attribute__((aligned(4)));            /* Error array (13 elements) */
    uint32_t total_errors __attribute__((aligned(4)));              /* Total error count */
    uint32_t frame_count __attribute__((aligned(4)));               /* 0x160: Frame counter */
    
    /* Buffer management structures (properly aligned) */
    spinlock_t buffer_lock __attribute__((aligned(4)));             /* Buffer lock */
    struct list_head queue_head __attribute__((aligned(4)));        /* Buffer queues */
    struct list_head free_head __attribute__((aligned(4)));
    struct list_head done_head __attribute__((aligned(4)));
    
    /* Buffer index array for VIC register mapping */
    int buffer_index[5] __attribute__((aligned(4)));                /* Buffer index array (5 buffers max) */

    /* IRQ handling members */
    int irq __attribute__((aligned(4)));                            /* IRQ number - main IRQ member */
    void (*irq_handler)(void *) __attribute__((aligned(4)));
    void (*irq_disable)(void *) __attribute__((aligned(4)));
    void *irq_priv __attribute__((aligned(4)));
    int irq_enabled __attribute__((aligned(4)));
    int irq_number __attribute__((aligned(4)));                     /* IRQ number from platform device */
    irq_handler_t irq_handler_func __attribute__((aligned(4)));     /* IRQ handler function pointer */
    
    /* CRITICAL FIX: Hardware interrupt enable flag (replaces unsafe offset 0x13c access) */
    int hw_irq_enabled __attribute__((aligned(4)));                 /* Hardware interrupt enable flag - SAFE replacement for offset 0x13c */
} __attribute__((aligned(4), packed));

/* VIC function declarations */
/* REMOVED: tx_isp_vic_configure_dma - function doesn't exist in reference driver */
/* Use vic_pipo_mdma_enable instead, which is called during streaming */
int vic_mdma_enable(struct tx_isp_vic_device *vic_dev, int channel, int dual_channel,
                    int buffer_count, dma_addr_t base_addr, int format_type);

#endif /* __TX_ISP_VIC_H__ */
