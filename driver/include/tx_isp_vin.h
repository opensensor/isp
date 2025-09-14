#ifndef __TX_ISP_VIN_H__
#define __TX_ISP_VIN_H__

#include <linux/list.h>
#include <linux/mutex.h>

/* Forward declarations */
struct tx_isp_subdev;
struct tx_isp_sensor;
struct platform_device;
struct tx_isp_config;

/* VIN Device Structure - Based on T30 reference implementation */
struct tx_isp_vin_device {
    struct tx_isp_subdev sd;        /* Base subdev - must be first */
    struct list_head sensors;       /* List of registered sensors */
    struct tx_isp_sensor *active;   /* Currently active sensor */
    
    struct mutex mlock;             /* Mutex for sensor list operations */
    int state;                      /* VIN module state */
    unsigned int refcnt;            /* Reference count */
    
    /* Hardware resources */
    void __iomem *base;             /* VIN register base */
    struct clk *vin_clk;            /* VIN clock */
    int irq;                        /* VIN interrupt */
    
    /* DMA and buffer management */
    dma_addr_t dma_addr;            /* DMA buffer physical address */
    void *dma_virt;                 /* DMA buffer virtual address */
    size_t dma_size;                /* DMA buffer size */
    
    /* Frame management */
    struct completion frame_complete; /* Frame completion */
    spinlock_t frame_lock;          /* Frame buffer lock */
    u32 frame_count;                /* Frame counter */
    
    /* Format and configuration */
    u32 width;                      /* Current frame width */
    u32 height;                     /* Current frame height */
    u32 format;                     /* Current pixel format */
    u32 fps;                        /* Current frame rate */
};

#define sd_to_vin_device(sd) (container_of(sd, struct tx_isp_vin_device, sd))

/* VIN Module States - Based on T30/T31 analysis */
#define TX_ISP_MODULE_SLAKE         1   /* Module inactive */
#define TX_ISP_MODULE_ACTIVATE      2   /* Module activated */
#define TX_ISP_MODULE_INIT          3   /* Module initialized */
#define TX_ISP_MODULE_DEINIT        4   /* Module deinitialized */
#define TX_ISP_MODULE_RUNNING       5   /* Module streaming */

/* VIN Functions */
int tx_isp_vin_probe(struct platform_device *pdev);
int tx_isp_vin_remove(struct platform_device *pdev);

/* VIN Operations */
int tx_isp_vin_start(struct tx_isp_subdev *sd);
int tx_isp_vin_stop(struct tx_isp_subdev *sd);
int tx_isp_vin_set_format(struct tx_isp_subdev *sd, struct tx_isp_config *config);

/* VIN Internal Operations - Based on T30 reference */
int tx_isp_vin_init(struct tx_isp_subdev *sd, int on);
int tx_isp_vin_reset(struct tx_isp_subdev *sd, int on);
int tx_isp_vin_activate_subdev(struct tx_isp_subdev *sd);
int tx_isp_vin_slake_subdev(struct tx_isp_subdev *sd);
int vin_s_stream(struct tx_isp_subdev *sd, int enable);

/* VIN Hardware Initialization */
int tx_isp_vin_hw_init(struct tx_isp_vin_device *vin);
int tx_isp_vin_hw_deinit(struct tx_isp_vin_device *vin);
int tx_isp_vin_setup_dma(struct tx_isp_vin_device *vin);
int tx_isp_vin_cleanup_dma(struct tx_isp_vin_device *vin);

/* VIN Interrupt Handling */
irqreturn_t tx_isp_vin_irq_handler(int irq, void *dev_id);
int tx_isp_vin_enable_irq(struct tx_isp_vin_device *vin);
int tx_isp_vin_disable_irq(struct tx_isp_vin_device *vin);

/* VIN Subdev Operations Structure */
extern struct tx_isp_subdev_ops vin_subdev_ops;

/* VIN States */
#define VIN_STATE_OFF       0
#define VIN_STATE_IDLE     1
#define VIN_STATE_ACTIVE   2
#define VIN_STATE_ERROR    3

/* VIN Configuration */
#define VIN_MAX_WIDTH      2592
#define VIN_MAX_HEIGHT     1944
#define VIN_MIN_WIDTH      64
#define VIN_MIN_HEIGHT     64

/* VIN DMA Buffer Configuration */
#define VIN_DMA_ALIGN      16       /* DMA alignment requirement */
#define VIN_MAX_BUFFERS    4        /* Maximum number of frame buffers */
#define VIN_BUFFER_SIZE    (VIN_MAX_WIDTH * VIN_MAX_HEIGHT * 2) /* Max buffer size for YUV422 */

/* VIN Error Flags */
#define VIN_ERR_OVERFLOW   BIT(0)
#define VIN_ERR_SYNC      BIT(1)
#define VIN_ERR_FORMAT    BIT(2)
#define VIN_ERR_SIZE      BIT(3)
#define VIN_ERR_DMA       BIT(4)
#define VIN_ERR_TIMEOUT   BIT(5)

/* VIN Format Flags */
#define VIN_FMT_YUV422    BIT(0)
#define VIN_FMT_RGB888    BIT(1)
#define VIN_FMT_RAW8      BIT(2)
#define VIN_FMT_RAW10     BIT(3)
#define VIN_FMT_RAW12     BIT(4)

/* VIN Interrupt Flags */
#define VIN_INT_FRAME_END    BIT(0)  /* Frame end interrupt */
#define VIN_INT_OVERFLOW     BIT(1)  /* Buffer overflow interrupt */
#define VIN_INT_SYNC_ERR     BIT(2)  /* Sync error interrupt */
#define VIN_INT_DMA_ERR      BIT(3)  /* DMA error interrupt */

/* VIN Hardware Register Definitions - Based on T31 Binary Ninja Analysis */
#define VIN_CTRL             0x00    /* VIN Control Register */
#define VIN_STATUS           0x04    /* VIN Status Register */
#define VIN_FORMAT           0x08    /* VIN Format Register */
#define VIN_FRAME_SIZE       0x0C    /* VIN Frame Size Register */
#define VIN_INT_STATUS       0x10    /* VIN Interrupt Status Register */
#define VIN_INT_MASK         0x14    /* VIN Interrupt Mask Register */
#define VIN_DMA_ADDR         0x18    /* VIN DMA Address Register */
#define VIN_DMA_SIZE         0x1C    /* VIN DMA Size Register */

/* VIN Control Register Bits */
#define VIN_CTRL_EN          BIT(0)  /* VIN Enable */
#define VIN_CTRL_START       BIT(1)  /* VIN Start */
#define VIN_CTRL_STOP        BIT(2)  /* VIN Stop */
#define VIN_CTRL_RST         BIT(3)  /* VIN Reset */

/* VIN Status Register Bits */
#define STATUS_BUSY          BIT(0)  /* VIN Busy Status */
#define STATUS_IDLE          BIT(1)  /* VIN Idle Status */

/* VIN Format Types */
#define FMT_YUV422           0x00    /* YUV422 Format */
#define FMT_RGB888           0x01    /* RGB888 Format */
#define FMT_RAW8             0x02    /* RAW8 Format */
#define FMT_RAW10            0x03    /* RAW10 Format */
#define FMT_RAW12            0x04    /* RAW12 Format */

#endif /* __TX_ISP_VIN_H__ */
