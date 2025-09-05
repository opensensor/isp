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


// VIC device structure matching Binary Ninja reference (0x21c bytes total)
// Based on decompiled tx_isp_vic_start function offsets
struct tx_isp_vic_device {
    // Use a simplified approach with explicit offsets for critical members
    uint8_t _pad_to_b8[0xb8];           // Padding to offset 0xb8
    void __iomem *vic_regs;             // 0xb8: VIC hardware registers
    uint8_t _pad_to_d4[0xd4 - 0xb8 - 8]; // Padding to offset 0xd4
    struct tx_isp_vic_device *self;     // 0xd4: Self-pointer  
    uint8_t _pad_to_dc[0xdc - 0xd4 - 8]; // Padding to offset 0xdc
    uint32_t width;                     // 0xdc: Frame width
    uint32_t height;                    // 0xe0: Frame height
    uint32_t pixel_format;              // 0xe4: Pixel format
    uint8_t _pad_to_110[0x110 - 0xe4 - 4]; // Padding to offset 0x110
    struct tx_isp_sensor_attribute sensor_attr; // 0x110: Sensor attributes
    
    // Additional members without strict offset requirements
    int state;                          // State: 1=init, 2=active
    spinlock_t lock;                    // General spinlock
    struct completion frame_complete;   // Frame completion
    struct mutex mlock;                 // Mutex (expected by tx-isp-module.c)
    struct mutex state_lock;            // State mutex
    uint32_t buffer_count;              // Buffer count
    bool processing;                    // Processing flag
    uint32_t vic_errors[13];            // Error array (13 elements, 0x34 bytes)
    uint32_t total_errors;              // Total error count
    spinlock_t buffer_lock;             // Buffer lock
    struct list_head queue_head;        // Buffer queues
    struct list_head free_head;
    struct list_head done_head;
    int streaming;                      // Streaming state
    uint32_t frame_count;               // Frame counter
    
    // Critical offset-dependent members placed at the end with explicit padding
    uint8_t _pad_to_1f4[0x1f4 - 0x110 - sizeof(struct tx_isp_sensor_attribute) - 0x80]; // Safe padding to 0x1f4
    spinlock_t buffer_mgmt_lock;        // 0x1f4: Buffer management spinlock
    uint8_t _pad_to_210[0x210 - 0x1f4 - sizeof(spinlock_t)]; // Padding to offset 0x210  
    int stream_state;                   // 0x210: Stream state (0=off, 1=on)
    uint8_t _pad_to_218[0x218 - 0x210 - sizeof(int)]; // Padding to offset 0x218
    uint32_t active_buffer_count;       // 0x218: Active buffer count
    
    // Final padding to ensure total size is 0x21c
    uint8_t _pad_to_end[0x21c - 0x218 - 4]; // Padding to end of structure
} __attribute__((packed));

#endif /* __TX_ISP_VIC_H__ */
