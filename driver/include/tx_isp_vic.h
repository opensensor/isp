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

uint32_t vic_start_ok = 0;  /* Global VIC interrupt enable flag */

/* VIC Functions */
int tx_isp_vic_probe(struct platform_device *pdev);
int tx_isp_vic_remove(struct platform_device *pdev);

/* VIC Operations - Use existing vic_device from tx_isp.h */
int tx_isp_vic_stop(struct tx_isp_subdev *sd);
int tx_isp_vic_set_buffer(struct tx_isp_subdev *sd, dma_addr_t addr, u32 size);
int tx_isp_vic_wait_frame_done(struct tx_isp_subdev *sd, int channel, int timeout_ms);
int tx_isp_vic_activate_subdev(struct tx_isp_subdev *sd);
int tx_isp_vic_slake_subdev(struct tx_isp_subdev *sd);

/* VIC File Operations */
int vic_chardev_open(struct inode *inode, struct file *file);
int vic_chardev_release(struct inode *inode, struct file *file);
long vic_chardev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

/* VIC Proc Operations - from spec driver */
int isp_vic_frd_show(struct seq_file *seq, void *v);
int dump_isp_vic_frd_open(struct inode *inode, struct file *file);
long isp_vic_cmd_set(struct file *file, unsigned int cmd, unsigned long arg);

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


// VIC subdev structure based on reference driver analysis (0x21c bytes)
// VIC device structure matching reference driver (0x21c bytes total)
// Using union to ensure exact offsets without complex calculations
struct tx_isp_vic_device {
    union {
        struct {
            // Base subdev structure at offset 0
            struct tx_isp_subdev sd;
        };
        struct {
            // Ensure exact offsets with fixed-size buffer
            uint8_t _pad_to_b8[0xb8];
            void __iomem *vic_regs;         // 0xb8: VIC hardware registers
            uint8_t _pad_to_d4[0xd4 - 0xb8 - 8];
            struct tx_isp_vic_device *self; // 0xd4: Self-pointer
            uint8_t _pad_to_dc[0xdc - 0xd4 - 8];
            uint32_t frame_width;           // 0xdc: Frame width
            uint32_t frame_height;          // 0xe0: Frame height
            uint8_t _pad_to_128[0x128 - 0xe0 - 4];
            int state;                      // 0x128: 1=init, 2=active
            uint8_t _pad_to_130[0x130 - 0x128 - 4];
            spinlock_t lock;                // 0x130: Spinlock
            struct mutex mlock;             // Mutex (overlapped)
            uint8_t _pad_to_148[0x148 - 0x130 - sizeof(spinlock_t) - sizeof(struct mutex)];
            struct completion frame_done;   // 0x148: Frame completion
            uint8_t _pad_to_154[0x154 - 0x148 - sizeof(struct completion)];
            struct mutex snap_mlock;        // 0x154: Snapshot mutex
            uint8_t _pad_to_1f4[0x1f4 - 0x154 - sizeof(struct mutex)];
            spinlock_t buffer_lock;         // 0x1f4: Buffer lock
            struct list_head queue_head;    // Buffer queues
            struct list_head free_head;
            struct list_head done_head;
            uint8_t _pad_to_210[0x210 - 0x1f4 - sizeof(spinlock_t) - 3*sizeof(struct list_head)];
            int streaming;                  // 0x210: Streaming state
            uint8_t _pad_to_218[0x218 - 0x210 - 4];
            uint32_t frame_count;           // 0x218: Frame counter
        };
        uint8_t _total_size[0x21c];        // Ensure total size is 0x21c
    };
};


#endif /* __TX_ISP_VIC_H__ */
