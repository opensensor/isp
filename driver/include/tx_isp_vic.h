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
long isp_vic_cmd_set(struct file *file, unsigned int cmd, unsigned long arg);
int vic_event_handler(void *subdev, int event_type, void *data);

// Forward declarations for initialization functions
void isp_core_tuning_deinit(void *core_dev);
int sensor_early_init(struct tx_isp_dev *isp);

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


/* VIC remote event handler structures - CRITICAL NULL POINTER CRASH FIX */
/* These structures fix the tx_isp_send_event_to_remote null pointer crash */

/* Event handler callback structure with function pointer at offset 0x1c */
struct vic_event_handler {
    char padding[28];                   // Padding to reach offset 0x1c  
    void *event_callback;               // Function pointer at offset 0x1c
} __attribute__((packed));

/* Remote handler structure accessed through frame channel's remote_dev */
struct vic_remote_handler {
    char padding[12];                   // Padding to reach offset 0xc
    struct vic_event_handler *event_handler_struct; // Pointer at offset 0xc
} __attribute__((packed));

// VIC device structure - CRITICAL MEMORY CORRUPTION FIX with proper MIPS alignment
// MIPS architecture requires 4-byte alignment for 32-bit values and 8-byte for 64-bit
// Structure packing ensures Binary Ninja offset compatibility while preventing corruption
struct tx_isp_vic_device {
    // Base subdev structure (expected by existing code)
    struct tx_isp_subdev sd;
    
    // VIC hardware registers - CRITICAL: Must be properly aligned
    void __iomem *vic_regs;             // 64-bit pointer on 64-bit systems
    
    // VIC device properties - Group 32-bit values for alignment
    uint32_t width;                     // Frame width
    uint32_t height;                    // Frame height
    uint32_t pixel_format;              // Pixel format
    uint32_t padding_align1;            // Padding to ensure sensor_attr alignment
    
    // CRITICAL: Sensor attributes - MUST be properly aligned for MIPS
    struct tx_isp_sensor_attribute sensor_attr __attribute__((aligned(8))); // Force 8-byte alignment
    
    // Device state and synchronization - Group related fields
    int state;                          // State: 1=init, 2=ready, 3=active, 4=streaming
    int streaming;                      // Streaming state
    bool processing;                    // Processing flag
    char padding_align2[3];             // Pad to 4-byte boundary
    
    // Synchronization primitives - These contain internal alignment requirements
    spinlock_t lock;                    // General spinlock
    struct completion frame_complete;   // Frame completion
    struct mutex mlock;                 // Main mutex
    struct mutex state_lock;            // State mutex
    wait_queue_head_t wait_queue;       // Wait queue for interrupt-driven process wakeup
    
    // Buffer management - CRITICAL: These offsets were causing corruption
    spinlock_t buffer_mgmt_lock;        // Buffer management spinlock (was at offset 0x1f4)
    int stream_state;                   // Stream state: 0=off, 1=on (was at offset 0x210)
    uint32_t active_buffer_count;       // Active buffer count (was at offset 0x218)
    uint32_t buffer_count;              // General buffer count
    
    // Error tracking - Group 32-bit values together
    uint32_t vic_errors[13];            // Error array (13 elements)
    uint32_t total_errors;              // Total error count
    uint32_t frame_count;               // Frame counter
    
    // Buffer management structures - List heads have internal alignment
    spinlock_t buffer_lock;             // Buffer lock
    struct list_head queue_head;        // Buffer queues
    struct list_head free_head;
    struct list_head done_head;
    
    // Buffer index array for VIC register mapping
    int buffer_index[5];                // Buffer index array (5 buffers max)
    char padding_align3[3];             // Pad to 4-byte boundary after 5 ints
    
    // IRQ handling members
    int irq_number;                     // IRQ number from platform device
    irq_handler_t irq_handler_func;     // IRQ handler function pointer
    
    // CRITICAL NULL POINTER CRASH FIX: Remote event handler for frame channels
    struct vic_remote_handler *remote_handler; // Event handler chain for QBUF calls
} __attribute__((packed, aligned(8)));  // Force structure packing with 8-byte alignment

#endif /* __TX_ISP_VIC_H__ */
