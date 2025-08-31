#ifndef __TX_ISP_VIC_H__
#define __TX_ISP_VIC_H__

#include <linux/errno.h>
#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/completion.h>

/* VIC Constants */
#define VIC_MAX_CHAN        2

/* VIC Device Structure - matches Binary Ninja reference */
struct vic_device {
    int state;                      /* VIC state machine */
    struct mutex state_lock;        /* State protection */
    void __iomem *vic_regs;        /* VIC register base */
    u32 width;                     /* Frame width */
    u32 height;                    /* Frame height */
    u32 buffer_count;              /* Number of buffers */
    int streaming;                 /* Streaming state flag */
    spinlock_t lock;               /* Register access protection */
    bool processing;               /* Processing state */
    struct completion frame_complete; /* Frame completion */
};

/* VIC Functions */
int tx_isp_vic_probe(struct platform_device *pdev);
int tx_isp_vic_remove(struct platform_device *pdev);

/* VIC Operations */
int tx_isp_vic_start(struct tx_isp_subdev *sd);
int tx_isp_vic_stop(struct tx_isp_subdev *sd);
int tx_isp_vic_set_format(struct tx_isp_subdev *sd, struct tx_isp_config *config);

int vic_chardev_open(struct inode *inode, struct file *file);
int vic_chardev_release(struct inode *inode, struct file *file);
long vic_chardev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

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

#endif /* __TX_ISP_VIC_H__ */
