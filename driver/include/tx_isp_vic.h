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


// VIC device structure - simplified without dangerous offset arithmetic
// Since we're fixing unsafe memory access patterns to use proper struct members,
// we don't need complex padding calculations
struct tx_isp_vic_device {
    // Base subdev structure (expected by existing code)
    struct tx_isp_subdev sd;
    
    // VIC hardware registers
    void __iomem *vic_regs;
    
    // VIC device properties
    struct tx_isp_vic_device *self;     // Self-pointer for reference driver compatibility
    uint32_t width;                     // Frame width
    uint32_t height;                    // Frame height
    uint32_t pixel_format;              // Pixel format
    struct tx_isp_sensor_attribute sensor_attr; // Sensor attributes
    
    // Device state and synchronization
    int state;                          // State: 1=init, 2=ready, 3=active, 4=streaming
    spinlock_t lock;                    // General spinlock
    struct completion frame_complete;   // Frame completion
    struct mutex mlock;                 // Main mutex
    struct mutex state_lock;            // State mutex
    
    // Buffer management (using safe struct members instead of offset arithmetic)
    spinlock_t buffer_mgmt_lock;        // Buffer management spinlock (was at offset 0x1f4)
    int stream_state;                   // Stream state: 0=off, 1=on (was at offset 0x210)
    uint32_t active_buffer_count;       // Active buffer count (was at offset 0x218)
    uint32_t buffer_count;              // General buffer count
    bool processing;                    // Processing flag
    int streaming;                      // Streaming state
    
    // Error tracking
    uint32_t vic_errors[13];            // Error array (13 elements)
    uint32_t total_errors;              // Total error count
    uint32_t frame_count;               // Frame counter
    
    // Buffer management structures
    spinlock_t buffer_lock;             // Buffer lock
    struct list_head queue_head;        // Buffer queues
    struct list_head free_head;
    struct list_head done_head;
};

#endif /* __TX_ISP_VIC_H__ */
